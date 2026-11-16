# Lecture 13: Locality, caches, and measured performance

[Agenda](Agenda.org) · [Term overview](../README.md)

Build the selected examples with `make` in this directory. `make clean` removes
its named build outputs. Additional contrast examples are not automatically included.

Run `./locality`, then `make locality-O2` and `./locality-O2`. All checksums should agree. Compare repeated elapsed times without assuming a fixed speedup.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M8 lessons 1–8.

- [Lesson 1 RAM.html](<../../../cs201-course-site/module-08/Lesson 1 RAM.html>)
- [Lesson 2 Locality.html](<../../../cs201-course-site/module-08/Lesson 2 Locality.html>)
- [Lesson 3 Caches.html](<../../../cs201-course-site/module-08/Lesson 3 Caches.html>)
- [Lesson 4 Optimization Why and When.html](<../../../cs201-course-site/module-08/Lesson 4 Optimization Why and When.html>)
- [Lesson 5 gcc Optimization Levels.html](<../../../cs201-course-site/module-08/Lesson 5 gcc Optimization Levels.html>)
- [Lesson 6 Measuring Performance.html](<../../../cs201-course-site/module-08/Lesson 6 Measuring Performance.html>)
- [Lesson 7 Optimizing Code.html](<../../../cs201-course-site/module-08/Lesson 7 Optimizing Code.html>)
- [Lesson 8 Cache-Aware Programming.html](<../../../cs201-course-site/module-08/Lesson 8 Cache-Aware Programming.html>)

Read locality and the basic cache organization before class; use RAM technology and the optimization catalog as reference. After class, complete a cache trace and compare optimization levels.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: identify which array addresses are adjacent and distinguish the call stack from the heap.
- 20 min: trace accesses 0,1,4,0,1 through a toy direct-mapped cache with two sets and two integers per line. Label offset, set, and tag; use a diagram before terminology.
- 25 min: predict the address stride in row and column traversal, then run locality.c. Check identical checksums before discussing elapsed time.
- 10 min: break.
- 25 min: pairs compare debug and -O2 builds over repeated trials, recording variation and alternating run order. Change one traversal parameter only after the baseline.
- 15 min: separate wall time, CPU time, cache effects, compiler transformations, and workload size. Optimize the measured bottleneck; no fixed speedup is promised.
- 5 min: complete one cache access and justify which traversal has spatial locality.
- 10 min: repeat the cache trace or take final-project questions.

## Example and scope notes

Detailed SRAM circuitry, all cache write policies, full profiler setup, matrix tiling, and a catalog of optimization tricks are reading/extensions. Do not teach this module as an unsupervised “reading week.”

## Assignment planning

Collect a short cache trace and a checksum/timing observation; these cover concepts the four coding assignments do not directly assess.
