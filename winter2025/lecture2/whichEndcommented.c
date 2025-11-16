// PEDAGOGICAL PURPOSE:
// This program determines the byte order (endianness) of the system by examining
// how a multi-byte integer is stored in memory at the byte level.
// Key learning objectives:
// 1. Understanding endianness (byte order) in computer systems
// 2. Big-endian vs little-endian storage of multi-byte values
// 3. Using pointer type casting to inspect individual bytes
// 4. The unsigned char* type for viewing raw bytes
// 5. How the same integer has different byte representations in memory
// 6. Platform-dependent behavior in low-level programming

#include <stdio.h>
#include <stdlib.h>

// ENDIANNESS CONCEPTS:
// Big-endian: Most significant byte stored at lowest address (like reading left-to-right)
// Little-endian: Least significant byte stored at lowest address (backwards)

int main(){

  // COMMENT EXPLANATION:
  // The comment states the question this program answers:
  // Is this system big-endian or little-endian?

  // TEST VALUE:
  // We use 0x12345678 because each byte is different and recognizable
  // Hex: 0x12345678
  // Binary: 00010010 00110100 01010110 01111000
  //
  // Breaking into bytes (each 2 hex digits = 1 byte):
  //   Byte 3 (most significant):  0x12
  //   Byte 2:                     0x34
  //   Byte 1:                     0x56
  //   Byte 0 (least significant): 0x78
  int num = 0x12345678;

  // POINTER TYPE CASTING:
  // &num gives us a pointer to num (type: int*)
  // (unsigned char*)&num casts it to unsigned char*
  //
  // WHY unsigned char*?
  // - char is 1 byte, so we can inspect individual bytes
  // - unsigned avoids sign extension issues
  // - unsigned char is the standard type for byte manipulation in C
  //
  // Now pointy points to the FIRST BYTE of num in memory
  unsigned char* pointy = (unsigned char*)&num;

  // PRINT BYTE ORDER:
  printf("Our number's bytes, in order, are: ");

  // ITERATE THROUGH BYTES:
  // sizeof(int) is 4 (on most systems), so we iterate through 4 bytes
  // i=0: first byte in memory (lowest address)
  // i=1: second byte in memory
  // i=2: third byte in memory
  // i=3: fourth byte in memory (highest address)
  for(int i = 0; i < sizeof(int); i++){

    // PRINT EACH BYTE IN HEXADECIMAL:
    // *(pointy+i) dereferences the pointer at offset i
    // This gives us the i-th byte of num
    // %x formats as hexadecimal (lowercase)
    //
    // WHAT WE'RE SEEING:
    // This prints the bytes in MEMORY ORDER (increasing addresses)
    //
    // On LITTLE-ENDIAN (x86, x86-64):
    //   i=0: 78 (least significant byte first)
    //   i=1: 56
    //   i=2: 34
    //   i=3: 12 (most significant byte last)
    //   Output: 78 56 34 12
    //
    // On BIG-ENDIAN (some ARM, SPARC, old PowerPC):
    //   i=0: 12 (most significant byte first)
    //   i=1: 34
    //   i=2: 56
    //   i=3: 78 (least significant byte last)
    //   Output: 12 34 56 78
    printf(" %x ",*(pointy+i));
  }
  printf("\n");

  return 0;
}

// COMPREHENSIVE EXPLANATION:
//
// ENDIANNESS DEFINED:
//
// For multi-byte values (int, long, short, pointers), bytes can be stored in two orders:
//
// LITTLE-ENDIAN:
// - Least significant byte (LSB) at lowest memory address
// - Common on: x86, x86-64, most ARM (modern)
// - Called "little" because the "little end" (LSB) comes first
//
// BIG-ENDIAN:
// - Most significant byte (MSB) at lowest memory address
// - Common on: network protocols, some ARM, SPARC, old PowerPC
// - Called "big" because the "big end" (MSB) comes first
// - More intuitive for humans (matches how we write numbers)

// MEMORY LAYOUT EXAMPLE FOR 0x12345678:
//
// LITTLE-ENDIAN (x86-64):
//   Address    Byte Value
//   0x1000     0x78  ← Least significant byte (LSB)
//   0x1001     0x56
//   0x1002     0x34
//   0x1003     0x12  ← Most significant byte (MSB)
//
// BIG-ENDIAN (network byte order):
//   Address    Byte Value
//   0x1000     0x12  ← Most significant byte (MSB)
//   0x1001     0x34
//   0x1002     0x56
//   0x1003     0x78  ← Least significant byte (LSB)
//
// In both cases, the VALUE is the same (0x12345678)
// But the STORAGE ORDER differs!

// WHY ENDIANNESS MATTERS:
//
// 1. NETWORK COMMUNICATION:
//    Network protocols use big-endian ("network byte order")
//    x86 systems must convert when sending/receiving network data
//    Functions: htonl(), ntohl() (host-to-network, network-to-host)
//
// 2. FILE FORMATS:
//    Binary file formats must specify endianness
//    Reading a file created on big-endian system on little-endian system requires conversion
//
// 3. MIXED-ENDIAN SYSTEMS:
//    Some ARM chips can switch endianness
//    Code must be aware of current mode
//
// 4. CROSS-PLATFORM COMPATIBILITY:
//    Sending binary data between different architectures
//    Must agree on byte order or convert

