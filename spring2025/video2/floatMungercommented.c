#include <stdio.h>  // Standard Input/Output library for functions like printf and scanf
#include <stdlib.h> // Standard Library, often included though not strictly necessary for this specific code's core logic

/*
 * ========================== TUTORIAL PREFACE ==========================
 *
 * This program demonstrates fundamental bit manipulation techniques in C,
 * operating directly on the memory representation of a floating-point number.
 *
 * Key Concepts Illustrated:
 *
 * 1.  Binary Representation:
 *     All data in a computer is stored as sequences of bits (0s and 1s).
 *     An 'int' on many systems is 32 bits wide. A 'float' (following the
 *     IEEE 754 standard) is also typically 32 bits wide. This program
 *     exploits this size similarity.
 *
 * 2.  Bitwise Operators:
 *     C provides operators to manipulate individual bits:
 *     - `<<` (Left Shift): Shifts bits to the left, filling vacated positions
 *       on the right with 0s. `x << y` is equivalent to multiplying `x` by 2^y.
 *       Example: `00000101 << 1` becomes `00001010` (5 becomes 10).
 *     - `>>` (Right Shift): Shifts bits to the right. For unsigned integers or
 *       non-negative signed integers, vacated positions on the left are filled
 *       with 0s. `x >> y` is roughly equivalent to dividing `x` by 2^y.
 *       Example: `00001010 >> 1` becomes `00000101` (10 becomes 5).
 *     - `&` (Bitwise AND): Performs logical AND on each pair of corresponding bits.
 *       Result bit is 1 only if both input bits are 1. Used for masking (isolating bits).
 *       Example: `00001101 & 00001011` becomes `00001001` (13 & 11 = 9).
 *     - `^` (Bitwise XOR - Exclusive OR): Performs logical XOR on each pair of
 *       corresponding bits. Result bit is 1 if the input bits are different.
 *       Used for flipping bits.
 *       Example: `00001101 ^ 00001011` becomes `00000110` (13 ^ 11 = 6).
 *       Crucially: `x ^ 0 = x` (XOR with 0 leaves unchanged), `x ^ 1 = ~x` (XOR with 1 flips the bit).
 *
 * 3.  Pointers and Type Punning:
 *     - A pointer stores a memory address. `int* n` means 'n' holds the address
 *       of an integer. `*n` dereferences the pointer to access the integer value.
 *     - Type Punning: The code uses `(int*)&num`. `&num` gets the address of the
 *       `float` variable `num`. `(int*)` casts this address, telling the
 *       compiler to *treat* the memory at that address *as if* it were an
 *       integer. This allows accessing the raw 32 bits of the float directly.
 *     - **Caution**: While common for demonstration and often works when `sizeof(int)`
 *       equals `sizeof(float)`, relying on type punning through pointer casting can
 *       technically violate C's strict aliasing rules and might lead to undefined
 *       behavior or non-portable code, especially with compiler optimizations.
 *       A safer (but more complex) method often involves `memcpy` or unions.
 *
 * 4.  IEEE 754 Floating-Point Standard:
 *     Floats are not stored like integers. A 32-bit float typically has:
 *     - 1 Sign bit (Bit 31)
 *     - 8 Exponent bits (Bits 30-23)
 *     - 23 Mantissa/Significand bits (Bits 22-0)
 *     Flipping a single bit can drastically change the float's value in non-intuitive
 *     ways because it might alter the sign, exponent, or mantissa. The `printBits`
 *     function adds spaces to visually separate these parts.
 *
 * 5.  Function Parameters (Pass by Pointer):
 *     Functions like `printBits` and `flipBit` take `int* n` (a pointer). This
 *     allows the functions to work with the original variable's data in `main`.
 *     `flipBit` *needs* a pointer so it can modify the original `num` variable.
 *     `printBits` could technically take an `int` by value, but using a pointer
 *     is consistent here and avoids copying the 32 bits unnecessarily.
 *
 * ======================================================================
 */

// << left shift operator: Shifts bits to the left. Example: (1 << 3) is 00...001000 (8)
// >> right shift operator: Shifts bits to the right. Example: (8 >> 1) is 00...000100 (4)

