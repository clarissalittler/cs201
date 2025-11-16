// PEDAGOGICAL PURPOSE:
// This program demonstrates endianness (byte order) in computer memory.
// It shows how multi-byte integers are stored in memory on different systems.
// Key learning objectives:
// 1. Understanding little-endian vs big-endian byte ordering
// 2. How to examine individual bytes of a multi-byte data type
// 3. Pointer type casting (int* to unsigned char*)
// 4. Memory layout of integers
// 5. Using sizeof and typeof operators
// 6. Hexadecimal representation and byte-level manipulation

#include <stdio.h>      // STANDARD I/O: For printf function
#include <stdlib.h>     // STANDARD LIBRARY: General utilities

int main(){

  int n = 0x89ABCDEF;   // INITIALIZE INTEGER: A specific hex value for testing
                        // 0x prefix indicates hexadecimal notation
                        // This number has distinct bytes: 89, AB, CD, EF
                        // In binary: 10001001 10101011 11001101 11101111
                        // We chose this value so each byte is visibly different
                        // This makes it easy to see the byte order in memory

  unsigned char* c = (unsigned char*) &n;  // POINTER TO BYTES: Create byte-level view
                                            // &n gets the address of n
                                            // (unsigned char*) casts int pointer to byte pointer
                                            // WHY unsigned char?
                                            // - char is 1 byte, perfect for examining individual bytes
                                            // - unsigned ensures values 0-255 (no negative values)
                                            // - This allows us to "peek" at each byte of the int
                                            //
                                            // IMPORTANT CONCEPT - TYPE PUNNING:
                                            // We're viewing the same memory location as different types
                                            // The int n occupies 4 bytes in memory
                                            // c points to the first byte of those 4 bytes
                                            // c+1 points to the second byte, etc.

  for(int i=0; i<sizeof(typeof(n)); i++){  // LOOP THROUGH EACH BYTE: Iterate over all bytes
                                            // typeof(n) gets the type of n (which is int)
                                            // sizeof(typeof(n)) gets size in bytes (typically 4)
                                            // Could also write: sizeof(int) or sizeof(n)
                                            //
                                            // WHY typeof?
                                            // - More generic: works even if we change n's type
                                            // - typeof is a GCC extension (not standard C)
                                            // - Standard C would use: sizeof(n)

    printf("The %d th byte of n is: %x \n",i,*(c+i));  // PRINT EACH BYTE
                                                        // i is the byte index (0, 1, 2, 3)
                                                        // c+i is pointer arithmetic: address of i-th byte
                                                        // *(c+i) dereferences to get the byte value
                                                        // %x formats as hexadecimal (lowercase)
                                                        // %X would give uppercase hex
                                                        //
                                                        // POINTER ARITHMETIC:
                                                        // c points to first byte at address, say, 0x1000
                                                        // c+0 = 0x1000 (first byte)
                                                        // c+1 = 0x1001 (second byte)
                                                        // c+2 = 0x1002 (third byte)
                                                        // c+3 = 0x1003 (fourth byte)
                                                        //
                                                        // Since c is unsigned char*, adding 1 advances 1 byte
                                                        // If c were int*, adding 1 would advance 4 bytes!
  }

  return 0;             // SUCCESS: Exit with status 0
}

// EXPECTED OUTPUT (on x86-64 Linux, which is little-endian):
// The 0 th byte of n is: ef
// The 1 th byte of n is: cd
// The 2 th byte of n is: ab
// The 3 th byte of n is: 89

// BYTE ORDER EXPLANATION:
//
// LITTLE-ENDIAN (x86, x86-64, ARM in little-endian mode):
//   - Least significant byte (LSB) stored at lowest address
//   - 0x89ABCDEF stored as: EF CD AB 89
//   - Byte 0 (lowest address): EF (least significant)
//   - Byte 3 (highest address): 89 (most significant)
//   - Think: "little end" (least significant) comes first
//
// BIG-ENDIAN (SPARC, PowerPC, network byte order):
//   - Most significant byte (MSB) stored at lowest address
//   - 0x89ABCDEF stored as: 89 AB CD EF
//   - Byte 0 (lowest address): 89 (most significant)
//   - Byte 3 (highest address): EF (least significant)
//   - Think: "big end" (most significant) comes first
//   - This matches how we write numbers: left-to-right, big to small

// MEMORY DIAGRAM (Little-Endian):
//
// Address:    0x1000  0x1001  0x1002  0x1003
// Byte:          EF      CD      AB      89
// Index:       c+0     c+1     c+2     c+3
// Significance: LSB                    MSB

// WHY DOES ENDIANNESS MATTER?
//
// 1. NETWORK COMMUNICATION:
//    - Network protocols use big-endian (network byte order)
//    - Must convert when sending/receiving on little-endian systems
//    - Functions: htonl() (host to network long), ntohl() (network to host long)
//
// 2. FILE FORMATS:
//    - Different systems may store data differently
//    - File formats must specify byte order
//    - BMP files: little-endian
//    - JPEG files: big-endian
//
// 3. BINARY DATA EXCHANGE:
//    - Sending binary data between different architectures
//    - Must agree on byte order or convert appropriately
//
// 4. LOW-LEVEL DEBUGGING:
//    - Understanding memory dumps
//    - Reverse engineering
//    - Hardware interfacing

// KEY CONCEPTS DEMONSTRATED:
//
// 1. POINTER CASTING: Viewing memory as different types
// 2. POINTER ARITHMETIC: Navigating through memory
// 3. BYTE-LEVEL ACCESS: Reading individual bytes
// 4. HEXADECIMAL: Compact representation of binary data
// 5. ENDIANNESS: Byte ordering in multi-byte values

// TO COMPILE AND RUN:
// gcc byteorder.c -o byteorder
// ./byteorder

// EXPERIMENT:
// Try different values for n:
// - 0x12345678 (each byte is 12, 34, 56, 78)
// - 0x00000001 (mostly zeros, only LSB set)
// - 0xFF000000 (only MSB set)
// This helps visualize how bytes are arranged in memory
