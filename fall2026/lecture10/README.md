# Lecture 10: Thread lifetimes, arguments, and a shared counter

[Agenda](Agenda.org) · [Term overview](../README.md)

Build the selected examples with `make` in this directory. `make clean` removes
its named build outputs. Additional contrast examples are not automatically included.

Run `./threadArgs` to see twenty distinct argument values, in unspecified order. `./threadCounterMutex` prints 10000. Broken argument/return/counter variants are diagnosis examples and are not default build targets.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M6 lessons 1–4, first pass.

- [Lesson 1 Threads vs Processes.html](<../../../cs201-course-site/module-06/Lesson 1 Threads vs Processes.html>)
- [Lesson 2 Creating and Joining Threads.html](<../../../cs201-course-site/module-06/Lesson 2 Creating and Joining Threads.html>)
- [Lesson 3 Race Conditions.html](<../../../cs201-course-site/module-06/Lesson 3 Race Conditions.html>)
- [Lesson 4 Mutexes.html](<../../../cs201-course-site/module-06/Lesson 4 Mutexes.html>)

Read creation/join and the shared-memory diagram. Review the void * callback signature; function pointers are a just-in-time prerequisite, not a map/fold detour.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: contrast private process variables with memory shared by threads.
- 20 min: one create/join lifecycle, callback arguments, and the lifetime of a per-thread argument slot.
- 25 min: predict threadArgsBad.c; compare with threadArgs.c and draw why its array remains alive through every join. Inspect pthread error return values in the site example.
- 10 min: break.
- 25 min: trace two updates of the shared counter on paper, then compare the bad and mutex versions. The racy C program has undefined behavior; output is not guaranteed to vary.
- 15 min: locate the shared state and the whole update that must be protected. A successful-looking racy run proves nothing.
- 5 min: repair an argument-lifetime sketch and mark one critical section.
- 10 min: redo create/join or argument lifetime before adding return-value examples.

## Example and scope notes

threadRetBad.c/threadRet.c are an optional lifetime comparison. Do not run every numbered thread variant. The source sketches omit some pthread error checks; the site’s checked creation example supplies the API-error model.

## Assignment planning

Proposed Assignment 2 completion. Preview Assignment 3; do not require its chunk-claim design until lecture 11.
