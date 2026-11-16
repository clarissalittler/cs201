# Lecture 14: Virtual addresses and linking

[Agenda](Agenda.org) · [Term overview](../README.md)

Build the selected examples with `make` in this directory. `make clean` removes
its named build outputs. Additional contrast examples are not automatically included.

`make` assembles and links `readWriteTest` from `readWriteTest.s`, `readInt.s`,
and `writeInt.s`, the same helpers as lecture 7. Run `./readWriteTest`
interactively. For the linking exercise, leave `writeInt.o` out of a scratch
link and inspect the missing symbol with `nm`:

```bash
ld readWriteTest.o readInt.o -o scratch
```

```bash
nm readWriteTest.o
```

`make fork_lifecycle` builds the process-copy reminder from lecture 8.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M9 lessons 1–2.

- [Lesson 1 Virtual Memory.html](<../../../cs201-course-site/module-09/Lesson 1 Virtual Memory.html>)
- [Lesson 2 Static and Dynamic Linking.html](<../../../cs201-course-site/module-09/Lesson 2 Static and Dynamic Linking.html>)

Read the address-translation worked example and the symbol-resolution
introduction. Reopen the multi-object assembly build from lecture 7.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: distinguish a cache miss from an invalid memory access, and recall the parent/child copy experiment.
- 20 min: translate virtual address 0x1234 with 256-byte pages and VPN 0x12 mapped to PPN 0xAB: offset 0x34, physical address 0xAB34. Then change the present/protection bits.
- 25 min: trace TLB/page-table use, demand paging, and copy-on-write conceptually. A page fault can be handled successfully; an invalid access may lead the kernel to deliver SIGSEGV.
- 10 min: break.
- 25 min: omit writeInt.o from a scratch link, inspect symbols with nm, then repair the build. Pairs classify compile, assembly, link, and runtime errors.
- 15 min: relate static/dynamic linking to loading, then a short project check-in: each student names the thing that's stuck.
- 5 min: complete one translation and explain an undefined-symbol diagnostic.
- 10 min: address translation or project help; do not expand into PLT/GOT mechanics.

## Example and scope notes

TLB bit-budget derivations, detailed relocation formats, and PLT/GOT disassembly
are reading. fork is not instantaneous: copy-on-write avoids eagerly copying all
data pages, but setup still has costs. The MMU detects translation/protection
faults and the kernel handles them.

## Assignment planning

Project check-in while there's still plenty of term left. Record the
translation/linking exit checks separately from project progress. HTTP and the
web server come in lecture 17, so keep the check-in general.
