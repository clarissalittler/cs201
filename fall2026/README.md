# Fall 2026 — Term Skeleton

This is a pre-built skeleton for the Fall 2026 run of CS201, assembled from
the audited Spring 2026 examples (all of which compile clean and passed the
`AUDIT-NOTES.org` checklist). Each `lectureN/` has a skeletal `Agenda.org`
— topic arc and file references filled in, admin/dates left as placeholders.

## What's different from Spring 2026

The concurrency block is restructured to fix the long-standing coverage gap
(see the topic table in `AUDIT-NOTES.org`): **condition variables** now get
their own lecture instead of being skipped.

| Lecture | Topic | Source |
|---|---|---|
| 1 | Intro, C, bits, two's complement | spring2026/lecture1 |
| 2 | Hex, endianness, IEEE 754 | spring2026/lecture2 |
| 3 | malloc, first assembly | spring2026/lecture3 |
| 4 | Registers, addressing, control flow, hello | spring2026/lecture4 |
| 5 | echo, call/ret, the stack | spring2026/lecture5 |
| 6 | Multi-file asm, recursion, readInt/writeInt | spring2026/lecture6 |
| 7 | Processes, fork, threads intro | spring2026/lecture7 |
| 8 | Thread args/returns, mutexes, critical sections | spring2026/lecture8 |
| **9** | **Condition variables, producer/consumer** | **new for fall2026** |
| 10 | Semaphores, dining philosophers | split from spring2026/lecture9 |
| 11 | Exceptions and signals | split from spring2026/lecture9 |
| 12 | Memory hierarchy + virtual memory (reading week) | CS:APP ch. 6 & 9 |
| 13 | File IO, both layers | spring2026/lecture13 |
| 14 | Everything is a file, IPC, shared memory | spring2026/lecture14 |
| 15 | Internet sockets, select/poll | spring2026/lecture15 |
| 16 | Rust I: ownership, borrowing, threads | spring2026/lecture16 |
| 17 | Rust II: structs, enums, ADTs, lists | spring2026/lecture17 |

The new lecture 9 files:

- `condBusyWait.c` — what waiting looks like with only a mutex (spins a core)
- `condWait.c` — the same program with `pthread_cond_wait` (diff them!)
- `condBroadcast.c` — signal vs. broadcast, as a footrace starting line
- `prodcons.c` — the bounded buffer with two condvars; the machinery inside
  pipes and work queues

Project/catch-up days aren't numbered here — slot them in wherever the term
calendar needs them. Deliberately-broken teaching files keep their
non-compiling extensions (`badborrow.moveme`, `func2bad`) so `make` and
students' editors don't trip on them.

## Before the term starts

Run through the "New Term Checklist" at the bottom of `AUDIT-NOTES.org`,
and re-read the assignments (`assignments/`) for dates and any drift from
these examples.
