# Assembly Quick-Start Guide (30 Minutes)

Welcome to x86-64 assembly programming! This guide will get you writing, compiling, and running your first assembly program in about 30 minutes.

## What You'll Learn

- How to write a simple assembly program
- How to assemble and link your code
- How to run and verify your program works
- Where to go next in your learning journey

## Prerequisites

- A Linux system (or WSL on Windows)
- The GNU assembler (`as`) and linker (`ld`) installed
- Basic understanding of programming concepts
- A text editor

## Your First Assembly Program (5 minutes)

Let's start with the classic "Hello, World!" program. Create a file called `hello.s`:

```assembly
.section .data
msg:    .asciz "Hello, World!\n"
msglen = . - msg

.section .text
.global _start

_start:
    # Write "Hello, World!" to stdout
    mov $1, %rax        # syscall number: write
    mov $1, %rdi        # file descriptor: stdout
    lea msg(%rip), %rsi # address of message
    mov $msglen, %rdx   # length of message
    syscall

    # Exit with status 0
    mov $60, %rax       # syscall number: exit
    xor %rdi, %rdi      # exit status: 0
    syscall
```

## Understanding the Code (5 minutes)

Let's break down what each part does:

**`.section .data`** - Declares the data section where we store constants and variables
- `msg:` - A label marking the start of our string
- `.asciz "Hello, World!\n"` - Defines a null-terminated string
- `msglen = . - msg` - Calculates the string length automatically

**`.section .text`** - Declares the code section where instructions live

**`.global _start`** - Makes `_start` visible to the linker (required!)

**`_start:`** - The entry point where your program begins

**First syscall (write)**:
- `mov $1, %rax` - Load syscall number 1 (write) into %rax
- `mov $1, %rdi` - First argument: file descriptor 1 (stdout)
- `lea msg(%rip), %rsi` - Second argument: address of our message
- `mov $msglen, %rdx` - Third argument: number of bytes to write
- `syscall` - Invoke the kernel to perform the write

**Second syscall (exit)**:
- `mov $60, %rax` - Load syscall number 60 (exit) into %rax
- `xor %rdi, %rdi` - First argument: exit status 0 (success)
- `syscall` - Invoke the kernel to exit the program

## Building Your Program (5 minutes)

Assembly programs require two steps: **assembling** and **linking**.

### Step 1: Assemble
Turn your assembly code into an object file:
```bash
as hello.s -o hello.o
```

What this does:
- `as` - The GNU assembler
- `hello.s` - Your source file
- `-o hello.o` - Output object file

### Step 2: Link
Turn the object file into an executable:
```bash
ld hello.o -o hello
```

What this does:
- `ld` - The linker
- `hello.o` - Your object file
- `-o hello` - Output executable

### Step 3: Run!
```bash
./hello
```

**Expected output:**
```
Hello, World!
```

### Verify Exit Status
```bash
echo $?
```

**Expected output:**
```
0
```

## Common Errors and Solutions (5 minutes)

### Error: "undefined reference to `_start`"
**Problem:** Forgot `.global _start`

**Solution:** Add this line before your `_start:` label

### Error: "Segmentation fault"
**Problem:** Forgot to exit properly

**Solution:** Always end with the exit syscall (mov $60, %rax; syscall)

### Error: "Bad immediate expression"
**Problem:** Syntax error, possibly missing `$` before a number

**Solution:** Constants need `$` prefix: `mov $1, %rax` not `mov 1, %rax`

### Error: Nothing prints
**Problem:** Wrong file descriptor or wrong syscall

**Solution:**
- Use `%rdi = 1` for stdout (not 0 or 2)
- Use `%rax = 1` for write syscall

## AT&T Syntax Quick Reference (5 minutes)

x86-64 assembly can be written in two syntaxes. We use **AT&T syntax** (used by GNU tools).

### Key Syntax Rules

1. **Source comes before destination**
   ```assembly
   mov $5, %rax    # Move 5 INTO %rax (destination)
   ```

2. **Constants need `$` prefix**
   ```assembly
   mov $10, %rax   # Immediate value 10
   ```

3. **Registers need `%` prefix**
   ```assembly
   mov %rax, %rbx  # Copy %rax to %rbx
   ```

4. **Memory dereferencing uses `()`**
   ```assembly
   mov (%rax), %rbx    # Load value AT address in %rax
   lea msg(%rip), %rax # Load ADDRESS of msg
   ```

5. **Comments use `#`**
   ```assembly
   mov $1, %rax    # This is a comment
   ```

## Essential Registers (5 minutes)

You have 16 general-purpose 64-bit registers:

