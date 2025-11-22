# Lecture 8 - Advanced Assembly Topics

This directory contains a comprehensive collection of x86-64 assembly programs demonstrating advanced assembly concepts including I/O operations, string handling, functions, floating-point arithmetic, and SIMD instructions.

## Contents

This lecture significantly expands assembly knowledge with practical examples covering real-world programming tasks.

### Hello World and Output
- **helloworld.s** - Classic "Hello, World!" using system calls
- **helloworldcommented.s** - Detailed explanation of string output
- **helloAgain.s** - Alternative hello world implementation
- **helloAgaincommented.s** - Commented version with annotations

### Input/Output Operations
- **readInt.s** - Reading integer input from user
- **readIntcommented.s** - Detailed explanation of input handling
- **readInt2.s** - Alternative integer reading method
- **readInt2commented.s** - Commented version
- **readInt3.s** - Third integer reading implementation
- **readInt3commented.s** - Detailed annotations
- **writeInt.s** - Writing integer output
- **writeIntcommented.s** - Explained integer output
- **writeTester.s** - Testing integer output functions
- **writeTestercommented.s** - Commented test program

### String Operations
- **echo.s** - Echo program (reads and prints input)
- **echocommented.s** - Detailed echo implementation
- **countStr.s** - String length calculation
- **countStrcommented.s** - Commented string counting

### Arithmetic Operations
- **mully.s** - Multiplication example
- **mullycommented.s** - Explained multiplication
- **badMully.s** - Common multiplication mistakes
- **badMullycommented.s** - What not to do (educational)
- **mulDiv.s** - Multiplication and division together
- **mulDivcommented.s** - Detailed arithmetic operations

### Functions and Stack
- **fib.s** - Fibonacci sequence implementation
- **fibcommented.s** - Detailed recursive/iterative Fibonacci
- **localVars1.s** - Using local variables on stack
- **localVars1commented.s** - Stack frame management explained
- **stolenWriter.s** - Function calling example
- **stolenWritercommented.s** - Commented function calls

### Floating-Point and SIMD
- **floaty.s** - Floating-point arithmetic
- **floatycommented.s** - SSE floating-point explained
- **avx512.s** - AVX-512 vector operations
- **avx512commented.s** - SIMD programming detailed

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific categories:
```bash
# Hello World examples
make helloworld helloAgain

# I/O examples
make readInt writeInt echo

# Arithmetic examples
make mully mulDiv fib

# Advanced examples
make floaty avx512
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Hello World
```bash
./helloworld
```
**Expected output:** `Hello, World!` followed by a newline

### Echo Program
```bash
./echo
```
**Expected behavior:**
- Waits for user input
- Echoes back what you type
- Demonstrates string I/O operations

### Read Integer
```bash
./readInt
```
**Expected behavior:**
- Prompts for integer input
- Reads integer from stdin
- Processes and displays/uses the value

### Fibonacci
```bash
./fib
echo $?
```
**Expected behavior:**
- Calculates Fibonacci numbers
- Exit code contains a Fibonacci value
- Demonstrates function calls or loops

### Floating-Point Example
```bash
./floaty
echo $?
```
**Expected behavior:**
- Performs floating-point arithmetic
- Demonstrates SSE instructions
- Returns result as exit code

### AVX-512 Vector Operations
```bash
./avx512
echo $?
```
**Expected behavior:**
- Demonstrates SIMD (Single Instruction, Multiple Data) operations
- Processes multiple values in parallel
- Requires CPU with AVX-512 support

## Key Concepts Demonstrated

### System Calls for I/O
- **read** (syscall 0) - Reading from stdin
- **write** (syscall 1) - Writing to stdout
- **exit** (syscall 60) - Program termination
- Buffer management for I/O operations

### String Handling
- `.data` section for string storage
- `.asciz` directive for null-terminated strings
- `.byte` directive for manual byte arrays
- String length calculation
- Loading string addresses with `lea`

### Data Section Usage
- Defining global variables
- Storing string constants
- Array initialization
- Calculating sizes with `. - label`

### Function Implementation
- Call stack management
- `call` and `ret` instructions
- `push` and `pop` for saving/restoring registers
- Stack frame setup with `%rbp` and `%rsp`
- Local variable allocation on stack
- Parameter passing conventions

### Advanced Arithmetic
- `imul` - Signed integer multiplication
- `idiv` - Signed integer division
- Handling multiplication overflow
- Division by zero considerations

### Floating-Point Operations
- XMM registers for SSE operations
- Floating-point arithmetic instructions
- Converting between integer and float
- SSE instruction set basics

### SIMD Programming
- AVX-512 instructions (if supported)
- Vector registers (ZMM registers)
- Processing multiple data elements simultaneously
- Performance optimization through parallelism

### RIP-Relative Addressing
- Position-independent code
- `lea` with `%rip` register
- Modern addressing mode

## Troubleshooting

### AVX-512 not supported
If AVX-512 example fails:
```bash
lscpu | grep avx512
```
If no output, your CPU doesn't support AVX-512. This is normal for many processors.

### Input/Output Issues
- Ensure proper buffer sizes for I/O operations
- Check file descriptor values (0=stdin, 1=stdout, 2=stderr)
- Verify syscall numbers match your Linux version

### Segmentation Faults
- Common in programs with stack operations
- Check `push`/`pop` balance
- Verify stack alignment (16-byte boundary)
- Ensure proper function prologue/epilogue

## Notes

- This lecture represents a significant step up in assembly complexity
- The .data section is crucial for storing strings and variables
- System calls require specific register setups (rax=syscall number, rdi, rsi, rdx for args)
- Reading the commented versions is essential for understanding
- String operations in assembly are manual and detailed
- Function calls follow the x86-64 System V calling convention
- Stack grows downward (toward lower addresses)
- Floating-point uses separate register sets (XMM, YMM, ZMM)
- Not all CPUs support all SIMD instruction sets
- Buffer overflow is a real risk with manual I/O handling
- These examples provide building blocks for complex programs
