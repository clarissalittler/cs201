// PEDAGOGICAL PURPOSE:
// This program demonstrates a fundamental problem with floating-point arithmetic:
// accumulation of rounding errors that make equality comparisons unreliable.
// Key learning objectives:
// 1. Understanding that floating-point arithmetic is inexact
// 2. Why 0.1 + 0.1 + ... (10000 times) ≠ 10.0 exactly
// 3. The danger of using == with floating-point numbers
// 4. Rounding errors accumulate with repeated operations
// 5. IEEE 754 binary representation limitations
// 6. Best practices for floating-point comparison

#include <stdio.h>
#include <stdlib.h>

int main(){

  // INITIALIZE ACCUMULATORS:
  // num1 will accumulate the sum
  // num2 is the value we'll add repeatedly
  double num1 = 0;
  double num2 = 0.001;

  // REPEATED ADDITION:
  // Add 0.001 to num1, 10000 times
  // Mathematically: 0 + 0.001 × 10000 = 10.0
  // But in floating-point: accumulates rounding errors
  for(int i = 0; i < 10000; i++){
    num1 = num1 + num2;
  }

  // THE PROBLEMATIC COMPARISON:
  // We expect num1 == 10.0, but this often fails!
  //
  // WHY IT FAILS:
  // 0.001 cannot be represented exactly in binary floating-point
  // Each addition introduces tiny rounding errors
  // After 10000 additions, errors accumulate
  // num1 ends up very close to 10.0, but not exactly 10.0
  if(num1 == 10){

    // THIS BRANCH RARELY EXECUTES:
    // You'd expect this to print, but it usually doesn't!
    printf("Yee-caw!\n");
  }
  else {

    // THIS BRANCH USUALLY EXECUTES:
    // The equality test failed due to rounding errors
    printf("Whoops!\n");

    // REVEAL THE ACTUAL VALUE:
    // %.20f shows 20 decimal places to reveal the error
    // The actual value will be something like:
    //   10.00000000000001776357...
    // or
    //   9.99999999999998223643...
    // Very close to 10.0, but not exactly 10.0
    printf("num1 is really: %.20f\n",num1);
  }

  return 0;
}

// COMPREHENSIVE EXPLANATION:
//
// WHY 0.001 CANNOT BE REPRESENTED EXACTLY:
//
// Decimal 0.001 = 1/1000
// In binary, this requires a repeating fraction
//
// Converting to binary:
//   0.001 in binary ≈ 0.00000001000001100101100... (repeating pattern)
//
// Double precision can only store 53 bits of precision
// The repeating pattern must be rounded/truncated
// Result: 0.001 is approximated, not exact
//
// Stored value: ~0.0010000000000000000208166817...
// Error per addition: ~0.0000000000000000002...
//
// After 10000 additions:
// Total error ≈ 10000 × 0.0000000000000000002... ≈ 0.000000000000002...

// DOUBLE-PRECISION FORMAT (IEEE 754):
//
// 64 bits total:
//   1 bit:  sign
//   11 bits: exponent (biased by 1023)
//   52 bits: mantissa (fraction), plus 1 implicit bit = 53 bits precision
//
// Decimal precision: ~15-17 significant digits
// Range: ~10^-308 to 10^308
//
// Even with this high precision, many simple decimals are not exactly representable

// WHICH NUMBERS CAN BE REPRESENTED EXACTLY?
//
// Exactly representable:
// - Powers of 2: 0.5, 0.25, 0.125, 1.0, 2.0, 4.0, etc.
// - Sums of powers of 2: 0.75 (0.5 + 0.25), 1.5 (1.0 + 0.5)
// - Any number of form: integer / 2^n
//   Examples: 0.5 (1/2), 0.375 (3/8), 6.25 (25/4)
//
// NOT exactly representable:
// - 0.1 (1/10) - requires infinite binary representation
// - 0.2 (1/5)  - requires infinite binary representation
// - 0.3 (3/10) - requires infinite binary representation
// - 0.001 (1/1000) - used in this program
//
// Why?
// Because these denominators (10, 5, 1000) have prime factors other than 2
// Binary can only exactly represent fractions with denominators that are powers of 2

// ERROR ACCUMULATION:
//
// Single addition error is tiny:
//   0.001 stored as ≈ 0.0010000000000000000208...
//   Error ≈ 0.0000000000000000002...
//
// After 1 addition:
//   Error ≈ 0.0000000000000000002...
//
// After 10000 additions:
//   Errors accumulate (not cancel out!)
//   Total error ≈ 0.000000000000002... (approximately)
//   Result: 10.00000000000001776... or 9.99999999999998223...
//
// The exact error depends on:
// - Rounding mode (round-to-nearest, round-down, etc.)
// - Order of operations
// - Compiler optimizations

