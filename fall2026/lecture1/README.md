# Lecture 1: C objects, addresses, and bytes

[Agenda](Agenda.org) · [Term overview](../README.md)

Build the selected examples with `make` in this directory. `make clean` removes
its named build outputs. Additional contrast examples are not automatically included.

Run `./c_objects`: the fields start as 10/20 and become 20/20. Change the addressed field, then draw the allocation and pointer.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M1 lessons 1–4.

- [M1 Lesson 1.html](<../../../cs201-course-site/module-01/M1 Lesson 1.html>)
- [M1 Lesson 2.html](<../../../cs201-course-site/module-01/M1 Lesson 2.html>)
- [M1 Lesson 3.html](<../../../cs201-course-site/module-01/M1 Lesson 3.html>)
- [M1 Lesson 4.html](<../../../cs201-course-site/module-01/M1 Lesson 4.html>)

Skim the C++-to-C comparison; compile a supplied C program. Read the short sizes, hex, and byte-order lessons. Bring one thing that differs from C++.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: course setup and a quick check that everyone can compile and run; collect C++ questions.
- 20 min: run c_objects.c, then draw the pointer variable and its allocated struct. Explain pass-by-value, address-of, member access, initialization, and the matching free.
- 25 min: pairs change which field double_value receives, predict both fields, and run. Then trace the existing arraything.c without adding more C syntax.
- 10 min: break.
- 25 min: sizes → hex → bytes. Predict the byte order of 0x0123456789abcdef and check byteOrderTest.c on the course Linux target; draw consecutive addresses.
- 15 min: sketch the ownership of a stack allocated with malloc for Assignment 1. Distinguish a stack data structure from the call stack; do not implement push/pop for them.
- 5 min: draw p, *p, and the allocation that free releases; translate one byte between hex and binary.
- 10 min: installation help or redo the pointer diagram; float previews are deferred to lecture 3.

## Example and scope notes

The complete C syntax survey is reference reading. Do not start bitwise operations or floats in this lecture. The local malloc.c/malloc2.c remain historical sketches; c_objects.c is the runnable allocation model.
