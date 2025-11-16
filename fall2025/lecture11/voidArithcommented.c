// PEDAGOGICAL PURPOSE:
// This program demonstrates a quirk of void* pointer arithmetic in C.
// Key learning objectives:
// 1. Understanding that void* has special arithmetic behavior
// 2. Learning that void* arithmetic treats void* like char*
// 3. Seeing the difference between void* and typed pointer arithmetic
// 4. Understanding why this behavior exists (and why it's controversial)
// 5. Learning when you might encounter this in practice
// 6. Understanding GCC's extension to the C standard
// 7. Recognizing a subtle edge case in pointer manipulation

#include "stdio.h"

int main(){

  // CREATE AN INTEGER ARRAY:
  // Declares an array of 5 integers on the stack
  // The elements are uninitialized (contain garbage values)
  // But we don't care about the values - we only care about addresses
  int arr[5];

  // IMPLICIT CONVERSION TO void*:
  // arr (an int*) is assigned to weirdo (a void*)
  //
  // TYPE CONVERSION:
  // arr has type int* (decays from int[5])
  // weirdo has type void*
  // int* → void* is an implicit conversion (always allowed)
  //
  // WHAT IS weirdo?
  // A void* pointing to the first element of arr
  // It points to the same memory location as arr
  // But it has "forgotten" that it's pointing to ints
  void* weirdo = arr;

  // PRINT THE ADDRESS:
  // %p is the format specifier for pointers
  // Prints the address where arr starts
  // This is the address of arr[0]
  //
  // EXAMPLE OUTPUT: 0x7ffc8a6b9a20
  printf("weirdo: %p\n",weirdo);

  // THE SURPRISING PART: void* POINTER ARITHMETIC
  // weirdo+1 performs pointer arithmetic on a void*
  //
  // WHAT YOU MIGHT EXPECT:
  // Since weirdo points to an int array, you might expect:
  // weirdo+1 would add sizeof(int) bytes (typically 4)
  // Like how int* arithmetic works
  //
  // WHAT ACTUALLY HAPPENS:
  // weirdo+1 adds exactly 1 BYTE!
  // void* arithmetic treats the pointer as char* (1-byte units)
  //
  // WHY?
  // The C standard says void* arithmetic is UNDEFINED
  // But GCC (and some other compilers) allow it as an extension
  // GCC treats void* like char* for arithmetic purposes
  // This means weirdo+1 moves forward by 1 byte, not 4 bytes
  //
  // COMPARISON:
  // If arr were int*: arr+1 would move sizeof(int) bytes = 4 bytes
  // Since weirdo is void*: weirdo+1 moves 1 byte
  //
  // OBSERVE:
  // The second address will be exactly 1 byte higher than the first
  // For example:
  //   First address:  0x7ffc8a6b9a20
  //   Second address: 0x7ffc8a6b9a21  (notice: +1, not +4!)
  printf("weirdo+1: %p\n",weirdo+1);

  // COMMENT FROM ORIGINAL CODE:
  // void star pointer arithmetic behaves
  // like a char
  //
  // This is the key insight!
  // void* + n adds n bytes (like char*)
  // Not n * sizeof(pointed-to-type) (like typed pointers)

  return 0;
}

// EXPECTED OUTPUT (addresses will vary):
// weirdo: 0x7ffc8a6b9a20
// weirdo+1: 0x7ffc8a6b9a21
//
// OBSERVE: The difference is exactly 1 byte (0x21 - 0x20 = 0x1)

// DETAILED COMPARISON: POINTER ARITHMETIC
//
// TYPED POINTER (int*):
//   int arr[5];
//   int* ptr = arr;
//   ptr + 1  →  adds sizeof(int) bytes = 4 bytes
//   ptr + 2  →  adds 2*sizeof(int) bytes = 8 bytes
//
// TYPED POINTER (double*):
//   double arr[5];
//   double* ptr = arr;
//   ptr + 1  →  adds sizeof(double) bytes = 8 bytes
//
// TYPED POINTER (char*):
//   char arr[5];
//   char* ptr = arr;
//   ptr + 1  →  adds sizeof(char) bytes = 1 byte
//
// void* (GCC extension):
//   void* ptr = arr;
//   ptr + 1  →  adds 1 byte (treats like char*)
//   ptr + 2  →  adds 2 bytes

// WHY THIS BEHAVIOR EXISTS:
//
// 1. C STANDARD:
//    The C standard says void* arithmetic is UNDEFINED BEHAVIOR
//    You're not supposed to do it at all!
//
// 2. GCC EXTENSION:
//    GCC (GNU Compiler Collection) extends C to allow void* arithmetic
//    For consistency, it treats void* like char*
//    char is the "byte" type in C
//    So void* arithmetic works in byte units
//
// 3. RATIONALE:
//    When you don't know the type, byte-level arithmetic makes sense
//    It's the most generic/basic unit
//    You can manually control exact byte offsets
//
// 4. COMPATIBILITY:
//    This behavior is also in GNU C++ (g++)
//    Many codebases rely on this extension

