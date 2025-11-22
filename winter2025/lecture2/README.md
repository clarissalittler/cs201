# Lecture 2 - Data Types and Floating Point Representation

This directory contains examples for Winter 2025 Lecture 2, covering data type sizes, pointer arithmetic, endianness, and floating point representation (IEEE 754 and Posits).

## Contents

### Data Types and Sizes
- **size1.c** - Demonstrates sizeof operator for various data types
- **size1commented.c** - Detailed explanation of data type sizes in C
- **charSigned.c** - Tests whether char is signed or unsigned
- **charSignedcommented.c** - Explains signed vs unsigned char behavior

### Pointer Arithmetic
- **pointArith.c** - Demonstrates pointer arithmetic and memory addressing
- **pointArithcommented.c** - Complete explanation of how pointers traverse memory

### Endianness
- **whichEnd.c** - Determines if system is big-endian or little-endian
- **whichEndcommented.c** - Explains byte ordering in memory

### Floating Point Representation
- **floatTest.c** - Tests floating point number representation
- **floatTestcommented.c** - Explains IEEE 754 floating point format
- **floatMunger.c** - Manipulates floating point bits directly
- **floatMungercommented.c** - Detailed walkthrough of float bit manipulation

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make size1
make whichEnd
make floatTest
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Data Type Sizes
```bash
./size1
```
**Expected output:**
Shows the size in bytes of various data types (char, int, long, float, double, pointers).

### Signed vs Unsigned Char
```bash
./charSigned
```
**Expected behavior:**
Determines whether char is signed or unsigned on your system (platform-dependent).

### Pointer Arithmetic
```bash
./pointArith
```
**Expected behavior:**
- Shows how pointer arithmetic scales by the size of the pointed-to type
- Demonstrates traversing arrays using pointer arithmetic
- Illustrates the relationship between arrays and pointers

### Endianness Check
```bash
./whichEnd
```
**Expected output:**
```
Our number's bytes, in order, are: 78 56 34 12
```
This output indicates little-endian (least significant byte first). On big-endian systems, you'd see: 12 34 56 78

### Floating Point Tests
```bash
./floatTest
./floatMunger
```
**Expected behavior:**
- Demonstrates IEEE 754 floating point representation
- Shows how to manipulate individual bits of floating point numbers
- Illustrates special values (infinity, NaN, denormalized numbers)

## Key Concepts Demonstrated

### Data Type Sizes
- Different integer sizes (char=1, short=2, int=4, long=8 bytes on 64-bit)
- Floating point sizes (float=4, double=8 bytes)
- Pointer size equals architecture width (8 bytes on 64-bit systems)
- Platform-dependent behavior

### Pointer Arithmetic
- Pointers increment by sizeof(type), not by 1 byte
- Array indexing is syntactic sugar for pointer arithmetic
- Casting pointers to different types
- Traversing memory one byte at a time

### Endianness
- **Little-endian**: Least significant byte stored at lowest address (x86-64)
- **Big-endian**: Most significant byte stored at lowest address (network byte order)
- Affects interpretation of multi-byte values in memory

### IEEE 754 Floating Point (32-bit)
- **1 bit**: Sign (0 = positive, 1 = negative)
- **8 bits**: Exponent (biased by 127)
- **23 bits**: Significand (fractional part)
- **Formula**: (-1)^sign × 2^(exponent-127) × (1 + significand)

**Special values:**
- Exponent all 0s, significand all 0s: Zero
- Exponent all 1s, significand all 0s: Infinity
- Exponent all 1s, significand not 0: NaN
- Exponent all 0s, significand not 0: Denormalized number

### Posits (Alternative to IEEE 754)
- Dynamic precision allocation
- Better accuracy for certain applications
- Two parameters: n (total bits) and es (max exponent bits)
- Regime bits determine dynamic range
- More efficient representation of numbers near zero

## Troubleshooting

### Unexpected sizes?
Data type sizes are platform-dependent. The examples assume a 64-bit Linux system. On other platforms, sizes may differ.

### Different endianness?
Most modern desktop systems are little-endian (x86-64, ARM in little-endian mode). Network protocols use big-endian.

## Notes

- Understanding data representation is crucial for systems programming
- Floating point arithmetic is approximate, not exact
- IEEE 754 is the standard, but Posits offer interesting alternatives
- Pointer arithmetic is powerful but requires careful use
- Always check the actual size of types on your platform using sizeof()
- The commented versions provide extensive explanations of the underlying concepts
