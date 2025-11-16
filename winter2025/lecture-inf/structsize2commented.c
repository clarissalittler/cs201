// PEDAGOGICAL PURPOSE:
// This program demonstrates how struct field ordering dramatically affects
// memory layout and total struct size through padding and alignment.
// Key learning objectives:
// 1. Understanding that IDENTICAL fields in DIFFERENT order create DIFFERENT sizes
// 2. Learning to visualize memory layout byte-by-byte
// 3. Seeing the practical impact of poor vs. optimal struct design
// 4. Understanding why alignment requirements force padding
// 5. Learning to minimize wasted space through strategic field ordering
// 6. Calculating padding manually to predict struct sizes
// 7. Understanding the cost of poor struct design in large-scale applications
//
// CORE CONCEPT:
// These three structs contain EXACTLY the same fields (same types, same names),
// but because the fields appear in different orders, the compiler must add
// different amounts of padding to maintain alignment. The result: identical
// fields can produce different struct sizes!

#include <stdio.h>
#include <stdlib.h>

// STRUCT A: ARRAYS FIRST, CHAR LAST (SUB-OPTIMAL)
// Fields in order: int[3], char, long[2]
// Let's calculate the expected size:
// - int[3]:      12 bytes (3 x 4 bytes)
// - char:        1 byte
// - long[2]:     16 bytes (2 x 8 bytes)
// Sum: 29 bytes, but actual size will be LARGER due to alignment
//
// MEMORY LAYOUT:
// Offset 0-11:   int_field[3] (12 bytes)
// Offset 12:     char_field (1 byte)
// Offset 13-15:  PADDING (3 bytes) - needed to align next long to 8-byte boundary
// Offset 16-31:  array[2] (16 bytes) - long must start at multiple of 8
// Total: 32 bytes (3 bytes wasted as padding)
struct exA {
	int int_field[3]; // 12 bytes = 29 bytes
	char char_field; // 1 byte
	long array[2];   // 16 bytes
};

// STRUCT B: CHAR FIRST, THEN LONGS, THEN INTS (WORST DESIGN)
// Fields in order: char, long[2], int[3]
// Same fields as struct A, just reordered
// Let's calculate the expected size:
// - char:        1 byte
// - long[2]:     16 bytes (2 x 8 bytes)
// - int[3]:      12 bytes (3 x 4 bytes)
// Sum: 29 bytes, but actual size will be LARGER
//
// MEMORY LAYOUT:
// Offset 0:      char_field (1 byte)
// Offset 1-7:    PADDING (7 bytes!) - needed to align next long to 8-byte boundary
// Offset 8-23:   array[2] (16 bytes) - long must start at multiple of 8
// Offset 24-35:  int_field[3] (12 bytes)
// Offset 36-39:  PADDING (4 bytes) - struct must be multiple of 8 for array alignment
// Total: 40 bytes (11 bytes wasted as padding!)
struct exB {
	char char_field; // 1 byte
	long array[2];   // 16 bytes
	int int_field[3];   // 12 bytes = 29 bytes
};

// STRUCT C: LONGS FIRST, INTS SECOND, CHAR LAST (OPTIMAL)
// Fields in order: long[2], int[3], char
// Same fields again, but ordered from largest to smallest
// Let's calculate the expected size:
// - long[2]:     16 bytes (2 x 8 bytes)
// - int[3]:      12 bytes (3 x 4 bytes)
// - char:        1 byte
// Sum: 29 bytes, but actual size will be rounded up
//
// MEMORY LAYOUT:
// Offset 0-15:   array[2] (16 bytes) - naturally aligned at offset 0
// Offset 16-27:  int_field[3] (12 bytes) - naturally aligned at offset 16
// Offset 28:     char_field (1 byte)
// Offset 29-31:  PADDING (3 bytes) - struct must be multiple of 8 for array alignment
// Total: 32 bytes (3 bytes wasted as padding)
struct exC {
	long array[2];   // 16 bytes
	int int_field[3];   // 12 bytes
    char char_field; // 1 byte = 29 bytes
};

