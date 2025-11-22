# Lecture 5 - System Calls and I/O in Assembly

This directory contains x86-64 assembly examples for Winter 2025 Lecture 5, demonstrating system calls for input/output and basic string operations.

## Contents

### Basic I/O Programs
- **hello.s** - Hello World using write system call
- **hellocommented.s** - Detailed explanation of write syscall
- **echo.s** - Echo program using read and write syscalls
- **echocommented.s** - Complete walkthrough of I/O operations
- **echoBroken.s** - Intentionally broken echo (for debugging practice)
- **echoBrokencommented.s** - Explanation of common errors

### Control Flow
- **labelTest.s** - Label usage and control flow
- **labelTestcommented.s** - Detailed explanation of labels and jumps

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make hello
make echo
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Hello World (System Call)
```bash
./hello
```
**Expected output:**
```
Hello, World!
```

### Echo Program
```bash
./echo
```
**Expected behavior:**
- Type text and press Enter
- Program reads input and echoes it back
- Note: This is raw byte I/O, not null-terminated strings
- The number of bytes read is returned by the read syscall

### Label Test
```bash
./labelTest
echo $?
```
**Expected behavior:**
Demonstrates control flow with labels and jumps.

## Key Concepts Demonstrated

### Linux System Calls

System calls are invoked with the `syscall` instruction after setting up:
1. System call number in %rax
2. Arguments in registers (in order): %rdi, %rsi, %rdx, %rcx, %r8, %r9

#### Important System Calls
- **read (0)**
  - %rdi: file descriptor (0 = stdin)
  - %rsi: buffer address
  - %rdx: maximum bytes to read
  - Returns: number of bytes read in %rax

- **write (1)**
  - %rdi: file descriptor (1 = stdout, 2 = stderr)
  - %rsi: buffer address
  - %rdx: number of bytes to write
  - Returns: number of bytes written in %rax

- **exit (60)**
  - %rdi: exit code
  - Does not return

### Memory Sections

#### .rodata (Read-Only Data)
```assembly
.section .rodata
message:
    .asciz "Hello, World!\n"
message_len = . - message
```

#### .bss (Uninitialized Data)
```assembly
.section .bss
buffer:
    .space 256    # Reserve 256 bytes
```

#### .data (Initialized Data)
```assembly
.section .data
value:
    .quad 42
```

### Register Conventions

#### Argument Passing (x86-64 calling convention)
1. %rdi - First argument
2. %rsi - Second argument
3. %rdx - Third argument
4. %rcx - Fourth argument
5. %r8 - Fifth argument
6. %r9 - Sixth argument

#### Special Registers
- **%rax**: Return value, syscall number
- **%rsp**: Stack pointer (must be 16-byte aligned before calls)
- **%rbp**: Base pointer (frame pointer)

#### Caller-Saved Registers
Caller must save before function calls:
%rax, %rcx, %rdx, %rsi, %rdi, %r8, %r9, %r10, %r11

#### Callee-Saved Registers
Callee must preserve:
%rbx, %rbp, %r12, %r13, %r14, %r15

### I/O Operations

#### Writing to stdout
```assembly
movq $1, %rax           # syscall: write
movq $1, %rdi           # fd: stdout
leaq message(%rip), %rsi # buffer address
movq $message_len, %rdx  # number of bytes
syscall
```

#### Reading from stdin
```assembly
movq $0, %rax           # syscall: read
movq $0, %rdi           # fd: stdin
leaq buffer(%rip), %rsi # buffer address
movq $256, %rdx         # max bytes
syscall
# %rax now contains bytes read
```

### Important Notes on I/O

- read() and write() operate on raw bytes, not strings
- No automatic null termination
- read() returns the number of bytes actually read
- This return value can be used directly for write()
- Always check return values for errors (-1 indicates error)

## Troubleshooting

### Program doesn't output anything?
- Check that %rax contains the correct syscall number (1 for write)
- Verify %rdi is 1 for stdout
- Ensure %rdx contains the correct byte count
- Make sure the buffer address in %rsi is valid

### Echo doesn't work?
- The read syscall returns bytes read in %rax
- Use this value for the write syscall's byte count
- Buffer must be in .bss or .data section, not on stack for simple examples

### Segmentation fault?
- Check stack alignment (16-byte before syscall)
- Verify buffer addresses are valid
- Make sure you're not writing beyond allocated buffer space

## Notes

- System calls are the interface between user programs and the kernel
- The syscall instruction switches from user mode to kernel mode
- File descriptors: 0 (stdin), 1 (stdout), 2 (stderr)
- Position-independent code uses %rip-relative addressing
- The echoBroken.s example demonstrates common mistakes
- Understanding system calls is essential for low-level programming
- Modern programs typically use C library functions that wrap system calls
