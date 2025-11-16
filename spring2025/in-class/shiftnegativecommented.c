// PEDAGOGICAL PURPOSE:
// This program demonstrates right shift behavior with unsigned integers and
// the effects of assigning a negative value to an unsigned variable.
// Key learning objectives:
// 1. Understanding how negative values are represented in unsigned integers
// 2. Bitwise right shift operator (>>) with unsigned types
// 3. Visualizing binary representation of numbers
// 4. The difference between logical shift (unsigned) and arithmetic shift (signed)
// 5. Using bit manipulation to extract and display individual bits
// 6. Understanding two's complement representation and type conversion
//
// KEY CONCEPT:
// When you assign -1 to an unsigned int, you get all bits set to 1 (maximum value).
// Right shifting an unsigned int performs a LOGICAL shift (fills with 0s),
// while right shifting a signed int performs an ARITHMETIC shift (fills with sign bit).

#include <stdlib.h>
#include <stdio.h>

// FUNCTION: printBits
// PURPOSE: Visualize the binary representation of a 32-bit integer
// TECHNIQUE: Extract each bit using right shift and masking
// PARAMETER: n - the integer to display in binary
void printBits(int n){
  // Iterate from bit 31 (leftmost/most significant) to bit 0 (rightmost/least significant)
  // This displays the binary number in standard big-endian format (left to right)
  for(int i = 31; i >= 0; i--){
    // BIT EXTRACTION TECHNIQUE:
    // Step 1: (n >> i) shifts n right by i positions
    //         This moves bit i into the least significant position (bit 0)
    // Step 2: & 1 performs bitwise AND with 1 (binary: 0000...0001)
    //         This masks out all bits except the least significant bit
    //         Result is either 0 or 1, representing the value of bit i
    printf("%d",(n >> i) & 1);
  }
  printf("\n");  // Newline after printing all 32 bits
}

// FUNCTION: flipBit
// PURPOSE: Toggle (flip) a specific bit in an integer (0->1 or 1->0)
// TECHNIQUE: Use XOR with a bit mask to flip exactly one bit
// PARAMETERS:
//   n - pointer to the integer to modify (passed by reference)
//   c - the bit position to flip (0 = rightmost bit, 31 = leftmost bit)
void flipBit(int* n, int c){
  // BIT FLIPPING WITH XOR:
  // Step 1: (1 << c) creates a mask with only bit c set to 1
  //         Example: 1 << 3 = 0000...0001000 (decimal 8)
  // Step 2: (*n) ^ mask performs XOR with the mask
  //         XOR truth table: 0^1=1, 1^1=0, 0^0=0, 1^0=1
  //         Effect: bit c is flipped, all other bits unchanged
  // Step 3: Store result back in *n (modifying the original variable)
  *n = (*n) ^ (1 << c);

  // Why XOR works for toggling:
  // - If bit c is 0: 0 ^ 1 = 1 (bit becomes 1)
  // - If bit c is 1: 1 ^ 1 = 0 (bit becomes 0)
  // - Other bits: x ^ 0 = x (unchanged)
}


