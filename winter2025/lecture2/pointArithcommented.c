// PEDAGOGICAL PURPOSE:
// This program demonstrates pointer arithmetic and how it automatically scales
// by the size of the pointed-to type, revealing the relationship between
// array indexing and memory addresses.
// Key learning objectives:
// 1. Understanding pointer arithmetic with different types
// 2. How adding to a pointer moves by sizeof(type) bytes, not 1 byte
// 3. The relationship between array notation and pointer arithmetic
// 4. Comparing address increments for int (4 bytes) vs double (8 bytes)
// 5. Format specifiers %p for pointers
// 6. Hexadecimal address representation

#include <stdio.h>
#include <stdlib.h>

int main(){

  // ARRAY INITIALIZATION:
  // Both arrays initialized with same values {0,1,2,3,4}
  // But different types: int vs double
  int arr1[5] = {0,1,2,3,4};
  double arr2[5] = {0,1,2,3,4};

  // FORMAT SPECIFIERS USED IN THIS PROGRAM:
  // %d - base 10 decimal integers (signed)
  // %x - base 16 hexadecimal integers (lowercase)
  // %o - base 8 octal integers
  // %p - pointer address (typically in hexadecimal with 0x prefix)

  // PRINT BASE ADDRESSES:
  // %p prints pointer addresses in hexadecimal
  // arr1 and arr2 are array names, which decay to pointers
  // They point to the first element of each array
  printf("The start of arr1 is: %p\n",arr1);
  printf("The start of arr2 is: %p\n",arr2);

  // EXPECTED OUTPUT (example):
  // The start of arr1 is: 0x7ffe5c6b1a10
  // The start of arr2 is: 0x7ffe5c6b1a20
  // (Actual addresses will vary due to ASLR)

  // ITERATE THROUGH ARRAY POSITIONS:
  // Starting at i=1 (not 0) to show pointer arithmetic from base
  for(int i = 1; i < 5; i++){

    // POINTER ARITHMETIC WITH INT ARRAY:
    // arr1 + i means:
    // - Start at arr1 (address of arr1[0])
    // - Add i * sizeof(int) bytes
    // - On most systems: i * 4 bytes
    //
    // Example (if arr1 starts at 0x1000):
    //   arr1 + 0 = 0x1000 (arr1[0])
    //   arr1 + 1 = 0x1004 (arr1[1]) ← 4 bytes later
    //   arr1 + 2 = 0x1008 (arr1[2]) ← 8 bytes from start
    //   arr1 + 3 = 0x100C (arr1[3]) ← 12 bytes from start
    //   arr1 + 4 = 0x1010 (arr1[4]) ← 16 bytes from start
    printf("The %dth element of arr1 is at: %p\n",i,arr1 +i);

    // POINTER ARITHMETIC WITH DOUBLE ARRAY:
    // arr2 + i means:
    // - Start at arr2 (address of arr2[0])
    // - Add i * sizeof(double) bytes
    // - On most systems: i * 8 bytes
    //
    // Example (if arr2 starts at 0x2000):
    //   arr2 + 0 = 0x2000 (arr2[0])
    //   arr2 + 1 = 0x2008 (arr2[1]) ← 8 bytes later
    //   arr2 + 2 = 0x2010 (arr2[2]) ← 16 bytes from start
    //   arr2 + 3 = 0x2018 (arr2[3]) ← 24 bytes from start
    //   arr2 + 4 = 0x2020 (arr2[4]) ← 32 bytes from start
    printf("The %dth element of arr2 is at: %p\n",i,arr2 +i);
  }

  return 0;
}

// COMPREHENSIVE EXPLANATION:
//
// POINTER ARITHMETIC RULES:
//
// When you add an integer to a pointer:
//   pointer + n
//
// The result is NOT pointer + n bytes
// The result IS pointer + (n * sizeof(*pointer)) bytes
//
// This is called "scaled pointer arithmetic"
// The scaling makes array indexing intuitive

