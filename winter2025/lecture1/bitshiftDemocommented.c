// PEDAGOGICAL PURPOSE:
// This minimal program demonstrates the right shift operator (>>) and its effect
// on integer values, showing how bit shifting is equivalent to division by powers of 2.
// Key learning objectives:
// 1. Understanding the right shift operator (>>)
// 2. Right shift as integer division by powers of 2
// 3. Bit-level operations as efficient alternatives to arithmetic
// 4. The concept of unused variables (num is declared but never used)
// 5. Simple demonstration programs for testing concepts

#include <stdio.h>
#include <stdlib.h>

int main(){

  // UNUSED VARIABLE:
  // This variable is declared and initialized but never used
  // The compiler may warn about this with -Wall flag
  // Likely left over from experimentation or incomplete code
  int num = 1;

  // RIGHT SHIFT DEMONSTRATION:
  // This line demonstrates the >> (right shift) operator
  //
  // WHAT IS 1024 >> 2?
  // Let's break it down step by step:
  //
  // 1024 in binary: 10000000000 (bit 10 is set, all others are 0)
  //                 ↑
  //                 This is the only 1 bit (at position 10)
  //
  // Right shift by 2 means: move all bits 2 positions to the right
  //
  // Before: 10000000000  (1024 in decimal)
  // After:  00100000000  (256 in decimal)
  //         ↑
  //         The 1 bit moved from position 10 to position 8
  //
  // MATHEMATICAL INTERPRETATION:
  // Right shifting by n positions divides by 2^n (with integer truncation)
  // 1024 >> 2 = 1024 / 2^2 = 1024 / 4 = 256
  //
  // WHY USE BIT SHIFTING INSTEAD OF DIVISION?
  // - Historically: bit shifts were faster than division on CPUs
  // - Modern compilers: optimize division by powers of 2 to shifts automatically
  // - Clarity: shifts make it explicit that you're working at the bit level
  // - Use case: when working with binary data structures, flags, or protocols
  printf("%d\n",1024 >> 2);

  // EXPECTED OUTPUT: 256

  // PROGRAM EXIT:
  return 0;
}

// DETAILED EXPLANATION OF RIGHT SHIFT:
//
// The right shift operator (>>) has two behaviors:
//
// 1. LOGICAL RIGHT SHIFT (for unsigned integers):
//    - Shifts bits right
//    - Fills empty bits on the left with 0s
//    - Example: 12 >> 2 = 3
//      12 in binary: 00001100
//      12 >> 1:      00000110  (6)
//      12 >> 2:      00000011  (3)
//
// 2. ARITHMETIC RIGHT SHIFT (for signed integers):
//    - Shifts bits right
//    - Fills empty bits with the SIGN BIT (leftmost bit)
//    - This preserves the sign of negative numbers
//    - Example: -8 >> 2 = -2 (on most systems)
//      -8 in two's complement: 11111111111111111111111111111000
//      -8 >> 2:                11111111111111111111111111111110  (-2)
//                              ↑↑ Sign bits filled in
//
// IN THIS PROGRAM:
// 1024 is a positive literal, treated as signed int
// Since it's positive, the sign bit is 0
// Right shifting fills with 0s, so it behaves like logical shift
//
// COMPARISON: SHIFT vs DIVISION:
//
// Shift:    1024 >> 2  = 256
// Division: 1024 / 4   = 256  (equivalent!)
//
// More examples:
// 100 >> 1 = 50   (same as 100 / 2)
// 100 >> 2 = 25   (same as 100 / 4)
// 100 >> 3 = 12   (same as 100 / 8)
//
// TRUNCATION WITH ODD NUMBERS:
// When the number isn't evenly divisible, right shift truncates (rounds down)
// 15 >> 1 = 7   (not 7.5, truncates like integer division)
// 15 / 2  = 7   (same truncation)
//
// 15 in binary: 1111
// 15 >> 1:      0111  (7)
//               ↑ The rightmost 1 bit is lost

// LEFT SHIFT OPERATOR (<<) FOR COMPARISON:
//
// While this program doesn't use it, here's how left shift works:
//
// Left shift (<<) multiplies by powers of 2:
// 1 << 2 = 4    (1 * 2^2 = 4)
// 5 << 3 = 40   (5 * 2^3 = 40)
//
// 5 in binary:  0101
// 5 << 1:       1010  (10)
// 5 << 2:       0100  (20)  -- wait, why 0100?
//               Actually: 00010100 when properly extended
// 5 << 3:       00101000  (40)

// PRACTICAL USES OF BIT SHIFTING:
//
// 1. Fast multiplication/division by powers of 2:
//    x << 3  is faster than  x * 8
//    x >> 2  is faster than  x / 4
//
// 2. Extracting/setting bit fields:
//    color = (rgb >> 16) & 0xFF;  // Extract red channel
//
// 3. Creating bit masks:
//    mask = 1 << bit_position;
//
// 4. Efficient array indexing with power-of-2 sizes:
//    index = hash >> 3;  // Divide by 8 to map into buckets
//
// 5. Binary protocols and data structures:
//    flags = (readable << 2) | (writable << 1) | executable;

// EDGE CASES AND WARNINGS:
//
// 1. SHIFTING BY NEGATIVE AMOUNTS:
//    1024 >> -1  // UNDEFINED BEHAVIOR! Don't do this
//
// 2. SHIFTING BY AMOUNTS >= BIT WIDTH:
//    1024 >> 32  // UNDEFINED BEHAVIOR for 32-bit ints!
//    (Shifting by the width of the type or more is undefined)
//
// 3. SHIFTING NEGATIVE NUMBERS:
//    -5 >> 1     // Implementation-defined behavior
//    (Most systems do arithmetic right shift, but not guaranteed by C standard)
//
// 4. LEFT SHIFT OF NEGATIVE NUMBERS:
//    -5 << 1     // UNDEFINED BEHAVIOR
//
// 5. OVERFLOW WITH LEFT SHIFT:
//    1000000 << 10  // May overflow, causing undefined behavior

// WHY THE UNUSED VARIABLE?
//
// The variable 'num' is declared but never used. This might be because:
// - This is a minimal demo that was simplified from a larger program
// - The instructor wanted to show that you CAN declare variables you don't use
//   (though it's poor practice and causes compiler warnings)
// - It was meant to be used in experiments but got left in
//
// Good practice: remove unused variables or comment them out

// TO COMPILE AND RUN:
// gcc bitshiftDemo.c -o bitshiftDemo
// ./bitshiftDemo
// Output: 256
//
// TO SEE COMPILER WARNING ABOUT UNUSED VARIABLE:
// gcc -Wall bitshiftDemo.c -o bitshiftDemo
// warning: unused variable 'num' [-Wunused-variable]
