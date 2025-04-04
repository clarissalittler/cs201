/**
 * @file sizeof_example.c
 * @brief Educational example demonstrating the `sizeof` operator in C.
 *
 * Purpose:
 * This program illustrates how to use the `sizeof` operator in C to determine
 * the amount of memory (in bytes) allocated for different fundamental data
 * types, a user-defined structure (`struct`), and pointers. Understanding
 * these sizes is crucial for memory management, performance optimization,
 * data serialization, and platform compatibility.
 *
 * Concepts Demonstrated:
 *
 * 1.  `sizeof` Operator:
 *     - A compile-time unary operator (meaning it takes one operand).
 *     - Returns the size, in bytes, of its operand.
 *     - The operand can be either a data type name (e.g., `sizeof(int)`)
 *       or an expression (e.g., `sizeof x`, `sizeof(x+1)`). When used with
 *       an expression, it gives the size of the type the expression would
 *       evaluate to, without actually evaluating the expression itself.
 *     - The value returned is of type `size_t`, which is an unsigned integer
 *       type defined in `<stddef.h>` (implicitly included by `<stdio.h>`).
 *       The `%zu` format specifier in `printf` is specifically for `size_t`.
 *
 * 2.  Fundamental Data Types:
 *     - `char`: Typically used for single characters (like 'a', 'Z', '?').
 *       Guaranteed by the C standard to be exactly 1 byte.
 *     - `int`: The processor's natural size for integers. Size varies by
 *       platform (e.g., 2 or 4 bytes on older 16/32-bit systems, often 4
 *       bytes on modern 32/64-bit systems).
 *     - `float`: Single-precision floating-point number. Typically 4 bytes
 *       (IEEE 754 standard).
 *     - `double`: Double-precision floating-point number. Typically 8 bytes
 *       (IEEE 754 standard). Offers greater precision than `float`.
 *     - `long`: A larger integer type. Size varies (e.g., 4 bytes on 32-bit
 *       Windows/Linux, 8 bytes on 64-bit Linux, 4 bytes on 64-bit Windows LLP64).
 *     *Note*: The exact sizes of `int`, `long`, etc. (except `char`) are
 *     implementation-defined (depend on the compiler and architecture).
 *
 * 3.  Structures (`struct`):
 *     - User-defined composite types that group variables (members) of
 *       potentially different types under a single name.
 *     - The size of a struct is *not* always the simple sum of the sizes of
 *       its members.
 *     - **Memory Alignment & Padding**: Compilers often insert unused bytes
 *       (padding) within or after the members of a struct. This is done to
 *       ensure that members are aligned on memory addresses that are
 *       multiples of their size (or some other boundary), which can significantly
 *       speed up access on many processor architectures. For example, an `int`
 *       (4 bytes) might be faster to access if it starts at an address
 *       divisible by 4. In `struct Thingy`, padding might be added after
 *       `thing1` so that `thing2` (a `double`, often requiring 8-byte
 *       alignment) starts on a suitable boundary. The total size of the
 *       struct might also be padded to ensure that elements in an array of
 *       these structs are properly aligned.
 *
 * 4.  Pointers:
 *     - Variables that store memory addresses.
 *     - The size of a pointer depends on the architecture's address space,
 *       *not* the size of the data type it points to.
 *     - On a 32-bit system (where memory addresses are 32 bits long), pointers
 *       are typically 4 bytes (`sizeof(any_pointer_type)` is 4).
 *     - On a 64-bit system (where memory addresses are 64 bits long), pointers
 *       are typically 8 bytes (`sizeof(any_pointer_type)` is 8).
 *     - `sizeof(int*)`, `sizeof(char*)`, `sizeof(struct Thingy*)` will all
 *       yield the same result on a given platform.
 *
 * Compilation and Execution:
 *   - Compile: `gcc sizeof_example.c -o sizeof_example`
 *   - Run: `./sizeof_example`
 *   - Observe the output, noting how the sizes might differ if you compile
 *     and run on different systems (e.g., 32-bit vs. 64-bit).
 */

// Include the Standard Input/Output library header file.
// This provides functions like printf() for printing output to the console.
#include <stdio.h>
// Include the Standard Library header file.
// This provides general utility functions (like memory allocation, etc.),
// though it's not strictly necessary for *this* specific example using only sizeof and printf.
#include <stdlib.h>

// Define a structure named 'Thingy'.
// Structures group different data types together under one name.
struct Thingy {
  int thing1;      // A member variable of type integer.
  double thing2;   // A member variable of type double (double-precision float).
  // Note: The compiler might add padding bytes between thing1 and thing2,
  // or after thing2, for memory alignment purposes.
};

// The main function - the entry point of every C program.
// It returns an integer value: 0 typically indicates successful execution.
int main() {

  // Print the size of the 'char' data type.
  // `sizeof(char)` calculates the number of bytes used to store a char.
  // By C standard, this is *always* 1.
  // `%zu` is the correct format specifier for printing `size_t` values (the type returned by sizeof).
  printf("The size of a char is: %zu byte(s)\n", sizeof(char));

  // Print the size of the 'int' data type.
  // `sizeof(int)` calculates the number of bytes for an integer.
  // This size commonly varies (e.g., 2 or 4 bytes historically, often 4 bytes now).
  printf("The size of an int is: %zu byte(s)\n", sizeof(int));

  // Print the size of the 'float' data type.
  // `sizeof(float)` calculates the number of bytes for a single-precision float.
  // This is typically 4 bytes (following the IEEE 754 standard).
  printf("The size of a float is: %zu byte(s)\n", sizeof(float));

  // Print the size of the 'double' data type.
  // `sizeof(double)` calculates the number of bytes for a double-precision float.
  // This is typically 8 bytes (following the IEEE 754 standard).
  printf("The size of a double is: %zu byte(s)\n", sizeof(double));

  // Print the size of the 'long' data type.
  // `sizeof(long)` calculates the number of bytes for a long integer.
  // This size varies significantly by platform and compiler (e.g., 4 or 8 bytes).
  printf("The size of a long is: %zu byte(s)\n", sizeof(long));

  // Print the size of the 'struct Thingy' type we defined earlier.
  // `sizeof(struct Thingy)` calculates the total bytes for the structure.
  // This might be *larger* than sizeof(int) + sizeof(double) due to padding
  // added by the compiler for alignment. For example, on a 64-bit system,
  // if int=4 and double=8, the size might be 16 (4 bytes int + 4 bytes padding + 8 bytes double).
  printf("The size of a struct Thingy is: %zu byte(s)\n", sizeof(struct Thingy));

  // Print the size of a pointer to an integer ('int*').
  // `sizeof(int*)` calculates the number of bytes needed to store a memory address.
  // This size depends on the system architecture (e.g., 4 bytes on 32-bit, 8 bytes on 64-bit),
  // *not* on the size of the data type being pointed to (int).
  // sizeof(char*), sizeof(double*), sizeof(struct Thingy*) would give the same result.
  printf("The size of a pointer (e.g., int*) is: %zu byte(s)\n", sizeof(int*));

  // Indicate that the program executed successfully.
  // Returning 0 from main is the standard way to signal success.
  return 0;
} // End of the main function
