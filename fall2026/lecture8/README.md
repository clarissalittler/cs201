# Lecture 8: Exceptions and process lifecycles

[Agenda](Agenda.org) · [Term overview](../README.md)

Build the selected examples with `make` in this directory. `make clean` removes
its named build outputs. Additional contrast examples are not automatically included.

Run `./fork_lifecycle`: parent value remains 10 and child status is 0. Run `./fork_lifecycle fail`: the intentional exec error produces child status 127 and a nonzero program exit.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M5 lessons 1–7.

- [Lesson 1 Flow Control.html](<../../../cs201-course-site/module-05/Lesson 1 Flow Control.html>)
- [Lesson 2 Exceptions.html](<../../../cs201-course-site/module-05/Lesson 2 Exceptions.html>)
- [Lesson 3 Classes of Exceptions.html](<../../../cs201-course-site/module-05/Lesson 3 Classes of Exceptions.html>)
- [Lesson 4 Exceptions in Linux x86-64 Systems.html](<../../../cs201-course-site/module-05/Lesson 4 Exceptions in Linux x86-64 Systems.html>)
- [Lesson 5 Processes.html](<../../../cs201-course-site/module-05/Lesson 5 Processes.html>)
- [Lesson 6 Process Control.html](<../../../cs201-course-site/module-05/Lesson 6 Process Control.html>)
- [Lesson 7 Examples of Using Fork.html](<../../../cs201-course-site/module-05/Lesson 7 Examples of Using Fork.html>)

Read the short exception taxonomy and process-control introduction. Predict the two return values from a successful fork.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: connect the assembly syscall to a user/kernel transition; a mode switch need not switch processes.
- 20 min: draw normal flow, interrupt/trap/fault/abort, then a process with registers, address space, and file descriptors. Use the site’s taxonomy without running deliberate C undefined behavior as a hardware guarantee.
- 25 min: predict fork_lifecycle.c before running; draw the two paths after fork and distinguish exec from creating another process.
- 10 min: break.
- 25 min: run the deliberate exec-failure branch; pairs label child/parent/error paths and the status reported by waitpid.
- 15 min: explain why the parent still has value 10, why the child must be reaped, and why output buffering can confuse older fork sketches.
- 5 min: a two-branch execution trace showing fork, exec, and wait.
- 10 min: repeat the lifecycle; do not introduce pthreads yet.

## Example and scope notes

fork1.c/fork3.c are minimal contrast sketches lacking full lifecycle/error handling. Process trees with many forks, Linux task_struct internals, and pipes are follow-up material.

## Assignment planning

Assignment 2 clinic questions remain welcome; no new major assignment this lecture.
