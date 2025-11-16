// PEDAGOGICAL PURPOSE:
// This program demonstrates pointer type casting to manipulate floating-point
// values at the bit level, revealing the internal IEEE 754 representation.
// Key learning objectives:
// 1. Understanding that floats are stored as bit patterns
// 2. Type punning with pointer casts to view float bits as integer bits
// 3. IEEE 754 floating-point format (sign, exponent, mantissa)
// 4. How flipping individual bits affects the float value
// 5. The power and danger of reinterpreting memory
// 6. Interactive bit manipulation to explore float representation

#include <stdio.h>
#include <stdlib.h>

// BITWISE OPERATORS REFERENCE:
// << left shift:  shifts bits left, fills with zeros
// >> right shift: shifts bits right
// These work on integer types, not floats directly

void printBits(int* n){
  // FUNCTION PURPOSE: Display all 32 bits of an integer
  // This is used to visualize the bit pattern of a float (after casting)

  // BIT EXTRACTION:
  // An int is 32 bits wide (on most modern systems)
  // We print from left to right (MSB to LSB)
  // Bit 31 is leftmost, bit 0 is rightmost
  for(int i = 31; i >= 0; i--){

    // EXTRACT i-th BIT:
    // (*n >> i) shifts the i-th bit to position 0
    // & 1 masks all but the rightmost bit
    // Result is 0 or 1
    printf("%d",(*n >> i)&1);
  }
  printf("\n");
}

void flipBit(int* n, int c){
  // FUNCTION PURPOSE: Toggle the c-th bit of an integer
  //
  // MECHANISM:
  // Use XOR to flip the bit at position c
  // (1 << c) creates a mask with only bit c set
  // XOR with mask: 0^1=1 (set), 1^1=0 (clear)
  *n = (*n) ^ (1 << c);
}

int main(){

  float num;      // The floating-point number we'll manipulate
  int choice;     // Which bit to flip

  // USER INPUT:
  printf("Enter a float: ");
  scanf("%f",&num);  // Read float value (e.g., 3.14, -0.5, 1.0)

  // INFINITE LOOP FOR INTERACTIVE EXPLORATION:
  while(1){

    // DISPLAY CURRENT VALUE:
    // %.40f shows 40 decimal places
    // This reveals floating-point precision/rounding issues
    // Many "simple" decimals (like 0.1) cannot be represented exactly in binary
    printf("The number is: %.40f\n",num);

    // CRITICAL TYPE PUNNING:
    // (int*)&num reinterprets the BITS of num as an integer
    //
    // BREAKDOWN:
    // &num - address of num (type: float*)
    // (int*)&num - cast to int* (reinterpret as pointer to int)
    // printBits expects int*, so we're viewing float's bits as int bits
    //
    // IMPORTANT: This is TYPE PUNNING
    // We're NOT converting the float value to int (that would be (int)num)
    // We're viewing the SAME MEMORY as a different type
    // The bit pattern stays identical, but we interpret it as int
    //
    // WHY THIS WORKS:
    // Both float and int are 32 bits
    // We can view those 32 bits as either type
    // This reveals the IEEE 754 representation
    printBits((int*)&num);

    // GET BIT TO FLIP:
    printf("Which bit to flip?: ");
    scanf("%d",&choice);

    // FLIP THE CHOSEN BIT:
    // Again, we cast float* to int* to manipulate bits
    // flipBit modifies the bits in place
    // Since num's memory is modified, the float value changes
    flipBit((int*)&num,choice);

    // EFFECT:
    // Flipping different bits has different effects:
    // - Bit 31 (sign bit): changes sign (positive ↔ negative)
    // - Bits 30-23 (exponent): changes magnitude dramatically
    // - Bits 22-0 (mantissa): changes precision/decimal part
  }

  return 0;
}

// COMPREHENSIVE EXPLANATION:
//
// IEEE 754 SINGLE-PRECISION FLOAT FORMAT (32 bits):
//
// Bit layout:
//   31    30-23        22-0
//   S     EEEEEEEE     MMMMMMMMMMMMMMMMMMMMMMM
//   ↑     ↑            ↑
//   Sign  Exponent     Mantissa (fraction)
//
// Sign bit (1 bit):
//   0 = positive
//   1 = negative
//
// Exponent (8 bits):
//   Biased by 127
//   Actual exponent = stored value - 127
//   Range: -126 to +127 (plus special values)
//
// Mantissa (23 bits):
//   Represents fractional part
//   Implicit leading 1 (normalized form: 1.mantissa)
//
// Value calculation:
//   (-1)^S × 1.mantissa × 2^(exponent-127)

// EXAMPLE: 1.0 in IEEE 754:
//
// Binary representation: 0 01111111 00000000000000000000000
//                         ↑ ↑        ↑
//                         S Exponent Mantissa
//
// Sign: 0 (positive)
// Exponent: 01111111 = 127 (biased)
//           Actual exponent: 127 - 127 = 0
// Mantissa: all zeros (represents 1.0 with implicit leading 1)
//
// Value: (-1)^0 × 1.0 × 2^0 = 1.0

