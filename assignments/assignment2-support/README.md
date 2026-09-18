# A small assembly debugging warmup

When an assembly program gives you the wrong answer, it helps to stop it and look at what is actually in the registers. Here's a small example you can try before working on Assignment 2.

The Assignment 2 starter ZIP includes `sum5.s`, `readInt.s`, `writeInt.s`, and a Makefile. Unzip it on the Linux server and open a terminal in the extracted directory. These are copies of the course's existing example and helpers. You can build them with `make`, or use the individual commands below.

Build the program with debugging information using the commands below. These use the shorter helper filenames above; the original comments in `sum5.s` use the helpers' paths in the course repository.

```sh
as --gdwarf-4 sum5.s -o sum5.o
as --gdwarf-4 readInt.s -o readInt.o
as --gdwarf-4 writeInt.s -o writeInt.o
ld -o sum5 sum5.o readInt.o writeInt.o
gdb ./sum5
```

At the GDB prompt, enter:

```text
set pagination off
set disassembly-flavor att
break sum_array
run
```

The first two commands keep the output from pausing between pages and use the AT&T syntax from class. If GDB asks whether to enable `debuginfod`, answer `n`; you don't need that service for this example.

The program will ask for five numbers. Type `10`, `20`, `30`, `40`, and `50`, pressing Enter after each prompt. Once it reaches `sum_array`, GDB will stop the program before that procedure's first instruction executes.

Now try:

```text
disassemble sum_array
x/5gd &nums
```

The first command shows the procedure's instructions. The second examines five eight-byte values at `nums` and displays them as signed decimal integers. You should see the numbers you entered.

Use `stepi` to execute one instruction at a time. After the three setup instructions, `%rdx` holds the array's base address, `%rcx` is zero, and `%rax` is zero. Inspect them with:

```text
info registers rax rcx rdx
x/i $pc
```

`x/i $pc` shows the next instruction to execute. Step through the comparison, the conditional jump, and the first `add`. What is `%rax` now? How does `(%rdx,%rcx,8)` identify the value that was added?

When you're ready to let the program finish:

```text
continue
quit
```

It should print `150`. If you lose your place, restart with `run` and enter the numbers again. This warmup isn't a separate submission; use it to get comfortable inspecting your own program.

For reference, the GDB manual explains [disassembly](https://sourceware.org/gdb/current/onlinedocs/gdb.html/Machine-Code.html) and [examining memory](https://sourceware.org/gdb/current/onlinedocs/gdb.html/Memory.html).
