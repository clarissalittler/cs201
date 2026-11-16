# Lecture 15: File descriptors: everything is a file

[Agenda](Agenda.org) · [Examples guide](FileExamples.org) · [Term overview](../README.md)

Build everything with `make` in this directory. `make clean` removes the
programs and the files they create (`out.txt`, `err.txt`, `listing.txt`).
[FileExamples.org](FileExamples.org) has a table of the main examples with a
command to try for each, and each source file has a `try:` block at the top.

Run `./fdCat FILE > NEW_OUTPUT`, then `cmp FILE NEW_OUTPUT`. Try a binary file,
`/proc/cpuinfo`, `/dev/null`, a directory, and a missing path. Always redirect
to a *new* file: redirecting onto the input makes the shell truncate it before
the program starts.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M7 lesson 1.

- [Lesson 1 File Descriptors and Low-Level IO.html](<../../../cs201-course-site/module-07/Lesson 1 File Descriptors and Low-Level IO.html>)

Read the descriptor lifecycle. Bring a text file and a small binary file to
copy; review argc/argv if needed (`argv.c`). Recall the semaphores shared between
forked processes for the `namedSem.c` demo.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: connect read/write to the assembly byte-count examples. Run `fdStandard` with `>`, `2>`, and a pipe: the shell decides what 0, 1, and 2 are.
- 20 min: run `fdCat` before walking through open/read/write/close. Distinguish EOF, error, and a short successful transfer; read's result is a byte count, not a string length.
- 25 min: trace the outer read loop and `writeAll` with a pretend short write of 3. Check the pointer and remaining count before the next call. Then run the same loop on `/proc`, `/dev/null`, a directory, and a missing file.
- 10 min: break.
- 25 min: `fdDevices` and `fdTable` with `/proc/self/fd`; `openfile2` shows which number the next open gets. `fdPipe` (EOF only when every write end is closed) and `fdRedirect` (`dup2` then `exec`, the way the shell does `>`).
- 15 min: `namedSem`: create it, `ls -l /dev/shm`, post and peek at its bytes, then wait in one terminal and post in another. Remove is `sem_unlink`, just like deleting a file.
- 5 min: state why read's result is not a string length and why one write need not finish.
- 10 min: `directory1` and `lstatTest` if there's time; otherwise repair copy-loop reasoning.

## Example and scope notes

`openfile1.c`, `readfile1.c`, and `writefile1.c` are smaller steps toward
`fdCat.c`, which is the complete byte-copy model. Do not present `writefile2.c`
as the final robust write loop. The FILE API is next lecture; FIFOs, domain
sockets, and shared memory remain optional clinics.

## Assignment planning

Preview the final-project specification and provided scaffold now. Students may
start preparing www files; sockets and HTTP are taught in lecture 17.
