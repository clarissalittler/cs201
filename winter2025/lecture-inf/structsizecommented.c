// PEDAGOGICAL PURPOSE:
// This program demonstrates struct padding and memory alignment in C.
// Key learning objectives:
// 1. Understanding that structs are not always the sum of their parts
// 2. Learning about memory alignment requirements on modern architectures
// 3. Seeing how the ORDER of struct fields affects total size
// 4. Understanding padding bytes and why compilers add them
// 5. Learning that 64-bit systems align on 8-byte boundaries
// 6. Recognizing the performance vs. space tradeoff
//
// CORE CONCEPT:
// Modern CPUs access memory most efficiently when data is aligned to
// specific boundaries (multiples of 4 or 8 bytes). The compiler adds
// invisible "padding" bytes to ensure proper alignment, which means
// the size of a struct may be LARGER than the sum of its fields.

#include <stdio.h>
#include <stdlib.h>

// STRUCT 1: TWO INTEGERS
// This struct contains two int fields
// Each int is 4 bytes, so total should be 8 bytes, right?
// EXPECTED SIZE: 8 bytes (and it will be!)
// WHY: Two 4-byte ints = 8 bytes total, already aligned
// memory access is done on boundaries that are multiples of the architecture size
// 64-bit will be on 8-byte boundaries
// structs may pad fields to keep alignment properties
struct Goofy1 {
  int num1;     // 4 bytes
  int num2;     // 4 bytes
                // Total: 8 bytes (no padding needed)
};

// STRUCT 2: INT FOLLOWED BY DOUBLE
// This struct contains an int and a double
// int: 4 bytes
// double: 8 bytes
// Sum: 12 bytes, but ACTUAL SIZE will be 16 bytes!
// WHY THE PADDING?
// doubles must be aligned on 8-byte boundaries
// After the 4-byte int, we need 4 bytes of padding to align the double
struct Goofy2 {
  int num1;     // 4 bytes at offset 0
                // 4 bytes of PADDING added here (offset 4-7)
  double num2;  // 8 bytes at offset 8 (aligned on 8-byte boundary!)
                // Total: 16 bytes (4 + 4 padding + 8)
};

// STRUCT 3: DOUBLE FOLLOWED BY INT
// This struct contains a double and an int
// double: 8 bytes
// int: 4 bytes
// Sum: 12 bytes, but ACTUAL SIZE will be 16 bytes!
// WHY THE PADDING?
// The struct itself must be aligned for use in arrays
// After the double (8 bytes) and int (4 bytes), we need 4 bytes of
// padding at the END so the next struct in an array is properly aligned
struct Goofy3 {
  double num1;  // 8 bytes at offset 0 (naturally aligned)
  int num2;     // 4 bytes at offset 8
                // 4 bytes of PADDING added at the end (offset 12-15)
                // Total: 16 bytes (8 + 4 + 4 padding)
};

int main(){

  // CREATE INSTANCES OF EACH STRUCT:
  // We create variables of each struct type
  // These are allocated on the stack
  struct Goofy1 g1;
  struct Goofy2 g2;
  struct Goofy3 g3;

  // PRINT THE SIZES:
  // sizeof() returns the TOTAL size including padding
  // %ld is the format specifier for size_t (what sizeof returns)
  //
  // EXPECTED OUTPUT:
  // Goofy1 size: 8
  // Goofy2 size: 16
  // Goofy3 size: 16
  //
  // OBSERVATION:
  // Goofy2 and Goofy3 have the SAME fields but in DIFFERENT order
  // Yet they have the SAME size! Both need padding due to alignment
  printf("Goofy1 size: %ld\n",sizeof(g1));
  printf("Goofy2 size: %ld\n",sizeof(g2));
  printf("Goofy3 size: %ld\n",sizeof(g3));

  return 0;
}

// DETAILED MEMORY LAYOUT:
//
// GOOFY1 LAYOUT (8 bytes total):
// Offset: 0    1    2    3    4    5    6    7
// Bytes:  [    num1 (int)    ] [    num2 (int)   ]
//         4 bytes              4 bytes
//
// GOOFY2 LAYOUT (16 bytes total):
// Offset: 0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15
// Bytes:  [    num1 (int)    ] [ PADDING (4 bytes)] [        num2 (double)           ]
//         4 bytes              4 bytes WASTED       8 bytes
//
// WHY THE PADDING?
// The double MUST start at offset 8 (a multiple of 8)
// After the 4-byte int at offset 0-3, we need padding at offset 4-7
//
// GOOFY3 LAYOUT (16 bytes total):
// Offset: 0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15
// Bytes:  [         num1 (double)          ] [   num2 (int)    ] [ PADDING (4 bytes)]
//         8 bytes                            4 bytes              4 bytes WASTED
//
// WHY THE PADDING AT THE END?
// Consider an array: struct Goofy3 arr[2];
// arr[0] ends at offset 12, but arr[1] MUST start at offset 16 (multiple of 8)
// So we need 4 bytes of padding at the end of each struct

