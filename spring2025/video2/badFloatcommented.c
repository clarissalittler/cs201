// PEDAGOGICAL PURPOSE:
// This program demonstrates floating-point representation inaccuracy in computers.
// It shows why floating-point arithmetic is not exact and why errors accumulate.
// Key learning objectives:
// 1. Understanding that floating-point numbers are approximations
// 2. IEEE 754 floating-point representation limitations
// 3. Why certain decimal numbers can't be represented exactly in binary
// 4. Accumulation of rounding errors in iterative calculations
// 5. The difference between mathematical precision and computer precision
// 6. Why you should never use == to compare floating-point numbers

#include <stdio.h>      // STANDARD I/O: For printf function
#include <stdlib.h>     // STANDARD LIBRARY: General utilities (not used here, but good practice)

int main(){

  float f = 0;          // INITIALIZE FLOAT: Start with exactly 0.0
                        // float is a 32-bit IEEE 754 single-precision floating-point number
                        // Format: 1 sign bit + 8 exponent bits + 23 mantissa bits
                        // This gives approximately 7 decimal digits of precision

  for(int i = 0; i < 100; i++){  // LOOP 100 TIMES: Add 0.01 repeatedly
                                  // We expect: 100 * 0.01 = 1.0 (mathematically)
                                  // But in binary floating-point, this won't be exact!

    f += 0.01;          // ADD 0.01: This is where the problem occurs!
                        // WHY IS THIS PROBLEMATIC?
                        // - 0.01 in decimal = 1/100
                        // - In binary: 0.01 = 0.00000010100011110101110... (repeating)
                        // - A float can only store a finite approximation of this
                        // - Each addition compounds the tiny approximation error
                        // - After 100 iterations, the errors accumulate noticeably
                        //
                        // The actual value stored is approximately:
                        // 0.00999999977648258209228515625 (not exactly 0.01)
  }

  printf("Our number is %.40f\n",f);  // PRINT WITH HIGH PRECISION: Show 40 decimal places
                                      // %.40f means print as floating-point with 40 digits after decimal
                                      // This reveals the accumulated inaccuracy
                                      // Expected result: 1.0000000000...
                                      // Actual result: ~0.9999999...something (close but not exact)
                                      //
                                      // WHY 40 DIGITS?
                                      // Even though floats only have ~7 digits of precision,
                                      // printf will show all 40 to demonstrate the error clearly
                                      // The digits beyond the 7th are essentially random/meaningless

  return 0;             // SUCCESS: Return 0 to indicate successful completion
}

// EXPECTED OUTPUT:
// Our number is 1.0000000149011611938476562500000000000000
// (The exact output may vary slightly by compiler/system)
// Notice it's NOT exactly 1.0!

// WHY THIS HAPPENS - BINARY REPRESENTATION:
//
// Decimal fractions like 0.1, 0.01, 0.001 cannot be represented exactly in binary,
// just like 1/3 (0.333...) cannot be represented exactly in decimal.
//
// Examples:
// - 0.1 (decimal) = 0.0001100110011001100110011... (repeating in binary)
// - 0.01 (decimal) = 0.0000001010001111010111... (repeating in binary)
//
// Since floats have finite bits (23 for mantissa), they must round/truncate.
// This introduces a small error that accumulates over many operations.

// KEY TAKEAWAYS:
//
// 1. NEVER compare floats with ==
//    BAD:  if (f == 1.0) { ... }
//    GOOD: if (fabs(f - 1.0) < 0.0001) { ... }  // Check if "close enough"
//
// 2. Use double for better precision (64 bits instead of 32)
//    - double would still have this problem, just smaller error
//    - Still not exact, but more accurate
//
// 3. For exact decimal arithmetic, use:
//    - Integer arithmetic with fixed decimal places (e.g., cents instead of dollars)
//    - Specialized decimal libraries
//
// 4. Floating-point is designed for:
//    - Scientific computing where approximation is acceptable
//    - Large range of values (very small to very large)
//    - Speed over exactness

// TO COMPILE AND RUN:
// gcc badFloat.c -o badFloat
// ./badFloat

// EXPERIMENT:
// Try changing float to double and see the difference:
// - The error will be smaller but still present
// - double has 53 mantissa bits vs float's 23 bits
// - This gives ~15-16 decimal digits of precision vs ~7 for float
