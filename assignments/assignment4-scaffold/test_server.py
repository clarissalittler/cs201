#!/usr/bin/env python3
"""Test a RUNNING student server. Standard-library Python only; no solution.

Run ./httpd [port] in another terminal, then python3 test_server.py --port PORT.
The supplied placeholder intentionally fails the file-serving tests.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import socket
import struct
import sys
import time

ROOT = Path(__file__).resolve().parent / "www"
FIXTURES = {
    "/": ("index.html", "text/html"),
    "/index.html": ("index.html", "text/html"),
    "/_tests/about.txt": ("_tests/about.txt", "text/plain"),
    "/_tests/style.css": ("_tests/style.css", "text/css"),
    "/_tests/pixel.png": ("_tests/pixel.png", "image/png"),
    "/_tests/pixel.jpg": ("_tests/pixel.jpg", "image/jpeg"),
    "/_tests/empty.txt": ("_tests/empty.txt", "text/plain"),
    "/_tests/large.bin": ("_tests/large.bin", "application/octet-stream"),
}

def connect(port):
    return socket.create_connection(("127.0.0.1", port), timeout=4)

def receive(sock):
    data = bytearray()
    while True:
        part = sock.recv(65536)
        if not part:
            break
        data.extend(part)
        if len(data) > 8 * 1024 * 1024:
            raise AssertionError("Response exceeded the test's 8 MB limit")
    header, separator, body = bytes(data).partition(b"\r\n\r\n")
    assert separator, "Response needs headers followed by a blank CRLF line"
    lines = header.decode("ascii").split("\r\n")
    version, status, _ = lines[0].split(" ", 2)
    assert version == "HTTP/1.0", f"Expected HTTP/1.0, got {version}"
    fields = {}
    for line in lines[1:]:
        name, value = line.split(":", 1)
        fields[name.lower()] = value.strip()
    assert int(fields["content-length"]) == len(body), "Content-Length differs from body byte count"
    assert fields.get("connection", "").lower() == "close", "Expected Connection: close"
    return int(status), fields, body

def request(port, path, fragments=False):
    data = f"GET {path} HTTP/1.1\r\nHost: localhost\r\n\r\n".encode("ascii")
    with connect(port) as sock:
        if fragments:
            for part in (data[:3], data[3:13], data[13:-1], data[-1:]):
                sock.sendall(part)
                time.sleep(0.02)
        else:
            sock.sendall(data)
        return receive(sock)

def check_file(port, path, fragments=False):
    status, fields, body = request(port, path, fragments)
    name, mime = FIXTURES[path]
    assert status == 200, f"Expected 200 for {path}; got {status} (501 means the handler is unfinished)"
    assert fields.get("content-type") == mime, f"Wrong Content-Type for {path}"
    assert body == (ROOT / name).read_bytes(), f"Downloaded bytes differ for {path}"

def check_missing(port, path):
    status, fields, body = request(port, path)
    assert status == 404, f"Expected 404, got {status}"
    assert body, "Include a short missing-file response body"
    assert fields.get("content-type") == "text/html"

def simultaneous(port):
    paths = list(FIXTURES) * 3
    with ThreadPoolExecutor(max_workers=8) as pool:
        list(pool.map(lambda path: check_file(port, path), paths))

def slow_client(port):
    # Keep one request incomplete. Another must finish without waiting for it.
    with connect(port) as sock:
        sock.sendall(b"GET /_tests/about.txt HTTP/1.0\r\nHost:")
        check_file(port, "/_tests/pixel.png")
        sock.sendall(b" localhost\r\n\r\n")
        status, _, body = receive(sock)
        assert status == 200 and body == (ROOT / "_tests/about.txt").read_bytes()

def invalid_requests(port):
    cases = [
        (b"GET /../outside HTTP/1.0\r\n\r\n", 400),
        (b"GET /%2e%2e/outside HTTP/1.0\r\n\r\n", 400),
        (b"GET /file?query HTTP/1.0\r\n\r\n", 400),
        (b"POST / HTTP/1.0\r\n\r\n", 405),
        (b"GET / HTTP/2.0\r\n\r\n", 505),
        (b"GET / HTTP/1.0\r\nX: " + b"a" * (4096-19), 431),
    ]
    for payload, expected in cases:
        with connect(port) as sock:
            sock.sendall(payload)
            status, _, _ = receive(sock)
            assert status == expected, f"Expected rejection {expected}, got {status}"

def disconnected_client(port):
    with connect(port) as sock:
        sock.sendall(b"GET /_tests/large.bin HTTP/1.0\r\n\r\n")
        sock.recv(1)  # the response has begun; now reset this connection
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, struct.pack("ii", 1, 0))
    check_file(port, "/_tests/about.txt")

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", type=int, default=8080)
    args = parser.parse_args()
    if not 1 <= args.port <= 65535:
        parser.error("port must be between 1 and 65535")
    tests = [(f"File {path}", lambda path=path: check_file(args.port, path)) for path in FIXTURES]
    tests += [
        ("Missing file", lambda: check_missing(args.port, "/_tests/missing.html")),
        ("Directory is not a regular file", lambda: check_missing(args.port, "/_tests")),
        ("Fragmented request", lambda: check_file(args.port, "/_tests/pixel.png", True)),
        ("Simultaneous clients", lambda: simultaneous(args.port)),
        ("Slow client doesn't block other clients", lambda: slow_client(args.port)),
        ("Supplied request validation", lambda: invalid_requests(args.port)),
        ("Client disconnect and recovery", lambda: disconnected_client(args.port)),
    ]
    failures = 0
    for name, test in tests:
        try:
            test()
            print(f"PASS: {name}")
        except (AssertionError, OSError, ValueError, KeyError) as error:
            failures += 1
            print(f"FAIL: {name}: {error}")
    print(f"\n{len(tests)-failures}/{len(tests)} checks passed.")
    return 1 if failures else 0

if __name__ == "__main__":
    sys.exit(main())