int main(){
  // UNSIGNED INTEGER AND NEGATIVE ASSIGNMENT:
  // This is the KEY line demonstrating a crucial concept!
  // unsigned int means: interpret all 32 bits as a positive number
  // -1 in two's complement (signed) is: 1111...1111 (all 32 bits set to 1)
  // When assigned to unsigned int: same bit pattern, but interpreted as largest unsigned value
  // Result: num = 4,294,967,295 (2^32 - 1) in decimal
  //         num = 0xFFFFFFFF in hexadecimal
  //         num = 1111...1111 in binary (all 32 bits are 1)
  unsigned int num = -1; // 1111111..11111

  // WHY THIS WORKS:
  // - Integer literals like -1 have a type (typically int, which is signed)
  // - In two's complement: -1 as signed int is all bits set to 1
  // - Assigning to unsigned int: bit pattern copied unchanged
  // - Interpretation changes: unsigned reads all 1s as maximum positive value

  // PRINTING BEFORE SHIFT:
  // %d interprets as SIGNED int (will show -1 due to sign bit)
  // This is actually undefined behavior since num is unsigned!
  // Should use %u for unsigned int
  printf("Our number before shifting is: %d\n",num);

  // Display the binary representation: all 1s
  printBits(num);

  // RIGHT SHIFT OPERATION ON UNSIGNED INT:
  // num >> 3 shifts all bits 3 positions to the right
  // LOGICAL SHIFT (because num is unsigned):
  // - Bits shifted off the right are discarded
  // - Zeros are shifted in from the left
  // Before: 1111 1111 1111 1111 1111 1111 1111 1111
  // After:  0001 1111 1111 1111 1111 1111 1111 1111
  //         ^^^
  //         Three 0s shifted in from left
  num = num >> 3;

  // WHY LOGICAL SHIFT?
  // - unsigned types always use logical shift (fill with 0)
  // - signed types use arithmetic shift (fill with sign bit)
  // - This preserves the "unsigned means positive" property

  // PRINTING AFTER SHIFT:
  // Now num = 536,870,911 (2^29 - 1) in decimal
  // The value is divided by 8 (2^3) due to the 3-bit right shift
  printf("Our number after shifting is: %d\n",num);

  // Display the binary: three 0s on the left, rest are 1s
  printBits(num);

  return 0;
}

