# Peeking at the linker, one step at a time

The linker takes object files full of holes and fills them in. Almost
everything here is a before/after pair: look at something in a `.o` file,
then look at the same thing in the finished program.

Build everything with `make`. A few examples are left out of `make` on
purpose because the link error is the point. You'll type those `gcc`
commands yourself. Exact addresses and offsets will differ from machine to
machine; the shapes won't.

## The tools

| Tool | What it shows |
|---|---|
| `nm FILE` | symbols: what a file defines, and what it still needs |
| `objdump -d FILE` | disassembly; add `-r` on a `.o` to see the relocations |
| `readelf -h` / `-S` / `-l` / `-r` / `-d` | header / sections / segments / relocations / dynamic info |
| `xxd FILE` | the raw bytes |
| `ldd PROGRAM` | the shared libraries the loader would use |
| `LD_DEBUG=... PROGRAM` | the loader explaining itself while it runs |

And our own helpers:

| Script | What it does |
|---|---|
| `./pipeline.sh hello.c` | runs preprocess, compile, assemble, link one at a time and keeps every file |
| `./peek.sh FILE` | a quick tour of any ELF file, printing the command behind each part |
| `./reloc.sh main.o prog` | a function before and after linking, side by side |
| `./patch-string.sh PROG OLD NEW` | overwrites a string in a copy of a compiled program |
| `./elfpeek FILE` | our own C program that reads the ELF header, sections, and segments |

## 1. The pipeline

```bash
./pipeline.sh hello.c
```

Four commands instead of one, and every in-between file stays behind:
`hello.i` (preprocessed), `hello.s` (assembly), `hello.o` (object file), and
`hello` (the program).

Things to notice:

- `hello.i` is hundreds of lines long. That's `stdio.h` pasted in. The macro
  `GREETING` is gone, replaced by its text.
- `hello.s` calls `puts`, not `printf`. gcc noticed that `printf("%s\n", s)`
  is just `puts(s)` and swapped it, even with no optimization turned on.
- At the end, the script shows what gcc really hands the linker. We wrote one
  `.o` file. The linker also gets `Scrt1.o`, `crti.o`, `crtbeginS.o`, `-lc`,
  and more. `Scrt1.o` is where `_start` comes from.

That last point answers something from the assembly unit. When we linked
assembly with plain `ld`, *we* wrote `_start`. In C, the runtime's `_start`
sets things up and then calls our `main`.

## 2. Symbols: who defines what

`prog` is built from `main.c` and `helper.c`. Between them they have one of
every common kind of symbol.

```bash
make prog
nm main.o
nm helper.o
```

| Letter | Meaning |
|---|---|
| `T` / `t` | code (`.text`) |
| `D` / `d` | initialized data (`.data`) |
| `B` / `b` | zero-initialized data (`.bss`) |
| `R` / `r` | read-only data (`.rodata`) |
| `U` | undefined: used here, defined somewhere else |
| `W` | weak (see section 4) |

Uppercase is global (other files can see it); lowercase is local to the file.
Match every line of `nm` output to a line of C. `static` is what makes
`clamp`, `calls`, and `secret` lowercase.

Now the undefined reference from lecture 14, with `nm` alongside:

```bash
gcc main.o -o broken
```

The linker lists every `U` from `nm main.o` that nothing else defined: `bump`
twice, then `describe`, then `counter`. Each error has a location like
`main.c:(.text+0x11)`. Keep those offsets in mind for the next step. (The
warnings about "read-only section" and "DT_TEXTREL" are side effects of
`counter` being missing. Fix the undefined references and they go away.)

## 3. Relocations: the holes

```bash
objdump -dr main.o
```

Every call to a function in another file looks like `e8 00 00 00 00`: a call
instruction with a zero offset. Under each one is a relocation note like
`11: R_X86_64_PLT32 bump-0x4`. It says: at byte 0x11 of `.text`, put the
distance to `bump`. Compare the offsets with the errors from step 2: 0x11,
0x24, 0x37, 0x46. The linker was telling us exactly which holes it couldn't
fill.

Now compare with the linked program:

```bash
./reloc.sh main.o prog
```

The zeros are filled in. Work one out by hand. For a line like

```
1159:   e8 6c 00 00 00    call   11ca <bump>
```

