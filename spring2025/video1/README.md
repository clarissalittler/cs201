# Video 1 - Introduction to C Programming

This directory contains introductory C programming examples covering basic syntax, I/O, bit manipulation, and memory allocation.

## Contents

### Basic Programs
- **hello.c** - Classic "Hello World" program
- **hellocommented.c** - Commented version explaining program structure
- **echo.c** - Reads and echoes back a string
- **echocommented.c** - Detailed explanation of string I/O
- **echoint.c** - Reads and echoes back an integer
- **echointcommented.c** - Shows integer I/O with scanf/printf

### Data Types and Sizes
- **sizeof1.c** - Demonstrates sizeof operator with various types
- **sizeof1commented.c** - Explains type sizes and memory layout

### Bit Manipulation
- **bitMunger.c** - Interactive bit manipulation tool
- **bitMungercommented.c** - Detailed explanation of bitwise operations

### Dynamic Memory
- **malloc1.c** - Basic malloc usage example
- **malloc1commented.c** - Comprehensive malloc explanation

### Lecture Notes
- **Agenda.org** - Lecture outline covering C basics and bit operations

## Compilation

### Compile all programs:
```bash
gcc hello.c -o hello
gcc echo.c -o echo
gcc echoint.c -o echoint
gcc sizeof1.c -o sizeof1
gcc bitMunger.c -o bitMunger
gcc malloc1.c -o malloc1
```

### Compile with warnings and debugging:
```bash
gcc -Wall -g bitMunger.c -o bitMunger
```

### Compile all at once:
```bash
for f in hello echo echoint sizeof1 bitMunger malloc1; do
  gcc ${f}.c -o ${f}
done
```

## Running the Examples

### hello - First Program

**Run:**
```bash
./hello
```

**Expected output:**
```
Hello world!
```

**What it demonstrates:**
- Basic C program structure
- main() function
- printf() for output
- Return value (exit code)

### echo - String I/O

**Run:**
```bash
./echo
```

**Expected behavior:**
- Prompts for input (implementation-dependent)
- Reads a string from stdin
- Echoes the string back
- Demonstrates basic I/O functions

### echoint - Integer I/O

**Run:**
```bash
./echoint
```

**Expected behavior:**
- Reads an integer using scanf()
- Prints it back using printf()
- Shows format specifiers (%d)

**Example session:**
```
42
42
```

### sizeof1 - Type Sizes

**Run:**
```bash
./sizeof1
```

**Expected output (system-dependent):**
```
sizeof(char) = 1
sizeof(int) = 4
sizeof(long) = 8
sizeof(float) = 4
sizeof(double) = 8
sizeof(void*) = 8
```

**What it demonstrates:**
- sizeof() operator usage
- Type sizes on your system
- Pointer size (architecture-dependent)
- Why sizeof is important for portability

### bitMunger - Interactive Bit Manipulation

**Run:**
```bash
./bitMunger
```

**Expected behavior:**
- Prompts: "Enter an integer: "
- Displays the integer value
- Shows all 32 bits of the integer
- Prompts: "Which bit to flip?: "
- Flips the specified bit (0-31)
- Repeats indefinitely (Ctrl+C to exit)

**Example session:**
```
Enter an integer: 5
The number is: 5
00000000000000000000000000000101
Which bit to flip?: 0
The number is: 4
00000000000000000000000000000100
Which bit to flip?: 3
The number is: 12
00000000000000000000000000001100
Which bit to flip?: 2
The number is: 8
00000000000000000000000000001000
^C
```

**What it demonstrates:**
- Binary representation of integers
- Bit indexing (rightmost bit is bit 0)
- XOR operation for bit flipping
- Bitwise AND and shift for bit reading
- Interactive loops in C

### malloc1 - Dynamic Memory

**Run:**
```bash
./malloc1
```

**Expected output:**
```
g->n1 is 10
g->n2 is 20
```

**What it demonstrates:**
- Heap allocation with malloc()
- sizeof() with structs
- Pointer dereferencing with ->
- Struct member access

## Key Concepts Demonstrated

### C vs C++ Differences

**From Agenda.org:**
- Different libraries (`stdio.h` not `iostream`)
- No boolean type (use 1 and 0)
- Must use `struct` keyword with struct types
- printf/scanf instead of cout/cin
- gcc compiler instead of g++
- `char*` instead of string class
- malloc/free instead of new/delete
- No pass by reference - only pointers!

