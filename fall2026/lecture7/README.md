# Lecture 7: Assembly integration and project workshop

[Agenda](Agenda.org) · [Term overview](../README.md)

Build the selected examples with `make` in this directory. `make clean` removes
its named build outputs. Additional contrast examples are not automatically included.

Run `./readWriteTest` interactively, type one non-negative integer and press Enter. The helper assumes one nonempty line per read; do not pipe multiple input lines to it. `./sumLoopArray` prints 10; use small inputs for `./fact`.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M3 lesson 9; M4 integration and remaining topics.

- [Lesson 9 Complete Program.html](<../../../cs201-course-site/module-03/Lesson 9 Complete Program.html>)
- [Lesson 7 Arrays Structs and Alignment.html](<../../../cs201-course-site/module-04/Lesson 7 Arrays Structs and Alignment.html>)
- [Lesson 8 Buffer Overflows and Security.html](<../../../cs201-course-site/module-04/Lesson 8 Buffer Overflows and Security.html>)
- [Lesson 9 Floating Point.html](<../../../cs201-course-site/module-04/Lesson 9 Floating Point.html>)

Read the Assignment 2 requirements and the readInt contract. Bring a procedure or loop to debug. Read the short alignment/security sections; floating-point instructions are reference material.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: reconstruct the call contract from memory using the register sheet to check.
- 20 min: assemble and link the supplied readInt/writeInt helpers with readWriteTest.s. Treat them as a library; do not live-code their parsers.
- 25 min: inspect sumLoopArray.s: the loop is currently in _start and calls only writeInt. Sketch extracting the loop into a sum procedure returning in rax, then calling it from _start. Trace fact(3) on paper only if the basic frame exit check passed.
- 10 min: break.
- 25 min: protected Assignment 2 work. Require one observable input-to-output path before extending the program.
- 15 min: inspect a struct-offset example and a buffer-boundary diagram from M4. Relate element widths and unchecked writes to the addresses just used.
- 5 min: explain one procedure’s inputs, result, and preserved state; show a successful build.
- 10 min: project debugging; recursion is the first live extension to cut.

## Example and scope notes

The local readInt helper assumes interactive, nonempty, non-negative decimal input followed by Enter, one line per read; no invalid input, negatives, or piped batch input. Use small factorial inputs (0..20 for signed 64-bit results). SSE, full exploit demonstrations, and parser internals remain reading/extensions.

## Assignment planning

Release the full Assignment 2 work period no later than this lecture; propose completion at lecture 10 so students have three later lectures for questions.