the offset is `0x0000006c` (little-endian!), and it's measured from the *next*
instruction: `0x1159 + 5 + 0x6c = 0x11ca`. That's `bump`.

Global variables get the same treatment: `mov 0x0(%rip),%eax` with an
`R_X86_64_PC32 counter-0x4` note becomes `mov 0x2e85(%rip),%eax  # 4018 <counter>`.

One odd one: the relocation for `secret` says `.data`, not `secret`. A
`static` variable is never looked up by name from another file, so the
compiler just says "this spot in my own `.data` section."

`./peek.sh main.o` shows all the relocations at once (`readelf -r`).

## 4. When resolution goes wrong

Each of these is a pair of files that compile fine separately. The trouble
only shows up when they're linked.

**Two definitions of one name.**

```bash
gcc dup_a.c dup_b.c -o dup
```

`multiple definition of 'shared'`. Make `dup_b.c`'s copy `static` and try again:

```bash
sed 's/^int shared/static int shared/' dup_b.c > dup_b_static.c
gcc dup_a.c dup_b_static.c -o dup
./dup
nm dup | grep shared
```

Two `shared`s now, one `D` and one `d`. Each file gets its own.

**Two definitions with different types.**

```bash
gcc mismatch_main.c mismatch_other.c -o mismatch
```

Modern gcc refuses. Older gcc (before version 10) used a setting called
"common symbols" that let this through. We can turn it back on:

```bash
gcc -fcommon mismatch_main.c mismatch_other.c -o mismatch
./mismatch
nm -S mismatch | grep -w x
```

No warning. The int and the double got merged into one 8-byte `x`, and
`mismatch_main.c` reads the first 4 bytes of 3.14 as an int. The fix is a
shared header with `extern int x;` so the compiler can see both declarations.

**Weak symbols: a default that can be replaced.**

```bash
make weak-default weak-override
./weak-default
./weak-override
nm weak_main.o | grep greeting
```

`W` means weak. With no other definition, the weak one is used. A normal
definition beats it without any "multiple definition" error.

**Static libraries, and why order matters.**