// Function to print the binary representation of a 32-bit integer.
// It takes a pointer `n` to the integer whose bits are to be printed.
void printBits(int* n){
  // Assuming an integer is 32 bits wide on this system.
  // Loop from the most significant bit (MSB, position 31) down to the
  // least significant bit (LSB, position 0).
  for(int i = 31; i >= 0; i--){
    // 1. `*n`: Dereference the pointer to get the integer value.
    // 2. `(*n >> i)`: Right-shift the integer value by `i` positions.
    //    This moves the bit originally at position `i` to position 0 (the LSB).
    // 3. `& 1`: Perform a bitwise AND with the integer 1 (binary 00...001).
    //    This isolates the LSB. If the bit moved to position 0 was 1, the result is 1.
    //    If the bit was 0, the result is 0.
    printf("%d",(*n >> i) & 1); // Print the isolated bit (0 or 1).

    // Add spaces for readability, aligning with IEEE 754 float format parts:
    // After the sign bit (bit 31) and after the exponent bits (bits 30-23).
    if(i == 31 || i == 23){
      printf(" "); // Print a space separator.
    }
  }
  printf("\n"); // Print a newline character at the end of the bit string.
}

// Function to flip a specific bit in an integer.
// Takes a pointer `n` to the integer and the bit position `c` (0-31) to flip.
void flipBit(int* n, int c){
  // Ensure the bit position `c` is within the valid range (0 to 31).
  // (Note: The original code lacks this check, which could lead to issues if c is outside 0-31)
  if (c < 0 || c > 31) {
      printf("Error: Bit position %d is out of range (0-31).\n", c);
      return; // Exit the function if the position is invalid.
  }

  // To flip the c-th bit (0-indexed from the right):
  // 1. `1 << c`: Create a "mask". This expression results in an integer
  //    that has only the c-th bit set to 1, and all other bits are 0.
  //    Example: if c=3, (1 << 3) is 00...00001000 (binary representation of 8).
  // 2. `(*n) ^ (1 << c)`: Perform a bitwise XOR operation between the original
  //    integer value (`*n`) and the mask created in step 1.
  //    - Where the mask has a 0, the original bit in `*n` remains unchanged (x ^ 0 = x).
  //    - Where the mask has a 1 (only at position `c`), the original bit in `*n`
  //      is flipped (0 ^ 1 = 1, 1 ^ 1 = 0).
  // 3. `*n = ...`: Assign the result of the XOR operation back to the integer
  //    variable pointed to by `n`, effectively modifying the original number in `main`.
  *n = (*n) ^ (1 << c);
}

// Main function where the program execution begins.
int main(){

  float num;  // Declare a floating-point variable to store the user's input.
  int choice; // Declare an integer variable to store the user's chosen bit position.

  // Prompt the user to enter a floating-point number.
  printf("Enter a float: ");
  // Read the float input from the user and store it in the 'num' variable.
  // `&num` provides the memory address of `num` to scanf.
  scanf("%f",&num);

  // Start an infinite loop (the loop condition is always true).
  // Use Ctrl+C in the terminal to exit the program.
  while(1){
    // Print the current value of the float, formatted to 10 decimal places
    // to better observe potentially small changes after bit flips.
    printf("The number is: %.10f\n",num);

    // Call printBits to display the binary representation of the float.
    // `&num`: Get the memory address of the float variable `num`.
    // `(int*)`: Cast this address to an integer pointer (`int*`). This tells
    //           `printBits` to interpret the 32 bits at that address as an integer's bits.
    //           This is the "type punning" step.
    printBits((int*)&num);

    // Prompt the user to enter which bit position they want to flip (0-31).
    printf("Which bit to flip? (0-31): ");
    // Read the integer input from the user and store it in the 'choice' variable.
    scanf("%d",&choice);

    // Call flipBit to modify the number by flipping the specified bit.
    // Pass the address of `num` (casted to `int*`) and the chosen bit position `choice`.
    // `flipBit` will modify the bits stored in the memory location of `num`.
    flipBit((int*)&num, choice);

    // Add an extra newline for better spacing between loop iterations.
    printf("\n");
  } // End of the infinite while loop.

  // This line is theoretically unreachable because of the infinite `while(1)` loop.
  // It indicates successful program termination (if the loop could somehow exit).
  return 0;
}
