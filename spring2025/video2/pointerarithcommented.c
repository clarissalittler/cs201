// PEDAGOGICAL PURPOSE:
// This program demonstrates pointer arithmetic and how it relates to array types.
// It shows that pointer arithmetic is scaled by the size of the pointed-to type.
// Key learning objectives:
// 1. Understanding pointer arithmetic (ptr + 1 doesn't add 1 byte!)
// 2. Relationship between array names and pointers
// 3. How sizeof() determines pointer arithmetic scaling
// 4. Different data types have different sizes
// 5. Memory addresses and pointer values
// 6. Why arrays and pointers are closely related but not identical

#include <stdio.h>      // STANDARD I/O: For printf
#include <stdlib.h>     // STANDARD LIBRARY: General utilities

int main(){

  int arr1[10];         // INTEGER ARRAY: Array of 10 ints
                        // Each int is typically 4 bytes
                        // Total size: 10 * 4 = 40 bytes
                        // Memory layout: [int][int][int]...[int]
                        // arr1 is the name of the array
                        // When used in most contexts, arr1 "decays" to a pointer
                        // to the first element

  double arr2[10];      // DOUBLE ARRAY: Array of 10 doubles
                        // Each double is typically 8 bytes
                        // Total size: 10 * 8 = 80 bytes
                        // Memory layout: [double][double]...[double]

  char arr3[10];        // CHARACTER ARRAY: Array of 10 chars
                        // Each char is 1 byte (by definition)
                        // Total size: 10 * 1 = 10 bytes
                        // Memory layout: [char][char]...[char]

  printf("The addr of arr1 is %p and the addr of arr1+1 is %p\n",arr1,arr1+1);
                        // PRINT INTEGER ARRAY ADDRESSES:
                        // %p is the format specifier for pointers (prints in hex)
                        // arr1: address of the first element (arr1[0])
                        // arr1+1: address of the SECOND element (arr1[1])
                        //
                        // KEY CONCEPT - POINTER ARITHMETIC SCALING:
                        // arr1+1 does NOT add 1 to the address!
                        // It adds sizeof(int) bytes (typically 4)
                        //
                        // Example:
                        // If arr1 is at address 0x1000:
                        // arr1   = 0x1000 (points to arr1[0])
                        // arr1+1 = 0x1004 (points to arr1[1], 4 bytes later)
                        // arr1+2 = 0x1008 (points to arr1[2], 8 bytes later)
                        //
                        // WHY? So you can access arrays naturally:
                        // *(arr1 + i) accesses the i-th element
                        // This is equivalent to: arr1[i]
                        //
                        // The difference: arr1+1 - arr1 = 4 bytes (one int)

  printf("The addr of arr2 is %p and the addr of arr2+1 is %p\n",arr2,arr2+1);
                        // PRINT DOUBLE ARRAY ADDRESSES:
                        // arr2+1 adds sizeof(double) bytes (typically 8)
                        //
                        // Example:
                        // If arr2 is at address 0x2000:
                        // arr2   = 0x2000
                        // arr2+1 = 0x2008 (8 bytes later, not 1!)
                        //
                        // The difference: arr2+1 - arr2 = 8 bytes (one double)

  printf("The addr of arr3 is %p and the addr of arr3+1 is %p\n",arr3,arr3+1);
                        // PRINT CHARACTER ARRAY ADDRESSES:
                        // arr3+1 adds sizeof(char) bytes (always 1)
                        //
                        // Example:
                        // If arr3 is at address 0x3000:
                        // arr3   = 0x3000
                        // arr3+1 = 0x3001 (only 1 byte later!)
                        //
                        // The difference: arr3+1 - arr3 = 1 byte (one char)

  return 0;             // SUCCESS: Exit with status 0
}

// EXPECTED OUTPUT (addresses will vary):
// The addr of arr1 is 0x7ffc9b2a3c40 and the addr of arr1+1 is 0x7ffc9b2a3c44
// The addr of arr2 is 0x7ffc9b2a3bf0 and the addr of arr2+1 is 0x7ffc9b2a3bf8
// The addr of arr3 is 0x7ffc9b2a3be0 and the addr of arr3+1 is 0x7ffc9b2a3be1