int main()
{
   // CREATE INSTANCES:
   // We create one instance of each struct type
   // These are allocated on the stack
   struct exA one;
   struct exB two;
   struct exC three;

   // MEASURE SIZES:
   // sizeof() returns the total size including all padding bytes
   // We store in int variables (could use size_t for pedantic correctness)
   int size1 = sizeof(one);
   int size2 = sizeof(two);
   int size3 = sizeof(three);

   // PRINT RESULTS:
   // This will show the dramatic effect of field ordering
   //
   // EXPECTED OUTPUT:
   // exA: 32  exB: 40  exC: 32
   //
   // ANALYSIS:
   // exA: 32 bytes (3 bytes padding = 9.4% waste)
   // exB: 40 bytes (11 bytes padding = 27.5% waste!)
   // exC: 32 bytes (3 bytes padding = 9.4% waste)
   //
   // KEY OBSERVATION:
   // exB is 25% LARGER than exA and exC despite having identical fields!
   // exA and exC have the same size despite different field orders
   printf("exA: %d  exB: %d  exC: %d\n", size1, size2, size3);

   return 0;
}

// DETAILED MEMORY LAYOUT VISUALIZATION:
//
// STRUCT exA (32 bytes):
// Offset: 00  01  02  03  04  05  06  07  08  09  10  11  12  13  14  15
// Bytes:  [int_field[0] ] [int_field[1] ] [int_field[2] ] [ch][  PAD    ]
//
// Offset: 16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31
// Bytes:  [    array[0] (long)          ] [    array[1] (long)          ]
//
// Padding: 3 bytes at offsets 13-15
//
// STRUCT exB (40 bytes) - THE WORST!:
// Offset: 00  01  02  03  04  05  06  07  08  09  10  11  12  13  14  15
// Bytes:  [ch][        PADDING (7 bytes!)                ] [array[0]     ->
//
// Offset: 16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31
// Bytes:  ->   (long)              ] [    array[1] (long)           ] [int->
//
// Offset: 32  33  34  35  36  37  38  39
// Bytes:  ->field[0]] [int_field[1]] [int_field[2]] [PAD]
//
// Padding: 7 bytes at offsets 1-7, PLUS 4 bytes at offsets 36-39 = 11 bytes total!
//
// STRUCT exC (32 bytes) - OPTIMAL:
// Offset: 00  01  02  03  04  05  06  07  08  09  10  11  12  13  14  15
// Bytes:  [    array[0] (long)          ] [    array[1] (long)          ]
//
// Offset: 16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31
// Bytes:  [int_field[0] ] [int_field[1] ] [int_field[2] ] [ch][  PAD    ]
//
// Padding: 3 bytes at offsets 29-31

// WHY DOES STRUCT ORDER MATTER?
//
// ALIGNMENT REQUIREMENTS:
// - char:  Can start at ANY offset (1-byte aligned)
// - int:   Must start at multiple of 4 (4-byte aligned)
// - long:  Must start at multiple of 8 (8-byte aligned)
//
// THE PROBLEM WITH exB:
// After the single char at offset 0, the next field is a long array
// longs MUST start at a multiple of 8
// The next multiple of 8 after offset 0 is offset 8
// So we waste offsets 1-7 (7 bytes!) just to align the long
//
// THE WISDOM OF exC:
// Start with longs (naturally aligned at offset 0)
// Follow with ints (offset 16 is a multiple of 4 - good!)
// End with char (can go anywhere)
// Only padding needed is at the end for struct alignment

// BEST PRACTICES FOR STRUCT DESIGN:
//
// 1. ORDER BY SIZE (largest to smallest):
//    - Put all 8-byte types first (long, double, pointers)
//    - Then 4-byte types (int, float)
//    - Then 2-byte types (short)
//    - Finally 1-byte types (char)
//
// 2. GROUP SIMILAR TYPES:
//    - Put all longs together
//    - Put all ints together
//    - Put all chars together
//
// 3. AVOID ALTERNATING SIZES:
//    - Don't do: char, long, char, long
//    - Each char will cause 7 bytes of padding!