// CORRECT WAYS TO COMPARE FLOATS:
//
// 1. EPSILON COMPARISON (most common):
//    const double EPSILON = 1e-9;  // Small tolerance
//    if (fabs(num1 - 10.0) < EPSILON) {
//      // Close enough!
//    }
//
// 2. RELATIVE EPSILON (for large or small numbers):
//    const double REL_EPSILON = 1e-9;
//    if (fabs(num1 - 10.0) < REL_EPSILON * fabs(10.0)) {
//      // Close enough!
//    }
//
// 3. ULP (Units in Last Place) comparison:
//    More sophisticated, considers floating-point representation
//    Checks if numbers are within N representable values of each other
//
// NEVER use == for floating-point unless you know values are exact
// (e.g., comparing with 0.0 after explicit assignment might be safe)

// ALTERNATIVE: EXACT DECIMAL ARITHMETIC
//
// For applications requiring exact decimal arithmetic (e.g., finance):
// 1. Use integer arithmetic with scaling:
//    int cents = 1;  // 0.001 dollars = 0.1 cents
//    int total = 0;
//    for (int i = 0; i < 10000; i++) {
//      total += cents;
//    }
//    // total == 10000 (exact!)
//    // Convert to dollars: total / 1000.0
//
// 2. Use decimal libraries:
//    GMP (GNU Multiple Precision)
//    MPFR (Multiple Precision Floating-Point)
//    decNumber library
//
// 3. Use languages with built-in decimal types:
//    Python: decimal.Decimal
//    Java: BigDecimal

// DEMONSTRATING THE PROBLEM:
//
// Try different values to see when equality works and when it fails:
//
// WORKS (powers of 2):
//   double num1 = 0;
//   double num2 = 0.5;  // 1/2 - exactly representable
//   for (int i = 0; i < 20; i++) num1 += num2;
//   if (num1 == 10.0) { ... }  // WORKS! 0.5 is exact
//
// FAILS (not powers of 2):
//   double num1 = 0;
//   double num2 = 0.1;  // 1/10 - NOT exactly representable
//   for (int i = 0; i < 100; i++) num1 += num2;
//   if (num1 == 10.0) { ... }  // FAILS! Accumulates errors

// COMPILER OPTIMIZATIONS:
//
// With optimization, compiler might:
// 1. Constant-fold: compute 0.001 * 10000 at compile time
// 2. Reorder operations
// 3. Use extended precision (80-bit) during computation
//
// This can make the result MORE or LESS accurate!
//
// Compile without optimization:
//   gcc -O0 floatTest.c -o floatTest
//   ./floatTest
//   # More likely to fail equality test
//
// Compile with optimization:
//   gcc -O3 floatTest.c -o floatTest
//   ./floatTest
//   # Might pass or fail, depending on optimizations

// LANGUAGE COMPARISONS:
//
// Python:
//   >>> sum(0.001 for _ in range(10000))
//   9.999999999999831
//   >>> sum(0.001 for _ in range(10000)) == 10.0
//   False
//
// JavaScript:
//   let sum = 0;
//   for (let i = 0; i < 10000; i++) sum += 0.001;
//   sum === 10.0  // false
//
// Java:
//   double sum = 0;
//   for (int i = 0; i < 10000; i++) sum += 0.001;
//   sum == 10.0  // false
//
// All have the same problem - IEEE 754 is universal

// IEEE 754 STANDARD:
//
// Adopted by most programming languages and hardware
// Pros:
// - Fast (hardware support)
// - Wide range
// - Good precision for most applications
//
// Cons:
// - Inexact for many decimal numbers
// - Counter-intuitive behavior (0.1 + 0.2 ≠ 0.3)
// - Requires careful handling of comparisons
// - Special values (NaN, Infinity) can propagate

// SPECIAL EDGE CASES:
//
// 0.1 + 0.2 in IEEE 754:
//   0.1 stored as: 0.1000000000000000055511151...
//   0.2 stored as: 0.2000000000000000111022302...
//   Sum:           0.3000000000000000444089210...
//   0.3 stored as: 0.2999999999999999888977698...
//   0.1 + 0.2 == 0.3  // FALSE!
//
// Catastrophic cancellation:
//   double a = 1.0e20;
//   double b = 1.0;
//   double c = 1.0e20;
//   (a + b) - c  // May be 0 instead of 1!
//   // Loss of precision when adding very different magnitudes

// BEST PRACTICES:
//
// 1. Never use == for float/double unless you know values are exact
// 2. Use epsilon comparison for equality tests
// 3. Minimize number of operations (errors accumulate)
// 4. Be aware that order of operations matters
// 5. For money/finance, use integer arithmetic or decimal libraries
// 6. Test with edge cases (very large, very small, NaN, infinity)
// 7. Understand your application's precision requirements

// TO COMPILE AND RUN:
// gcc floatTest.c -o floatTest
// ./floatTest
//
// Expected output:
// Whoops!
// num1 is really: 10.00000000000001776357
// (or similar, exact value may vary slightly by system)
//
// TO SEE EFFECT OF OPTIMIZATION:
// gcc -O0 floatTest.c -o floatTest && ./floatTest
// gcc -O3 floatTest.c -o floatTest && ./floatTest
// (Results might differ!)
