# Lecture 4: Build a program that talks to the machine

[Agenda](Agenda.org) · [Term overview](../README.md)

Build the selected examples with `make` in this directory. `make clean` removes
its named build outputs. Additional contrast examples are not automatically included.

Run `./firstfixed` then `echo $?` to see status 255. `first.s` is deliberately incomplete. `hello.s` includes a NUL in its output count: inspect and repair that length in a scratch copy.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M3 lessons 1–7 and 10.

- [Lesson 1 Build Process and First Program.html](<../../../cs201-course-site/module-03/Lesson 1 Build Process and First Program.html>)
- [Lesson 2 Writing Makefiles.html](<../../../cs201-course-site/module-03/Lesson 2 Writing Makefiles.html>)
- [Lesson 3 Registers and CPU Environment.html](<../../../cs201-course-site/module-03/Lesson 3 Registers and CPU Environment.html>)
- [Lesson 4 Moving Data.html](<../../../cs201-course-site/module-03/Lesson 4 Moving Data.html>)
- [Lesson 5 Data Section and Labels.html](<../../../cs201-course-site/module-03/Lesson 5 Data Section and Labels.html>)
- [Lesson 6 Arithmetic Operations.html](<../../../cs201-course-site/module-03/Lesson 6 Arithmetic Operations.html>)
- [Lesson 7 Logical and Shift Operations.html](<../../../cs201-course-site/module-03/Lesson 7 Logical and Shift Operations.html>)
- [Lesson 10 Debugging with GDB.html](<../../../cs201-course-site/module-03/Lesson 10 Debugging with GDB.html>)

Read the first assembly program and the register/move introduction. Keep the register and instruction tables open. The remaining lessons support the demonstrations and later practice.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: close out the C allocation questions; distinguish source, object, and executable.
- 20 min: write the exit program by hand and build with as then ld. Change the exit status; explain why the shell shows only its low eight bits.
- 25 min: step firstadd.s in GDB; watch registers change and identify immediate/register operands. Compare a tiny gcc -S output only after the hand-written program works.
- 10 min: break.
- 25 min: extend the program to hello.s. Inspect its extra terminator byte: .asciz includes a zero, so use .ascii or subtract one from hellolen in a scratch copy. Label address and byte count; use only the syscall registers needed for write and exit.
- 15 min: each pair changes a message and verifies its length; read one logical/shift instruction using the reference table.
- 5 min: explain mov versus lea and why _start cannot simply ret.
- 10 min: toolchain/GDB catch-up; defer division’s implicit operands to reference practice.

## Example and scope notes

first.s is intentionally incomplete: it falls through after loading a register. Use firstfixed.s for the successful path and first.s only as a labeled debugging contrast. The full function calling convention comes in lecture 6.

## Assignment planning

Proposed Assignment 1 completion. Preview Assignment 2’s whole-program goal without expecting functions yet.