// ALIGNMENT RULES:
//
// On 64-bit systems:
// - char:   1-byte aligned (can start at any offset)
// - short:  2-byte aligned (must start at even offset)
// - int:    4-byte aligned (must start at multiple of 4)
// - long:   8-byte aligned (must start at multiple of 8)
// - double: 8-byte aligned (must start at multiple of 8)
// - pointer: 8-byte aligned (must start at multiple of 8)
//
// STRUCT ALIGNMENT:
// The struct itself must be aligned to the LARGEST alignment of its members
// Goofy2 and Goofy3 both contain a double (8-byte aligned)
// So the entire struct must be a multiple of 8 bytes

// WHY DOES THE COMPILER DO THIS?
//
// 1. PERFORMANCE:
//    Accessing unaligned data can be 2-10x slower
//    On some architectures, unaligned access causes crashes!
//
// 2. ATOMIC OPERATIONS:
//    Aligned data can be read/written atomically
//    Unaligned data may require multiple memory operations
//
// 3. CACHE LINES:
//    CPU caches work in chunks (typically 64 bytes)
//    Aligned data fits better in cache lines

// PRACTICAL IMPLICATIONS:
//
// 1. MEMORY WASTE:
//    Goofy2 and Goofy3 waste 4 bytes each (25% overhead!)
//    In large arrays, this adds up quickly
//
// 2. STRUCT ORDERING MATTERS:
//    You can minimize padding by ordering fields from largest to smallest
//    Example: Put all doubles first, then ints, then chars
//
// 3. OPTIMAL GOOFY STRUCT:
//    struct Goofy2 {
//      double num2;  // 8 bytes
//      int num1;     // 4 bytes
//    };              // 12 bytes? NO! Still 16 due to array alignment
//
// 4. TRULY OPTIMAL:
//    If you need to save space and don't mind slight performance hit:
//    Use compiler directives: #pragma pack(1)
//    Or: __attribute__((packed))
//    This removes all padding, but may hurt performance

// COMMON MISCONCEPTIONS:
//
// MISCONCEPTION 1:
// "sizeof(struct) = sum of sizeof(fields)"
// REALITY: sizeof(struct) >= sum of fields due to padding
//
// MISCONCEPTION 2:
// "Field order doesn't matter"
// REALITY: Order affects padding and total size
//
// MISCONCEPTION 3:
// "The compiler is wasting space for no reason"
// REALITY: Padding enables much faster memory access

// REAL-WORLD EXAMPLE:
//
// BAD STRUCT DESIGN (lots of padding):
// struct Person {
//   char gender;      // 1 byte + 7 bytes padding
//   double salary;    // 8 bytes
//   char initial;     // 1 byte + 7 bytes padding
//   long id;          // 8 bytes
// };                  // Total: 32 bytes (14 bytes wasted!)
//
// GOOD STRUCT DESIGN (minimal padding):
// struct Person {
//   double salary;    // 8 bytes
//   long id;          // 8 bytes
//   char gender;      // 1 byte
//   char initial;     // 1 byte
//                     // 6 bytes padding at end
// };                  // Total: 24 bytes (6 bytes wasted)
//
// Savings: 8 bytes per struct (25% reduction!)
// In an array of 1 million: 8 MB saved!

// TRY IT:
// 1. Compile: gcc structsize.c -o structsize
// 2. Run: ./structsize
// 3. Observe the output - do the sizes match predictions?
// 4. EXPERIMENT: Add a char to Goofy1 - what happens to size?
// 5. EXPERIMENT: Create a struct with only chars - any padding?
// 6. EXPERIMENT: Use sizeof() on individual fields to verify calculations
// 7. ADVANCED: Try #pragma pack(1) and see how sizes change
//    Add before the structs: #pragma pack(1)
//    Add after the structs: #pragma pack()
