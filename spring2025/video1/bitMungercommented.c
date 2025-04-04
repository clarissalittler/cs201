/**
 * @file bit_manipulation_example.c
 * @brief Educational example demonstrating bitwise operations in C.
 *
 * TUTORIAL: Understanding Bits and Bitwise Operations
 *
 * 1. What are Bits?
 *    - Computers store and process information using binary digits, called 'bits'.
 *    - A bit can have only two values: 0 or 1.
 *    - Bits are grouped together to represent more complex data. A common grouping is a 'byte', which consists of 8 bits.
 *
 * 2. Integer Representation:
 *    - In C, an `int` (integer) is typically represented using a fixed number of bits, commonly 32 bits (4 bytes) on many systems, although this can vary.
 *    - These 32 bits are used to store the number's value in binary format. For positive numbers, it's a straightforward binary conversion. For negative numbers, typically 'Two's Complement' representation is used.
 *    - The bits are ordered from the 'Most Significant Bit' (MSB) on the left to the 'Least Significant Bit' (LSB) on the right. The MSB determines the sign in signed integers (0 for positive, 1 for negative in Two's Complement).
 *    - Example (8-bit for simplicity): The number 5 is `00000101`. The number 9 is `00001001`.
 *
 * 3. Bitwise Operators:
 *    - These operators work directly on the individual bits of their operands.
 *
 *    a) `>>` (Right Shift):
 *       - `number >> count` shifts the bits of `number` to the right by `count` positions.
 *       - Bits shifted off the right end are discarded.
 *       - For unsigned integers, zeros are shifted in from the left.
 *       - For signed integers, the behavior is implementation-defined, but it usually performs an 'arithmetic shift', meaning the original sign bit (MSB) is copied into the vacated positions on the left to preserve the sign.
 *       - Use Case in this code: `(n >> i)` moves the i-th bit (counting from the right, starting at 0) to the rightmost position (position 0).
 *
 *    b) `<<` (Left Shift):
 *       - `number << count` shifts the bits of `number` to the left by `count` positions.
 *       - Bits shifted off the left end are discarded.
 *       - Zeros are always shifted in from the right.
 *       - Use Case in this code: `(1 << c)` creates a 'mask' with only the c-th bit set to 1. Example: `1 << 2` results in binary `...00000100`.
 *
 *    c) `&` (Bitwise AND):
 *       - Compares corresponding bits of two operands. If both bits are 1, the result bit is 1; otherwise, it's 0.
 *       - `1 & 1 = 1`
 *       - `1 & 0 = 0`
 *       - `0 & 1 = 0`
 *       - `0 & 0 = 0`
 *       - Use Case in this code: `(...) & 1` isolates the rightmost bit (LSB). Since `1` in binary is `...00000001`, ANDing with `1` keeps the LSB unchanged but sets all other bits to 0. This effectively extracts the value (0 or 1) of the LSB.
 *
 *    d) `^` (Bitwise XOR - Exclusive OR):
 *       - Compares corresponding bits of two operands. If the bits are different, the result bit is 1; if they are the same, it's 0.
 *       - `1 ^ 1 = 0`
 *       - `1 ^ 0 = 1`
 *       - `0 ^ 1 = 1`
 *       - `0 ^ 0 = 0`
 *       - Key Property: `x ^ 0 = x` (XOR with 0 leaves the bit unchanged).
 *       - Key Property: `x ^ 1 = !x` (XOR with 1 flips the bit).
 *       - Use Case in this code: `(*n) ^ (1 << c)` flips the c-th bit of `*n`. The mask `(1 << c)` has a 1 only at position `c`. So, the c-th bit of `*n` is XORed with 1 (flipping it), while all other bits are XORed with 0 (leaving them unchanged).
 *
 * 4. Pointers (`*`):
 *    - In the `flipBit` function, `int* n` means `n` is a pointer to an integer. It holds the memory address of the original `num` variable from `main`.
 *    - Using `*n` inside `flipBit` 'dereferences' the pointer, meaning it accesses or modifies the actual value stored at that memory address (the original `num`). This allows `flipBit` to change the value of `num` in the `main` function.
 *
 */

