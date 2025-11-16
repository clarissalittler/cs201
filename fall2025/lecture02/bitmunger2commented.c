// PEDAGOGICAL PURPOSE:
// This program demonstrates advanced pointer casting and IEEE 754 floating-point representation.
// Key learning objectives:
// 1. Understanding that floats are stored as bits in memory (IEEE 754 format)
// 2. Pointer casting allows us to reinterpret the same memory as different types
// 3. Visualizing the internal binary representation of floating-point numbers
// 4. Understanding the three components of IEEE 754: sign bit, exponent, mantissa
// 5. How manipulating individual bits in a float changes its value
// 6. The powerful (and dangerous) technique of type punning via pointer casting
//
// IEEE 754 SINGLE PRECISION FORMAT (32 bits):
// Bit 31:      Sign bit (0 = positive, 1 = negative)
// Bits 30-23:  Exponent (8 bits, biased by 127)
// Bits 22-0:   Mantissa/Fraction (23 bits, represents the fractional part)
//
// This program adds spaces after bits 31 and 23 to visually separate these components

#include "stdio.h"
#include "stdlib.h"

// FUNCTION: printBits
// PURPOSE: Display the binary representation of a float's underlying bits
// PARAMETER: num1 - pointer to integer (which actually points to a float's memory)
// KEY CONCEPT: This function treats a float's memory as an integer to examine its bits
void printBits(int* num1){
  printf("in bits is: ");

  // Loop through all 32 bits from most significant (31) to least significant (0)
  for(int i=31; i>=0;i--){
    // BIT EXTRACTION:
    // (*num1 >> i) shifts the bits right by i positions
    // & 1 masks all bits except the least significant one
    // This extracts bit i and prints it
    printf("%d",(*num1>>i)&1);

    // FORMATTING FOR IEEE 754 VISUALIZATION:
    // Add a space after bit 31 (sign bit) and after bit 23 (exponent)
    // This visually separates: [sign] [exponent] [mantissa]
    // Example: "0 01111101 01000000000000000000000" for 0.3125
    if(i == 31 || i == 23){
      printf(" ");
    }
  }
  printf("\n");
}

// FUNCTION: toggleBit
// PURPOSE: Flip a specific bit in the number (works on any type via pointer)
// PARAMETERS:
//   num - pointer to integer (but could be pointing to float's memory)
//   place - which bit position to toggle (0 = rightmost/least significant bit)
void toggleBit(int* num,int place){
  // XOR TOGGLE TECHNIQUE:
  // 1 << place creates a mask with only that bit set to 1
  // XOR flips that specific bit: 0→1 or 1→0
  *num = (*num) ^ (1 << place);
}

int main(){

  // VARIABLE DECLARATION:
  // num1 is a float - we'll manipulate its internal bit representation
  float num1;
  printf("Choose a starting number: ");
  scanf("%f",&num1);

  int choice;  // User's choice of which bit to flip

  do {
    // Display the current float value
    printf("The number %f ",num1);

    // TYPE PUNNING WITH POINTER CASTING:
    // &num1 is the address of a float variable
    // (int*)&num1 CASTS this address to be treated as a pointer to int
    // This allows us to read the float's memory as if it were an integer
    // WHY: We want to see/manipulate the individual bits of the float
    // WARNING: This is a powerful technique but violates strict aliasing rules
    printBits((int*)&num1);

    printf("Enter a bit to flip: ");
    scanf("%d",&choice);

    // MANIPULATING FLOAT BITS:
    // Again we cast the float's address to int* to modify its bits
    // When we flip a bit in a float's representation:
    // - Flipping bit 31 changes the sign (positive ↔ negative)
    // - Flipping bits 30-23 changes the exponent (magnitude/scale)
    // - Flipping bits 22-0 changes the mantissa (precision/fraction)
    toggleBit((int*)&num1,choice);

  }while (choice != -1);  // Exit when user enters -1

  return 0;
}

// CONCEPTUAL EXPLANATION:
//
// This program reveals that floating-point numbers are just bit patterns in memory.
// By treating a float's memory as an integer, we can examine and manipulate
// the individual bits that make up the IEEE 754 representation.
//
// EXAMPLE EXPLORATION:
// Try starting with 1.0:
//   Binary: 0 01111111 00000000000000000000000
//   Sign: 0 (positive)
//   Exponent: 01111111 (127 in decimal, which means 2^0 = 1)
//   Mantissa: all zeros (represents 1.0 in normalized form)
//
// Flip bit 31 (sign bit):
//   Result: -1.0
//   Binary: 1 01111111 00000000000000000000000
//
// Flip bit 23 (least significant exponent bit) from original 1.0:
//   Result: 2.0 (exponent becomes 128, meaning 2^1 = 2)
//   Binary: 0 10000000 00000000000000000000000
//
// WHY THIS MATTERS:
// Understanding float representation helps explain:
// - Why floating-point arithmetic isn't exact
// - Why you should never use == to compare floats
// - How floats can represent huge ranges but with limited precision
// - What happens when you exceed float's range (infinity, NaN)
