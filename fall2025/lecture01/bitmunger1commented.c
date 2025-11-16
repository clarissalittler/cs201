// PEDAGOGICAL PURPOSE:
// This program teaches bitwise operations and bit manipulation in C.
// Key learning objectives:
// 1. Understanding bitwise operators (&, |, ^, <<, >>)
// 2. Visualizing binary representation of integers
// 3. Bit manipulation techniques (extracting and toggling individual bits)
// 4. The relationship between signed and unsigned integer interpretation
// 5. Using bit shifting for efficient multiplication/division by powers of 2
// 6. Passing pointers to modify variables in functions

#include "stdio.h"
#include "stdlib.h"

// BITWISE OPERATORS REFERENCE:
// & bitwise and    - Both bits must be 1 to result in 1
// | bitwise or     - Either bit being 1 results in 1
// ^ bitwise xor    - Bits must be different to result in 1
//
// XOR TRUTH TABLE (useful for toggling bits):
// 1 ^ 1 = 0  - Same bits produce 0
// 0 ^ 1 = 1  - Different bits produce 1
// 1 ^ 0 = 1  - Different bits produce 1
// 0 ^ 0 = 0  - Same bits produce 0
//
// SHIFT OPERATORS:
// num1 << num2 - move all the bits in num1 to the left num2 places
//                (equivalent to multiplying by 2^num2)
// num1 >> num2 - move all the bits in num1 to the right num2 places
//                (equivalent to dividing by 2^num2 for positive numbers)



// FUNCTION: printBits
// PURPOSE: Visualize the binary representation of an integer
// TECHNIQUE: Uses bit shifting and masking to extract each bit
void printBits(int num1){
  printf("%d in bits is: ",num1);

  // Start from bit 31 (most significant bit) down to bit 0 (least significant)
  // This prints the binary representation in standard left-to-right order
  for(int i=31; i>=0;i--){
    // BIT EXTRACTION TECHNIQUE:
    // 1. (num1 >> i) shifts the number right by i positions
    //    This moves bit i into the least significant position
    // 2. & 1 masks off all bits except the least significant one
    //    This gives us just the value of bit i (either 0 or 1)
    printf("%d",(num1>>i)&1);
  }
  printf("\n");
}

// FUNCTION: toggleBit
// PURPOSE: Flip a specific bit in a number (0->1 or 1->0)
// PARAMETERS:
//   num - pointer to the integer to modify (passed by reference)
//   place - which bit position to toggle (0 = rightmost bit)
// TECHNIQUE: Uses XOR to toggle bits (x ^ 1 flips the bit)
void toggleBit(int* num,int place){
  // BIT TOGGLING TECHNIQUE:
  // 1. (1 << place) creates a number with only bit 'place' set to 1
  //    For example: 1 << 3 = 0000...0001000 in binary (= 8 in decimal)
  // 2. XOR with this mask flips only that specific bit
  //    If the bit was 0, XOR with 1 makes it 1
  //    If the bit was 1, XOR with 1 makes it 0
  // 3. We dereference num to modify the value at the pointer location
  *num = (*num) ^ (1 << place);
}

int main(){

  int num1 = 80;     // Starting value: 80 = 0101 0000 in binary
  int choice;        // User's choice of which bit to flip

  // Interactive loop: let user toggle bits until they enter -1
  do {
    // SIGNED vs UNSIGNED INTERPRETATION:
    // Same bit pattern can represent different values depending on interpretation
    // %d interprets as signed (using two's complement for negatives)
    // %u interprets as unsigned (all bits contribute to positive magnitude)
    printf("The number %d when interpreted as unsigned is: %u\n",num1,(unsigned int)num1);

    // Show the actual binary representation
    printBits(num1);

    // Get user input for which bit to flip
    printf("Enter a bit to flip: ");
    scanf("%d",&choice);

    // Toggle the chosen bit (passing address of num1 so function can modify it)
    toggleBit(&num1,choice);

  }while (choice != -1);  // Loop continues until user enters -1

  return 0;
}
