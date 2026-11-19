# Lecture 17: TCP and the multi-client server project

[Agenda](Agenda.org) · [Socket steps walkthrough](SocketSteps.md) · [Term overview](../README.md)

Build everything with `make` in this directory; `make clean` removes the
executables and any leftover `.sock` files. Each TCP and UDP server takes a
port number (`./step1HelloOnce 5000`), and `unixEcho` takes a socket path
(`./unixEcho echo.sock`); connect from a second terminal.
[SocketSteps.md](SocketSteps.md) has the `nc` command for every example (`-d`
for steps 1–2, `-N` for steps 3–5, `-u` for UDP, `-U -N` for Unix sockets)
plus things to try. If `bind` reports "Address already in use", pick another
port.

[Assignment 4](../../assignments/assignment4.md) has no scaffold: students
copy `step4EchoPoll.c` or `echoServerThread.c` and turn the echo into their own
multi-client server (chat, a game, a tiny MUD).

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Examples

| File | Part | What it shows |
|---|---|---|
| `step1HelloOnce.c` | 1 | `socket`, `bind`, `listen`, one `accept`, fixed message, exit |
| `step2HelloLoop.c` | 1 | `accept` in a loop: one client after another |
| `step3EchoLoop.c` | 1 | Echo until `recv` returns 0; a second client waits |
| `step4EchoPoll.c` | 1 | `poll()` serves many clients at once |
| `step5EchoSelect.c` | 1 | The same server with `select()` |
| `step6Client.c` | 1 | Our own client; needs `poll()` for keyboard *and* socket |
| `udpEcho.c` | 1 | Step 3 over UDP: no `accept`, `recvfrom`/`sendto`, test with `nc -u` |
| `udpSend.c` | 1 | UDP client: each argument is one datagram, boundaries kept, loss unreported |
| `unixEcho.c` | 1 | Step 3 over `AF_UNIX`: a file name for an address, peer pid/uid, `nc -U -N` |
| `socketPair.c` | 1 | `socketpair` + `fork`: a two-way pipe between parent and child |
| `echoServerThread.c` | 2 | One thread per client, with some rough edges to discuss |
| `webServer.c` | 2 | Fixed-text HTTP server: `curl -v http://localhost:8080/` |
| `webClient.c` | 2 | Minimal HTTP client: `./webClient 127.0.0.1 8080 /path` |

Each step changes as little as possible from the one before, so
`diff step1HelloOnce.c step2HelloLoop.c` shows exactly what's new. The UDP and
Unix servers are each step 3 with a different `socket()` call and only the
changes that follow from it; SocketSteps.md shows how to diff just the code.

## Preparation

M7 lessons 3–6.

- [Lesson 3 TCP-IP and Socket Basics.html](<../../../cs201-course-site/module-07/Lesson 3 TCP-IP and Socket Basics.html>)
- [Lesson 4 Building a Server.html](<../../../cs201-course-site/module-07/Lesson 4 Building a Server.html>)
- [Lesson 5 Clients and Multi-Client Servers.html](<../../../cs201-course-site/module-07/Lesson 5 Clients and Multi-Client Servers.html>)
- [Lesson 6 From Echo Server to Web Server.html](<../../../cs201-course-site/module-07/Lesson 6 From Echo Server to Web Server.html>)

Read TCP addresses/ports and the server lifecycle, and recall the byte-copy loop
from the file lectures. Read Assignment 4 and come with an idea for what your
server will do with its clients' input.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: revisit network byte order using the bytes from lecture 1. Draw socket → bind → listen → accept, and separate the listening descriptor from the accepted one.
- 20 min: run step 1 and step 2 with `nc -d`. Show "Connection refused" after step 1 exits, then `diff` the two files to find the accept loop.
- 25 min: run step 3 with `nc -N`. TCP is a byte stream, not messages; `recv` returning 0 is EOF; `MSG_NOSIGNAL` avoids SIGPIPE. Connect two clients to see the second one wait, then repeat with step 4. If there's room, take 5 minutes to diff `udpEcho.c` and `unixEcho.c` against step 3 and run `udpSend` with the server stopped. Otherwise they, steps 5 and 6, and `socketPair.c` are follow-up reading.
- 10 min: break.
- 20 min: two ways to juggle clients. Compare step 4's `poll` loop with `echoServerThread.c`: where does shared state live, and what needs a mutex? Connect the per-client heap slot to the thread-argument lifetime lesson: who frees the slot, and who closes the socket?
- 15 min: a line is not a `recv`. Send `printf 'one\ntwo\n' | nc -N` to step 4 and sketch a per-client line buffer. If there's room, show `webServer.c` with `curl -v` as "HTTP is just another text protocol."
- 15 min: protected project work. Copy a step server, switch it to loopback, and make it do one tiny non-echo thing with two `nc` clients connected. Discuss disconnects, short sends, `MSG_NOSIGNAL`, and unbounded lines as questions come up.
- 5 min: each student shows two clients being served or a concrete next failing check, and names who closes a departing client's socket and frees its state.

## Example and scope notes

The step servers skip most error checking and the short-send loop so the
socket calls stand out; point back to the file-lecture copy loop when discussing
robustness. They bind `INADDR_ANY`, so they accept connections on any of the
machine's addresses, not just loopback. `echoServerThread.c`, `webServer.c`, and
`webClient.c` are reading contrasts, not robust reference servers.

`select`/`poll` internals, `epoll`, and the step 6 client are extensions. So
are the UDP and Unix-socket examples: the point is that the same calls make a
different kind of socket, not a protocol survey. Reliability over UDP, fd
passing over Unix sockets, and abstract socket names are out of scope.
`../optional/ipc/domainServer.c` is an older, hard-coded-path version of
`unixEcho.c`.

Do not complete the final assignment for students or live-build competing
server architectures. Sketch the per-client line buffer, but leave writing it
to the students; it's the piece most often skipped, and the assignment's
two-commands-at-once check is there to catch it.

## Assignment planning

Final-project implementation begins here at the latest. Students should leave
with two `nc` clients connected to their own server or a concrete next failing
check. Lecture 19's
ownership audit and lecture 20's checkpoint both use the project, so it needs
to be moving by then.
