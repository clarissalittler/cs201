// PEDAGOGICAL PURPOSE:
// This program demonstrates the sizeof operator for various C data types,
// showing that type sizes are platform-dependent but follow certain rules.
// Key learning objectives:
// 1. Using sizeof to determine type sizes at compile time
// 2. Understanding that int, char, float, double, and long have different sizes
// 3. Type sizes can vary between platforms (32-bit vs 64-bit, different architectures)
// 4. The %lu format specifier for size_t (sizeof's return type)
// 5. The C standard guarantees minimum sizes but not exact sizes
// 6. The unusual syntax "long int unsigned" (equivalent to "unsigned long int")

#include <stdio.h>
#include <stdlib.h>

int main(){

  // SIZEOF OPERATOR:
  // sizeof is a compile-time operator (not a function!)
  // It returns the size in bytes of a type or expression
  // Return type: size_t (an unsigned integer type)
  // Format specifier: %zu (C99+) or %lu (older, but widely used)

  // INTEGER TYPE:
  // int is the "natural" integer size for the platform
  // C standard guarantees: at least 16 bits (2 bytes)
  // Typical values:
  //   32-bit systems: 4 bytes (32 bits)
  //   64-bit systems: 4 bytes (32 bits) - int doesn't grow on 64-bit!
  printf("The size of an int is: %lu\n",sizeof(int));

  // CHARACTER TYPE:
  // char is ALWAYS 1 byte by C standard definition
  // 1 byte = 8 bits on almost all modern systems
  // (Historically, some systems had 9-bit or 7-bit bytes, but very rare)
  // sizeof(char) is ALWAYS 1, by definition
  printf("The size of a char is: %lu\n",sizeof(char));

  // SINGLE-PRECISION FLOAT:
  // float is single-precision floating-point (IEEE 754)
  // C standard: doesn't specify exact size
  // IEEE 754 single-precision: 32 bits (4 bytes)
  // Typical on all modern systems: 4 bytes
  printf("The size of a float is: %lu\n",sizeof(float));

  // DOUBLE-PRECISION FLOAT:
  // double is double-precision floating-point (IEEE 754)
  // C standard: sizeof(double) >= sizeof(float)
  // IEEE 754 double-precision: 64 bits (8 bytes)
  // Typical on all modern systems: 8 bytes
  printf("The size of a double is: %lu\n",sizeof(double));

  // UNSIGNED LONG INT:
  // This line has an unusual syntax: "long int unsigned"
  // More common order: "unsigned long int"
  // Both mean the same thing - order of type specifiers doesn't matter
  //
  // long int (or long) is a "long integer"
  // C standard guarantees: at least 32 bits (4 bytes)
  // Typical values:
  //   32-bit systems: 4 bytes (same as int)
  //   64-bit Unix/Linux: 8 bytes (64 bits) - LP64 model
  //   64-bit Windows: 4 bytes (32 bits) - LLP64 model
  //
  // "unsigned" means no negative numbers, doubled positive range
  printf("The size of a long int is: %lu\n",sizeof(long int unsigned));

  return 0;
}

// COMPREHENSIVE EXPLANATION:
//
// C STANDARD SIZE GUARANTEES:
//
// The C standard specifies minimum sizes:
//   char:      exactly 1 byte (by definition)
//   short:     at least 16 bits (2 bytes)
//   int:       at least 16 bits (2 bytes)
//   long:      at least 32 bits (4 bytes)
//   long long: at least 64 bits (8 bytes) [C99+]
//
// Relations:
//   sizeof(char) <= sizeof(short) <= sizeof(int) <= sizeof(long) <= sizeof(long long)
//
// But exact sizes are implementation-defined!

// TYPICAL SIZES ON MODERN SYSTEMS:
//
// 32-bit systems (ILP32 model):
//   char:      1 byte
//   short:     2 bytes (16 bits)
//   int:       4 bytes (32 bits) ← "I" in ILP32
//   long:      4 bytes (32 bits) ← "L" in ILP32
//   long long: 8 bytes (64 bits)
//   pointer:   4 bytes (32 bits) ← "P" in ILP32
//
// 64-bit Unix/Linux (LP64 model):
//   char:      1 byte
//   short:     2 bytes (16 bits)
//   int:       4 bytes (32 bits) - doesn't grow!
//   long:      8 bytes (64 bits) ← "L" in LP64
//   long long: 8 bytes (64 bits)
//   pointer:   8 bytes (64 bits) ← "P" in LP64
//
// 64-bit Windows (LLP64 model):
//   char:      1 byte
//   short:     2 bytes (16 bits)
//   int:       4 bytes (32 bits)
//   long:      4 bytes (32 bits) - doesn't grow!
//   long long: 8 bytes (64 bits) ← "LL" in LLP64
//   pointer:   8 bytes (64 bits) ← "P" in LLP64

// FLOATING-POINT SIZES:
//
// IEEE 754 standard (almost universally adopted):
//   float:       32 bits (4 bytes)  - single precision
//   double:      64 bits (8 bytes)  - double precision
//   long double: 80 bits (10 bytes) or 128 bits (16 bytes) - extended precision
//
// long double varies:
//   x86: 80 bits (stored in 12 or 16 bytes with padding)
//   x86-64: same as x86
//   ARM: often same as double (64 bits)
//   Some systems: 128 bits (quad precision)

