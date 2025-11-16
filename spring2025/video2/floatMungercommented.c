// PEDAGOGICAL PURPOSE:
// This interactive program demonstrates IEEE 754 floating-point representation
// by allowing users to flip individual bits and see how it affects the float value.
// Key learning objectives:
// 1. Understanding IEEE 754 floating-point format (sign, exponent, mantissa)
// 2. Bit manipulation techniques (left shift, right shift, AND, XOR)
// 3. Type punning (viewing a float as an int)
// 4. Extracting and manipulating individual bits
// 5. The relationship between bit patterns and floating-point values
// 6. Interactive programs with while loops and user input

#include <stdio.h>      // STANDARD I/O: For printf and scanf
#include <stdlib.h>     // STANDARD LIBRARY: General utilities

// << left shift
// >> right shift

void printBits(int* n){  // PRINT BITS FUNCTION: Display binary representation
                         // Takes pointer to int (we'll pass address of our float)
                         // This function treats the float's memory as an integer
                         // to examine its binary representation

  // an int is 32 bits wide
  // write a for-loop that reads
  // each bit in order from left
  // to right and prints them out

  for(int i = 31; i >= 0; i--){  // ITERATE FROM MSB TO LSB: Start at bit 31 (leftmost)
                                  // i = 31, 30, 29, ..., 2, 1, 0
                                  // This prints bits from left to right (MSB first)

    printf("%d",(*n >> i) & 1);  // EXTRACT AND PRINT i-th BIT:
                                  // *n dereferences the pointer to get the int value
                                  // *n >> i: Right-shift by i positions
                                  //   This moves the i-th bit to the rightmost position (bit 0)
                                  //   Example: if *n = 0b10110, and i = 3:
                                  //            0b10110 >> 3 = 0b00010
                                  // & 1: Bitwise AND with 1 (0b00000001)
                                  //   This masks off all bits except the rightmost bit
                                  //   Example: 0b00010 & 0b00001 = 0b00000 = 0
                                  //            0b00011 & 0b00001 = 0b00001 = 1
                                  // So: (*n >> i) & 1 gives us the value of the i-th bit
                                  // prints the ith place each go round of the loop

    if(i == 31 || i == 23){      // ADD SPACING: Separate sign, exponent, mantissa
                                  // IEEE 754 format for 32-bit float:
                                  // Bit 31: sign bit (1 bit)
                                  // Bits 30-23: exponent (8 bits)
                                  // Bits 22-0: mantissa/fraction (23 bits)
                                  // Space after bit 31 separates sign from exponent
                                  // Space after bit 23 separates exponent from mantissa
                                  // Output format: "S EEEEEEEE MMMMMMMMMMMMMMMMMMMMMMM"
                                  // S = sign, E = exponent, M = mantissa
      printf(" ");
    }
  }
  printf("\n");          // NEWLINE: Move to next line after printing all bits
}

void flipBit(int* n, int c){  // FLIP BIT FUNCTION: Toggle a specific bit
                               // n: pointer to the integer (representing our float)
                               // c: which bit to flip (0-31)

  // you can flip the c-th bit
  // by taking (1 << c) and taking the ^
  // with the number n

  *n = (*n) ^ (1 << c);  // XOR TO FLIP BIT:
                         // 1 << c: Create a bit mask with only bit c set
                         //   Example: 1 << 3 = 0b00001000 (only bit 3 is 1)
                         // XOR (^) flips bits:
                         //   0 ^ 1 = 1 (off becomes on)
                         //   1 ^ 1 = 0 (on becomes off)
                         //   0 ^ 0 = 0 (stays off)
                         //   1 ^ 0 = 1 (stays on)
                         // So: *n ^ (1 << c) flips only bit c, leaves others unchanged
                         //
                         // WHY XOR FOR FLIPPING?
                         // - To set a bit: use OR (|)
                         // - To clear a bit: use AND with NOT (~)
                         // - To flip a bit: use XOR (^)
}

