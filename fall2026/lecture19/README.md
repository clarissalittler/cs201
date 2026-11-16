# Lecture 19: Heap ownership and a memory-debugging clinic

[Agenda](Agenda.org) · [Term overview](../README.md)

Build the selected examples with `make` in this directory. `make clean` removes
its named build outputs. Additional contrast examples are not automatically included.

Run `./c_objects`. For the memory-checker lesson, remove free only in a disposable copy. Use Valgrind if available, or build the copy with `-fsanitize=address -g`; retain the original correct file.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M9 lessons 3–5.

- [Lesson 3 Dynamic Memory Allocation.html](<../../../cs201-course-site/module-09/Lesson 3 Dynamic Memory Allocation.html>)
- [Lesson 4 Debugging Memory Errors with Valgrind.html](<../../../cs201-course-site/module-09/Lesson 4 Debugging Memory Errors with Valgrind.html>)
- [Lesson 5 Garbage Collection.html](<../../../cs201-course-site/module-09/Lesson 5 Garbage Collection.html>)

Read the allocator overview and memory-error examples. Bring a small reproducer or a project resource-lifetime diagram.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: revisit lecture 1’s allocation diagram, now distinguishing virtual addresses from backing storage.
- 20 min: draw a small heap with block metadata and free blocks; contrast internal/external fragmentation without implementing an allocator.
- 25 min: run c_objects.c under a memory checker, then remove free in a disposable copy to create one deliberate leak. Read the allocation stack and repair it. Use Valgrind on the course host or AddressSanitizer here.
- 10 min: break.
- 25 min: pairs audit their project’s success/error paths for socket, file, buffer, and per-client allocation ownership. Fix one small reproducible issue.
- 15 min: compare explicit free, reference counting, and tracing GC on a two-node cycle; distinguish memory safety from resource cleanup.
- 5 min: annotate one diagnostic and identify the responsible allocation/lifetime.
- 10 min: project help or rerun the repaired example.

## Example and scope notes

Full allocator construction and collector implementations are reading/extensions. Rust ownership can be a later contrast; it is not required to understand the C project.

## Assignment planning

Final-project checkpoint: success and failure paths, binary data, and cleanup. A clean dynamic-checker run covers only executed paths.
