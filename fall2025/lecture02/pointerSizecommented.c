// PEDAGOGICAL PURPOSE:
// This program demonstrates a fundamental property of pointers in C.
// Key learning objectives:
// 1. ALL pointer types have the SAME SIZE regardless of what they point to
// 2. Pointer size depends on the system architecture (32-bit vs 64-bit)
// 3. Understanding WHY all pointers are the same size (they're just addresses)
// 4. The difference between sizeof(variable) and sizeof(pointer to variable)
// 5. Pointers are memory addresses, not the actual data
//
// CORE CONCEPT:
// A pointer is just a memory address. On a 64-bit system, all addresses
// are 64 bits (8 bytes), regardless of whether they point to an int,
// a float, a struct, or anything else.

#include "stdio.h"
#include "stdlib.h"

int main(){
  // VARIABLE DECLARATIONS:
  // We declare four variables of different types
  // Each type has a DIFFERENT SIZE:
  // - int: typically 4 bytes
  // - float: 4 bytes
  // - long: 8 bytes (on most modern systems)
  // - double: 8 bytes
  int num1;
  float num2;
  long num3;
  double num4;

  // POINTER SIZE DEMONSTRATION:
  // &num1 gets the ADDRESS of num1 (creates an int* pointer)
  // sizeof(&num1) returns the SIZE OF THE POINTER, not the size of num1
  // Expected output on 64-bit systems: 8 bytes for all pointer types
  // Expected output on 32-bit systems: 4 bytes for all pointer types
  printf("The size of an int* is: %ld\n",sizeof(&num1));
  printf("The size of a long* is: %ld\n",sizeof(&num3));
  printf("The size of a float* is: %ld\n",sizeof(&num2));
  printf("The size of a double* is: %ld\n",sizeof(&num4));

  // ALL FOUR LINES WILL PRINT THE SAME VALUE!
  // This demonstrates that pointer size is independent of the pointed-to type

  return 0;
}

// CONCEPTUAL EXPLANATION:
//
// WHY ARE ALL POINTERS THE SAME SIZE?
//
// A pointer is simply a MEMORY ADDRESS.
// It's a number that indicates a location in the computer's memory.
//
// ANALOGY:
// Think of memory as a giant array of bytes.
// A pointer is like an index into this array.
// The index (address) is always the same size, regardless of what's
// stored at that location.
//
// Just like:
// - A street address "123 Main St" takes the same amount of space to write,
//   whether it points to a small house or a large mansion.
//
// SYSTEM ARCHITECTURE:
// - 32-bit system: Memory addresses are 32 bits (4 bytes)
//                  All pointers are 4 bytes
//                  Can address up to 2^32 bytes (4 GB) of memory
//
// - 64-bit system: Memory addresses are 64 bits (8 bytes)
//                  All pointers are 8 bytes
//                  Can address up to 2^64 bytes (16 exabytes) of memory
//
// WHAT THE POINTER CONTAINS:
// int* ptr points to an int, but the pointer itself just contains an address.
//
// Example memory layout on a 64-bit system:
//
// Variable    Address      Value         Type
// --------    --------     -------       ------
// num1        0x7FFF1000   42            int (4 bytes)
// num2        0x7FFF1010   3.14          float (4 bytes)
// num3        0x7FFF1020   1000000       long (8 bytes)
// num4        0x7FFF1030   2.71828       double (8 bytes)
//
// When we take addresses:
// &num1 = 0x7FFF1000  (this address itself is 8 bytes on 64-bit system)
// &num2 = 0x7FFF1010  (this address itself is 8 bytes on 64-bit system)
// &num3 = 0x7FFF1020  (this address itself is 8 bytes on 64-bit system)
// &num4 = 0x7FFF1030  (this address itself is 8 bytes on 64-bit system)
//
// Notice: All addresses are 8 bytes, even though num1 and num2 are only 4 bytes!
//
// SIZEOF COMPARISON:
//
// sizeof(int) = 4         // Size of an integer variable
// sizeof(int*) = 8        // Size of a pointer to an integer (on 64-bit)
//
// sizeof(double) = 8      // Size of a double variable
// sizeof(double*) = 8     // Size of a pointer to a double (on 64-bit)
//
// sizeof(char) = 1        // Size of a character variable
// sizeof(char*) = 8       // Size of a pointer to a character (on 64-bit)
//
// The pattern: Variables have different sizes, but pointers to them don't!
//
// TYPE SAFETY vs SIZE:
// Even though all pointers are the same size, they have DIFFERENT TYPES:
// - int* can only (safely) point to int
// - float* can only (safely) point to float
// - etc.
//
// The type tells the compiler:
// 1. How many bytes to read when dereferencing (*ptr)
// 2. How to interpret those bytes (int vs float vs etc.)
// 3. How much to move when doing pointer arithmetic (ptr + 1)
//
// POINTER ARITHMETIC EXAMPLE:
// int* ptr;       // Points to 4-byte integers
// ptr + 1;        // Moves forward 4 bytes (size of int)
//
// double* ptr2;   // Points to 8-byte doubles
// ptr2 + 1;       // Moves forward 8 bytes (size of double)
//
// Even though both pointers are 8 bytes in size, they move by different
// amounts because they point to different-sized types.
//
// PRACTICAL IMPLICATION:
// This is why void* exists in C:
// void* is a "generic pointer" - it's the same size as any other pointer,
// but doesn't specify what it points to. You must cast it to use it.
//
// This is also why malloc returns void*:
// malloc doesn't know what type you're allocating, but it returns an
// address (pointer) that's the right size for any pointer type.
//
// COMMON MISCONCEPTION:
// "If I have a pointer to a large struct, won't the pointer be bigger?"
// NO! The pointer is just an address. It's the same size whether it points
// to a char (1 byte) or a huge struct (1000 bytes).
//
// CHECKING YOUR SYSTEM:
// Run this program and check the output:
// - If you see 8: You're on a 64-bit system
// - If you see 4: You're on a 32-bit system
