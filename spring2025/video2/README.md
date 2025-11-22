# Video 2 - Floating Point and Memory Representation

This directory contains examples demonstrating floating point representation, byte ordering (endianness), and pointer arithmetic.

## Contents

### Floating Point Manipulation
- **floatMunger.c** - Interactive float bit manipulation tool
- **floatMungercommented.c** - Detailed explanation of IEEE 754 representation
- **badFloat.c** - Demonstrates floating point precision issues
- **badFloatcommented.c** - Explains why floats aren't exact

### Memory and Byte Ordering
- **byteorder.c** - Demonstrates endianness (little vs big endian)
- **byteordercommented.c** - Explains how multi-byte values are stored

### Pointer Arithmetic
- **pointerarith.c** - Shows how pointer arithmetic works
- **pointerarithcommented.c** - Detailed pointer arithmetic explanation

### Lecture Notes
- **Agenda.org** - Comprehensive notes on IEEE 754, endianness, and floats

## Compilation

### Compile all programs:
```bash
gcc floatMunger.c -o floatMunger
gcc badFloat.c -o badFloat
gcc byteorder.c -o byteorder
gcc pointerarith.c -o pointerarith
```

### Compile with warnings:
```bash
gcc -Wall floatMunger.c -o floatMunger
gcc -Wall badFloat.c -o badFloat
gcc -Wall byteorder.c -o byteorder
gcc -Wall pointerarith.c -o pointerarith
```

### Compile all at once:
```bash
for f in floatMunger badFloat byteorder pointerarith; do
  gcc ${f}.c -o ${f}
done
```

## Running the Examples

### floatMunger - IEEE 754 Exploration

**Run:**
```bash
./floatMunger
```

**Expected behavior:**
- Prompts: "Enter a float: "
- Displays the float value (10 decimal places)
- Shows all 32 bits with spaces separating sign, exponent, and mantissa
- Prompts: "Which bit to flip?: "
- Flips the specified bit
- Shows how bit changes affect the float value
- Loops indefinitely (Ctrl+C to exit)

**Example session:**
```
Enter a float: 1.0
The number is: 1.0000000000
0 01111111 00000000000000000000000
Which bit to flip?: 31
The number is: -1.0000000000
1 01111111 00000000000000000000000
Which bit to flip?: 23
The number is: -1.0000002384
1 01111111 00000000000000000000001
^C
```

**What it demonstrates:**
- IEEE 754 single-precision format
- Sign bit (bit 31)
- 8 exponent bits (bits 30-23)
- 23 mantissa bits (bits 22-0)
- How small bit changes create large value changes
- Visual representation of float storage

### badFloat - Precision Problems

**Run:**
```bash
./badFloat
```

**Expected behavior:**
- Demonstrates that floats cannot represent all decimal numbers exactly
- Shows accumulation of rounding errors
- Illustrates why float equality comparisons are problematic

**Example output (implementation-dependent):**
```
0.1 + 0.2 = 0.30000001192092896
Expected: 0.3
Difference: 0.00000001192092896
```

**What it demonstrates:**
- Floats use binary representation, not decimal
- Many decimal fractions are infinite binary fractions
- Rounding errors accumulate
- Never use == for float comparison
- Use epsilon for float comparisons: `fabs(a - b) < EPSILON`

### byteorder - Endianness

**Run:**
```bash
./byteorder
```

**Expected output:**
- Shows how multi-byte integers are stored in memory
- Demonstrates little-endian byte ordering (most x86-64 systems)
- Displays bytes at different memory addresses

**Example output:**
```
Value: 0x01020304
Address   Byte
0x...00   0x04  (least significant byte at lowest address - little endian)
0x...01   0x03
0x...02   0x02
0x...03   0x01  (most significant byte at highest address)
```

**What it demonstrates:**
- Endianness: byte ordering in memory
- Little endian: least significant byte first (Intel/AMD)
- Big endian: most significant byte first (some network protocols)
- Why byte order matters for file I/O and networking

### pointerarith - Pointer Math

**Run:**
```bash
./pointerarith
```

**Expected output:**
- Shows how pointer arithmetic works with different types
- Demonstrates that p+1 doesn't always mean "next byte"
- Illustrates pointer/type relationship

**Example output:**
```
char* p = 0x100
p + 1 = 0x101  (moves 1 byte)

int* p = 0x100
p + 1 = 0x104  (moves 4 bytes = sizeof(int))

long* p = 0x100
p + 1 = 0x108  (moves 8 bytes = sizeof(long))
```

**What it demonstrates:**
- Pointer arithmetic is scaled by pointed-to type size
- p + n means "address of nth element after p"
- Array indexing is pointer arithmetic: a[i] == *(a + i)
- Why pointer types matter

## Key Concepts Demonstrated

### IEEE 754 Floating Point Standard

**32-bit Single Precision Format:**
```
[Sign: 1 bit][Exponent: 8 bits][Mantissa: 23 bits]
```

**Formula:**
```
value = (-1)^sign × 2^(exponent - 127) × (1 + mantissa)
```

**Components:**

**Sign bit (bit 31):**
- 0 = positive
- 1 = negative

**Exponent (bits 30-23):**
- 8 bits, interpreted as unsigned (0-255)
- Bias of 127 for normalization
- Actual exponent = stored value - 127
- Range: 2^-126 to 2^127

