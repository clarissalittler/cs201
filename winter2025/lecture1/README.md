# Lecture 1 - Introduction to C and Bit Operations

This directory contains introductory examples for Winter 2025 Lecture 1, covering C programming basics, bitwise operations, and memory management.

## Contents

### Basic C Programs
- **hello.c** - Classic "Hello, World!" program
- **hellocommented.c** - Fully commented version explaining program structure
- **echo.c** - Simple echo program reading and printing input
- **echocommented.c** - Commented version with detailed explanations
- **adder.c** - Basic integer addition program
- **addercommented.c** - Commented version explaining input/output operations

### Bitwise Operations
- **bit1.c** - Interactive bit manipulation program (flip individual bits)
- **bit1commented.c** - Detailed explanation of bitwise operators
- **bitshiftDemo.c** - Demonstrates left shift (<<) and right shift (>>) operators
- **bitshiftDemocommented.c** - Complete explanation of bit shifting

### Memory Management
- **malloc1.c** - Introduction to dynamic memory allocation
- **malloc1commented.c** - Detailed explanation of malloc, free, and pointers
- **malloc2.c** - More advanced malloc examples
- **malloc2commented.c** - Complete walkthrough of dynamic memory concepts

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make hello
make bit1
make malloc1
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Hello World
```bash
./hello
```
**Expected output:**
```
Hello, world!
```

### Echo Program
```bash
./echo
```
Type text and press Enter to see it echoed back.

### Bit Manipulation Demo
```bash
./bit1
```
**Expected behavior:**
- Enter an integer
- Program displays its binary representation
- Choose which bit to flip (0-31)
- See the updated number and its bits
- Loop continues until you exit (Ctrl+C)

### Bit Shift Demo
```bash
./bitshiftDemo
```
Shows how left and right shift operations work on integers.

### Memory Allocation Examples
```bash
./malloc1
./malloc2
```
Demonstrate dynamic memory allocation and pointer manipulation.

## Key Concepts Demonstrated

### C Programming Basics
- Program structure (main function, includes, return values)
- Input/output with printf and scanf
- Basic data types (int, char)
- Compilation process

### Bitwise Operations
- AND (&), OR (|), XOR (^), NOT (~)
- Left shift (<<) and right shift (>>)
- Bit manipulation techniques
- Binary representation of integers
- Boolean vs bitwise operators

### Two's Complement Representation
- How signed integers are represented
- Range of n-bit integers: -2^(n-1) to 2^(n-1) - 1
- Converting between positive and negative values

### Memory Management
- Dynamic memory allocation with malloc()
- Freeing memory with free()
- Pointer basics
- Memory leaks and proper cleanup

## Notes

- All programs include both minimal and heavily commented versions
- The commented versions are excellent learning resources
- Bit operations are fundamental to understanding how computers work
- Practice with bit1.c to build intuition for binary representations
- Always free() memory that you malloc() to avoid memory leaks