```bash
make libhelper.a
ar t libhelper.a                       what's in the archive?
gcc main.o -L. -lhelper -o prog-lib    works
gcc -L. -lhelper main.o -o prog-lib    undefined reference to `bump'
```

The linker reads its arguments left to right. When it reaches a library, it
only pulls out members that define something it *currently* needs. In the
second command, nothing is needed yet when it sees `-lhelper`, so it skips
the whole library. Then `main.o` shows up needing `bump`, and it's too late.

## 5. The ELF format itself

Programs, object files, and shared libraries are all ELF files. The first 64
bytes are the header:

```bash
xxd -l 64 hello
```

| Bytes | Field | In `hello` |
|---|---|---|
| 0–3 | magic | `7f 45 4c 46` = `\x7f` `E` `L` `F` |
| 4 | class | `02` = 64-bit |
| 5 | byte order | `01` = little-endian |
| 16–17 | type | `03 00` = 3 = DYN |
| 18–19 | machine | `3e 00` = 62 = x86-64 |
| 24–31 | entry point | `60 10 00 ...` = 0x1060 (that's `_start`) |
| 32–39 | where the segment table starts | `40 00 ...` = 64, right after the header |
| 40–47 | where the section table starts | |
| 56–57 | number of segments | |
| 60–61 | number of sections | |

Decode the last three yourself, then check with `readelf -h hello`.

Why DYN and not EXEC? gcc builds *position-independent executables* by
default, so the program can be loaded at a different address every run
(ASLR, from lecture 14). To the loader, that looks a lot like a shared library.

Now read it ourselves:

```bash
./elfpeek hello
./elfpeek main.o
```

`elfpeek.c` reads the whole file into memory and points `<elf.h>` structs at
the right offsets. It's the struct-and-bytes reasoning from lecture 1 applied
to a real file format.

**Sections vs. segments.** Sections (`.text`, `.data`, `.rodata`, `.bss`, ...)
are what the linker works with. Segments are what the loader maps into
memory, and they're the ones with permissions:

```bash
./peek.sh hello
```

Look at "Which sections go in which segment". `.text` lands in the `R E`
segment. `.data` and `.bss` land in the `RW` segment. `.rodata` lands in a
read-only one. These are the page permissions from lecture 14. A `.o` file
has sections but no segments, since nobody loads a `.o`.

Find `.bss` in the `RW` segment: `MemSiz` is bigger than `FileSiz`. The loader
zero-fills the difference, so `.bss` costs nothing in the file.

## 6. Poking at a program

**Change a string.**

```bash
./patch-string.sh hello "pipeline" "linker!!"
./hello-patched
```

The script finds the bytes with `grep -ob`, says which section they're in
(`.rodata`), and overwrites them in a copy. The new string can't be longer
than the old one, because nothing moves to make room. A shorter one is padded
with zero bytes, which ends the C string early:

```bash
./patch-string.sh hello "pipeline!" "ELF"
./hello-patched
```

**Strip the symbols.**

```bash
cp prog prog-stripped
strip prog-stripped
nm prog-stripped
./prog-stripped
ls -l prog prog-stripped
```

No symbols, smaller file, runs the same. Symbol names are for the linker and
the debugger, not the CPU. (`nm -D prog-stripped` still shows the *dynamic*
symbols. Why can't `strip` remove those?)

## 7. Dynamic linking

**Linking and loading are two different steps.**

```bash
make libgreet.so usegreet
./usegreet
```

It linked fine, but it won't run: `cannot open shared object file`. The
linker checked that `greet` exists and wrote down "NEEDS libgreet.so". The
*loader* has to find it every time the program starts, and it doesn't look in
the current directory.

```bash
readelf -d usegreet | grep NEEDED
LD_LIBRARY_PATH=. ./usegreet
LD_LIBRARY_PATH=. ldd ./usegreet
./usegreet-rpath
readelf -d usegreet-rpath | grep RUNPATH
```

`usegreet-rpath` carries its own hint: `$ORIGIN`, "the directory I'm in."

**Change the library without relinking the program.** Edit the message in
`greet.c`, then:

```bash
make libgreet.so
./usegreet-rpath
```

New message. `usegreet-rpath` was never rebuilt. This is how a security fix to
`libc.so.6` reaches every program on the machine at once.

**Get in the middle with LD_PRELOAD.**

```bash
./dice
LD_PRELOAD=./fakerand.so ./dice
LD_DEBUG=bindings ./dice 2>&1 | grep -w rand
LD_DEBUG=bindings LD_PRELOAD=./fakerand.so ./dice 2>&1 | grep -w rand
```

The loader looks in preloaded libraries first, so `fakerand.so`'s `rand` wins
over libc's. `LD_DEBUG` shows the loader binding `rand` to one or the other.
(`LD_DEBUG=help ./dice` lists everything else it can report.)

**How does the call get there?**

```bash
objdump -d --disassemble=main dice
```

The call goes to `rand@plt`, a small stub the linker added in `.plt.sec`. The
stub jumps through a pointer in `.got` that the loader fills in. On our
machines programs are linked with `BIND_NOW` (`readelf -d dice | grep FLAGS`),
so the loader fills every pointer before `main` starts. The lazy,
first-call-only version is in the reading.

**Where did it all end up?**

```bash
./whereami
./whereami
```

The program's own memory map. Find `main` (an `r-xp` region of our file), the
global (`rw-p`), `printf` (libc), the heap, the stack, and
`ld-linux-x86-64.so.2`, the loader itself. Run it twice: every address
changes, but the layout doesn't.

**Static linking: copy everything in.**

```bash
make hello-static
ls -l hello hello-static
ldd hello-static
./peek.sh hello-static
```

Around 50 times bigger, with no INTERP segment, no NEEDED libraries, and
nothing for `LD_PRELOAD` to get in between. (This needs the static C library installed. If `make hello-static`
can't find `-lc`, skip it.)

## Things to try

- Run `./peek.sh` on your final-project server. What does it need from libc?
  Which `.o` files did it come from?
- Add symbols to `elfpeek.c`: find the section with type `SHT_SYMTAB`, and
  use its `sh_link` to find the string table for the names. You've just
  written `nm`.
- Write a `fakerand.c` whose `rand()` counts how many times it's called and
  prints the count when the program exits. (Hint: `__attribute__((destructor))`.)
- Patch a number instead of a string. `limits` in `helper.c` is in `.rodata`:
  find its offset with `nm prog` and `readelf -S prog`, then change the 10.
- `gcc -Wl,--verbose hello.c` prints the default *linker script*: the rules
  for which sections go where. Find `.rodata` in it.
