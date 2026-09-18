#!/usr/bin/env python3
"""Rebuild the two student ZIPs from the canonical support directories.

Standard library only. Fixed timestamps make rebuilds reproducible.
Compiled files, caches, and any unlisted support files are excluded.
"""
from pathlib import Path
from zipfile import ZipFile, ZipInfo, ZIP_DEFLATED

ROOT = Path(__file__).resolve().parent
PACKAGES = {
    'assignment2-starter': ('assignment2-support', [
        'README.md', 'Makefile', '.gitignore', 'sum5.s', 'readInt.s', 'writeInt.s',
    ]),
    'assignment4-starter': ('assignment4-scaffold', [
        'README.md', 'Makefile', '.gitignore', 'httpd.c', 'http_support.c',
        'http_support.h', 'function-guide.md', 'test_server.py',
        'tests/test_helpers.c', 'www/index.html', 'www/_tests/about.txt',
        'www/_tests/style.css', 'www/_tests/empty.txt', 'www/_tests/pixel.png',
        'www/_tests/pixel.jpg', 'www/_tests/large.bin',
    ]),
}

def main():
    (ROOT/'downloads').mkdir(exist_ok=True)
    for name, (directory, files) in PACKAGES.items():
        destination = ROOT/'downloads'/f'{name}.zip'
        with ZipFile(destination, 'w', compression=ZIP_DEFLATED) as archive:
            for relative in sorted(files):
                item = ZipInfo(f'{name}/{relative}', (2026, 9, 18, 0, 0, 0))
                item.compress_type = ZIP_DEFLATED
                item.external_attr = 0o100644 << 16
                archive.writestr(item, (ROOT/directory/relative).read_bytes())
        print(destination.name)

if __name__ == '__main__':
    main()
