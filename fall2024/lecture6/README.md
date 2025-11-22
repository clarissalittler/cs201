# Lecture 6 - Comparisons and Conditional Branching

This directory contains x86-64 assembly programs demonstrating comparison operations, conditional jumps, and control flow in assembly language.

## Contents

This lecture covers the `cmp` instruction and conditional branching, showing how to implement if-statements and decision-making in assembly.

### Assembly Source Files

- **compare1.s** - Basic comparison using `cmp` and conditional jump
- **compare1commented.s** - Detailed explanation of comparison and branching
- **comparefr1.s** - More complex comparison example
- **comparefr1commented.s** - Commented version with detailed annotations
- **comparefr2.s** - Advanced comparison demonstrating multiple conditions
- **comparefr2commented.s** - Fully explained version

### Reference Materials

- **agendaAndSuch.txt** - Lecture agenda and notes covering:
  - Review of assembly basics
  - The `cmp` instruction and its many uses
  - If-statements in assembly
  - Labels and functions
  - Stack operations (push, pop, ret, call)
  - Case statements and loops

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific examples:
```bash
make compare1
make comparefr1
make comparefr2
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Compare1 - Basic Comparison
```bash
./compare1
echo $?
```

**Expected behavior:**
- Compares a register value (0) with 5
- Uses `jle` (jump if less than or equal) to make a decision
- Returns 10 if the value is <= 5, otherwise returns -1
- Exit code shows the result of the comparison

### CompareFr1 - Function-based Comparison
```bash
./comparefr1
echo $?
```

**Expected behavior:**
- Demonstrates comparison with function-like structure
- Shows more complex branching logic
- Exit code reflects the comparison result

### CompareFr2 - Advanced Comparisons
```bash
./comparefr2
echo $?
```

**Expected behavior:**
- Multiple comparison operations
- Complex control flow with several branches
- Exit status demonstrates the final result

## Key Concepts Demonstrated

### Comparison Operations
- `cmp` instruction - Compares two values by subtraction (without storing result)
- Sets CPU flags (Zero Flag, Sign Flag, Carry Flag, Overflow Flag)
- Used in conjunction with conditional jumps

### Conditional Jumps
- `jle` - Jump if less than or equal
- `jl` - Jump if less than
- `je` - Jump if equal
- `jne` - Jump if not equal
- `jg` - Jump if greater
- `jge` - Jump if greater than or equal

### Control Flow
- Labels as jump targets (e.g., `.less`, `.finish`)
- Unconditional jumps with `jmp`
- If-else statements in assembly
- Nested conditionals

### Register Usage
- `%rdi` - First function argument / comparison operand
- `%rax` - Return value / result storage
- Immediate values in comparisons

### Program Flow
- Sequential execution
- Branching based on conditions
- Rejoining execution paths

## Notes

- The `cmp` instruction performs subtraction but discards the result, only setting flags
- Conditional jumps check these flags to determine whether to branch
- Labels starting with `.` are local labels (scoped to the current function)
- Multiple paths through code can lead to the same exit point
- Understanding CPU flags is crucial for conditional logic
- The commented versions explain each instruction's purpose in detail
- These examples form the foundation for implementing if-statements, switch-case, and loops