// EXAMPLE: -2.5 in IEEE 754:
//
// -2.5 = -1 × 2.5 = -1 × 1.25 × 2^1
// Binary: 1 10000000 01000000000000000000000
//         ↑ ↑        ↑
//         S Exponent Mantissa
//
// Sign: 1 (negative)
// Exponent: 10000000 = 128 → 128-127 = 1
// Mantissa: 01000000... = 0.25 (in binary: 0.01 = 1/4)
//           1 + 0.25 = 1.25
// Value: -1 × 1.25 × 2^1 = -2.5

// TYPE PUNNING EXPLANATION:
//
// What (int*)&num does:
// 1. &num gets the ADDRESS of num (where it's stored in memory)
// 2. (int*) casts that address from float* to int*
// 3. The MEMORY CONTENTS don't change
// 4. We now view those 32 bits as an integer instead of a float
//
// Memory view:
//   Address 0x1000: [10111111100000000000000000000000]  ← 32 bits
//                    ↑
//   As float*:      interpreted as floating-point value
//   As int*:        interpreted as integer value
//
// This is NOT the same as:
//   int x = (int)num;  // Converts float VALUE to int (e.g., 3.14 → 3)
// vs:
//   int x = *(int*)&num;  // Reinterprets float BITS as int

// EFFECTS OF FLIPPING DIFFERENT BITS:
//
// Starting with 1.0 (bits: 0 01111111 00000000000000000000000):
//
// Flip bit 31 (sign):
//   Result: 1 01111111 00000000000000000000000 = -1.0
//   Effect: Changes sign
//
// Flip bit 30 (exponent MSB):
//   Result: 0 11111111 00000000000000000000000 = NaN or Infinity
//   Effect: Creates special value (all exponent bits = 1)
//
// Flip bit 23 (exponent LSB):
//   Exponent changes from 127 to 126 or 128
//   Result: 0.5 or 2.0 (power of 2 change)
//
// Flip bit 22 (mantissa MSB):
//   Mantissa changes slightly
//   Result: ~1.25 (small change in value)
//
// Flip bit 0 (mantissa LSB):
//   Smallest possible change
//   Result: 1.0 + epsilon (next representable number)

// WHY FLOATS ARE IMPRECISE:
//
// Many decimal numbers can't be represented exactly in binary
//
// Example: 0.1 in decimal
// In binary: 0.0001100110011001100... (repeating)
// Stored (rounded): closest 32-bit representation
// Displayed with %.40f: 0.1000000014901161193847656250000000000000
//
// This is why:
//   float x = 0.1 + 0.2;
//   x == 0.3  // Often FALSE!

// SPECIAL FLOAT VALUES:
//
// +Infinity: 0 11111111 00000000000000000000000
// -Infinity: 1 11111111 00000000000000000000000
// NaN:       x 11111111 xxxxxxxxxxxxxxxxxxxxxxxxx (mantissa nonzero)
// +0:        0 00000000 00000000000000000000000
// -0:        1 00000000 00000000000000000000000 (yes, negative zero exists!)
//
// Denormalized numbers (very small, exponent = 0):
//   Value = (-1)^S × 0.mantissa × 2^-126

// PRACTICAL EXPLORATION WITH THIS PROGRAM:
//
// Try these inputs to see bit patterns:
//
// Enter 1.0:
//   Output: 0 01111111 00000000000000000000000
//   Flip bit 31 → -1.0
//
// Enter 2.0:
//   Output: 0 10000000 00000000000000000000000
//   Exponent is 1 larger than 1.0
//
// Enter 0.5:
//   Output: 0 01111110 00000000000000000000000
//   Exponent is 1 smaller than 1.0
//
// Enter 0.1:
//   Output shows repeating pattern in mantissa
//   Display: 0.1000000014901161193847656250000000000000

// DANGERS OF TYPE PUNNING:
//
// 1. UNDEFINED BEHAVIOR (in some contexts):
//    Strict aliasing rules in C can make this undefined
//    GCC flag: -fno-strict-aliasing (relaxes rules)
//
// 2. PLATFORM DEPENDENCIES:
//    Assumes float and int are both 32 bits
//    Assumes IEEE 754 representation (not guaranteed by C standard)
//
// 3. ENDIANNESS ISSUES:
//    On big-endian systems, byte order differs
//    Bit positions might appear reversed
//
// SAFER ALTERNATIVE (C99):
//   union FloatInt {
//     float f;
//     int i;
//   };
//   union FloatInt fi;
//   fi.f = 1.0;
//   printBits(&fi.i);  // Access as int through union

// APPLICATIONS OF BIT-LEVEL FLOAT MANIPULATION:
//
// 1. Fast inverse square root (Quake III algorithm)
//    Uses bit manipulation for fast approximation
//
// 2. Floating-point comparison with tolerance
//    Check if two floats are "close enough" at bit level
//
// 3. Extracting exponent/mantissa for analysis
//
// 4. Creating special values (NaN, infinity) for testing
//
// 5. Understanding floating-point arithmetic edge cases

// TO COMPILE AND RUN:
// gcc floatMunger.c -o floatMunger
// ./floatMunger
//
// Example session:
// Enter a float: 1.0
// The number is: 1.0000000000000000000000000000000000000000
// 00111111100000000000000000000000
// Which bit to flip?: 31
// The number is: -1.0000000000000000000000000000000000000000
// 10111111100000000000000000000000
// Which bit to flip?: 23
// (etc.)