// REAL-WORLD IMPACT:
//
// Consider a database with 10 million records using struct exB:
// Memory used: 10,000,000 × 40 bytes = 400 MB
//
// If we redesign to use struct exC:
// Memory used: 10,000,000 × 32 bytes = 320 MB
//
// SAVINGS: 80 MB (20% reduction!)
//
// This affects:
// - RAM usage
// - Cache efficiency
// - Disk I/O if structures are serialized
// - Network bandwidth if structures are transmitted
// - Overall application performance

// CALCULATING PADDING MANUALLY:
//
// FORMULA FOR FIELD PADDING:
// padding_before_field = (alignment - (current_offset % alignment)) % alignment
//
// EXAMPLE: char at offset 0, next field is long (8-byte aligned)
// current_offset = 1 (after the char)
// alignment = 8 (for long)
// padding = (8 - (1 % 8)) % 8 = (8 - 1) % 8 = 7 bytes
//
// FORMULA FOR STRUCT END PADDING:
// struct must be a multiple of its largest alignment requirement
// end_padding = (largest_alignment - (size_so_far % largest_alignment)) % largest_alignment

// ARRAY CONSIDERATION:
//
// WHY STRUCT SIZE MUST BE A MULTIPLE OF ALIGNMENT?
// Consider: struct exC array[2];
//
// If exC was 29 bytes (no end padding):
// array[0] is at offset 0   - array[0].array[0] is at offset 0  ✓ (multiple of 8)
// array[1] is at offset 29  - array[1].array[0] is at offset 29 ✗ (NOT multiple of 8!)
//
// With exC at 32 bytes (3 bytes end padding):
// array[0] is at offset 0   - array[0].array[0] is at offset 0  ✓ (multiple of 8)
// array[1] is at offset 32  - array[1].array[0] is at offset 32 ✓ (multiple of 8)
//
// The end padding ensures ALL elements in an array are properly aligned!

// COMPILER-SPECIFIC NOTES:
//
// DIFFERENT COMPILERS MAY VARY:
// Most modern compilers follow the same alignment rules, but:
// - Some embedded systems use different alignment
// - Some compilers have flags to change alignment
// - Cross-platform code may see different sizes
//
// CHECKING ALIGNMENT:
// Use: _Alignof(struct exB)  (C11)
// Or:  __alignof__(struct exB)  (GCC extension)

// COMMON MISCONCEPTIONS:
//
// MISCONCEPTION 1:
// "Reordering struct fields breaks binary compatibility"
// REALITY: TRUE! Never reorder fields in published APIs/ABIs
//          But for internal structs, optimization is worth it
//
// MISCONCEPTION 2:
// "The comments '29 bytes' in the original code are correct"
// REALITY: Those comments show the SUM of field sizes, not actual struct size
//          Actual sizes are 32, 40, and 32 bytes respectively
//
// MISCONCEPTION 3:
// "I can remove padding with #pragma pack"
// REALITY: Yes, but at a performance cost (slower access, possible crashes)

// TRY IT:
// 1. Compile: gcc structsize2.c -o structsize2
// 2. Run: ./structsize2
// 3. Verify output: exA: 32  exB: 40  exC: 32
// 4. EXPERIMENT: Add more fields and predict the sizes
//    - Add "double d;" to each struct - what are the new sizes?
//    - Where should you put the double for minimal padding?
// 5. EXPERIMENT: Create a struct with alternating char and long
//    struct Bad { char c1; long l1; char c2; long l2; };
//    Predict the size before checking!
// 6. EXPERIMENT: Use pahole tool (Linux) to visualize padding:
//    gcc -g structsize2.c -o structsize2
//    pahole structsize2
// 7. ADVANCED: Try #pragma pack(1) to remove all padding
//    Compare sizes and measure performance impact
