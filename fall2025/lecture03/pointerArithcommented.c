// PEDAGOGICAL PURPOSE:
// This program demonstrates pointer arithmetic and memory addressing in C.
// Key learning objectives:
// 1. Understanding that variables have addresses in memory
// 2. Learning about the address-of operator (&)
// 3. Seeing how pointer arithmetic works with the pointer's type
// 4. Understanding that &var + 1 moves to the NEXT variable location
// 5. Learning hexadecimal representation of memory addresses (%p)
// 6. Understanding the relationship between array indexing and pointer arithmetic
// 7. Visualizing that memory is a sequence of addressable locations

#include "stdio.h"

int main(){

  // VARIABLE DECLARATION:
  // Declare a single integer variable on the stack
  // The compiler allocates 4 bytes of memory for this variable
  // The memory location is determined at runtime
  int num1;

  // IMPORTANT COMMENT FROM ORIGINAL CODE:
  // This is a FUNDAMENTAL relationship in C:
  // *(pointer + i) is EXACTLY EQUIVALENT to pointer[i]
  // This means array indexing is just syntactic sugar for pointer arithmetic!
  // Example: arr[3] is the same as *(arr + 3)
  // *(pointer + i) = pointer[i]

  // DEMONSTRATION 1: FINDING THE ADDRESS OF A VARIABLE
  // The & operator (address-of) gives us the memory location where num1 lives
  // %p is the format specifier for printing pointers (shows address in hex)
  // WHY HEXADECIMAL?
  //   - Memory addresses are easier to read in hex
  //   - Each hex digit represents exactly 4 bits
  //   - On 64-bit systems, addresses are 8 bytes = 16 hex digits
  printf("The address of num1 is: %p\n",&num1);

  // DEMONSTRATION 2: POINTER ARITHMETIC ON ADDRESS
  // &num1 gives us a pointer to num1 (type: int*)
  // &num1 + 1 performs POINTER ARITHMETIC
  // This does NOT just add 1 to the address!
  // It adds sizeof(int) bytes to the address
  // On most systems: sizeof(int) = 4 bytes
  //
  // CONCEPTUAL MEANING:
  // If we had an array int arr[2], and num1 was arr[0],
  // then &num1 + 1 would point to where arr[1] would be
  //
  // WHY THIS MATTERS:
  // This shows how arrays work in memory - consecutive elements
  // are sizeof(type) bytes apart
  //
  // WHAT WE'LL SEE:
  // The second address will be 4 bytes higher than the first
  // For example:
  //   First address:  0x7ffee4b2a8fc
  //   Second address: 0x7ffee4b2a900  (note: fc + 4 = 100, but in hex!)
  //   Difference: 4 bytes (size of an int)
  printf("The address of &num1 + 1 is: %p \n",&num1 + 1);

  // VISUAL REPRESENTATION:
  // Memory layout (assuming num1 starts at address 0x1000):
  //
  // Address:  0x1000  0x1001  0x1002  0x1003  0x1004  0x1005  0x1006  0x1007
  // Content:  [        num1 (4 bytes)        ] [  next int would go here   ]
  //           ^                                 ^
  //           &num1 points here                 &num1 + 1 points here
  //
  // Note: The "next int" doesn't actually exist in this program,
  // but this shows where it WOULD be if we had an array!

  // KEY INSIGHT:
  // Pointer arithmetic is SCALED by the size of the type
  // ptr + n actually means: (address of ptr) + (n * sizeof(type))
  // This is why:
  //   int* + 1 moves 4 bytes
  //   long* + 1 moves 8 bytes (on 64-bit systems)
  //   char* + 1 moves 1 byte
  //   double* + 1 moves 8 bytes

  return 0;
}

// EXPECTED OUTPUT PATTERN (actual addresses will vary):
// The address of num1 is: 0x7ffc8a6b9a2c
// The address of &num1 + 1 is: 0x7ffc8a6b9a30
//
// OBSERVE: The difference is 4 bytes (0x30 - 0x2c = 4 in hexadecimal)

// WHY THIS MATTERS IN PRACTICE:
// 1. Arrays: Understanding that arr[i] is at address (arr + i)
// 2. Strings: char* strings use pointer arithmetic to traverse characters
// 3. Memory manipulation: Moving through memory buffers
// 4. Data structures: Accessing elements in linked structures

// TRY IT:
// 1. Compile: gcc pointerArith.c -o pointerArith
// 2. Run: ./pointerArith
// 3. Calculate the difference between the two addresses
// 4. Verify it equals sizeof(int) - try: sizeof(int) in a printf!
// 5. Try changing num1 to other types (long, char, double) and observe!