// TYPE-SPECIFIC SCALING:
//
// int* ptr;
//   ptr + 1 → advances by sizeof(int) = 4 bytes (typically)
//
// double* ptr;
//   ptr + 1 → advances by sizeof(double) = 8 bytes (typically)
//
// char* ptr;
//   ptr + 1 → advances by sizeof(char) = 1 byte (always)
//
// struct BigThing* ptr;
//   ptr + 1 → advances by sizeof(struct BigThing) bytes

// ARRAY INDEXING IS POINTER ARITHMETIC:
//
// These are EXACTLY equivalent:
//   arr[i]  ≡  *(arr + i)
//
// When you write arr[i], the compiler translates it to:
// 1. Take the base address: arr
// 2. Add i (scaled by element size): arr + i
// 3. Dereference to get the value: *(arr + i)
//
// This is why arr[i] and i[arr] both work (though i[arr] is confusing!)
//   arr[3] = *(arr + 3) = *(3 + arr) = 3[arr]

// MEMORY LAYOUT EXAMPLE:
//
// Suppose arrays are allocated on stack:
//
// int arr1[5] = {0,1,2,3,4};
// Memory (assuming arr1 starts at 0x1000):
//   Address    Value      Expression
//   0x1000     0          arr1[0] or *arr1
//   0x1004     1          arr1[1] or *(arr1+1)
//   0x1008     2          arr1[2] or *(arr1+2)
//   0x100C     3          arr1[3] or *(arr1+3)
//   0x1010     4          arr1[4] or *(arr1+4)
//
// double arr2[5] = {0,1,2,3,4};
// Memory (assuming arr2 starts at 0x2000):
//   Address    Value      Expression
//   0x2000     0.0        arr2[0] or *arr2
//   0x2008     1.0        arr2[1] or *(arr2+1)
//   0x2010     2.0        arr2[2] or *(arr2+2)
//   0x2018     3.0        arr2[3] or *(arr2+3)
//   0x2020     4.0        arr2[4] or *(arr2+4)

// HEXADECIMAL ADDRESSES:
//
// %p prints addresses in hexadecimal (base 16)
// Hexadecimal uses digits: 0-9, A-F (or a-f)
//
// Example output:
//   The start of arr1 is: 0x7ffe5c6b1a10
//   The 1th element of arr1 is at: 0x7ffe5c6b1a14
//   The 2th element of arr1 is at: 0x7ffe5c6b1a18
//   The 3th element of arr1 is at: 0x7ffe5c6b1a1c
//   The 4th element of arr1 is at: 0x7ffe5c6b1a20
//
// Notice the differences:
//   0x...1a14 - 0x...1a10 = 0x4 = 4 bytes
//   0x...1a18 - 0x...1a14 = 0x4 = 4 bytes
//   Each increment is 4 bytes (sizeof(int))
//
// For arr2:
//   The start of arr2 is: 0x7ffe5c6b1a20
//   The 1th element of arr2 is at: 0x7ffe5c6b1a28
//   The 2th element of arr2 is at: 0x7ffe5c6b1a30
//   The 3th element of arr2 is at: 0x7ffe5c6b1a38
//   The 4th element of arr2 is at: 0x7ffe5c6b1a40
//
// Notice the differences:
//   0x...1a28 - 0x...1a20 = 0x8 = 8 bytes
//   0x...1a30 - 0x...1a28 = 0x8 = 8 bytes
//   Each increment is 8 bytes (sizeof(double))

// WHY POINTER ARITHMETIC IS SCALED:
//
// Imagine if pointers weren't scaled:
//   int* p = arr1;
//   p + 1 would add 1 byte, not 4 bytes
//   You'd have to write: p + sizeof(int)
//   Array indexing: *(arr1 + i*sizeof(int))  // Horrible!
//
// With scaling:
//   int* p = arr1;
//   p + 1 adds 4 bytes (one int)
//   Array indexing: arr1[i] = *(arr1 + i)  // Clean!
//
// The compiler handles the scaling automatically

