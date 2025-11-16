// PEDAGOGICAL PURPOSE:
// This program demonstrates bit manipulation operations including bit shifting,
// bitwise operators, and how to inspect and modify individual bits in integers.
// Key learning objectives:
// 1. Understanding left shift (<<) and right shift (>>) operators
// 2. Using bitwise AND (&) to test/extract specific bits
// 3. Using bitwise XOR (^) to flip/toggle bits
// 4. Pointer usage for modifying values in functions
// 5. Binary representation of integers (32-bit two's complement)
// 6. Infinite loops with while(1)
// 7. Working with bits as the fundamental unit of data

#include <stdio.h>
#include <stdlib.h>

// BITWISE OPERATORS REFERENCE:
// << left shift:  shifts bits left, fills with zeros (multiplies by powers of 2)
// >> right shift: shifts bits right (divides by powers of 2, sign-extends for signed ints)
// & AND:  both bits must be 1 to produce 1
// | OR:   either bit being 1 produces 1
// ^ XOR:  bits must differ to produce 1 (exclusive OR)
// ~ NOT:  inverts all bits

void printBits(int n){
  // FUNCTION PURPOSE: Display all 32 bits of an integer from left to right

  // BIT LAYOUT EXPLANATION:
  // An int is 32 bits wide (on most modern systems)
  // Bits are numbered from 0 (rightmost/least significant) to 31 (leftmost/most significant)
  //
  // Example: The number 5 in binary:
  // Bit position: 31 30 29 ... 3  2  1  0
  // Bit value:     0  0  0 ... 0  1  0  1  = 5 in decimal
  //                             ↑     ↑
  //                             4  +  1  = 5

  // LOOP EXPLANATION:
  // We start at bit 31 (leftmost) and work down to bit 0 (rightmost)
  // This prints bits in the conventional order (most significant first)
  for(int i = 31; i >= 0; i--){

    // BIT EXTRACTION TECHNIQUE:
    // Goal: Print the value (0 or 1) of the i-th bit
    //
    // Step 1: (n >> i)
    //   Right-shift n by i positions
    //   This moves the i-th bit to the rightmost position (bit 0)
    //   Example: if n = 5 (binary: ...00101) and i = 2
    //            5 >> 2 = 1 (binary: ...00001)
    //            The bit that was at position 2 is now at position 0
    //
    // Step 2: & 1
    //   Bitwise AND with 1 (binary: ...00001)
    //   This masks off all bits except the rightmost one
    //   Result is either 0 or 1
    //   Example: ...00001 & 1 = 1
    //            ...00010 & 1 = 0
    //
    // Combined: (n >> i) & 1 extracts the i-th bit
    printf("%d",(n >> i)&1);  // Prints the i-th bit each iteration
  }
  printf("\n");

  // WHAT THIS OUTPUTS:
  // For n = 5: 00000000000000000000000000000101
  // For n = -1: 11111111111111111111111111111111 (two's complement)
  // For n = 0: 00000000000000000000000000000000
}

void flipBit(int* n, int c){
  // FUNCTION PURPOSE: Toggle (flip) the c-th bit of the integer pointed to by n
  //
  // PARAMETER EXPLANATION:
  // int* n - pointer to the integer we want to modify
  //          Using a pointer allows us to modify the original variable
  //          Without the pointer, we'd only modify a local copy
  // int c  - which bit position to flip (0 = rightmost, 31 = leftmost)

  // BIT FLIPPING TECHNIQUE USING XOR:
  //
  // Step 1: Create a "mask" with only the c-th bit set to 1
  //   (1 << c) creates this mask
  //   Example: if c = 3
  //            1 << 3 = 8 (binary: ...00001000)
  //            Only bit 3 is set to 1, all others are 0
  //
  // Step 2: XOR the number with the mask
  //   XOR (^) has a special property: it flips bits
  //   - If original bit is 0: 0 ^ 1 = 1 (bit gets set)
  //   - If original bit is 1: 1 ^ 1 = 0 (bit gets cleared)
  //   - Other bits: x ^ 0 = x (unchanged)
  //
  // Example: flip bit 2 of 5 (binary: ...00101)
  //   Mask: 1 << 2 = 4 (binary: ...00100)
  //   5 ^ 4 = 1 (binary: ...00001)
  //   The bit at position 2 changed from 1 to 0
  //
  // WHY XOR FOR FLIPPING?
  // - XOR with 1 always flips: 0^1=1, 1^1=0
  // - XOR with 0 preserves: 0^0=0, 1^0=1
  // - Our mask has 1 only at position c, so only that bit flips

  *n = (*n) ^ (1 << c);
  //   ↑    ↑      ↑
  //   |    |      └─ Create mask with c-th bit set
  //   |    └──────── Dereference to get current value
  //   └─────────────── Dereference to store result back

  // POINTER DEREFERENCING:
  // *n means "the value at the address stored in n"
  // We must dereference BOTH for reading (*n on right) and writing (*n on left)
}

