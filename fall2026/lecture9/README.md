# Lecture 9: Signals and waiting for an event

[Agenda](Agenda.org) · [Term overview](../README.md)

Build the selected examples with `make` in this directory. `make clean` removes
its named build outputs. Additional contrast examples are not automatically included.

Run `./signal_wait`, then execute its printed `kill -USR1 PID` command in another terminal. Main notices the handler’s flag and exits.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M5 lessons 8–12.

- [Lesson 8 Signals in Linux Systems.html](<../../../cs201-course-site/module-05/Lesson 8 Signals in Linux Systems.html>)
- [Lesson 9 Signal Management.html](<../../../cs201-course-site/module-05/Lesson 9 Signal Management.html>)
- [Lesson 10 Blocking and Unblocking Signals.html](<../../../cs201-course-site/module-05/Lesson 10 Blocking and Unblocking Signals.html>)
- [Lesson 11 Writing Signal Handlers.html](<../../../cs201-course-site/module-05/Lesson 11 Writing Signal Handlers.html>)
- [Lesson 12 Signal Handler Examples.html](<../../../cs201-course-site/module-05/Lesson 12 Signal Handler Examples.html>)

Read signal delivery, masks, and handler rules. Identify which work should occur in a handler and which in main.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: process identity, execution state, and default signal actions.
- 20 min: a signal is a notification, not a queued payload. Separate disposition, pending state, and blocked mask.
- 25 min: run signal_wait.c from two terminals. Read the small handler first: set a sig_atomic_t flag, then do the printing in main.
- 10 min: break.
- 25 min: draw the race in while(!flag) pause(), then trace the supplied blocked-mask/sigsuspend solution. Pairs explain a signal arriving before the wait.
- 15 min: compare the handler shape with sigModernWay.c and explain why the latter’s check-then-pause loop is only a contrast example. Discuss coalescing and SIGKILL/SIGSTOP.
- 5 min: identify the lost-wakeup window in a short timeline and state one handler restriction.
- 10 min: redo a delivery timeline or take assembly questions.

## Example and scope notes

Signal duels, catching SIGSEGV/division faults, and a catalog of signal APIs are optional. Do not promise recovery by returning from a handler for a faulting instruction.

## Assignment planning

Assignment 2: debug the last missing input/output or call path before lecture 10.
