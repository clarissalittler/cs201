// PEDAGOGICAL PURPOSE:
// This program demonstrates an important portability issue in C: whether the
// 'char' type is signed or unsigned is implementation-defined.
// Key learning objectives:
// 1. Understanding that 'char' signedness is platform-dependent
// 2. The difference between signed and unsigned integer representations
// 3. How -1 behaves differently in signed vs unsigned types
// 4. Platform-specific behavior and portability issues
// 5. The importance of explicit types (signed char vs unsigned char)
// 6. Testing implementation-defined behavior at runtime

#include <stdio.h>
#include <stdlib.h>

int main(){

  // TESTING CHAR SIGNEDNESS:
  // Assigning -1 to a char to test if it's signed or unsigned
  //
  // THE AMBIGUITY:
  // In C, the signedness of 'char' is IMPLEMENTATION-DEFINED
  // This means different compilers/platforms may behave differently
  //
  // PLATFORM VARIATIONS:
  // - Most x86/x64 systems: char is SIGNED (like signed char)
  // - ARM systems: often UNSIGNED (like unsigned char)
  // - PowerPC systems: often UNSIGNED
  //
  // The C standard allows both, so code shouldn't assume either way
  char test = -1;

  // TWO POSSIBLE INTERPRETATIONS OF -1:
  //
  // IF char IS SIGNED (typical on x86/x64):
  // - -1 stored as two's complement: 11111111 (binary)
  // - Interpreted as negative: -1 (decimal)
  // - Comparison: test > 0 evaluates to FALSE
  //
  // IF char IS UNSIGNED (common on ARM):
  // - -1 converted to unsigned: 255 (0xFF)
  // - All 1 bits: 11111111 (binary) = 255 (unsigned)
  // - Comparison: test > 0 evaluates to TRUE

  // THE TEST:
  if(test > 0){
    // UNSIGNED CHAR:
    // test = -1 became 255 (all bits set)
    // 255 > 0, so this branch executes
    printf("Well, char is unsigned!\n");
  }
  else{
    // SIGNED CHAR:
    // test = -1 stayed as -1
    // -1 is not > 0, so this branch executes
    printf("char is signed, I guess\n");
  }

  return 0;
}

// COMPREHENSIVE EXPLANATION:
//
// SIGNED vs UNSIGNED REPRESENTATION:
//
// For 8-bit types (char):
//
// SIGNED char range: -128 to +127
//   Binary 00000000 = 0
//   Binary 01111111 = +127 (largest positive)
//   Binary 10000000 = -128 (most negative)
//   Binary 11111111 = -1
//   ↑ MSB (sign bit): 1 = negative, 0 = positive
//
// UNSIGNED char range: 0 to 255
//   Binary 00000000 = 0
//   Binary 01111111 = 127
//   Binary 10000000 = 128
//   Binary 11111111 = 255
//   No sign bit, all bits represent magnitude

// WHAT HAPPENS WITH "char test = -1":
//
// SCENARIO 1: char is SIGNED (like on x86)
// 1. Literal -1 has type 'int': 11111111111111111111111111111111 (32 bits)
// 2. Assignment truncates to 8 bits: 11111111
// 3. Interpreted as signed: -1 (two's complement)
// 4. Comparison: -1 > 0 is FALSE
// 5. Output: "char is signed, I guess"
//
// SCENARIO 2: char is UNSIGNED (like on some ARM)
// 1. Literal -1 has type 'int': 11111111111111111111111111111111 (32 bits)
// 2. Assignment truncates to 8 bits: 11111111
// 3. Interpreted as unsigned: 255 (all bits represent magnitude)
// 4. Comparison: 255 > 0 is TRUE
// 5. Output: "Well, char is unsigned!"

// THE THREE CHAR TYPES IN C:
//
// 1. char
//    - Signedness is IMPLEMENTATION-DEFINED
//    - Used for character storage (ASCII, UTF-8)
//    - May be signed or unsigned
//
// 2. signed char
//    - Always signed: range -128 to +127
//    - Explicitly signed
//    - Use when you need arithmetic on small signed integers
//
// 3. unsigned char
//    - Always unsigned: range 0 to 255
//    - Explicitly unsigned
//    - Use for byte manipulation, binary data
//    - Guaranteed no sign extension
//
// IMPORTANT: These are THREE DISTINCT TYPES in C!
// Even on platforms where char is signed, 'char' and 'signed char'
// are different types (though they have the same representation)

