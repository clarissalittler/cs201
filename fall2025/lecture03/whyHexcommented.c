// PEDAGOGICAL PURPOSE:
// This program demonstrates that numbers can be represented in different bases
// (decimal, hexadecimal, octal) without changing their actual value.
// Key learning objectives:
// 1. Understanding that a number's VALUE is independent of its REPRESENTATION
// 2. Learning three common number bases: decimal (10), hexadecimal (16), octal (8)
// 3. Understanding printf format specifiers for different bases
// 4. Seeing WHY hexadecimal is useful (relationship to binary and bytes)
// 5. Learning to convert between bases mentally
// 6. Understanding that the computer stores the value in binary, regardless of how we display it

#include "stdio.h"

int main(){

  // INITIALIZATION:
  // num1 is initialized with the decimal value 1234
  // The computer stores this in binary: 10011010010
  // We can DISPLAY it in any base we want, but the VALUE stays the same
  int num1 = 1234;

  // DEMONSTRATION 1: DECIMAL (BASE 10)
  // %d format specifier displays the number in decimal (base 10)
  // This is the most familiar representation for humans
  // 1234 in decimal = 1×10³ + 2×10² + 3×10¹ + 4×10⁰
  //                 = 1000 + 200 + 30 + 4
  printf("As decimal: %d\n",num1);

  // DEMONSTRATION 2: HEXADECIMAL (BASE 16)
  // %x format specifier displays the number in hexadecimal (base 16)
  // Hexadecimal uses digits 0-9 and letters A-F (where A=10, B=11, ..., F=15)
  // 1234 in decimal = 4D2 in hexadecimal
  // CALCULATION: 1234 ÷ 16 = 77 remainder 2 (this is the rightmost digit)
  //              77 ÷ 16 = 4 remainder 13 (13 = D in hex)
  //              4 ÷ 16 = 0 remainder 4
  // Reading remainders bottom-to-top: 4D2
  // VERIFICATION: 4×16² + 13×16¹ + 2×16⁰ = 4×256 + 13×16 + 2 = 1024 + 208 + 2 = 1234
  //
  // WHY HEXADECIMAL IS USEFUL:
  // - Each hex digit represents exactly 4 bits
  // - Two hex digits represent exactly 1 byte (8 bits)
  // - Much more compact than binary
  // - Easy to convert to/from binary (just group by 4 bits)
  // - Example: 10011010010 (binary) = 0100 1101 0010 = 0x4D2
  printf("As hex: %x\n",num1);

  // DEMONSTRATION 3: OCTAL (BASE 8)
  // %o format specifier displays the number in octal (base 8)
  // Octal uses only digits 0-7
  // 1234 in decimal = 2322 in octal
  // CALCULATION: 1234 ÷ 8 = 154 remainder 2
  //              154 ÷ 8 = 19 remainder 2
  //              19 ÷ 8 = 2 remainder 3
  //              2 ÷ 8 = 0 remainder 2
  // Reading remainders bottom-to-top: 2322
  // VERIFICATION: 2×8³ + 3×8² + 2×8¹ + 2×8⁰ = 2×512 + 3×64 + 2×8 + 2 = 1024 + 192 + 16 + 2 = 1234
  //
  // OCTAL IS LESS COMMON TODAY:
  // - Historically used because 3 bits = 1 octal digit
  // - Still seen in Unix file permissions (e.g., chmod 755)
  // - Mostly replaced by hexadecimal in modern programming
  printf("As octal: %o\n",num1);

  // IMPORTANT INSIGHT:
  // All three printf statements display THE SAME VALUE
  // We're not converting or changing num1 - just displaying it differently
  // It's like saying "twelve" (English), "doce" (Spanish), "douze" (French)
  // - same value, different representation

  return 0;
}

// EXPECTED OUTPUT:
// As decimal: 1234
// As hex: 4d2
// As octal: 2322
//
// These are THREE DIFFERENT REPRESENTATIONS of the SAME NUMBER!

// PRACTICAL APPLICATIONS:
//
// DECIMAL is used for:
// - Human-readable numbers (counts, measurements, etc.)
// - Mathematical calculations
// - User input/output
//
// HEXADECIMAL is used for:
// - Memory addresses (e.g., 0x7ffc8a6b9a20)
// - Color codes in graphics (e.g., #FF5733 = red-green-blue)
// - Byte values and bit patterns (e.g., 0xFF = all bits set in a byte)
// - Machine code and assembly language
// - Debugging and memory dumps
//
// OCTAL is used for:
// - Unix/Linux file permissions (e.g., 0755, 0644)
// - Occasionally in embedded systems
// - Legacy systems and code

// BASE CONVERSION CHEAT SHEET:
//
// DECIMAL  HEXADECIMAL  OCTAL     BINARY
// -------  -----------  -----     ------------
//    0         0          0        0000
//    1         1          1        0001
//    8         8         10        1000
//   10         A         12        1010
//   15         F         17        1111
//   16        10         20        0001 0000
//  255        FF        377        1111 1111
// 1234       4D2       2322        0100 1101 0010

// TRY IT YOURSELF:
// 1. Compile: gcc whyHex.c -o whyHex
// 2. Run: ./whyHex
// 3. Try different values for num1:
//    - num1 = 255 (notice hex = ff, octal = 377)
//    - num1 = 16 (notice hex = 10, octal = 20)
//    - num1 = 0xFF (you can initialize in hex too!)
// 4. Use a calculator to verify the conversions
// 5. Notice that 0x prefix denotes hex, 0 prefix denotes octal in C code
