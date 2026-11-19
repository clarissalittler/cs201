# Lecture 11: Lock scope, then condition variables

[Agenda](Agenda.org) · [Term overview](../README.md)

Build the selected examples with `make` in this directory. `make clean` removes
its named build outputs. Additional contrast examples are not automatically included.

Compare `./condBusyWait` with `./condWait`; each waits about five seconds before completing. Draw the protected predicate before explaining the API.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M6 lessons 4–5.

- [Lesson 4 Mutexes.html](<../../../cs201-course-site/module-06/Lesson 4 Mutexes.html>)
- [Lesson 5 Condition Variables and Semaphores.html](<../../../cs201-course-site/module-06/Lesson 5 Condition Variables and Semaphores.html>)

Read the busy-wait and condition-variable sections only; identify the state protected by each mutex in the counter example.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: repair the counter update and recall why per-thread argument storage matters.
- 20 min: distinguish claiming a chunk under the mutex from processing the claimed disjoint range outside it. Draw one two-worker claim trace; do not supply the full assignment solution.
- 25 min: diff condBusyWait.c and condWait.c. The protected predicate stays the same; the waiting mechanism changes.
- 10 min: break.
- 25 min: pairs draw unlock-and-wait, notification, lock reacquisition, and predicate recheck. Try moving the producer earlier: the predicate must still make the program correct.
- 15 min: explain while rather than if and connect the atomic wait to last lecture’s signal-wait timeline. The condition variable is not stored “permission to proceed.”
- 5 min: write the predicate loop and mark exactly which state is protected.
- 10 min: repeat the predicate timeline; save the bounded buffer for lecture 12.

## Example and scope notes

threadCounterStruct.c bundles data and mutex but holds its lock across simulated work; use it to critique lock scope, not as the finished chunk-processing design. condBroadcast.c is follow-up reading; scheduling and CPU measurements are observations, not fixed promises.

## Assignment planning

Release Assignment 3 after the chunk-claim sketch; propose completion at lecture 14. It requires mutexes and worker lifetimes, not condition variables.
