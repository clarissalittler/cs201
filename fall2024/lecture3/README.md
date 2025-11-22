# Lecture 3 - Data Types and Structures

This lecture explores data type details, signed/unsigned integers, floating-point representation, and introduces structs.

## Contents

### Numeric Type Details
- **bitMunger.c** - More bit manipulation practice
- **signTest.c** - Signed vs unsigned integers
- **charSign.c** - Character signedness issues
- **floatMunger.c** - Floating-point representation
- **trunc1.c** - Type truncation and overflow

### Program Structure
- **defConst.c** - #define constants and preprocessor
- **structs1.c** - Introduction to structures

## Compilation

```bash
# Build all examples
make

# Build specific examples
make signTest
make structs1
make floatMunger

# Clean up
make clean
```

## Key Concepts

### Signed vs Unsigned
```c
signed int x = -1;    // Can be negative
unsigned int y = 1;   // Only positive

// Careful with comparisons!
if (x < y)  // Undefined behavior if types mismatch
```

### Type Truncation
- Assigning larger type to smaller loses data
- Example: `int` to `char` loses upper bytes

### Floating-Point
- Not exact (0.1 + 0.2 ≠ 0.3 exactly)
- Has limited precision
- Special values: NaN, Infinity

### Structs
```c
struct Point {
    int x;
    int y;
};

struct Point p = {10, 20};
printf("%d, %d\n", p.x, p.y);
```

### Preprocessor
- `#define` creates compile-time constants
- Replaced before compilation
- No type checking

## Running Examples

```bash
./signTest     # See signed/unsigned differences
./floatMunger  # Explore floating-point representation
./structs1     # Learn struct basics
./trunc1       # Observe type truncation
./defConst     # See #define in action
```

## Common Pitfalls

- Mixing signed and unsigned in comparisons
- Forgetting floating-point imprecision
- Struct padding and alignment (system dependent)
- #define doesn't check types

## See Also

- [Lecture 2](../lecture2/README.md) - Bit manipulation basics
- [Lecture 4](../lecture4/README.md) - More on data representation
- [Fall 2024 README](../README.md) - All lectures