int main(){

  float num;             // FLOAT VARIABLE: The number we'll manipulate
  int choice;            // USER INPUT: Which bit to flip

  printf("Enter a float: ");
  scanf("%f",&num);      // READ FLOAT: Get initial value from user
                         // %f format specifier for float
                         // &num gives address where scanf should store the value

  while(1){              // INFINITE LOOP: Keep looping until program is killed
                         // while(1) means "while true" - loop forever
                         // User must press Ctrl+C to exit

    printf("The number is: %.10f\n",num);  // DISPLAY FLOAT: Show current value
                                            // %.10f shows 10 digits after decimal point

    // need a function that will
    printBits((int*)&num);  // PRINT BITS: Show binary representation
                            // &num gets address of num
                            // (int*) casts float pointer to int pointer
                            // This is TYPE PUNNING: viewing float memory as int
                            // The bits don't change, we're just interpreting them differently
                            // This allows us to examine/manipulate the float's bit pattern

    printf("Which bit to flip?: ");
    scanf("%d",&choice);    // READ BIT INDEX: Get user's choice (0-31)

    flipBit((int*)&num,choice);  // FLIP THE BIT: Toggle the chosen bit
                                 // &num: address of the float
                                 // (int*): cast to int pointer
                                 // choice: which bit to flip
                                 // After this, num's bit pattern is modified
                                 // and its floating-point value changes accordingly
  }

  return 0;              // RETURN: This line is never reached due to while(1)
                         // Good practice to include anyway
}

// IEEE 754 SINGLE-PRECISION (32-bit) FLOAT FORMAT:
//
// Bit layout:
//   31    30-23   22-0
//   [S] [EEEEEEEE] [MMMMMMMMMMMMMMMMMMMMMMM]
//    |      |                |
//   sign  exponent      mantissa/fraction
//
// Sign (1 bit):
//   0 = positive, 1 = negative
//
// Exponent (8 bits):
//   Biased by 127
//   Actual exponent = stored value - 127
//   Range: -126 to +127 (0 and 255 are special)
//   00000000: zero or subnormal
//   11111111: infinity or NaN
//
// Mantissa (23 bits):
//   Represents fractional part
//   Implicit leading 1 (normalized form)
//   Actual mantissa = 1.MMMMMMMMMMMMMMMMMMMMMMM (binary)
//
// Value calculation (normalized):
//   (-1)^S × 1.M × 2^(E-127)
//
// Examples:
//   1.0 = 0 01111111 00000000000000000000000
//         S=0 (positive), E=127 (2^0), M=0 (1.0)
//         = (+1) × 1.0 × 2^0 = 1.0
//
//   2.0 = 0 10000000 00000000000000000000000
//         S=0, E=128 (2^1), M=0 (1.0)
//         = (+1) × 1.0 × 2^1 = 2.0
//
//   -3.5 = 1 10000000 11000000000000000000000
//          S=1 (negative), E=128 (2^1), M=0.75 (1.75)
//          = (-1) × 1.75 × 2^1 = -3.5

// EXPERIMENT:
// Try entering 1.0 and flipping different bits:
//
// Flip bit 31 (sign):
//   1.0 becomes -1.0 (changes sign)
//
// Flip bit 30 (highest exponent bit):
//   1.0 becomes 2.0 (exponent changes from 127 to 191)
//
// Flip bit 0 (lowest mantissa bit):
//   1.0 becomes 1.00000011920928955078125... (tiny change in mantissa)
//
// Try special values:
//   0.0: all bits zero
//   Infinity: sign=0, exponent=all 1s, mantissa=all 0s
//   NaN: sign=any, exponent=all 1s, mantissa=non-zero

// TO COMPILE AND RUN:
// gcc floatMunger.c -o floatMunger
// ./floatMunger

// SAMPLE SESSION:
// Enter a float: 1.0
// The number is: 1.0000000000
// 0 01111111 00000000000000000000000
// Which bit to flip?: 31
// The number is: -1.0000000000
// 1 01111111 00000000000000000000000
// Which bit to flip?: 31
// The number is: 1.0000000000
// 0 01111111 00000000000000000000000
// (Press Ctrl+C to exit)

// WHY THIS PROGRAM IS VALUABLE:
// - Shows that floats are just bit patterns with special interpretation
// - Demonstrates how small bit changes can cause large value changes
// - Helps understand the components of IEEE 754 format
// - Reveals the underlying binary nature of floating-point numbers
// - Makes abstract concepts concrete through hands-on experimentation
