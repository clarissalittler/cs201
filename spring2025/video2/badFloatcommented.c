/**
 * @file float_inaccuracy_demo.c
 * @brief Educational example demonstrating floating-point inaccuracy.
 *
 * ## Tutorial: Understanding Floating-Point Numbers and Inaccuracy ##
 *
 * 1.  **What are Floating-Point Numbers?**
 *     Computers need a way to represent real numbers (numbers with decimal points,
 *     like 3.14 or -0.001). Floating-point types (`float`, `double`, `long double`
 *     in C) are used for this. They store numbers in a format similar to scientific
 *     notation (e.g., 1.23 x 10^4), but using base-2 (binary) instead of base-10 (decimal).
 *     The standard format is typically IEEE 754.
 *
 * 2.  **Binary Representation:**
 *     Computers store everything in binary (0s and 1s). Just like some fractions
 *     cannot be represented perfectly in decimal (e.g., 1/3 = 0.3333...), some
 *     decimal fractions cannot be represented perfectly *exactly* in binary using
 *     a finite number of bits.
 *
 * 3.  **The Problem with 0.01:**
 *     The decimal number 0.01 (or 1/100) is one such number. It has a non-terminating,
 *     repeating representation in binary.
 *     Decimal: 0.01
 *     Binary (approx): 0.00000010100011110101110000101... (repeating pattern)
 *     Since a `float` (or `double`) has a limited number of bits to store the fractional
 *     part, it can only store an *approximation* of 0.01.
 *
 * 4.  **Accumulation of Error:**
 *     This program adds the approximate binary representation of 0.01 to a running
 *     total 100 times. Each addition introduces a tiny error because we're not adding
 *     *exactly* 0.01. These tiny errors accumulate over the 100 iterations.
 *
 * 5.  **`float` vs. `double`:**
 *     - `float`: Single-precision floating-point. Uses fewer bits (typically 32).
 *       Less precise, smaller range, but uses less memory.
 *     - `double`: Double-precision floating-point. Uses more bits (typically 64).
 *       More precise, larger range, but uses more memory.
 *     While `double` is more precise than `float`, it still cannot represent 0.01
 *     *exactly*, so using `double` would result in a *smaller* error, but not zero error.
 *
 * 6.  **The Demonstration:**
 *     We expect 100 * 0.01 to equal exactly 1.0. However, due to the accumulated
 *     representation errors, the final value stored in the `float` variable `f`
 *     will be slightly different from 1.0. Printing the result with high precision
 *     (`%.40f`) makes this tiny difference visible.
 *
 * 7.  **Key Takeaway:**
 *     - Floating-point arithmetic is approximate, not exact, for many decimal values.
 *     - Never rely on exact equality comparisons (e.g., `if (f == 1.0)`) with
 *       floating-point numbers. Instead, check if the difference between two numbers
 *       is within a small tolerance (epsilon): `if (fabs(f - 1.0) < 0.00001)`.
 *     - Be aware that small errors can accumulate in loops or long calculations.
 *     - Use `double` when higher precision is needed, but understand it doesn't
 *       eliminate the fundamental representation issue.
 */

// Include the Standard Input/Output library for functions like printf.
#include <stdio.h>
// Include the Standard Library, which contains general utilities (though not strictly needed for this specific code, it's common practice).
#include <stdlib.h>

// The main function - the entry point of every C program.
int main() {
  // Declare a variable 'f' of type 'float'.
  // 'float' is a single-precision floating-point type, used to store numbers with decimal points.
  // Initialize 'f' to 0.0.
  float f = 0.0f; // Using 0.0f explicitly denotes a float literal, though 0 works too.

  // Start a 'for' loop that will execute 100 times.
  // 'i' is the loop counter, starting at 0 and incrementing by 1 in each iteration.
  // The loop continues as long as 'i' is less than 100 (i.e., for i = 0, 1, 2, ..., 99).
  for (int i = 0; i < 100; i++) {
    // In each iteration, add the value 0.01 to the current value of 'f'.
    // IMPORTANT: The number 0.01 cannot be represented *exactly* in binary floating-point.
    // The computer uses the closest possible approximation.
    // Each addition potentially adds a tiny representation error.
    f += 0.01f; // Using 0.01f explicitly denotes a float literal.
                // Adding the *approximation* of 0.01 repeatedly accumulates error.
  }

  // Print the final value of 'f' to the console.
  // `printf` is the function used for formatted output.
  // `"Our number is %.40f\n"` is the format string:
  //   - `"Our number is "` is literal text to be printed.
  //   - `%f` is a format specifier for printing a floating-point number (float or double).
  //   - `.40` before the `f` specifies that we want to print the number with 40 digits
  //     after the decimal point. This high precision is used specifically to make the
  //     small accumulated error visible. Without it (e.g., using just `%f`), the output
  //     might be rounded to 1.000000, hiding the inaccuracy.
  //   - `\n` represents a newline character, moving the cursor to the next line after printing.
  // The variable `f` provides the value to be formatted and printed according to `%.40f`.
  // We expect 1.0, but due to accumulated errors, we will likely see something slightly different.
  printf("Our number is %.40f\n", f);

  // Return 0 from main to indicate that the program executed successfully.
  return 0;
}

/* Expected output (will vary slightly depending on compiler/architecture but will NOT be exactly 1.0):
   Something like: Our number is 1.0000001192092895507812500000000000000000
   or             Our number is 0.9999999403953552246093750000000000000000
*/