#include <stdio.h>  // Include standard input/output library for functions like printf and scanf
#include <stdlib.h> // Include standard library (often used for general utilities, though not strictly needed here)

// Function declaration comments already explain these operators
// << left shift
// >> right shift

/**
 * @brief Prints the binary representation of an integer.
 * @param n The integer whose bits are to be printed.
 */
void printBits(int n){
  // Assuming an int is 32 bits wide on this system.
  // Loop from the most significant bit (MSB, position 31) down to the least significant bit (LSB, position 0).
  for(int i = 31; i >= 0; i--){
    // 1. (n >> i): Right-shift the number 'n' by 'i' positions.
    //    This moves the bit originally at position 'i' to the rightmost position (position 0).
    // 2. (...) & 1: Perform a bitwise AND with the integer 1 (binary ...0001).
    //    This isolates the rightmost bit. If it's 1, the result is 1. If it's 0, the result is 0.
    // 3. printf("%d", ...): Print the resulting bit (0 or 1).
    printf("%d",(n >> i) & 1);
  }
  // Print a newline character to move to the next line after printing all bits.
  printf("\n");
}

/**
 * @brief Flips (inverts) a specific bit in an integer.
 * @param n A pointer to the integer to modify. Using a pointer allows the function
 *          to change the original variable passed from the calling function (main).
 * @param c The index of the bit to flip (0-31, where 0 is the LSB).
 */
void flipBit(int* n, int c){
  // To flip the c-th bit (0-indexed from the right):
  // 1. (1 << c): Create a 'mask' integer. This shifts the binary value 1 (...0001)
  //    to the left by 'c' positions. The result is an integer with only the c-th bit set to 1
  //    and all other bits set to 0. (e.g., if c=2, mask is ...0100)
  // 2. (*n) ^ mask: Perform a bitwise XOR between the original number (*n, dereferenced pointer)
  //    and the mask.
  //    - Where the mask has a 0, the corresponding bit in *n remains unchanged (x ^ 0 = x).
  //    - Where the mask has a 1 (only at position c), the corresponding bit in *n is flipped (x ^ 1 = !x).
  // 3. *n = ...: Assign the result back to the original integer via the pointer.
  *n = (*n) ^ (1 << c);
  // Note: It's crucial to use parentheses around *n due to operator precedence.
  //       `*n ^ (1 << c)` is correct. `*n ^ 1 << c` would be parsed as `(*n ^ 1) << c`, which is wrong.
}

// Main function where the program execution begins
int main(){

  int num;    // Declare an integer variable to store the user's number
  int choice; // Declare an integer variable to store the user's choice of bit to flip

  // Prompt the user to enter an integer
  printf("Enter an integer: ");
  // Read the integer entered by the user and store it in the 'num' variable.
  // The '&' gets the memory address of 'num', which scanf needs to write the value into it.
  scanf("%d",&num);

  // Start an infinite loop (the program will run until manually stopped, e.g., with Ctrl+C).
  while(1){
    // Print the current decimal value of the number.
    printf("The number is: %d\n",num);

    // Call the printBits function to display the binary representation of the current number.
    printBits(num);

    // Prompt the user to enter which bit position they want to flip (0-31).
    printf("Which bit to flip? (0-31): ");
    // Read the user's choice and store it in the 'choice' variable.
    scanf("%d",&choice);

    // Call the flipBit function.
    // Pass the memory address of 'num' (&num) so the function can modify the original 'num'.
    // Pass the user's 'choice' as the bit index to flip.
    // Note: No input validation is done here. If 'choice' is outside 0-31, the behavior might be unexpected.
    flipBit(&num, choice);

    // Add a newline for better formatting before the next loop iteration.
    printf("\n");
  }

  // This line is technically unreachable because of the infinite `while(1)` loop.
  // In practice, the program is terminated externally (e.g., Ctrl+C).
  // It indicates successful execution to the operating system if it were reachable.
  return 0;
}
