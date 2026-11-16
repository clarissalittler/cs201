# Lecture 18: The linker and the ELF format

[Agenda](Agenda.org) · [Linker walkthrough](LinkerSteps.md) · [Term overview](../README.md)

Build everything with `make` in this directory; `make clean` removes the
programs, libraries, and the files the scripts and exercises create.
[LinkerSteps.md](LinkerSteps.md) has every command in teaching order. The
`dup_*.c` and `mismatch_*.c` pairs aren't in `make` on purpose: students type
those `gcc` commands to see the link errors. `make hello-static` is separate
because it needs the static C library (`libc.a`).

The examples are built without `-g` so that `readelf -S` isn't full of
`.debug_*` sections. Exact addresses and offsets vary by machine.

Agenda.org is the outline to teach from; preparation and pacing notes follow below.

## Examples

| File | What it shows |
|---|---|
| `hello.c` | the whole build pipeline; target for `xxd`, `elfpeek`, and string patching |
| `main.c`, `helper.c`, `helper.h` | every symbol kind (`T t D d B b R U`); relocations; `prog` and `prog-lib` |
| `dup_a.c`, `dup_b.c` | multiple definition, then `static` as the fix |
| `mismatch_main.c`, `mismatch_other.c` | `int x` vs `double x`: an error by default, silent corruption with `-fcommon` |
| `weak_main.c`, `weak_override.c` | a weak default replaced by a strong definition |
| `elfpeek.c` | reads the ELF header, sections, and segments with `<elf.h>` structs |
| `greet.c`, `usegreet.c` | `libgreet.so`; linking vs. loading; `LD_LIBRARY_PATH`, `$ORIGIN` |
| `dice.c`, `fakerand.c` | `LD_PRELOAD` interposition, `LD_DEBUG=bindings`, `rand@plt` |
| `whereami.c` | addresses plus `/proc/self/maps`; ASLR across runs |

| Script | What it does |
|---|---|
| `pipeline.sh FILE.c` | preprocess, compile, assemble, link one at a time; shows the crt files gcc adds |
| `peek.sh FILE` | header, sections, segments with section mapping, symbols, relocations or NEEDED |
| `reloc.sh OBJ PROG [FUNC]` | a function before and after linking, then just the calls |
| `patch-string.sh PROG OLD NEW` | overwrites a string in `PROG-patched`; refuses if NEW is longer or OLD isn't unique |

## Preparation

M9 lesson 2.

- [Lesson 2 Static and Dynamic Linking.html](<../../../cs201-course-site/module-09/Lesson 2 Static and Dynamic Linking.html>)

Read the symbol resolution and relocation sections. Reopen the lecture 14
undefined-reference demo and the multi-object assembly build from lecture 7.
Have the term's first struct/byte-layout examples handy for the ELF header.

## Pacing notes

These are approximate allowances for the two-hour class, including the break
and time to catch up. The agenda follows the topic flow rather than a stopwatch.

- 10 min: `pipeline.sh hello.c`. Point out `puts` in `hello.s` and the crt files in the link line; connect `_start` to the assembly unit.
- 20 min: `nm` on `main.o` and `helper.o`; match each letter to a line of C. Rerun the lecture 14 undefined reference and read every `U` it reports.
- 25 min: `objdump -dr main.o`, then `reloc.sh main.o prog`. Match relocation offsets to the error offsets, and compute one call target by hand.
- 10 min: break.
- 20 min: pairs work through the resolution puzzles: `dup`, the `static` fix, `-fcommon` mismatch, weak symbols, library order.
- 20 min: decode the ELF header with `xxd`, run `elfpeek`, then `peek.sh hello` for sections vs. segments and `.bss`. Patch a string and strip a copy.
- 10 min: `usegreet` failing to load, `LD_LIBRARY_PATH`/`$ORIGIN`, swapping the library, and `LD_PRELOAD` on `dice`.
- 5 min: exit check: connect one `U` to its relocation, or name something the project server needs from libc.

## Example and scope notes

This is more material than fits; `whereami`, `hello-static`, and the "Things
to try" list in LinkerSteps.md are the first things to drop or leave for
after class. Lazy binding and instruction-level PLT/GOT tracing are reading.
Ubuntu links with `BIND_NOW`, so `LD_DEBUG=bindings` shows everything binding
at startup. Linker scripts, relocation-type details beyond PC32/PLT32, and
symbol versioning are out of scope.

`-fcommon` exists only to show the old behavior. Don't present it as an
option to use.

## Assignment planning

Final-project work continues outside class. `peek.sh` on the student's own
server is a low-stakes way to connect today to the project.
