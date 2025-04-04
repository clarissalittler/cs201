/*
 * =====================================================================================
 *        File: pointer_arithmetic_example.c
 * Description: Demonstrates basic pointer arithmetic using arrays in C.
 *
 * Concept Tutorial:
 * -----------------
 * 1. Arrays in C:
 *    - An array is a collection of items of the same data type stored in
 *      contiguous (one after another) memory locations.
 *    - Example: `int arr[10];` reserves space for 10 integers right next to
 *      each other in memory. Access elements using `arr[0]`, `arr[1]`, etc.
 *
 * 2. Pointers in C:
 *    - A pointer is a special variable that stores the memory address of
 *      another variable.
 *
 * 3. Array Name as a Pointer:
 *    - In most expressions (like when passed to functions or used in arithmetic,
 *      as shown below), an array name "decays" into a pointer to its very
 *      first element.
 *    - So, `arr` (when used this way) is essentially equivalent to `&arr[0]`
 *      (the address of the first element). It holds the starting memory address
 *      of the array's block.
 *
 * 4. Pointer Arithmetic:
 *    - The core concept demonstrated here. When you perform arithmetic (like
 *      addition or subtraction) on a pointer, it's automatically scaled by the
 *      size of the data type it points to.
 *    - If `p` is a pointer to a data type `T` (e.g., `int *p`), then `p + 1`
 *      does NOT simply add 1 byte to the address stored in `p`.
 *    - Instead, `p + 1` calculates a new memory address:
 *      `address_currently_in_p + (1 * sizeof(T))`.
 *    - This makes it easy to move between elements in an array. `arr + 1`
 *      gives the memory address of the *next element* (which would be `arr[1]`),
 *      `arr + 2` gives the address of `arr[2]`, and so on.
 *
 * This Example:
 * -------------
 * This code declares three arrays: one of integers (`int`), one of doubles
 * (`double`), and one of characters (`char`). It then prints:
 *   a) The starting memory address of each array (using the array name, which
 *      decays to a pointer).
 *   b) The memory address calculated by adding 1 to that starting address
 *      (e.g., `arr1 + 1`).
 * You will observe that the difference between the address `arr` and the
 * address `arr + 1` is different for each array type. This difference will
 * exactly match the size (in bytes) of the data type stored in that array
 * (`sizeof(int)`, `sizeof(double)`, `sizeof(char)` respectively).
 *
 * How to Compile and Run:
 *   (Using GCC, a common C compiler)
 *   1. Save the code as `pointer_arithmetic_example.c`
 *   2. Open a terminal or command prompt.
 *   3. Compile: `gcc pointer_arithmetic_example.c -o pointer_arithmetic_example`
 *   4. Run: `./pointer_arithmetic_example` (or `pointer_arithmetic_example.exe` on Windows)
 *
 * Expected Output (Addresses will vary on your system):
 *   The addr of arr1 is 0x7ffc...XX0 and the addr of arr1+1 is 0x7ffc...XX4  (Difference is 4 bytes = sizeof(int))
 *   The addr of arr2 is 0x7ffc...YY0 and the addr of arr2+1 is 0x7ffc...YY8  (Difference is 8 bytes = sizeof(double))
 *   The addr of arr3 is 0x7ffc...ZZ0 and the addr of arr3+1 is 0x7ffc...ZZ1  (Difference is 1 byte = sizeof(char))
 * =====================================================================================
 */

// Include the Standard Input/Output library for functions like printf.
#include <stdio.h>
// Include the Standard Library, often used for memory allocation, exit, etc.
// Not strictly necessary for this *specific* code, but good practice.
#include <stdlib.h>

// The main function - execution of the program begins here.
int main() {
  // Declare an array named 'arr1' that can hold 10 integers.
  // Memory for 10 * sizeof(int) bytes is allocated contiguously.
  int arr1[10];

  // Declare an array named 'arr2' that can hold 10 double-precision floating-point numbers.
  // Memory for 10 * sizeof(double) bytes is allocated contiguously.
  double arr2[10];

  // Declare an array named 'arr3' that can hold 10 characters.
  // Memory for 10 * sizeof(char) bytes is allocated contiguously.
  char arr3[10];

  // --- Demonstrating Pointer Arithmetic with int array ---

  // Print the starting memory address of arr1 and the address of the next element.
  // 'arr1' used here decays into a pointer to its first element (&arr1[0]).
  // 'arr1 + 1' performs pointer arithmetic: it calculates the address of the *second*
  // integer element (&arr1[1]). The address increases by sizeof(int) bytes.
  // '%p' is the format specifier to print memory addresses (pointers) in hexadecimal.
  printf("The addr of arr1 is %p and the addr of arr1+1 is %p\n", arr1, arr1 + 1);

  // --- Demonstrating Pointer Arithmetic with double array ---

  // Print the starting memory address of arr2 and the address of the next element.
  // 'arr2' decays into a pointer to its first element (&arr2[0]).
  // 'arr2 + 1' performs pointer arithmetic: it calculates the address of the *second*
  // double element (&arr2[1]). The address increases by sizeof(double) bytes.
  printf("The addr of arr2 is %p and the addr of arr2+1 is %p\n", arr2, arr2 + 1);

  // --- Demonstrating Pointer Arithmetic with char array ---

  // Print the starting memory address of arr3 and the address of the next element.
  // 'arr3' decays into a pointer to its first element (&arr3[0]).
  // 'arr3 + 1' performs pointer arithmetic: it calculates the address of the *second*
  // char element (&arr3[1]). The address increases by sizeof(char) bytes (usually 1 byte).
  printf("The addr of arr3 is %p and the addr of arr3+1 is %p\n", arr3, arr3 + 1);

  // Indicate that the program executed successfully.
  // A return value of 0 is conventional for success.
  return 0;
}