**Special-purpose:**
- `%rax` - Accumulator (syscall numbers, return values)
- `%rdi` - First function/syscall argument
- `%rsi` - Second function/syscall argument
- `%rdx` - Third function/syscall argument
- `%rsp` - Stack pointer (don't modify directly!)
- `%rbp` - Base pointer (for stack frames)
- `%rip` - Instruction pointer (use for position-independent code)

**General-purpose:**
- `%rbx`, `%rcx` - General use
- `%r8` through `%r15` - Additional general registers

## System Calls You'll Use Often

System calls let you interact with the OS. Put the syscall number in `%rax`:

| Syscall | Number | Arguments (%rdi, %rsi, %rdx) | Purpose |
|---------|--------|------------------------------|---------|
| read    | 0      | fd, buffer, count            | Read from file/stdin |
| write   | 1      | fd, buffer, count            | Write to file/stdout |
| exit    | 60     | status                       | Exit program |

**File descriptors:**
- 0 = stdin (standard input)
- 1 = stdout (standard output)
- 2 = stderr (standard error)

## Your Second Program: Echo (5 minutes)

Now try a program that reads input and echoes it back. Create `echo.s`:

```assembly
.section .bss
buffer: .skip 128       # Allocate 128 bytes for input

.section .text
.global _start

_start:
    # Read from stdin
    mov $0, %rax            # syscall: read
    mov $0, %rdi            # fd: stdin
    lea buffer(%rip), %rsi  # buffer to store input
    mov $128, %rdx          # max bytes to read
    syscall

    # Write to stdout (rax has bytes read)
    mov %rax, %rdx          # bytes to write
    mov $1, %rax            # syscall: write
    mov $1, %rdi            # fd: stdout
    lea buffer(%rip), %rsi  # buffer to write from
    syscall

    # Exit
    mov $60, %rax
    xor %rdi, %rdi
    syscall
```

**Build and test:**
```bash
as echo.s -o echo.o
ld echo.o -o echo
./echo
```

Type something and press Enter. Your input should be echoed back!

## Next Steps

Now that you've written your first programs, here's your learning path:

### 1. Work Through the Examples (in order)

**Start here:**
- `01-basics/` - Master basic program structure
  - Start with `01-hello-world.s` and its commented version
  - Try `02-simple.s`, `03-simplest.s`, `04-simpler.s`

**Then:**
- `02-data/` - Learn data manipulation
  - Arrays, data types, constants

**Next:**
- `03-control-flow/` - Conditionals and loops
  - for loops, while loops, comparisons

**Continue with:**
- `04-functions/` - Function calls and stack
  - Basic functions, parameters, recursion

**Then:**
- `05-io/` - Input/output operations
  - Reading integers, writing integers

**Finally:**
- `06-advanced/` - Advanced topics
  - AVX-512, floating-point, complex operations

### 2. Study the Commented Versions

Each example has a "commented" version (e.g., `01-hello-worldcommented.s`) with:
- Detailed line-by-line explanations
- Why things work the way they do
- Common pitfalls to avoid
- Debugging tips

### 3. Reference Materials

- **`assemblyGuide.org`** (in repository root) - Comprehensive reference
  - Register usage conventions
  - Calling conventions
  - Memory addressing modes
  - More example programs

- **`README.md`** (in this directory) - Complete example index

### 4. Debugging Tools

**Use `strace` to see system calls:**
```bash
strace ./hello
```
Shows: `write(1, "Hello, World!\n", 14) = 14`

**Use `gdb` to step through code:**
```bash
gdb ./hello
(gdb) break _start
(gdb) run
(gdb) stepi         # Step one instruction
(gdb) info registers # View register values
```

**Check exit codes:**
```bash
./program
echo $?  # Shows the exit status
```

## Practice Exercises

Try these to cement your understanding:

1. **Modify Hello World**
   - Change the message to your name
   - Make it print twice
   - Try writing to stderr (fd=2) instead

2. **Exit Codes**
   - Write a program that exits with status 42
   - Verify with `echo $?`

3. **Simple Math**
   - Write a program that adds 5 + 7
   - Exit with the sum as the status code
   - Verify: `echo $?` should print `12`

4. **Multiple Syscalls**
   - Print "Hello"
   - Then print "World" on a new line
   - Use two separate write syscalls

## Getting Help

**Stuck? Check these resources:**

1. **Commented examples** - Most questions are answered in the detailed versions
2. **assemblyGuide.org** - Comprehensive reference with more examples
3. **README.md** - Index of all examples by topic
4. **Error messages** - Read them carefully, they usually tell you exactly what's wrong

## Key Takeaways

After 30 minutes, you should know:

✅ Assembly programs have `.data` (constants/variables) and `.text` (code) sections

✅ Programs start at `_start` (must be `.global`)

✅ System calls use `syscall` instruction with number in `%rax`

✅ Arguments go in: `%rdi`, `%rsi`, `%rdx`, `%rcx`, `%r8`, `%r9`

✅ Build process: `as` (assemble) → `ld` (link) → executable

✅ AT&T syntax: source before destination, `$` for constants, `%` for registers

✅ Always exit cleanly with exit syscall

## What's Next?

You're ready to dive deeper! Start with `01-basics/01-hello-world.s` and its commented version, then work through the examples in order. Each one builds on the previous concepts.

**Happy assembling!** 🚀

---

**Quick Commands Reference:**
```bash
# Assemble
as program.s -o program.o

# Link
ld program.o -o program

# Run
./program

# Check exit status
echo $?

# Debug
gdb ./program
strace ./program
```