int main(){

  int num;
  int choice;

  // INITIAL INPUT:
  printf("Enter an integer: ");
  scanf("%d",&num);

  // INFINITE LOOP:
  // while(1) creates an infinite loop because 1 is always true
  // This is a common pattern for interactive programs
  // User can exit with Ctrl+C (sends SIGINT signal)
  while(1){

    // DISPLAY CURRENT STATE:
    printf("The number is: %d\n",num);

    // DISPLAY BIT REPRESENTATION:
    // This helps visualize what the bits look like
    // Essential for understanding bit manipulation effects
    printBits(num);

    // GET USER'S CHOICE:
    printf("Which bit to flip?: ");
    scanf("%d",&choice);

    // FLIP THE CHOSEN BIT:
    // Pass &num (address of num) so flipBit can modify the original
    // Pass choice (value) to specify which bit
    flipBit(&num,choice);

    // Loop continues, showing the updated number and bits
  }

  return 0;  // Never reached due to infinite loop
}

// COMPREHENSIVE EXAMPLE WALKTHROUGH:
//
// Starting with num = 5:
// Binary: 00000000000000000000000000000101
//
// User enters: 0 (flip bit 0, the rightmost bit)
// Mask: 1 << 0 = 1 (binary: ...00001)
// 5 ^ 1 = 4 (binary: ...00100)
// Result: bit 0 changed from 1 to 0
//
// Now num = 4:
// Binary: 00000000000000000000000000000100
//
// User enters: 2 (flip bit 2)
// Mask: 1 << 2 = 4 (binary: ...00100)
// 4 ^ 4 = 0 (binary: ...00000)
// Result: bit 2 changed from 1 to 0
//
// Now num = 0:
// Binary: 00000000000000000000000000000000
//
// User enters: 31 (flip the leftmost bit)
// Mask: 1 << 31 = -2147483648 (binary: 10000000000000000000000000000000)
// 0 ^ -2147483648 = -2147483648
// Result: bit 31 changed from 0 to 1 (this is the sign bit!)
// Now num is negative due to two's complement representation

// BIT SHIFTING DETAILS:
//
// LEFT SHIFT (<<):
// - Shifts bits to the left, filling with 0s on the right
// - Each shift left multiplies by 2
// - Example: 5 << 1 = 10 (binary: 101 → 1010)
// - Example: 1 << 3 = 8 (binary: 1 → 1000)
// - Shifting left by n is equivalent to multiplying by 2^n
//
// RIGHT SHIFT (>>):
// - Shifts bits to the right
// - For SIGNED integers: sign-extends (fills with sign bit)
// - For UNSIGNED integers: fills with 0s
// - Each shift right divides by 2 (integer division)
// - Example: 5 >> 1 = 2 (binary: 101 → 10)
// - Example: -4 >> 1 = -2 (sign extends)
//
// WHY (n >> i) & 1 WORKS:
// Shifting right by i moves bit i to position 0
// AND with 1 isolates just that bit
// This is a fundamental bit manipulation pattern

// BITWISE OPERATORS TRUTH TABLES:
//
// AND (&):  0&0=0  0&1=0  1&0=0  1&1=1  (both must be 1)
// OR  (|):  0|0=0  0|1=1  1|0=1  1|1=1  (either can be 1)
// XOR (^):  0^0=0  0^1=1  1^0=1  1^1=0  (must be different)
// NOT (~):  ~0=1   ~1=0                  (inverts)

// COMMON BIT MANIPULATION PATTERNS:
//
// 1. Set bit n:        x |= (1 << n)   // OR with mask
// 2. Clear bit n:      x &= ~(1 << n)  // AND with inverted mask
// 3. Toggle bit n:     x ^= (1 << n)   // XOR with mask (used here!)
// 4. Test bit n:       x & (1 << n)    // Non-zero if bit is set
// 5. Extract bit n:    (x >> n) & 1    // Get 0 or 1 (used here!)
// 6. Set to value v:   x = (x & ~(1 << n)) | (v << n)

// TO COMPILE AND RUN:
// gcc bit1.c -o bit1
// ./bit1
// Enter an integer: 5
// The number is: 5
// 00000000000000000000000000000101
// Which bit to flip?: 0
// The number is: 4
// 00000000000000000000000000000100
// Which bit to flip?: 2
// (etc.)
