# Lecture 6: Calls, return addresses, and local storage

[Agenda](Agenda.org) · [Term overview](../README.md)

Build the selected examples with `make` in this directory. `make clean` removes
its named build outputs. Additional contrast examples are not automatically included.

Run `./02-basic-function` then `echo $?`: the result is 15. The fun1/fun2 comparison has different callers as well as different callees; see the agenda.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M3 lesson 8; M4 lessons 5–6.

- [Lesson 5 Writing Functions.html](<../../../cs201-course-site/module-04/Lesson 5 Writing Functions.html>)
- [Lesson 6 Local Variables and Stack Frames.html](<../../../cs201-course-site/module-04/Lesson 6 Local Variables and Stack Frames.html>)

Read call/ret and stack frames; recall push/pop. Sketch the C function f(x)=20+2*x.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: explain how a jump reaches a label and why it does not remember a return point.
- 20 min: use 02-basic-function.s to draw what call pushes and ret consumes. Introduce argument/result registers and the caller/callee preservation contract as a reference.
- 25 min: compare fun1.s and fun2.s with register tables. Both are allowed to clobber rdi; fun2 merely happens to leave it intact. Their callers also differ, so this is not a one-line repair.
- 10 min: break.
- 25 min: trace funStack.s and draw its two local slots, saved rbp, and return address. Pairs change one local value and predict the result.
- 15 min: explain preservation across a nested call and 16-byte pre-call alignment. A temporary in r9 is not protection across another call.
- 5 min: identify what a caller must save and where the return address lives.
- 10 min: repeat a stack diagram; no recursive Fibonacci today.

## Example and scope notes

echo.s and helloRet.s are syscall return-value/byte-count review exercises; despite its name, helloRet.s has no call/ret instructions. funStack.s demonstrates frame-local storage; it does not itself demonstrate saving a live register across a nested call. Recursive calls come after this lecture.

## Assignment planning

Assignment 2: students can now specify their procedure inputs, outputs, and register ownership.