// WHY THIS MATTERS:
//
// PORTABILITY ISSUES:
// Code that assumes char is signed may break on ARM
// Code that assumes char is unsigned may break on x86
//
// EXAMPLE PROBLEM:
//   char c = getchar();
//   if (c == EOF) { ... }  // BUG if char is unsigned!
//   // EOF is -1, which becomes 255 if char is unsigned
//   // The comparison will never be true
//
// CORRECT VERSION:
//   int c = getchar();     // getchar returns int, not char!
//   if (c == EOF) { ... }  // Works correctly

// BYTE MANIPULATION:
//
// For working with raw bytes/binary data:
//   unsigned char* buffer = malloc(1024);
//
// Why unsigned?
// - No sign extension when promoting to larger types
// - All 8 bits represent data (0-255)
// - Bitwise operations have predictable behavior
//
// Example with sign extension problem:
//   signed char sc = -1;        // 11111111 in binary
//   int i = sc;                 // Sign-extended: 11111111111111111111111111111111 (-1)
//
//   unsigned char uc = 255;     // 11111111 in binary
//   int i = uc;                 // Zero-extended: 00000000000000000000000011111111 (255)

// COMPILER FLAGS:
//
// You can control char signedness with compiler flags:
//
// GCC/Clang:
//   -fsigned-char     Force char to be signed
//   -funsigned-char   Force char to be unsigned
//
// Example:
//   gcc -funsigned-char charSigned.c -o charSigned
//   ./charSigned
//   Output: "Well, char is unsigned!"
//
//   gcc -fsigned-char charSigned.c -o charSigned
//   ./charSigned
//   Output: "char is signed, I guess"

// BEST PRACTICES:
//
// 1. Use 'char' only for character data (strings, text)
// 2. Use 'signed char' when you need small signed integers
// 3. Use 'unsigned char' for byte/binary data
// 4. Never assume char signedness in portable code
// 5. For getchar/EOF comparisons, use int not char
// 6. When in doubt, be explicit with signed/unsigned

// OTHER IMPLEMENTATION-DEFINED BEHAVIOR:
//
// Besides char signedness, C has many implementation-defined aspects:
// - Size of int (must be at least 16 bits, often 32)
// - Right shift of negative signed integers (arithmetic vs logical)
// - Byte order (endianness): big-endian vs little-endian
// - Representation of signed integers (two's complement is common but not required)
// - Size and alignment of types
//
// Portable code should not rely on any implementation-defined behavior

// TWO'S COMPLEMENT REVIEW:
//
// Most systems use two's complement for signed integers
// To negate a number: invert all bits and add 1
//
// Example: +1 → -1
//   +1:  00000001
//   Invert: 11111110
//   Add 1:  11111111  = -1 in two's complement
//
// Range for n-bit two's complement: -2^(n-1) to 2^(n-1) - 1
// For 8 bits: -2^7 to 2^7 - 1 = -128 to +127

// CHECKING AT COMPILE TIME:
//
// You can check char signedness at compile time:
//
// #include <limits.h>
// #if CHAR_MIN < 0
//   // char is signed
// #else
//   // char is unsigned
// #endif
//
// Or using static assertions (C11):
// #include <assert.h>
// #include <limits.h>
// static_assert(CHAR_MIN < 0, "char must be signed");

// TO COMPILE AND RUN:
// gcc charSigned.c -o charSigned
// ./charSigned
// (Output depends on your platform)
//
// On most x86/x64 Linux systems:
// Output: "char is signed, I guess"
//
// TO TEST WITH DIFFERENT SIGNEDNESS:
// gcc -funsigned-char charSigned.c -o charSigned
// ./charSigned
// Output: "Well, char is unsigned!"
//
// gcc -fsigned-char charSigned.c -o charSigned
// ./charSigned
// Output: "char is signed, I guess"
