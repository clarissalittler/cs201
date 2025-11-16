// PEDAGOGICAL PURPOSE:
// This program introduces hexadecimal (base-16) number representation
// and demonstrates printf format specifiers for different number bases.
// Key learning objectives:
// 1. Understanding that the SAME number can be displayed in different bases
// 2. Learning about decimal (base-10), hexadecimal (base-16), and octal (base-8)
// 3. Understanding hexadecimal notation and hex-to-decimal conversion
// 4. Learning printf format specifiers: %d, %x, %o
// 5. Recognizing WHY hexadecimal is preferred in systems programming
// 6. Understanding that the representation doesn't change the value, only how we view it

#include "stdio.h"

/*
  HEXADECIMAL REFERENCE TABLE:
  This table shows the mapping between hex and decimal digits

  hex    / decimal
  0        0        (same in all bases)
  1        1
  2        2
  3        3
  4        4
  5        5
  6        6
  7        7
  8        8
  9        9
  A        10       (hex uses letters for values 10-15)
  B        11
  C        12
  D        13
  E        14
  F        15

  MULTI-DIGIT HEXADECIMAL:
  Each position in hex represents a power of 16
  For two-digit hex numbers:

  00   ->  0                    (0×16 + 0 = 0)
  FF   ->  16*15 + 15 = 255     (15×16 + 15 = 240 + 15 = 255)

  This is just like decimal where:
  25 = 2×10 + 5

  But in hex:
  25 (hex) = 2×16 + 5 = 37 (decimal)
*/

int main(){

  int num;  // Declared but not used in this version

  // SIZE OF CHAR:
  // char is the smallest addressable unit in C (always 1 byte)
  // sizeof(char) is GUARANTEED to be 1 by the C standard
  // %ld is the format specifier for long integers
  // sizeof returns size_t, which should be printed with %zu, but %ld works too
  printf("The size of a char is: %ld\n",sizeof(char));

  // ARRAY OF CHARACTERS:
  // Create an array of 10 chars (10 bytes of memory)
  char arr[10];

  // POINTER ARITHMETIC WITH CHAR:
  // This loop demonstrates addresses of consecutive array elements
  // WHY USE CHAR ARRAY FOR THIS DEMO?
  // - sizeof(char) = 1, so addresses increment by 1
  // - This makes it easy to see consecutive memory addresses
  // - Perfect for understanding memory layout
  for(int i = 0; i < 10; i++){
    // ARRAY ADDRESS CALCULATION:
    // arr is a pointer to the first element (arr[0])
    // arr + i calculates the address of the i-th element
    // Since arr is char*, arr+i moves forward by i*sizeof(char) = i bytes
    //
    // %p prints the pointer in HEXADECIMAL
    // WHY HEXADECIMAL FOR ADDRESSES?
    // - Memory addresses align nicely with hex (powers of 2)
    // - Each byte is exactly 2 hex digits
    // - Easier to see byte boundaries and alignment
    printf("The address of the %d th element is %p\n",i,arr+i);
  }

  // WHAT YOU'LL SEE:
  // Output will show 10 consecutive addresses, each 1 byte apart
  // For example:
  //   The address of the 0 th element is 0x7ffc8a6b9a20
  //   The address of the 1 th element is 0x7ffc8a6b9a21
  //   The address of the 2 th element is 0x7ffc8a6b9a22
  //   ...
  //   The address of the 9 th element is 0x7ffc8a6b9a29
  //
  // OBSERVE: The last hex digit increments by 1 each time
  // This shows memory is a LINEAR sequence of bytes!

  return 0;
}

// WHY HEXADECIMAL IN SYSTEMS PROGRAMMING?
// 1. BYTE REPRESENTATION: One byte = exactly 2 hex digits (00 to FF)
//    In decimal, one byte could be 1 to 3 digits (0 to 255)
//
// 2. BIT PATTERNS: Each hex digit = exactly 4 bits
//    0xF = 1111 in binary
//    0xA = 1010 in binary
//    Easy mental conversion!
//
// 3. MEMORY ALIGNMENT: Addresses often align to powers of 2
//    0x1000, 0x2000, etc. are clean in hex
//    4096, 8192, etc. are awkward in decimal
//
// 4. BITMASKS: Binary operations are clearer in hex
//    0xFF = 11111111 (all bits set in one byte)
//    0x0F = 00001111 (lower 4 bits set)

// RELATIONSHIP TO OTHER FILES:
// This program sets up the foundation for understanding:
// - byteOrder.c: How multi-byte values are stored (uses hex to show bytes)
// - whyHex.c: Demonstrates different representations of the same value
// - byteMunging.c: Uses hex addresses to show pointer arithmetic

// TRY IT:
// 1. Compile: gcc printHex.c -o printHex
// 2. Run: ./printHex
// 3. Observe that addresses differ by 0x1 (hexadecimal 1 = decimal 1)
// 4. Try changing char arr[10] to int arr[10] and arr+i
//    What happens to the address spacing? (Should be 4 bytes apart!)