// DETERMINING ENDIANNESS:
//
// This program's approach:
//   int num = 0x12345678;
//   unsigned char* p = (unsigned char*)&num;
//   if (p[0] == 0x78) → Little-endian
//   if (p[0] == 0x12) → Big-endian
//
// Alternative approach:
//   union {
//     int i;
//     char c[4];
//   } test;
//   test.i = 0x12345678;
//   if (test.c[0] == 0x78) → Little-endian

// EXAMPLE OUTPUT:
//
// On x86-64 Linux (little-endian):
//   Our number's bytes, in order, are:  78  56  34  12
//
// On big-endian system:
//   Our number's bytes, in order, are:  12  34  56  78

// BI-ENDIAN SYSTEMS:
//
// Some processors can operate in either mode:
// - ARM (some versions): can switch between big and little
// - PowerPC: can switch
// - MIPS: can be configured at boot
//
// Modern trend: most systems use little-endian
// - x86/x86-64: always little-endian
// - ARM: usually little-endian (can be big)
// - RISC-V: little-endian by specification

// NETWORK BYTE ORDER:
//
// TCP/IP and other network protocols use BIG-ENDIAN
// Called "network byte order"
//
// Conversion functions (from <arpa/inet.h> on Unix):
//   htons() - host to network short (16-bit)
//   htonl() - host to network long (32-bit)
//   ntohs() - network to host short
//   ntohl() - network to host long
//
// On big-endian systems, these do nothing
// On little-endian systems, these swap bytes
//
// Example:
//   uint32_t host_val = 0x12345678;
//   uint32_t net_val = htonl(host_val);
//   // On little-endian: net_val = 0x78563412 (bytes swapped)
//   // On big-endian:    net_val = 0x12345678 (unchanged)

// BYTE SWAPPING:
//
// Manual byte swap for 32-bit value:
//   uint32_t swap32(uint32_t x) {
//     return ((x & 0xFF000000) >> 24) |
//            ((x & 0x00FF0000) >>  8) |
//            ((x & 0x0000FF00) <<  8) |
//            ((x & 0x000000FF) << 24);
//   }
//
// For 16-bit value:
//   uint16_t swap16(uint16_t x) {
//     return (x >> 8) | (x << 8);
//   }

// LITTLE-ENDIAN ADVANTAGES:
// - Easier for CPU to process (can read partial values)
// - Type casting between different-sized integers simpler
// - Example: *(short*)&int_val reads lower 2 bytes naturally
//
// BIG-ENDIAN ADVANTAGES:
// - More intuitive for humans (matches written number order)
// - Easier to debug (hex dumps read naturally)
// - Comparison operations can be faster (compare from MSB)

// HISTORICAL NOTE:
//
// "Endian" terminology comes from Gulliver's Travels
// Big-Endians: break eggs from big end
// Little-Endians: break eggs from little end
// Satirizes trivial debates over arbitrary conventions
//
// In computing, coined by Danny Cohen in 1980 paper
// "On Holy Wars and a Plea for Peace"

// MIDDLE-ENDIAN (rare):
//
// Some very old systems used "middle-endian" (also called PDP-endian)
// Example: PDP-11 stored 32-bit value as 2 16-bit words in one order,
//           but bytes within each word in another order
// For 0x12345678: stored as 34 12 78 56
// Extremely rare now, mostly historical

// DETECTING AT COMPILE TIME:
//
// Some compilers define macros:
//   #ifdef __LITTLE_ENDIAN__
//     // Little-endian
//   #elif defined(__BIG_ENDIAN__)
//     // Big-endian
//   #endif
//
// Or use predefined macros:
//   __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
//   __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

// PRACTICAL IMPLICATIONS:
//
// When does endianness matter?
// 1. Reading/writing binary files across platforms
// 2. Network communication
// 3. Memory-mapped I/O (hardware registers)
// 4. Type punning (viewing same memory as different types)
// 5. Parsing binary protocols
//
// When doesn't it matter?
// 1. Normal arithmetic (CPU handles it)
// 2. String operations (bytes processed individually)
// 3. Most high-level code

// TO COMPILE AND RUN:
// gcc whichEnd.c -o whichEnd
// ./whichEnd
//
// On typical x86/x86-64 system (little-endian):
// Output: Our number's bytes, in order, are:  78  56  34  12
//
// Interpretation:
// The byte 78 (LSB) is stored first (lowest address)
// This is LITTLE-ENDIAN
//
// On big-endian system:
// Output: Our number's bytes, in order, are:  12  34  56  78
//
// Interpretation:
// The byte 12 (MSB) is stored first (lowest address)
// This is BIG-ENDIAN
