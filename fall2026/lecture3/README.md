# Lecture 3: Floating-point representation and its limits

[Agenda](Agenda.org) · [Term overview](../README.md)

Build the selected examples with `make` in this directory. `make clean` removes
its named build outputs. Additional contrast examples are not automatically included.

Run `./floatmunger1Corrected` and `./floatlimit`. The earlier float sketch is preserved as `floatmungerPreview.c`; use the corrected companion to check bit patterns.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M2 lessons 1–5; lesson 6 extension.

- [Lesson 1 Fractional Binary Numbers.html](<../../../cs201-course-site/module-02/Lesson 1 Fractional Binary Numbers.html>)
- [Lesson 2 IEEE 754 Floating Point Encoding Scheme.html](<../../../cs201-course-site/module-02/Lesson 2 IEEE 754 Floating Point Encoding Scheme.html>)
- [Lesson 3 Floating Point Examples.html](<../../../cs201-course-site/module-02/Lesson 3 Floating Point Examples.html>)
- [Lesson 4 Floating Point Rounding.html](<../../../cs201-course-site/module-02/Lesson 4 Floating Point Rounding.html>)
- [Lesson 5 Floating Point Operations.html](<../../../cs201-course-site/module-02/Lesson 5 Floating Point Operations.html>)

Read fractional binary and the binary32 layout; attempt the site’s 6.5 example before class. Use the rounding and operations lessons for follow-up.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: sum 1/2 + 1/8; recall that a bit pattern needs an interpretation.
- 20 min: normalize 6.5 as 1.101 × 2^2; separate the stored fraction from the implicit leading one and explain the exponent bias.
- 25 min: decode 0 10000001 1010…0, then pairs encode -0.75. Check with floatmunger1Corrected.c, changing its initial float in a scratch copy for each value; inspect the representation with memcpy.
- 10 min: break.
- 25 min: predict repeated additions in floatlimit.c, run it, and change float to double in a working copy. Explain spacing and one halfway rounding example.
- 15 min: classify zero, subnormal, infinity, and NaN patterns with a decision table. Finite storage is enough to motivate approximation; no cardinality detour is needed.
- 5 min: one normal decode and an explanation of why accumulated error is not a compiler bug.
- 10 min: redo an encode if needed; stop before introducing another number format.

## Example and scope notes

Posits and guard/round/sticky implementation are follow-up reading or discussion, not another live encoding system. Keep the original floatmunger1.c and corrected comparison; the original pointer-punning is not a copyable model. Equality of floats can be meaningful; avoid a blanket “never use ==” rule.

## Assignment planning

Give feedback on Assignment 1 before its proposed completion at lecture 4.