// ANALYSIS OF OUTPUT:
// arr1: 0x...40 to 0x...44 → difference = 0x04 = 4 bytes (sizeof(int))
// arr2: 0x...f0 to 0x...f8 → difference = 0x08 = 8 bytes (sizeof(double))
// arr3: 0x...e0 to 0x...e1 → difference = 0x01 = 1 byte (sizeof(char))

// POINTER ARITHMETIC RULES:
//
// Given: T* ptr; (where T is any type)
//
// ptr + n:
//   - Result: ptr + (n * sizeof(T))
//   - Adds n elements, not n bytes
//   - Returns: pointer to the n-th element after ptr
//
// ptr - n:
//   - Result: ptr - (n * sizeof(T))
//   - Subtracts n elements
//   - Returns: pointer to the n-th element before ptr
//
// ptr1 - ptr2 (both of type T*):
//   - Result: (ptr1 - ptr2) / sizeof(T)
//   - Returns: number of elements between pointers
//   - Only valid if both pointers point into the same array
//
// ptr++:
//   - Equivalent to: ptr = ptr + 1
//   - Moves to next element
//
// ptr--:
//   - Equivalent to: ptr = ptr - 1
//   - Moves to previous element

// ARRAY-POINTER EQUIVALENCE:
//
// Given: int arr[10];
//
// The following are equivalent:
//   arr[i]  ≡  *(arr + i)
//   &arr[i] ≡  arr + i
//
// Example:
//   arr[0] = *(arr + 0) = *arr
//   arr[5] = *(arr + 5)
//   &arr[3] = arr + 3

// IMPORTANT DIFFERENCES BETWEEN ARRAYS AND POINTERS:
//
// 1. SIZEOF:
//    int arr[10];
//    int* ptr = arr;
//    sizeof(arr) = 40 (10 * 4 bytes - whole array)
//    sizeof(ptr) = 8 (just the pointer itself on 64-bit)
//
// 2. ADDRESS-OF:
//    &arr = same numerical value as arr, but different type
//    &ptr = address of the pointer variable itself
//
// 3. MODIFIABILITY:
//    arr = something_else; // ERROR! Can't reassign array name
//    ptr = something_else; // OK - can reassign pointer

// WHY POINTER ARITHMETIC IS SCALED:
//
// Imagine we didn't scale by sizeof(T):
//   int* p = arr1;
//   int x = *(p + 4); // Without scaling, would read 4 BYTES ahead
//                      // This would read partial ints (garbage!)
//
// With scaling:
//   int x = *(p + 4); // Reads 4 INTS ahead (16 bytes)
//                      // This correctly accesses arr1[4]
//
// The scaling makes pointer arithmetic match array indexing naturally.

// MEMORY LAYOUT VISUALIZATION:
//
// int arr1[3] at address 0x1000:
//
// Address:  0x1000    0x1004    0x1008
// Element:  arr1[0]   arr1[1]   arr1[2]
// Pointer:  arr1+0    arr1+1    arr1+2
// Value:    *(arr1+0) *(arr1+1) *(arr1+2)
//
// char arr3[3] at address 0x2000:
//
// Address:  0x2000  0x2001  0x2002
// Element:  arr3[0] arr3[1] arr3[2]
// Pointer:  arr3+0  arr3+1  arr3+2
// Value:    *(arr3+0) *(arr3+1) *(arr3+2)

// TO COMPILE AND RUN:
// gcc pointerarith.c -o pointerarith
// ./pointerarith

// EXPERIMENT:
// 1. Add print statements to show sizeof(int), sizeof(double), sizeof(char)
// 2. Calculate the differences manually:
//    printf("Difference for int array: %ld bytes\n", (char*)(arr1+1) - (char*)arr1);
// 3. Try with other types: long, short, float, struct
// 4. Create multidimensional arrays and explore their pointer arithmetic
