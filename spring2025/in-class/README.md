# In-Class Examples (April 10, 2025)

This directory contains in-class coding examples from the April 10, 2025 lecture, focusing on dynamic memory allocation and bit manipulation.

## Contents

### Memory Allocation
- **malloc1.c** - Basic malloc usage with structs
- **malloc1commented.c** - Detailed explanation of malloc and struct allocation
- **malloc2.c** - More complex malloc example
- **malloc2commented.c** - Commented version with advanced malloc concepts

### Bit Manipulation
- **shiftnegative.c** - Demonstrates bit shifting with negative numbers
- **shiftnegativecommented.c** - Explains two's complement and arithmetic shift behavior

### Lecture Notes
- **041025.org** - Lecture agenda covering bit shifts, registers, and performance

## Compilation

### Compile all programs:
```bash
gcc malloc1.c -o malloc1
gcc malloc2.c -o malloc2
gcc shiftnegative.c -o shiftnegative
```

### Compile with warnings and debugging:
```bash
gcc -Wall -g malloc1.c -o malloc1
gcc -Wall -g malloc2.c -o malloc2
gcc -Wall -g shiftnegative.c -o shiftnegative
```

### Compile all at once:
```bash
for f in malloc1 malloc2 shiftnegative; do
  gcc ${f}.c -o ${f}
done
```

## Running the Examples

### malloc1 - Basic Dynamic Allocation

**Run:**
```bash
./malloc1
```

**Expected behavior:**
- Allocates memory for a struct on the heap using malloc
- Initializes struct members (n1=10, n2=20)
- Prints the values
- Demonstrates basic heap allocation

**Expected output:**
```
g->n1 is 10
g->n2 is 20
```

**What it demonstrates:**
- How malloc allocates memory by size in bytes
- Using sizeof() to get struct size
- Casting malloc's void* return to appropriate type
- Accessing struct members via pointer with ->

### malloc2 - Advanced Allocation

**Run:**
```bash
./malloc2
```

**Expected behavior:**
- More complex malloc usage patterns
- May demonstrate arrays, multiple allocations, or realloc
- Shows advanced heap memory management

### shiftnegative - Bit Shift Behavior

**Run:**
```bash
./shiftnegative
```

**Expected behavior:**
- Demonstrates how bit shifts work with negative numbers
- Shows difference between arithmetic and logical shifts
- Explains two's complement representation

**Key insights:**
- Right shift on negative numbers may do arithmetic shift (sign extension)
- Left shift on negative numbers is multiplication by powers of 2
- Behavior can be implementation-defined for signed types

## Key Concepts Demonstrated

### Dynamic Memory Allocation (malloc)

**What is malloc?**
- Allocates memory on the heap at runtime
- Takes size in bytes as parameter
- Returns void* pointer (must be cast to appropriate type)
- Memory persists until explicitly freed
- Different from C++ `new` operator

**malloc vs Stack Allocation:**
```c
// Stack allocation (automatic)
struct Garbage g;  // Dies when function returns

// Heap allocation (dynamic)
struct Garbage* g = malloc(sizeof(struct Garbage));  // Persists until freed
```

**Important malloc practices:**
- Always check if malloc returns NULL (allocation failed)
- Use sizeof() to calculate size, never hardcode
- Cast the result to appropriate pointer type
- Free memory when done (prevent memory leaks)
- In these examples, free() is omitted for simplicity

**sizeof() operator:**
- Returns size of type or variable in bytes
- Evaluated at compile time
- Essential for portable malloc usage
- `sizeof(struct Garbage)` gives total struct size

### Bit Shifting Operations

**Left Shift (<<):**
- Shifts bits to the left, fills with zeros on right
- Equivalent to multiplication by powers of 2
- `n << m` equals `n * 2^m`

**Examples (8-bit for simplicity):**
```
00001101 = 13
00001101 << 1
=> 00011010 = 26 = 2*13
```

**Right Shift (>>):**
- Shifts bits to the right
- For unsigned: logical shift (fills with 0s)
- For signed: arithmetic shift (fills with sign bit)
- Equivalent to integer division by powers of 2

**Why is bit shift multiplication/division?**
- Binary representation: Each position is a power of 2
- Shifting left by 1 doubles the value
- Shifting right by 1 halves the value (integer division)
- Much faster than actual multiplication/division

### Two's Complement (Negative Numbers)

**How negative numbers are stored:**
```
For 8-bit signed numbers:
13 = 00001101

To get -13:
1. Invert bits:  11110010
2. Add 1:        11110011 = -13 in two's complement
```

**Arithmetic shift behavior:**
- Right shift preserves sign bit
- Ensures negative numbers stay negative after division
- Example: -13 >> 1 gives -7 (not +121)

### Registers and Performance

**From lecture notes:**
- 16 general-purpose registers per core (x86-64)
- Each register holds 64 bits (8 bytes)
- Accessed in single clock cycle (nanoseconds)
- Memory is 10,000s-100,000s times slower
- Only 128 bytes total in registers!
- This is why compilers heavily optimize register usage

**Register names:**
- %rax, %rbx, %rcx, %rdx (traditional roles)
- %rsi, %rdi (string/array operations)
- %rbp (base pointer), %rsp (stack pointer)
- %r8 through %r15 (additional general purpose)

## Troubleshooting

### Segmentation Fault in malloc examples
```bash
# Common causes:
# 1. Dereferencing NULL pointer (malloc failed)
# 2. Using freed memory
# 3. Buffer overflow

# Always check malloc return:
if (g == NULL) {
    fprintf(stderr, "malloc failed\n");
    exit(1);
}
```

### Memory Leaks
```bash
# Check for leaks with valgrind:
valgrind --leak-check=full ./malloc1

# Proper cleanup:
free(g);  // Always free what you malloc
```

### Unexpected shift results
```bash
# Be aware of:
# 1. Signed vs unsigned types
# 2. Integer promotion
# 3. Undefined behavior for shift >= width

# Safe shifting:
unsigned int n = 13;
n = n << 1;  // Well-defined for unsigned
```

### Compiler Warnings
```bash
# If you see "implicit declaration of malloc":
# Missing header
#include <stdlib.h>  // For malloc, free

# If you see "format specifies type 'int' but argument has type":
# Use correct format specifier
printf("%d\n", value);  // For int
printf("%ld\n", value); // For long
```

## Notes

- malloc allocates from the heap, not the stack
- The heap is much larger than the stack (megabytes vs kilobytes)
- Memory allocated with malloc persists across function calls
- Always pair malloc with free to prevent memory leaks
- sizeof() is a compile-time operator, not a runtime function
- Bit shifts are much faster than multiplication/division
- Two's complement is the standard for representing signed integers
- Understanding registers helps understand why variables are fast
- Caching helps bridge the speed gap between registers and memory
- The commented versions provide extensive educational details

## Related Topics

See also:
- `/home/user/cs201/spring2025/video1/malloc1.c` - Another malloc example
- `/home/user/cs201/spring2025/video1/bitMunger.c` - Interactive bit manipulation
- `/home/user/cs201/spring2025/video1/sizeof1.c` - sizeof demonstrations
- `/home/user/cs201/spring2025/video2/` - More bit and pointer manipulation
- `/home/user/cs201/spring2025/assembly/` - How this looks at assembly level
