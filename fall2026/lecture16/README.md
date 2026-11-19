# Lecture 16: The FILE API and buffering

[Agenda](Agenda.org) · [Examples guide](FileExamples.org) · [Term overview](../README.md)

Build everything with `make` in this directory. `make clean` removes the
programs and the files they create (`scores.txt`, `points.bin`, `wrapped.txt`).
[FileExamples.org](FileExamples.org) has a table of the stdio examples with a
command to try for each, and each source file has a `try:` block at the top.

Run `./stdioBuffering` to the terminal, then pipe it into `cat` and compare the
output order. Run `./stdioBinary` and look at `points.bin` with `xxd`.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Preparation

M7 lesson 2.

- [Lesson 2 The FILE API and Directories.html](<../../../cs201-course-site/module-07/Lesson 2 The FILE API and Directories.html>)

Read the FILE comparison with descriptors and the buffering modes. Reopen
`fdCat.c` from lecture 15 for comparison.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: list what we did by hand with descriptors (counting bytes, short-write loops, turning numbers into text). A `FILE*` is a descriptor plus a buffer plus conveniences.
- 20 min: `openfileagain` for the shape, then `stdioOpen` (modes, `NULL`, `perror`) and `stdioLines`; change `LINE_SIZE` to 16 and count the calls.
- 25 min: `weirdcopy` copies line by line with `fgets`/`fputs`. What's missing compared to `fdCat.c`? Then `stdioChars` (why `c` is an `int`) and `stdioFormatted`.
- 10 min: break.
- 25 min: `stdioBinary` with `xxd`, `stdioSeek` (why can't a pipe seek?), and `stdioBuffering` to a terminal vs. a pipe. Line, full, and no buffering; `fflush`; what a crash does to the buffer.
- 15 min: two layers, one file: `stdioFileno` (`fileno`, `fdopen`) and `procSelf` reading `/proc/self/status` with `fgets`.
- 5 min: explain why the output order changed when piped, or why `fgetc`'s result needs an `int`.
- 10 min: look ahead at the final project: a multi-client server where each client is just another descriptor. Collect ideas (chat, games, a tiny MUD). Offer `lineEditor.c` to anyone who wants a larger file example.

## Example and scope notes

`weirdcopy.c` deliberately skips checking whether the input opened; use that as
a discussion point, not a model. `lineEditor.c` is an optional larger example.
Directory traversal beyond lecture 15's `directory1.c` is reading.

## Assignment planning

Students can pick an idea and sketch the commands a client would type.
Sockets come next lecture, where final-project implementation begins.