// CONCEPTUAL EXPLANATION:
//
// TWO'S COMPLEMENT REPRESENTATION:
// In two's complement (the standard for signed integers):
// - Positive numbers: leftmost bit (sign bit) is 0
// - Negative numbers: leftmost bit is 1
// - To negate a number: flip all bits and add 1
//
// Example for -1 in 8-bit two's complement:
// Start with +1:   0000 0001
// Flip all bits:   1111 1110
// Add 1:           1111 1111  <- This is -1
//
// In 32-bit: -1 is 1111 1111 1111 1111 1111 1111 1111 1111
//
// UNSIGNED vs SIGNED INTERPRETATION:
// Same bit pattern, different interpretation:
//
// Bit pattern: 1111 1111 1111 1111 1111 1111 1111 1111
// As signed int:   -1 (two's complement negative)
// As unsigned int: 4,294,967,295 (2^32 - 1, maximum unsigned value)
//
// ASSIGNING -1 TO UNSIGNED INT:
// unsigned int num = -1;
//
// What happens:
// 1. Literal -1 is created as signed int (all bits = 1)
// 2. Bit pattern is copied to num unchanged
// 3. num interprets the pattern as unsigned (positive)
// 4. Result: num = 4,294,967,295
//
// This is well-defined behavior in C (not undefined)!
// The standard specifies the conversion: value modulo 2^n
//
// LOGICAL vs ARITHMETIC SHIFT:
//
// LOGICAL SHIFT (unsigned types):
// Right shift fills with 0s from the left
// Example: 1111 1111 >> 3 = 0001 1111
//          ^^^                ^^^
//          Lost               0s filled in
//
// ARITHMETIC SHIFT (signed types):
// Right shift fills with sign bit from the left
// Preserves sign for negative numbers
// Example (signed): 1111 1111 >> 3 = 1111 1111
//                   ^^^                ^^^
//                   Lost               Sign bit (1) filled in
//
// RIGHT SHIFT DIVISION:
// For unsigned and positive signed numbers:
// num >> n is equivalent to num / (2^n)
//
// Before shift: 1111...1111 = 4,294,967,295
// After >> 3:   0001...1111 = 536,870,911
// Calculation:  4,294,967,295 / 8 = 536,870,911 (integer division)
//
// VISUALIZATION OF THE SHIFT:
//
// Before shift (num = all 1s):
// Position: 31 30 29 28 ... 3  2  1  0
// Bits:     1  1  1  1  ... 1  1  1  1
//
// After shift right by 3 (num >> 3):
// Position: 31 30 29 28 ... 3  2  1  0
// Bits:     0  0  0  1  ... 1  1  1  1
//           ^^^^^^^^        ^^^^^^^^^^^^
//           Filled with 0   Original bits shifted right
//           (logical shift) (bits 31-3 moved to positions 28-0)
//
// Bits that were in positions 2, 1, 0 are lost (shifted off the right edge)
//
// BIT EXTRACTION TECHNIQUE EXPLAINED:
// To get bit i of number n:
// 1. n >> i moves bit i to position 0
// 2. & 1 masks out all other bits
//
// Example: Get bit 5 of 0101 0110
// Step 1: 0101 0110 >> 5 = 0000 0010
// Step 2: 0000 0010 & 0001 = 0000 0000 = 0
// Result: bit 5 is 0
//
// FLIPBIT FUNCTION EXPLAINED:
// To flip bit c of number n:
// 1. Create mask: 1 << c = number with only bit c set
// 2. XOR: n ^ mask flips only bit c
//
// Example: Flip bit 3 of 0101 0110
// Step 1: 1 << 3 = 0000 1000 (mask)
// Step 2: 0101 0110 ^ 0000 1000 = 0101 1110
//         Before: bit 3 was 0
//         After:  bit 3 is 1
//
// SIGNED vs UNSIGNED RIGHT SHIFT COMPARISON:
//
// Starting value (binary): 1111 1111 1111 1111 1111 1111 1111 1111
//
// If treated as SIGNED int (our flipBit uses int):
//   Value: -1
//   After >> 3: 1111 1111 1111 1111 1111 1111 1111 1111
//   Value: still -1 (arithmetic shift preserves sign)
//
// If treated as UNSIGNED int (our num):
//   Value: 4,294,967,295
//   After >> 3: 0001 1111 1111 1111 1111 1111 1111 1111
//   Value: 536,870,911 (logical shift fills with 0)
//
// COMMON MISCONCEPTIONS:
//
// MISCONCEPTION 1: "unsigned int num = -1; is an error"
// Reality: This is legal and well-defined. The bit pattern of -1 is
//          copied, resulting in the maximum unsigned value.
//
// MISCONCEPTION 2: "Shifting right always divides by power of 2"
// Reality: Only true for unsigned or positive signed numbers.
//          For negative signed numbers with arithmetic shift,
//          the result is not simple division.
//
// MISCONCEPTION 3: "Right shift fills with 0"
// Reality: Depends on whether the type is signed or unsigned.
//          Unsigned: always fills with 0 (logical shift)
//          Signed: implementation-defined, but typically fills with
//                  sign bit (arithmetic shift)
//
// PRACTICAL APPLICATIONS:
//
// 1. Creating bit masks:
//    unsigned int all_ones = -1;  // Quick way to get all bits set
//    unsigned int mask = all_ones >> 16;  // Create 16-bit mask
//
// 2. Fast division by powers of 2:
//    unsigned int x = 1000;
//    unsigned int div8 = x >> 3;  // Faster than x / 8
//
// 3. Extracting bit fields:
//    unsigned int flags = 0xF0;
//    unsigned int upper = flags >> 4;  // Get upper 4 bits
//
// 4. Aligning values:
//    unsigned int addr = get_address();
//    unsigned int aligned = addr >> 3;  // Align to 8-byte boundary
//
// UNDEFINED vs IMPLEMENTATION-DEFINED BEHAVIOR:
//
// Well-defined:
// - unsigned int num = -1;  // Defined: wraps around to max value
// - num >> 3;               // Defined: logical shift (fills with 0)
//
// Implementation-defined (behavior varies by compiler/platform):
// - signed int num = -1;
// - num >> 3;               // Most do arithmetic shift, but not required
//
// Undefined:
// - num >> 32;              // Shifting by >= bit width is undefined
// - num >> -1;              // Negative shift count is undefined
