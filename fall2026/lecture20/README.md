# Lecture 20: Kernel organization: follow one request through the system

[Agenda](Agenda.org) · [Term overview](../README.md)

Use the server project and the course-site readings linked in Agenda.org. Today’s core example is a request traced through different kernel service boundaries.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M10 lessons 1–4.

- [Lesson 1 What a Kernel Does.html](<../../../cs201-course-site/module-10/Lesson 1 What a Kernel Does.html>)
- [Lesson 2 Monolithic Kernels.html](<../../../cs201-course-site/module-10/Lesson 2 Monolithic Kernels.html>)
- [Lesson 3 Microkernel Architectures.html](<../../../cs201-course-site/module-10/Lesson 3 Microkernel Architectures.html>)
- [Lesson 4 Tradeoffs and Modern Approaches.html](<../../../cs201-course-site/module-10/Lesson 4 Tradeoffs and Modern Approaches.html>)

Read the four short kernel lessons. Draw the layers involved when a browser requests a file from your server.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: list the syscalls, memory objects, and concurrent participants in a request.
- 20 min: trace one file request through user code, system calls, filesystem/network services, scheduling, and address translation.
- 25 min: redraw the same service with monolithic and microkernel boundaries. Compare which services share privileged memory and which communicate by messages.
- 10 min: break.
- 25 min: pairs choose one failure (driver crash, bad pointer, stuck worker) and explain which boundaries contain it in each design. Supply the service layout; avoid a kernel-history catalog.
- 15 min: discuss performance, isolation, and verification tradeoffs, then answer current final-project questions.
- 5 min: explain one architectural tradeoff using the request traced today.
- 10 min: project help or a modern-design question grounded in the course reading.

## Example and scope notes

Rust’s ADTs/lists and the historical kernel reading list remain extensions. Kernel organization has an actual required lecture rather than a link at the end of Rust.

## Assignment planning

For the final-project checkpoint, show two clients connected at once and getting answers, then disconnect one and show the other still works.

This is the last numbered lecture, but the term has a couple of sessions after it. The [project workshop](../optional/project-workshop/Agenda.org) is the plan for those, and final-project completion can come after them. Set actual dates and any exam arrangements only against the confirmed section calendar.
