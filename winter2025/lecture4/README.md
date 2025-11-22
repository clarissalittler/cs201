# Lecture 4 - Assembly Instructions and Data Sections

This directory contains x86-64 assembly examples for Winter 2025 Lecture 4, demonstrating fundamental instructions, data manipulation, comparisons, and arrays.

## Contents

### Arithmetic Operations
- **add2.s** - Addition operations with multiple operands
- **add2commented.s** - Detailed explanation of addition and register operations

### Comparison and Branching
- **cmp1.s** - Comparison operations and conditional jumps
- **cmp1commented.s** - Complete explanation of cmp and jump instructions

### Data Section Usage
- **data1.s** - Basic data section with variables
- **data1commented.s** - Explains .quad, .long, .word, .byte directives
- **data2.s** - More complex data manipulation
- **data2commented.s** - Dereferencing addresses and loading values
- **data3.s** - Array operations in assembly
- **data3commented.s** - Iterating through arrays and summing elements

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make add2
make cmp1
make data1
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Addition Program
```bash
./add2
echo $?
```
**Expected behavior:**
Returns the result of arithmetic operations via exit code.

### Comparison Program
```bash
./cmp1
echo $?
```
**Expected behavior:**
Demonstrates conditional jumps based on comparisons. Check exit code to see results.

### Data Section Examples
```bash
./data1
./data2
./data3
```
**Expected behavior:**
- data1: Demonstrates defining variables in data section
- data2: Shows dereferencing and memory access
- data3: Sums an array and returns result

## Key Concepts Demonstrated

### Assembly Instructions

#### Arithmetic
- **add**: Add source to destination
- **sub**: Subtract source from destination
- **imul**: Signed multiplication
- **mul**: Unsigned multiplication
- **inc**: Increment by 1
- **dec**: Decrement by 1

#### Comparison and Jumps
- **cmp %a, %b**: Compare (computes %b - %a, sets flags)
- **jl**: Jump if less than
- **jg**: Jump if greater than
- **je**: Jump if equal
- **jne**: Jump if not equal
- **jle**: Jump if less than or equal
- **jge**: Jump if greater than or equal

### Data Section Directives
- **.quad**: 8 bytes (64-bit integer)
- **.long**: 4 bytes (32-bit integer)
- **.word**: 2 bytes (16-bit integer)
- **.byte**: 1 byte (8-bit integer)
- **.asciz**: Null-terminated string

### Memory Access
- **Direct value**: `$10` (immediate value)
- **Register**: `%rax` (value in register)
- **Memory address**: `label` or `(%rax)` (dereference)
- **Address of label**: `$label` (load address, not value)

### Array Operations
- Using labels as array base addresses
- Pointer arithmetic for array traversal
- Loop structures with labels and jumps
- Accumulating results in registers

### Calculating String Length
- Using `.` (current position) in assembly
- Formula: `message_len = . - message`
- Null termination with `.asciz`

## Instruction Format (AT&T Syntax)

```assembly
instruction source, destination
```

Examples:
- `movq $5, %rax` - Move 5 into rax
- `addq %rbx, %rax` - Add rbx to rax (result in rax)
- `cmpq %rcx, %rdx` - Compare (rdx - rcx)

### Size Suffixes
- **b**: byte (8-bit)
- **w**: word (16-bit)
- **l**: long (32-bit)
- **q**: quad (64-bit)

## Notes

- The `cmp` instruction subtracts the first operand from the second (%b - %a)
- Jump instructions use the flags set by `cmp` to make decisions
- Data section allows defining variables and constants at assembly level
- Arrays are accessed by treating labels as base addresses
- The `.` symbol represents the current position in the assembly file
- Understanding data representation is crucial for working with arrays
- Always remember AT&T syntax order: source, destination
- All commented versions provide line-by-line explanations
