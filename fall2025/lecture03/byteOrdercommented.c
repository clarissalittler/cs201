// PEDAGOGICAL PURPOSE:
// This program demonstrates byte ordering (endianness) in computer memory.
// Key learning objectives:
// 1. Understanding little-endian byte order (least significant byte first)
// 2. Learning how to examine memory byte-by-byte using char pointers
// 3. Seeing the difference between logical value and physical memory layout
// 4. Understanding type casting to access data at different granularities
// 5. Learning hexadecimal notation for representing multi-byte values
// 6. Discovering that x86-64 processors store integers in little-endian format

#include "stdio.h"

int main(){

  // MULTI-BYTE INTEGER INITIALIZATION:
  // Create a 4-byte integer with a carefully chosen hexadecimal value
  // 0x1a2b3c4d is written in BIG-ENDIAN notation (most significant byte first)
  // Breaking down: 0x 1a 2b 3c 4d
  //                   ^^most significant byte (MSB)
  //                            ^^least significant byte (LSB)
  // In decimal: 439,041,101
  int num = 0x1a2b3c4d;

  // BYTE-LEVEL MEMORY ACCESS:
  // To examine how this integer is actually stored in memory,
  // we cast the ADDRESS of num to unsigned char*
  // WHY unsigned char*?
  //   1. char is 1 byte, so char* arithmetic moves by 1 byte at a time
  //   2. unsigned ensures values 0-255 (not -128 to 127)
  //   3. This lets us examine each individual byte of the 4-byte integer
  //
  // IMPORTANT: &num gives us the ADDRESS where num is stored
  // Casting to (unsigned char*) lets us treat that memory as an array of bytes
  unsigned char* pty = (unsigned char*)&num;

  // BYTE-BY-BYTE EXAMINATION:
  // Loop through all 4 bytes of the integer
  // On x86-64 systems, this will reveal LITTLE-ENDIAN byte order
  for(int i = 0; i<4; i++){
    // Print each byte in hexadecimal format
    // %x prints in hex WITHOUT the 0x prefix
    // pty[i] accesses the i-th byte of num's memory representation
    //
    // WHAT WILL WE SEE?
    // If BIG-ENDIAN (most significant byte first): 1a 2b 3c 4d
    // If LITTLE-ENDIAN (least significant byte first): 4d 3c 2b 1a
    //
    // On x86-64, we'll see LITTLE-ENDIAN: 4d 3c 2b 1a
    printf("%x ",pty[i]);
  }
  printf("\n");

  // CONCEPTUAL EXPLANATION OF LITTLE-ENDIAN:
  // Memory Address:  [addr+0] [addr+1] [addr+2] [addr+3]
  // Logical value:      LSB      ...      ...      MSB
  // For 0x1a2b3c4d:     0x4d    0x3c    0x2b    0x1a
  //
  // WHY little-endian? Historical reasons in x86 architecture
  // ADVANTAGES:
  //   - Easy to cast between different integer sizes
  //   - LSB is always at the lowest address
  // DISADVANTAGES:
  //   - Counterintuitive to human reading (we write MSB first)
  //   - Network protocols use big-endian (network byte order)

  return 0;
}

// EXPECTED OUTPUT ON x86-64:
// 4d 3c 2b 1a
//
// This shows the bytes are stored BACKWARDS from how we write them!

// WHY THIS MATTERS:
// 1. When sending data over networks, you may need to convert byte order
// 2. When reading binary files from different architectures
// 3. When doing low-level memory manipulation
// 4. When interfacing with hardware or binary protocols

// TRY IT:
// 1. Compile: gcc byteOrder.c -o byteOrder
// 2. Run: ./byteOrder
// 3. Observe the output: 4d 3c 2b 1a (reversed from 0x1a2b3c4d)
// 4. Try changing num to different hex values and see the pattern!
//    Example: num = 0x12345678 will output: 78 56 34 12
