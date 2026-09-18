# Assignment revision validation — September 18, 2026

These are local checks on Linux. They do not claim execution on PCC's teaching server.

- The four assignment prompts were revised from the approved drafts. Required work remains a C stack, an assembly array operation, a parallel array map, and a file-serving HTTP handler.
- The assembly package preserves the existing sum example and I/O helper code. The build/run and GDB walkthrough were exercised locally: normal inputs, zeros, and the maximum assigned input values produced the expected sums.
- The web-server scaffold and a private reference handler compiled with C11, warnings enabled, and strict conversion/shadow checks.
- Helper tests passed for reads split at every chunk size of a sample request, interrupted reads/writes, short writes, EOF/errors, embedded NUL, exact/exceeded header and path limits, parser/path rejections, bounded response formatting, and port validation. The helper tests also passed with AddressSanitizer and UndefinedBehaviorSanitizer (leak detection disabled in the tracing environment).
- A private completed handler passed all 15 client checks: exact contents and MIME types, zero-length and 256 KiB files, 404 and directory handling, split requests, simultaneous clients, a slow client alongside another request, invalid requests, and recovery from a connection reset.
- The published scaffold retains the student TODO. It passes the supplied request-validation client check and intentionally fails the 14 checks requiring a completed handler. No reference handler is included in the repository or student downloads.
- Student ZIPs were rebuilt from an explicit allowlist and checked against their source files. Relative documentation links and formatted Brightspace code blocks were checked against repository source during publication.

The parser intentionally supports a limited request format, not the full HTTP standard. URL validation is not general filesystem confinement; the assignment uses a controlled, unchanging document root without symlinks. The one-thread-per-connection server has no production resource limits or idle-client timeout.
