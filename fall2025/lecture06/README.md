# Lecture 6 - Assembly Control Flow

This directory contains x86-64 assembly examples demonstrating control flow for Fall 2025 Lecture 6.

## Contents

### Basic Arithmetic
- **arith.s** - Basic arithmetic operations (add, mov)
- **arithcommented.s** - Detailed explanation of arithmetic instructions
- **smallredux.s** - Small program revisiting basic concepts
- **smallreduxcommented.s** - Detailed explanation

### Comparison and Conditional Jumps
- **cmp1.s** - Basic comparison and conditional branching
- **cmp1commented.s** - Detailed explanation of compare and jump instructions
- **cmp2.s** - More complex comparison examples
- **cmp2commented.s** - Detailed explanation
- **cmp3.s** - Additional comparison patterns
- **cmp3commented.s** - Detailed explanation

### Loops
- **for1.s** - For-loop style iteration (sum 1 to 10)
- **for1commented.s** - Detailed explanation of loop structure
- **for2.s** - More complex loop example
- **for2commented.s** - Detailed explanation

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make arith
make cmp1
make for1
```

Assembly files are assembled and linked using:
```bash
as arith.s -o arith.o
ld arith.o -o arith
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Arithmetic Operations
```bash
./arith
echo $?  # Check exit code (should show 30: 5 + 10 + 5 + 10)
```

**Expected behavior:**
- Performs arithmetic operations using registers
- Stores result in %rdi for exit code
- Exit code shows the final computed value

### Comparison Examples
```bash
./cmp1
echo $?  # Check exit code based on comparison result

./cmp2
echo $?

./cmp3
echo $?
```

**Expected behavior:**
- Demonstrates conditional execution using cmp and jump instructions
- Different exit codes based on comparison outcomes
- Shows how to implement if/else logic in assembly

### Loop Examples
```bash
./for1
echo $?  # Should show 55 (sum of 1+2+3+...+10)

./for2
echo $?
```

**Expected behavior:**
- for1: Sums numbers 1 through 10, exits with result (55)
- Demonstrates loop initialization, condition testing, and increment
- Uses labels and jumps to create loop structure

### Small Redux
```bash
./smallredux
echo $?
```

**Expected behavior:**
- Reviews basic assembly concepts from previous lecture
- Demonstrates register usage and system calls

## Key Concepts Demonstrated

### Control Flow Instructions
- **cmp** - Compare two values (sets flags in RFLAGS register)
- **jmp** - Unconditional jump to label
- **jge** - Jump if greater or equal (conditional)
- **jg** - Jump if greater
- **jl** - Jump if less
- **jle** - Jump if less or equal
- **je** - Jump if equal
- **jne** - Jump if not equal

### Loop Structure
- **Initialization** - Set loop counter (typically %rcx by convention)
- **Condition** - Compare counter to limit
- **Body** - Execute loop operations
- **Increment** - Update loop counter
- **Jump** - Return to condition check or exit

### Labels
- **Loop labels** - Mark locations in code for jumps
- **Finish/exit labels** - Mark cleanup and exit sequences
- Labels are resolved by assembler to memory addresses

### Register Conventions
- **%rcx** - Commonly used for loop counters
- **%r8** - Often used for accumulation in examples
- **%rdi** - Holds exit code (first argument register)
- **%rax** - Holds system call number

### RIP-Relative Addressing
- **%rip** - Instruction pointer (program counter)
- Used for position-independent code
- Global variables accessed relative to current instruction
- Format: `(%rip + offset)` or `(%rip - offset)`

### Arithmetic Operations
- **add** - Addition (add source, destination)
- **sub** - Subtraction
- **mov** - Move/copy values between registers and memory
- AT&T syntax: source first, destination second

## Notes

- All loops use labels and conditional jumps (no dedicated loop instruction)
- Comparison instructions set CPU flags, jumps test those flags
- Loop counters typically use %rcx by convention
- Exit codes are limited to 0-255 (single byte), larger values wrap around
- Commented versions explain each instruction and register usage in detail
- Compare C for-loops to assembly equivalents to understand compilation

## Troubleshooting

### Exit code shows unexpected value:
- Remember exit codes are modulo 256 (only lowest byte used)
- For example, 300 % 256 = 44

### Program doesn't terminate:
- Check loop condition and ensure it will eventually be false
- Verify jump targets are correct labels
- Make sure loop counter is being incremented

## Resources

- Review Lecture 5 materials for assembly basics
- Chapter 7 of "Dive into Systems" covers control flow in detail
- AT&T syntax reference for instruction formats
