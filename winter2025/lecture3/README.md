# Lecture 3 - Introduction to x86-64 Assembly

This directory contains introductory x86-64 assembly examples for Winter 2025 Lecture 3, demonstrating the transition from C to assembly language.

## Contents

### C Source and Assembly
- **hello.c** - Simple C program
- **hellocommented.c** - Commented C version
- **hello.s** - Assembly version of hello program (compiler-generated)
- **hellocommented.s** - Detailed annotation of assembly code

### Hand-Written Assembly
- **ret1.s** - Minimal assembly program that returns a value
- **ret1commented.s** - Detailed explanation of assembly program structure

## Compilation

### Compile all examples:
```bash
make
```

### Compile C programs:
```bash
make hello
```

### Assemble and link assembly programs:
```bash
make ret1
```

### Generate assembly from C:
```bash
gcc -S hello.c -o hello.s
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Hello Program (C)
```bash
./hello
```
**Expected output:**
```
Hello CS201!
```

### Return Value Program (Assembly)
```bash
./ret1
echo $?
```
**Expected output:**
The exit code (check with `echo $?` after running).

## Key Concepts Demonstrated

### Assembly Language Basics
- **AT&T syntax** vs Intel syntax (course uses AT&T)
- Source and destination operand order in AT&T: `instruction source, destination`
- Register naming conventions
- GNU Assembler (as) usage

### Program Structure
- **.text section**: Contains executable code
- **.data section**: Contains initialized data
- **.rodata section**: Contains read-only data (constants, strings)
- **.bss section**: Contains uninitialized data

### x86-64 Registers
- **%rax**: Accumulator, return values
- **%rbx**: Base register
- **%rcx**: Counter register
- **%rdx**: Data register
- **%rsi**: Source index
- **%rdi**: Destination index (first function argument)
- **%rsp**: Stack pointer
- **%rbp**: Base pointer
- **%r8-%r15**: Additional general-purpose registers

### Basic Instructions
- **mov**: Move data between registers and memory
- **push**: Push value onto stack
- **pop**: Pop value from stack
- **call**: Call a function
- **ret**: Return from function
- **lea**: Load effective address

### Calling Conventions
- Arguments passed in registers: %rdi, %rsi, %rdx, %rcx, %r8, %r9
- Additional arguments passed on stack
- Return value in %rax
- Stack must be 16-byte aligned before calls

### Compilation Process
1. **Preprocessing**: Handle #include, #define directives
2. **Compilation**: Convert C to assembly (.s file)
3. **Assembly**: Convert assembly to object code (.o file)
4. **Linking**: Combine object files into executable

## Comparing C and Assembly

The hello.c and hello.s files demonstrate how a simple C program translates to assembly:

**C version:**
```c
printf("Hello CS201!\n");
```

**Assembly equivalent:**
```assembly
lea rax, .LC0[rip]    # Load address of string
mov rdi, rax          # Put address in first argument
call puts@PLT         # Call puts function
```

## Notes

- This lecture introduces the foundation for understanding how programs execute at the machine level
- Assembly language gives you direct control over the CPU and memory
- AT&T syntax: source first, then destination (opposite of Intel syntax)
- The commented versions (.commented.s) provide line-by-line explanations
- Understanding assembly helps with debugging, optimization, and security analysis
- Modern compilers generate highly optimized assembly code
- Hand-writing assembly is rarely needed but valuable for understanding system behavior