// THE sizeof OPERATOR:
//
// sizeof is NOT a function - it's an operator
// Evaluated at compile time (for most cases)
//
// Syntax:
//   sizeof(type)         // Size of a type
//   sizeof expression    // Size of expression's type (expression not evaluated!)
//
// Examples:
//   sizeof(int)          → 4 (on most systems)
//   sizeof(char)         → 1 (always)
//   sizeof(int*)         → 8 (on 64-bit systems)
//   sizeof arr           → size of entire array (if arr is array)
//   sizeof(arr)          → same as sizeof arr
//
// Important: sizeof on arrays vs pointers
//   int arr[10];
//   sizeof(arr) = 40 (10 ints × 4 bytes)
//
//   int* p = arr;
//   sizeof(p) = 8 (size of pointer, not array!)

// RETURN TYPE: size_t
//
// sizeof returns size_t, an unsigned integer type
// size_t is defined in <stddef.h> (automatically included by <stdio.h>)
//
// size_t is:
//   - Unsigned (can't be negative)
//   - Large enough to hold the size of any object
//   - Typically: unsigned int (32-bit) or unsigned long (64-bit)
//
// Format specifiers for size_t:
//   %zu - C99 standard, correct and portable
//   %lu - Works if size_t is unsigned long (common on 64-bit)
//   %u  - Works if size_t is unsigned int (common on 32-bit)

// TYPE SPECIFIER ORDER:
//
// In C, the order of type specifiers doesn't matter:
//   unsigned long int
//   long unsigned int  ← Used in this program
//   unsigned int long
//   int long unsigned
//   ... etc.
//
// All mean the same thing!
//
// Common style:
//   unsigned long int   ← Most readable
//   unsigned long       ← Shortened (int is implicit)

// SIGNED vs UNSIGNED:
//
// unsigned: no negative numbers, range doubled
//
// For 32-bit types:
//   int:          -2,147,483,648 to 2,147,483,647
//   unsigned int:              0 to 4,294,967,295
//
// For 8-bit types:
//   char (if signed): -128 to 127
//   unsigned char:       0 to 255

// CHECKING SIZES IN YOUR CODE:
//
// You can use sizeof for portable code:
//
//   int arr[100];
//   size_t arr_bytes = sizeof(arr);           // 400 bytes
//   size_t arr_elements = sizeof(arr) / sizeof(arr[0]);  // 100 elements
//
//   void* p = malloc(n * sizeof(int));        // Allocate n integers
//
// Never hardcode sizes:
//   BAD:  malloc(n * 4);           // Assumes int is 4 bytes
//   GOOD: malloc(n * sizeof(int)); // Works regardless of int size

// LIMITS AND RANGES:
//
// To get the actual ranges, use <limits.h>:
//
//   #include <limits.h>
//   INT_MIN, INT_MAX       - range of int
//   LONG_MIN, LONG_MAX     - range of long
//   CHAR_BIT               - bits in a char (usually 8)
//   UCHAR_MAX              - max value of unsigned char
//
// For floating-point, use <float.h>:
//   FLT_MIN, FLT_MAX       - range of float
//   DBL_MIN, DBL_MAX       - range of double

// PRACTICAL IMPLICATIONS:
//
// Why int is 4 bytes even on 64-bit systems:
// - Backward compatibility
// - 32 bits is enough for most integer arithmetic
// - Saves memory compared to 64-bit ints
//
// When to use long vs int:
// - Use int for most integer arithmetic
// - Use long when you need guaranteed 32+ bits
// - Use long long when you need 64 bits
// - Use int32_t, int64_t (from <stdint.h>) for exact sizes
//
// Why pointer size matters:
// - On 64-bit systems, pointers are 8 bytes
// - Arrays of pointers use more memory than on 32-bit
// - void* can't be cast to int without data loss on 64-bit

// FIXED-WIDTH INTEGER TYPES (C99):
//
// For exact sizes, use <stdint.h>:
//   int8_t    - exactly 8 bits (signed)
//   int16_t   - exactly 16 bits (signed)
//   int32_t   - exactly 32 bits (signed)
//   int64_t   - exactly 64 bits (signed)
//   uint8_t   - exactly 8 bits (unsigned)
//   uint16_t  - exactly 16 bits (unsigned)
//   uint32_t  - exactly 32 bits (unsigned)
//   uint64_t  - exactly 64 bits (unsigned)
//
// Use these when binary compatibility is critical

// TO COMPILE AND RUN:
// gcc size1.c -o size1
// ./size1
//
// Expected output on typical 64-bit Linux system:
// The size of an int is: 4
// The size of a char is: 1
// The size of a float is: 4
// The size of a double is: 8
// The size of a long int is: 8
//
// Expected output on typical 32-bit system:
// The size of an int is: 4
// The size of a char is: 1
// The size of a float is: 4
// The size of a double is: 8
// The size of a long int is: 4  ← Different!
//
// Expected output on typical 64-bit Windows:
// The size of an int is: 4
// The size of a char is: 1
// The size of a float is: 4
// The size of a double is: 8
// The size of a long int is: 4  ← Different! (LLP64 model)