// C STANDARD COMPLIANCE:
//
// STANDARD C (C99, C11):
//   void* ptr;
//   ptr + 1;  // UNDEFINED BEHAVIOR!
//
// To be strictly compliant, cast first:
//   ((char*)ptr) + 1;  // Well-defined (adds 1 byte)
//   ((int*)ptr) + 1;   // Well-defined (adds sizeof(int) bytes)
//
// GCC WITH -pedantic:
//   gcc -pedantic voidArith.c
//   You'll get a warning about void* arithmetic

// WHEN YOU MIGHT ENCOUNTER THIS:
//
// 1. LOW-LEVEL MEMORY MANIPULATION:
//    void* memcpy(void* dest, const void* src, size_t n)
//    Inside memcpy, you work with bytes, not typed data
//
// 2. GENERIC DATA STRUCTURES:
//    Implementing a generic buffer or queue
//    void* is used to store arbitrary data
//
// 3. BINARY PROTOCOLS:
//    Parsing network packets or file formats
//    void* lets you walk through bytes
//
// 4. CUSTOM ALLOCATORS:
//    void* malloc(size_t size)
//    Returns void* because it doesn't know what type you want

// PRACTICAL EXAMPLE:
//
// Walking through a buffer byte by byte:
//   void* buffer = malloc(100);
//   void* current = buffer;
//
//   // In standard C, you'd cast:
//   current = (char*)current + 10;
//
//   // In GCC, this works (but non-portable):
//   current = current + 10;

// THE SAFE WAY:
//
// Always cast void* before arithmetic:
//
// BYTE-LEVEL ACCESS:
//   void* ptr = ...;
//   char* byte_ptr = (char*)ptr;
//   byte_ptr + 1;  // Clearly adds 1 byte
//
// TYPE-LEVEL ACCESS:
//   void* ptr = ...;
//   int* int_ptr = (int*)ptr;
//   int_ptr + 1;  // Clearly adds sizeof(int) bytes
//
// This makes your intent clear and is portable

// VISUAL REPRESENTATION:
//
// Memory layout of arr (int[5]):
//
// Address:  0x1000  0x1001  0x1002  0x1003  0x1004  0x1005  0x1006  0x1007  ...
//           [      arr[0] (int)            ] [      arr[1] (int)            ] ...
//
// int* ptr = arr;
// ptr + 1:  Points to 0x1004 (next int)
//
// void* weirdo = arr;
// weirdo + 1:  Points to 0x1001 (next BYTE, middle of arr[0]!)

// DANGER: MISALIGNMENT
//
// weirdo+1 points to the MIDDLE of an int!
// If you cast it back to int* and dereference:
//
//   int* misaligned = (int*)(weirdo + 1);
//   int value = *misaligned;  // DANGER!
//
// PROBLEMS:
// 1. ALIGNMENT: Many architectures require ints to be 4-byte aligned
//    Accessing misaligned int may cause:
//    - Slowdown (extra memory accesses)
//    - Bus error/crash (on some architectures like ARM)
// 2. GARBAGE VALUE: Reading from the middle of an int gives nonsense

// COMPILER WARNINGS:
//
// COMPILE WITH -pedantic:
//   gcc -pedantic voidArith.c -o voidArith
//   Warning: pointer of type 'void *' used in arithmetic [-Wpointer-arith]
//
// This warns you that void* arithmetic is a GCC extension
// Your code won't be portable to other compilers

// RELATED CONCEPTS:
//
// 1. SIZEOF(void):
//    void has no size (sizeof(void) is invalid)
//    But void* has a size (sizeof(void*) is typically 8 on 64-bit)
//
// 2. VOID PARAMETERS:
//    void func(void) means "no parameters"
//    void func() in C means "unspecified parameters" (different!)
//
// 3. POINTER ARITHMETIC RULES:
//    For type T*:
//      ptr + n  means  (T*)((char*)ptr + n * sizeof(T))
//    For void*:
//      ptr + n  means  (void*)((char*)ptr + n)  [GCC extension]

// TRY IT:
// 1. Compile: gcc voidArith.c -o voidArith
// 2. Run: ./voidArith
// 3. Calculate the difference between the two addresses
//    Should be exactly 1 (0x01 in hex)
// 4. EXPERIMENT: Change weirdo+1 to weirdo+4
//    Now the difference should be 4 bytes
// 5. EXPERIMENT: Cast before arithmetic:
//    printf("%p\n", (int*)weirdo + 1);
//    Now it should add 4 bytes (sizeof(int))
// 6. EXPERIMENT: Compile with -pedantic:
//    gcc -pedantic -Wall voidArith.c
//    See the warning about void* arithmetic
// 7. COMPARE: Try with different array types:
//    double arr[5];
//    void* weirdo = arr;
//    Compare weirdo+1 vs ((double*)weirdo)+1
// 8. CHALLENGE: What happens if you dereference weirdo+1?
//    *((int*)(weirdo+1))  - likely garbage or crash!