// POINTER SUBTRACTION:
//
// You can also subtract pointers of the same type:
//   int* p1 = &arr1[4];
//   int* p2 = &arr1[1];
//   ptrdiff_t diff = p1 - p2;  // Result: 3 (number of elements)
//
// Note: Result is in ELEMENTS, not bytes
// p1 - p2 = (address_p1 - address_p2) / sizeof(int)
// If p1 is 12 bytes after p2, and sizeof(int)==4, then p1-p2 == 3

// VOID POINTERS (special case):
//
// void* pointers have no type, so sizeof(void) is undefined
// You CANNOT do arithmetic on void* directly:
//   void* vp = arr1;
//   vp + 1;  // ERROR! Compiler doesn't know how much to add
//
// You must cast first:
//   void* vp = arr1;
//   (int*)vp + 1;  // OK, adds sizeof(int) bytes
//
// GCC extension: treats void* arithmetic as if void has size 1
// But this is non-standard

// ARRAY DECAY:
//
// When you use an array name in an expression, it "decays" to a pointer
//
// arr1 in an expression is equivalent to &arr1[0]
//
// Exceptions (array does NOT decay):
// 1. sizeof(arr1) - returns size of entire array
// 2. &arr1 - returns pointer to entire array (different type!)
//
// Example:
//   sizeof(arr1) = 20 bytes (5 ints × 4 bytes)
//   sizeof(arr1+0) = 8 bytes (size of pointer on 64-bit system)

// MULTI-DIMENSIONAL ARRAYS:
//
// For 2D arrays, pointer arithmetic gets more complex:
//   int matrix[3][4];
//   matrix + 1 → skips entire row (4 ints = 16 bytes)
//   matrix[1] → pointer to second row
//   matrix[1] + 2 → pointer to matrix[1][2]
//   *(matrix[1] + 2) → value at matrix[1][2]

// PRACTICAL USES:
//
// 1. Iterating through arrays:
//    for (int* p = arr1; p < arr1 + 5; p++) {
//      printf("%d ", *p);
//    }
//
// 2. Working with strings:
//    char* str = "hello";
//    while (*str) {  // Until null terminator
//      putchar(*str);
//      str++;  // Advance by 1 byte
//    }
//
// 3. Buffer manipulation:
//    void process(char* buffer, size_t len) {
//      char* end = buffer + len;
//      while (buffer < end) {
//        // Process each byte
//        buffer++;
//      }
//    }

// COMMON MISTAKES:
//
// 1. FORGETTING SCALING:
//    int* p = arr1;
//    char* cp = (char*)p;
//    cp + 1;  // Advances 1 BYTE, not one int!
//
// 2. MIXING POINTER TYPES:
//    int* ip = arr1;
//    double* dp = (double*)ip;  // Type mismatch!
//    dp + 1;  // Advances 8 bytes, not aligned with int array
//
// 3. OUT OF BOUNDS:
//    int* p = arr1 + 5;  // Points past end (arr1[5] doesn't exist)
//    *p = 42;  // UNDEFINED BEHAVIOR!
//
// 4. NULL POINTER ARITHMETIC:
//    int* p = NULL;
//    p + 1;  // Technically defined, but useless
//    *p;     // CRASH!

// TO COMPILE AND RUN:
// gcc pointArith.c -o pointArith
// ./pointArith
//
// Expected output (addresses will vary):
// The start of arr1 is: 0x7ffc8b2a4e10
// The start of arr2 is: 0x7ffc8b2a4e20
// The 1th element of arr1 is at: 0x7ffc8b2a4e14
// The 1th element of arr2 is at: 0x7ffc8b2a4e28
// The 2th element of arr1 is at: 0x7ffc8b2a4e18
// The 2th element of arr2 is at: 0x7ffc8b2a4e30
// The 3th element of arr1 is at: 0x7ffc8b2a4e1c
// The 3th element of arr2 is at: 0x7ffc8b2a4e38
// The 4th element of arr1 is at: 0x7ffc8b2a4e20
// The 4th element of arr2 is at: 0x7ffc8b2a4e40
//
// Notice: arr1 addresses increase by 4 (hex: 0x4)
//         arr2 addresses increase by 8 (hex: 0x8)
