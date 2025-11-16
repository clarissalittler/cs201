// PEDAGOGICAL PURPOSE:
// This program demonstrates the critical concept of pointer casting and how
// pointer type affects pointer arithmetic in C.
// Key learning objectives:
// 1. Understanding that pointer arithmetic is scaled by the SIZE of the type
// 2. Demonstrating that pointer + 1 moves forward by sizeof(type) bytes
// 3. Showing how casting changes pointer arithmetic behavior
// 4. Visualizing the difference between int*, long*, and char* arithmetic
// 5. Understanding that the same memory location can be accessed with different interpretations
// 6. Learning about pointer address representation in hexadecimal

#include "stdio.h"

int main(){

  // ARRAY INITIALIZATION:
  // Create an array of 10 integers on the stack
  // This allocates 40 bytes of memory (10 * sizeof(int) = 10 * 4 = 40 bytes)
  int arr[10];

  // Fill the array with squares (0, 1, 4, 9, 16, 25, 36, 49, 64, 81)
  // This is just to have some meaningful data in memory
  for(int i = 0; i < 10; i++){
    arr[i] = i*i;
  }

  // DEMONSTRATION 1: DEFAULT INT* ARITHMETIC
  // When we use arr without casting, it's treated as int*
  // arr points to the first element (index 0)
  // arr+1 points to the SECOND element (index 1)
  // WHY? Because arr is an int*, so +1 means "move forward by sizeof(int) bytes"
  // On most systems, this moves forward 4 bytes
  // %p format specifier prints the pointer in hexadecimal
  printf("If we don't cast at all arr+1 is: %p\n", arr+1);

  // DEMONSTRATION 2: LONG* ARITHMETIC
  // Casting arr to long* changes how pointer arithmetic works
  // ((long*)arr) + 1 moves forward by sizeof(long) bytes
  // On 64-bit systems, sizeof(long) = 8 bytes
  // So this moves 8 bytes forward from arr (skipping TWO integers)
  // IMPORTANT: We're not changing the data, just how we're interpreting it!
  printf("If we cast to long, arr+1 is: %p\n", ((long*)arr) + 1);

  // DEMONSTRATION 3: CHAR* ARITHMETIC
  // Casting arr to char* makes the finest-grained pointer arithmetic
  // ((char*)arr) + 1 moves forward by sizeof(char) = 1 byte
  // This only advances ONE byte from the start of arr
  // This demonstrates we can access memory at the byte level
  // PRACTICAL USE: This technique is used to examine byte-by-byte memory layout
  printf("If we cast to char, arr+1 is: %p\n", ((char*)arr) + 1);

  // VISUAL REPRESENTATION (assuming arr starts at address 0x1000):
  // Memory Layout (each box is 1 byte):
  // Address:  0x1000 0x1001 0x1002 0x1003 | 0x1004 0x1005 0x1006 0x1007 | 0x1008 ...
  // Content:  [  arr[0] = 0 (4 bytes)    ] | [  arr[1] = 1 (4 bytes)    ] | [ arr[2]...
  //
  // arr+1 (as int*):    Points to 0x1004 (moved 4 bytes)
  // ((long*)arr)+1:     Points to 0x1008 (moved 8 bytes)
  // ((char*)arr)+1:     Points to 0x1001 (moved 1 byte)

  // KEY INSIGHT:
  // Pointer arithmetic is NOT just adding to a number!
  // The compiler automatically multiplies by sizeof(type)
  // This is why we can write arr[i] instead of *(arr + i*sizeof(int))

  return 0;
}

// EXPECTED OUTPUT PATTERN (addresses will vary):
// If we don't cast at all arr+1 is: 0x7ffc1234abcd
// If we cast to long, arr+1 is: 0x7ffc1234abc8  (notice: different offset)
// If we cast to char, arr+1 is: 0x7ffc1234abc1  (notice: smallest offset)
//
// Looking at the last hex digit differences reveals the byte offsets!

// TRY IT YOURSELF:
// 1. Compile: gcc byteMunging.c -o byteMunging
// 2. Run: ./byteMunging
// 3. Compare the addresses - subtract the first from each subsequent one
// 4. You should see differences of 4, 8, and 1 bytes respectively