**Mantissa/Significand (bits 22-0):**
- 23 bits representing fractional part
- Implicit leading 1 (normalized form)
- Interpreted as binary fraction: 0.b₁b₂b₃...b₂₃
- Each bit represents 1/2, 1/4, 1/8, etc.

**Example (from Agenda):**
```
1.0 in IEEE 754:
Sign: 0 (positive)
Exponent: 01111111 = 127 (actual exponent: 127 - 127 = 0)
Mantissa: 00000000000000000000000 (represents 0)
Value: (-1)^0 × 2^0 × (1 + 0) = 1.0
```

### Fractional Binary Numbers

**How mantissa works:**
```
For 8-bit example: b₁ b₂ b₃ b₄ b₅ b₆ b₇ b₈

Value = b₁×(1/2) + b₂×(1/4) + b₃×(1/8) + b₄×(1/16) + ...

Example: 11110000
= 1/2 + 1/4 + 1/8 + 1/16
= 8/16 + 4/16 + 2/16 + 1/16
= 15/16 = 0.9375
```

### Special Float Values

**Denormalized numbers (exponent = 0):**
```
value = (-1)^sign × 2^(-126) × mantissa
```
- Allows gradual underflow near zero
- Smaller than normalized minimum

**Infinity (exponent = 255, mantissa = 0):**
- Positive infinity: sign=0
- Negative infinity: sign=1

**NaN (exponent = 255, mantissa ≠ 0):**
- Result of undefined operations (0/0, sqrt(-1), etc.)

### Endianness

**Little Endian (Intel/AMD x86-64):**
- Least significant byte at lowest address
- Most common on desktop/laptop systems

**Example: 0x01020304 at address 0x1000:**
```
Address   Value
0x1000    0x04  ← least significant byte
0x1001    0x03
0x1002    0x02
0x1003    0x01  ← most significant byte
```

**Big Endian (network byte order):**
- Most significant byte at lowest address
- Used in network protocols
- Some ARM processors

**Example: 0x01020304 at address 0x1000:**
```
Address   Value
0x1000    0x01  ← most significant byte
0x1001    0x02
0x1002    0x03
0x1003    0x04  ← least significant byte
```

**Important note from Agenda:**
- Endianness is about byte ordering, NOT bit ordering within bytes
- Bits within a byte are always ordered the same way

### Pointer Arithmetic

**Key principle:**
- Pointer + n moves forward n elements (not n bytes)
- Element size = sizeof(*pointer)

**Examples:**
```c
char* p = 0x100;
p + 1 → 0x101  // +1 byte (sizeof(char) = 1)

int* p = 0x100;
p + 1 → 0x104  // +4 bytes (sizeof(int) = 4)

double* p = 0x100;
p + 1 → 0x108  // +8 bytes (sizeof(double) = 8)
```

**Array indexing relationship:**
```c
int arr[10];
arr[3] is equivalent to *(arr + 3)
&arr[3] is equivalent to (arr + 3)
```

## Troubleshooting

### Float Comparison Issues
```c
// WRONG:
if (f1 == f2) { ... }

// CORRECT:
#define EPSILON 0.000001
if (fabs(f1 - f2) < EPSILON) { ... }
```

### Unexpected Float Values
```bash
# Remember:
# - 0.1 cannot be represented exactly in binary
# - Repeated operations accumulate error
# - Use double for better precision (but still not exact)
# - Use integer arithmetic when possible
```

### Pointer Arithmetic Errors
```c
// Common mistake:
int* p;
p = p + sizeof(int);  // WRONG! Moves by 4*sizeof(int) bytes

// Correct:
p = p + 1;  // Moves to next int

// Or for byte-level access:
char* byte_p = (char*)p;
byte_p = byte_p + 4;  // Moves 4 bytes
```

### Endianness Confusion
```bash
# Endianness only matters for:
# - Reading/writing binary files
# - Network communication
# - Hardware interfacing
# - Interpreting raw memory dumps

# Most C code doesn't need to worry about endianness
# The compiler handles it automatically for normal operations
```

## Notes

- Floats sacrifice precision for range
- IEEE 754 is universal standard (same on all modern systems)
- Endianness varies by architecture but is transparent in most C code
- Pointer arithmetic is why array indexing is fast
- Type casts (like `(int*)&num`) allow reinterpreting bit patterns
- floatMunger demonstrates this by treating float bits as int bits
- Bit representation is fundamental to understanding computer behavior
- Never represent money with floats - use integers (cents) or decimal libraries
- Understanding these concepts is crucial for systems programming
- The Agenda.org provides extensive detail on IEEE 754 math

## Advanced Topics (from Agenda)

### Very Tiny Numbers
When exponent bits are all 0:
```
value = (-1)^sign × 2^(-126) × mantissa
```
- Allows smooth approach to zero
- Prevents abrupt underflow

### Why Floats Aren't Exact
- Infinite real numbers between 0 and 1
- Uncountably infinite (more than all possible programs!)
- Impossible to represent all values exactly
- Rounding happens constantly
- Transcendental numbers (π, e) require infinite storage

## Related Topics

See also:
- `/home/user/cs201/spring2025/video1/bitMunger.c` - Integer bit manipulation
- `/home/user/cs201/spring2025/video3/` - How this looks in assembly
- `/home/user/cs201/spring2025/in-class/shiftnegative.c` - Two's complement shifts
