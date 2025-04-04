/**
 * @file endianness_demonstration.c
 * @brief Demonstrates how to inspect the individual bytes of an integer in memory.
 *
 * ## Tutorial: Understanding the Concepts
 *
 * This program illustrates several fundamental C concepts related to memory
 * manipulation and data representation:
 *
 * 1.  **Data Representation:** Computers store data, including integers, in binary format
 *     in memory. An `int` typically occupies multiple bytes (often 4 bytes or 32 bits
 *     on many modern systems).
 *
 * 2.  **Memory Addresses:** Every variable stored in memory has a unique address. The `&`
 *     (address-of) operator retrieves this memory address. In this code, `&n` gives
 *     the starting memory address where the integer `n` is stored.
 *
 * 3.  **Pointers:** A pointer is a variable that stores a memory address. Pointers have
 *     a type that indicates what kind of data resides at the address they point to
 *     (e.g., `int*` points to an integer, `char*` points to a character/byte).
 *
 * 4.  **Pointer Casting:** Sometimes, we need to treat a memory location as if it
 *     contains a different type of data than originally declared. This is done via
 *     casting. Here, `(unsigned char*) &n` tells the compiler: "Take the address
 *     of the integer `n` (which is of type `int*`), but treat it as an address
 *     pointing to an `unsigned char` (a single byte)". This is key to examining
 *     the integer byte by byte. We use `unsigned char` because it's guaranteed
 *     to be exactly 1 byte and avoids potential sign-extension issues when printing.
 *
 * 5.  **Pointer Arithmetic:** When you add an integer `i` to a pointer `p` (`p + i`),
 *     the address is incremented by `i * sizeof(*p)` bytes, where `sizeof(*p)` is
 *     the size of the data type the pointer points to. Since `c` is an `unsigned char*`,
 *     `sizeof(*c)` is 1. Therefore, `c + i` increments the address by exactly `i` bytes,
 *     allowing us to move sequentially through the bytes of the original integer `n`.
 *
 * 6.  **Dereferencing:** The `*` (dereference) operator retrieves the value stored at
 *     the memory address a pointer points to. `*(c + i)` accesses the byte value
 *     at the memory location pointed to by `c + i`.
 *
 * 7.  **`sizeof` Operator:** `sizeof(type)` or `sizeof(variable)` returns the size (in bytes)
 *     of a data type or variable. `sizeof(typeof(n))` gets the size of whatever type `n`
 *     is (in this case, `int`), ensuring the loop runs for the correct number of bytes.
 *
 * 8.  **Endianness:** This program powerfully demonstrates **endianness**, which refers to
 *     the order in which bytes of a multi-byte data type (like `int`) are stored in
 *     memory.
 *     *   **Little-Endian:** The least significant byte (LSB) is stored at the lowest
 *         memory address. (e.g., Intel x86/x64). For `0x89ABCDEF`, memory would look like:
 *         Address: Low -> High
 *         Content: EF CD AB 89
 *         Output: Byte 0: ef, Byte 1: cd, Byte 2: ab, Byte 3: 89
 *     *   **Big-Endian:** The most significant byte (MSB) is stored at the lowest
 *         memory address. (e.g., older PowerPC, network byte order). For `0x89ABCDEF`,
 *         memory would look like:
 *         Address: Low -> High
 *         Content: 89 AB CD EF
 *         Output: Byte 0: 89, Byte 1: ab, Byte 2: cd, Byte 3: ef
 *
 *     The output of this program will differ depending on the architecture of the
 *     machine it's run on, revealing its endianness.
 */

#include <stdio.h> // Include standard input/output library for printf function
#include <stdlib.h> // Include standard library (though not strictly needed for this specific example)

int main() {
  // Declare an integer variable 'n' and initialize it with a hexadecimal value.
  // 0x89ABCDEF is a 32-bit integer value.
  // In binary, this would be represented across 4 bytes.
  int n = 0x89ABCDEF;

  // Declare a pointer 'c' of type 'unsigned char*'.
  // '&n' gets the memory address of the integer 'n'. The type of &n is 'int*'.
  // '(unsigned char*)' casts this 'int*' address to an 'unsigned char*' address.
  // This makes 'c' point to the *first byte* of the memory location where 'n' is stored.
  // We use 'unsigned char*' because a 'char' is typically 1 byte, allowing us
  // to access the integer's memory byte by byte. 'unsigned' prevents sign issues.
  unsigned char* c = (unsigned char*) &n;

  // Print the value of n and its size in bytes for context.
  printf("Integer n = 0x%X (%d bytes)\n", n, (int)sizeof(n));
  printf("Memory address of n: %p\n", (void*)&n); // Print the starting address
  printf("------------------------------------\n");

  // Loop through each byte of the integer 'n'.
  // 'sizeof(typeof(n))' calculates the size of the variable 'n' in bytes (typically 4 for an int).
  // 'typeof(n)' gets the type of 'n' (which is 'int'). Using typeof makes the code
  // more robust if the type of 'n' were to change later.
  // The loop runs from i = 0 up to (but not including) the total number of bytes in 'n'.
  for (int i = 0; i < sizeof(typeof(n)); i++) {
    // 'c' points to the first byte of 'n'.
    // 'c + i' performs pointer arithmetic. Since 'c' is a pointer to a 1-byte type ('unsigned char'),
    // 'c + i' calculates the memory address of the i-th byte (0-indexed) relative to the start of 'n'.
    // '*(c + i)' dereferences this pointer, retrieving the actual value (the single byte)
    // stored at that memory location.
    // 'printf' then displays the byte index 'i' and the value of that byte in hexadecimal format ('%x').
    // The output order of these bytes depends on the system's endianness.
    printf("The %d th byte of n (at address %p) is: 0x%x \n", i, (void*)(c + i), *(c + i));
  }

  // Indicate successful program execution by returning 0.
  return 0;
}
