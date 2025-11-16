// PEDAGOGICAL PURPOSE:
// This program demonstrates floating-point precision errors and proper comparison techniques.
// Key learning objectives:
// 1. Floating-point arithmetic is NOT exact (due to binary representation limitations)
// 2. NEVER use == to compare floating-point numbers
// 3. Understanding accumulation of rounding errors in loops
// 4. Proper technique: use epsilon-based comparison (check if "close enough")
// 5. Why seemingly simple decimal numbers (0.00001) can't be exactly represented in binary
//
// THE CORE PROBLEM:
// Just as 1/3 = 0.333... cannot be exactly represented in decimal (infinite repeating),
// many decimal fractions cannot be exactly represented in binary floating-point.
// 0.00001 in decimal = 0.00000000000101000111101... (repeating pattern in binary)

#include "stdio.h"
#include "stdlib.h"

int main(){

  // SETUP:
  // We're going to add 0.00001 to itself 100,000 times
  // Mathematically: 0.00001 × 100,000 = 1.0
  // But will floating-point arithmetic agree?
  double num = 0.00001;   // The number to add repeatedly
  double acc = 0;         // Accumulator (should reach 1.0)


  // ACCUMULATION LOOP:
  // We add 0.00001 exactly 100,000 times
  // In theory, this should give us exactly 1.0
  for(int i = 0; i < 100000; i++){
    acc = acc + num;

    // WHAT ACTUALLY HAPPENS:
    // Each addition introduces a tiny rounding error because 0.00001
    // cannot be exactly represented in binary floating-point format.
    // These tiny errors accumulate over 100,000 iterations.
  }

  // WRONG WAY TO COMPARE FLOATS:
  // This demonstrates why you should NEVER use == with floating-point numbers
  // Due to accumulated rounding errors, acc is probably something like:
  // 1.0000000000139778 or 0.9999999999860222 (not exactly 1.0)
  if(acc != 1){
    // This will almost certainly execute because acc ≠ 1.0 exactly
    // The %f format shows the difference from our expected value
    printf("UH OH %f is not equal to 1\n",acc);
  }

  // RIGHT WAY TO COMPARE FLOATS:
  // Use an EPSILON comparison: check if the values are "close enough"
  // We define "close enough" as within ±0.000001 of the expected value
  if((acc < 1 + 0.000001) && acc > (1 - 0.000001)){
    // This checks if acc is in the range [0.999999, 1.000001]
    // This is the proper way to compare floating-point numbers
    // We accept values that are within our tolerance (epsilon)
    printf("But it's close enough!\n");
  }

  // TYPICAL OUTPUT:
  // UH OH 1.000000 is not equal to 1
  // But it's close enough!
  //
  // Note: printf's %f format defaults to 6 decimal places, so the difference
  // might not be visible, but it exists at the bit level.

  return 0;
}

// CONCEPTUAL EXPLANATION:
//
// WHY DOES THIS HAPPEN?
// Floating-point numbers use binary (base-2) representation, just like integers.
// But not all decimal fractions can be exactly represented in binary.
//
// ANALOGY:
// In decimal, 1/3 = 0.333333... (infinite repeating)
// We can never write it exactly; we must round it.
//
// Similarly, in binary:
// 0.1 (decimal) = 0.0001100110011... (infinite repeating in binary)
// 0.00001 (decimal) also has a repeating pattern in binary
//
// When we store these in a float/double (which has finite bits),
// we must round them to the nearest representable value.
//
// ACCUMULATED ERRORS:
// Each time we add num to acc, we add a tiny error.
// After 100,000 iterations, these errors accumulate to a noticeable difference.
//
// THE LESSON:
// 1. NEVER use == or != to compare floats/doubles
// 2. ALWAYS use an epsilon-based comparison: abs(a - b) < epsilon
// 3. Be aware that floating-point arithmetic trades precision for range
// 4. For financial calculations, use integer arithmetic (cents, not dollars)
//
// PROPER COMPARISON PATTERN:
// Instead of: if (a == b)
// Use: if (fabs(a - b) < EPSILON)  where EPSILON = 0.000001 or smaller
//
// FURTHER EXPLORATION:
// Try changing num to 0.1 or 0.2 and see similar effects.
// Try using float instead of double - errors will be worse (less precision).
// Try adding 0.5 (which CAN be exactly represented in binary) - no error!
