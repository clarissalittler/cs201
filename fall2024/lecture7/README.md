# Lecture 7 - Loops and Arrays

This directory contains x86-64 assembly programs demonstrating loop constructs and array manipulation in assembly language.

## Contents

This lecture builds on conditional branching to implement loops (for and while) and demonstrates working with arrays in assembly.

### Assembly Source Files

#### Loop Examples
- **forLoop.s** - Implementation of a for-loop pattern
- **forLoopcommented.s** - Detailed explanation of for-loop structure (calculates sum of squares)
- **whileLoop.s** - Implementation of a while-loop pattern
- **whileLoopcommented.s** - Commented version explaining while-loop logic

#### Array Examples
- **array1.s** - Array manipulation program
- **array1commented.s** - Detailed array operations (doubling elements and summing)

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific examples:
```bash
make forLoop
make whileLoop
make array1
```

### Clean up:
```bash
make clean
```

## Running the Examples

### For Loop Example
```bash
./forLoop
echo $?
```

**Expected behavior:**
- Implements a for-loop that calculates the sum of squares from 0 to 5
- Computes: 0² + 1² + 2² + 3² + 4² + 5² = 0 + 1 + 4 + 9 + 16 + 25 = 55
- Exit status will be 55 (or 55 mod 256 if displayed as byte)

**Loop structure:**
```
for (i = 0; i < 6; i++) {
    result += i * i;
}
```

### While Loop Example
```bash
./whileLoop
echo $?
```

**Expected behavior:**
- Demonstrates while-loop pattern in assembly
- Iterates while a condition remains true
- Exit code shows the result of the computation

### Array Example
```bash
./array1
echo $?
```

**Expected behavior:**
- Defines an array with values [1, 2, 3, 4, 5]
- Doubles each element: [2, 4, 6, 8, 10]
- Calculates the sum of doubled elements: 2 + 4 + 6 + 8 + 10 = 30
- Exit status will be 30

**Operations performed:**
1. First loop: doubles each array element
2. Second loop: sums all the doubled elements

## Key Concepts Demonstrated

### Loop Constructs

#### For Loop Pattern
- Initialize counter register (`%rcx` typically)
- Loop body executes computation
- Increment counter (`inc`)
- Compare counter with limit (`cmp`)
- Conditional jump back to loop start (`jl .loopStart`)

#### While Loop Pattern
- Condition check at loop start
- Loop body execution
- Update variables
- Jump back to condition check

### Loop Components
- Loop counter registers
- Loop labels (e.g., `.loopStart`, `.sumLoop`)
- Exit conditions using `cmp` and conditional jumps
- Accumulator registers for results

### Array Operations

#### Array Definition
- `.section .data` - Data section for arrays
- `.quad` - Quadword (8-byte) array elements
- Array initialization with values

#### Array Indexing
- Scaled index addressing: `myArray(,%rcx,8)`
  - Base address: `myArray`
  - Index register: `%rcx`
  - Scale factor: `8` (for 8-byte quadwords)
- Formula: `address = base + (index * scale)`

#### Array Processing
- Iterating through array elements
- Reading values with `mov`
- Modifying values with arithmetic operations
- Writing values back to array

### Register Usage
- `%rcx` - Loop counter/array index
- `%rax` - Accumulator for results
- `%rbx` - Temporary storage for calculations

### Arithmetic Operations
- `imul` - Integer multiplication
- `add` - Addition for accumulation
- `inc` - Increment counter

## Notes

- Loop patterns in assembly mirror high-level language constructs
- `cmp` and conditional jumps (`jl`, `jle`, `jg`, etc.) control loop iteration
- Array indexing uses scaled addressing modes for efficiency
- The scale factor (8 for quadwords) matches the element size
- Multiple loops can operate on the same data sequentially
- Loop counters typically start at 0 for array indexing
- Exit codes are limited to 0-255, so results may wrap around
- The commented versions provide line-by-line explanations
- These patterns form the basis for more complex data structure manipulation
