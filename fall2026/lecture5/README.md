# Lecture 5: Branches, loops, and array addresses

[Agenda](Agenda.org) · [Term overview](../README.md)

Build the selected examples with `make` in this directory. `make clean` removes
its named build outputs. Additional contrast examples are not automatically included.

Run `./sumLoop` then `echo $?`: the sum 1 through 10 is 55. Trace the loop before changing its bound.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M3 lesson 8; M4 lessons 1–4 and 7.

- [Lesson 8 Stack Push and Pop.html](<../../../cs201-course-site/module-03/Lesson 8 Stack Push and Pop.html>)
- [Lesson 1 Condition Codes and Jumps.html](<../../../cs201-course-site/module-04/Lesson 1 Condition Codes and Jumps.html>)
- [Lesson 2 If Statements.html](<../../../cs201-course-site/module-04/Lesson 2 If Statements.html>)
- [Lesson 3 Loops.html](<../../../cs201-course-site/module-04/Lesson 3 Loops.html>)
- [Lesson 4 Switch Statements.html](<../../../cs201-course-site/module-04/Lesson 4 Switch Statements.html>)
- [Lesson 7 Arrays Structs and Alignment.html](<../../../cs201-course-site/module-04/Lesson 7 Arrays Structs and Alignment.html>)

Read comparisons and loops; write the C version of summing 1 through 10. Read push/pop as preparation for the next lecture, and use switch/alignment as reference.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: distinguish an address from the value there; trace two register assignments.
- 20 min: translate one if statement. For cmp source,destination, reason about destination-source; choose a signed or unsigned jump deliberately.
- 25 min: build sumLoop.s one small change at a time. Trace the initial state, body, update, test, and exit before running.
- 10 min: break.
- 25 min: use addArray3.s to connect base + index × scale to C indexing, then have pairs sketch an array-summing loop using .quad and scale 8.
- 15 min: test an empty range and a one-element range on paper; use GDB to inspect one indexed element.
- 5 min: compute an element address and diagnose one off-by-one comparison.
- 10 min: reteach the loop back edge; do not run all four array variants.

## Example and scope notes

Switch/jump tables, unsigned comparison variants, and the .long/scale-4 contrast are structured reading practice. Avoid adding call/ret before the loop trace is secure.

## Assignment planning

Assignment 2: choose a small program idea and an array representation.
