# Assembly Examples

This directory contains a collection of x86-64 AT&T syntax assembly language examples organized by category. These examples are designed to help you learn and understand assembly programming concepts from the simplest "Hello World" program to more advanced topics.

## Directory Structure

- **[01-basics](01-basics/)** - Basic assembly programs and concepts
- **[02-data](02-data/)** - Data sections, arrays, and data manipulation
- **[03-control-flow](03-control-flow/)** - Loops, conditionals, and branching
- **[04-functions](04-functions/)** - Function definitions, calls, and parameters
- **[05-io](05-io/)** - Input/output operations
- **[06-advanced](06-advanced/)** - More complex assembly concepts

## Example Index

### 01-basics - Basic Assembly Programs
| File | Description |
|------|-------------|
| [01-hello-world.s](01-basics/01-hello-world.s) | Simple program that prints "Hello, World!" to standard output |
| [02-simple.s](01-basics/02-simple.s) | Basic assembly program structure |
| [03-simplest.s](01-basics/03-simplest.s) | Minimal assembly program showing the essential elements |
| [04-simpler.s](01-basics/04-simpler.s) | Similar to simplest but with slight variations |

### 02-data - Data and Variables
| File | Description |
|------|-------------|
| [01-array.s](02-data/01-array.s) | Defines and manipulates an array of integers, doubling each value |
| [02-data-section.s](02-data/02-data-section.s) | Demonstrates how to define data in the .data section |
| [03-data-types.s](02-data/03-data-types.s) | Shows different data types in assembly (.byte, .word, .long, .quad) |
| [04-data-constants.s](02-data/04-data-constants.s) | Uses constants and equates in assembly |

### 03-control-flow - Control Flow Structures
| File | Description |
|------|-------------|
| [01-for-loop.s](03-control-flow/01-for-loop.s) | Implements a for loop to calculate sum of squares |
| [02-while-loop.s](03-control-flow/02-while-loop.s) | Implements a while loop structure |
| [03-comparisons.s](03-control-flow/03-comparisons.s) | Demonstrates comparison operations and conditional jumps |
| [04-float-comparisons.s](03-control-flow/04-float-comparisons.s) | Shows how to compare floating-point values |

### 04-functions - Function Implementation
| File | Description |
|------|-------------|
| [01-fibonacci.s](04-functions/01-fibonacci.s) | Recursive function to calculate Fibonacci numbers |
| [02-basic-function.s](04-functions/02-basic-function.s) | Simple function definition and calling |
| [03-function-parameters.s](04-functions/03-function-parameters.s) | Passing parameters to functions |

### 05-io - Input/Output Operations
| File | Description |
|------|-------------|
| [01-echo.s](05-io/01-echo.s) | Reads input and echoes it back to the user |
| [02-read-integer.s](05-io/02-read-integer.s) | Reads an integer from standard input |
| [03-write-integer.s](05-io/03-write-integer.s) | Writes an integer to standard output |
| [04-echo-integer.s](05-io/04-echo-integer.s) | Reads an integer and echoes it back |

### 06-advanced - Advanced Assembly Concepts
| File | Description |
|------|-------------|
| [01-avx512.s](06-advanced/01-avx512.s) | Demonstrates AVX-512 vector instructions |
| [02-multiplication-division.s](06-advanced/02-multiplication-division.s) | Shows multiplication and division operations |
| [03-floating-point.s](06-advanced/03-floating-point.s) | Floating-point operations in assembly |
| [04-stack-manipulation.s](06-advanced/04-stack-manipulation.s) | Advanced stack manipulation techniques |

## How to Assemble and Run

Each example can be assembled and executed using the following commands:

```bash
# Assemble the code
as filename.s -o filename.o

# Link the object file
ld filename.o -o filename

# Execute the resulting binary
./filename
```

Some examples may require linking with additional object files or using gcc for compilation if they call external functions.

## References

For more in-depth information on x86-64 assembly programming, refer to the [assemblyGuide.org](/assemblyGuide.org) file in the main repository, which covers:

- Assembly basics
- Register usage
- Stack operations
- Function calling conventions
- Memory addressing
- I/O operations