### Bitwise Operations

**Operators:**
- `&` - Bitwise AND
- `|` - Bitwise OR
- `^` - Bitwise XOR (exclusive or)
- `>>` - Right shift
- `<<` - Left shift

**Truth tables:**
```
AND:  1 & 1 = 1,  1 & 0 = 0,  0 & 0 = 0
OR:   1 | 1 = 1,  1 | 0 = 1,  0 | 0 = 0
XOR:  1 ^ 1 = 0,  1 ^ 0 = 1,  0 ^ 0 = 0
```

**Example (from Agenda):**
```
  11100101
& 01110100
-----------
  01100100

  11100101
| 01110100
-----------
  11110101

  11100101
^ 01110100
-----------
  10010001
```

### Bit Shifts

**Right shift (>>):**
- Shifts bits right, fills with 0s on left
- Divides by powers of 2
- `10000000 >> 1 => 01000000`
- `10000000 >> 3 => 00010000`

**Left shift (<<):**
- Shifts bits left, fills with 0s on right
- Multiplies by powers of 2
- `00000001 << 1 => 00000010`
- `00000001 << 3 => 00001000`

### Integer Representations

**Two's Complement (for signed integers):**
- Most significant bit is sign bit
- For 8-bit number: range is -128 to 127
- General n-bit: range is -2^(n-1) to 2^(n-1)-1

**Example (8-bit):**
```
10010101 = -1*2^7 + 0*2^6 + 0*2^5 + 1*2^4 + 0*2^3 + 1*2^2 + 0*2^1 + 1*2^0
         = -128 + 16 + 4 + 1
         = -107
```

**Unsigned integers:**
- All bits contribute positively
- n-bit range: 0 to 2^n - 1

### Bit Manipulation Techniques

**Reading a specific bit:**
```c
// Read bit i of n
int bit_i = (n >> i) & 1;
```

**Flipping a specific bit:**
```c
// Flip bit i of n
n = n ^ (1 << i);
```

**Setting a bit to 1:**
```c
// Set bit i to 1
n = n | (1 << i);
```

**Clearing a bit to 0:**
```c
// Clear bit i to 0
n = n & ~(1 << i);
```

## Troubleshooting

### scanf Issues
```bash
# If scanf doesn't wait for input:
# 1. Flush stdout before scanf:
fflush(stdout);

# 2. Check format string matches variable type
scanf("%d", &num);  // For int, note the &

# 3. Check return value
if (scanf("%d", &num) != 1) {
    printf("Invalid input\n");
}
```

### Infinite Loop in bitMunger
```bash
# bitMunger loops forever by design
# Use Ctrl+C to exit

# Or modify the code to add exit condition:
scanf("%d", &choice);
if (choice < 0) break;  // Exit on negative input
```

### Bit Indexing Confusion
```bash
# Remember: bits are numbered right-to-left
# Bit 0 is rightmost (least significant)
# Bit 31 is leftmost (most significant) for 32-bit int

# Example: 0b00000101
# Bit 0 = 1
# Bit 1 = 0
# Bit 2 = 1
# Bits 3-31 = 0
```

### Compiler Errors
```bash
# "undefined reference to sqrt"
# Need to link math library: gcc file.c -o file -lm

# "implicit declaration of printf"
# Missing header: #include <stdio.h>

# "implicit declaration of malloc"
# Missing header: #include <stdlib.h>

# "format '%d' expects argument of type 'int *'"
# Missing & in scanf: scanf("%d", &num);  // Correct
```

## Notes

- This directory covers fundamental C concepts for beginners
- The Agenda.org provides lecture context and learning objectives
- Commented versions are extensively documented for self-study
- bitMunger is excellent for understanding binary representation visually
- Bit manipulation is essential for low-level programming, graphics, cryptography
- Understanding sizeof helps write portable code
- malloc is the foundation of dynamic data structures in C
- No booleans in C: use int with 0 (false) and non-zero (true)
- Always use & with scanf for simple types
- C strings are char* (arrays of characters ending with '\0')

## Related Topics

See also:
- `/home/user/cs201/spring2025/video2/` - Floating point and pointer arithmetic
- `/home/user/cs201/spring2025/video3/` - Assembly language introduction
- `/home/user/cs201/spring2025/in-class/` - More malloc and bit shift examples
- `/home/user/cs201/spring2025/assembly/` - How C compiles to assembly
