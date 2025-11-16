// PEDAGOGICAL PURPOSE:
// This program demonstrates the write() system call for outputting data.
// It introduces UNIX I/O system calls and file descriptors.
// Key learning objectives:
// 1. Understanding system calls vs standard library functions
// 2. File descriptors (0=stdin, 1=stdout, 2=stderr)
// 3. The write() system call interface
// 4. Direct I/O without buffering (unlike printf)
// 5. Character arrays and string literals in C

#include "stdio.h"      // For printf (not used here, but good practice)
                        // Provides declarations for standard I/O functions

#include "unistd.h"     // For write() system call
                        // UNIX standard header for POSIX operating system API
                        // Provides access to low-level I/O functions
                        // "unistd" = "UNIX standard"

int main(){

  // STRING LITERAL STORED IN CHARACTER ARRAY:
  // We declare a local array and initialize it with a string
  char hiThere[] = "Hey there, goofuses and goofuxen\n";

  // BREAKING DOWN THE DECLARATION:
  // - char: Each element is one byte (character)
  // - hiThere[]: Array name, size computed from initializer
  // - "...": String literal - sequence of characters
  // - \n: Newline character (ASCII 0x0A)
  // - Implicit \0: String automatically null-terminated
  //
  // MEMORY LAYOUT:
  // hiThere[0]  = 'H'
  // hiThere[1]  = 'e'
  // hiThere[2]  = 'y'
  // hiThere[3]  = ' '
  // ...
  // hiThere[32] = 'n'
  // hiThere[33] = '\n'
  // hiThere[34] = '\0'  (null terminator)
  //
  // SIZE: 34 bytes including \n, 35 including \0
  //
  // WHY LOCAL ARRAY?
  // - char hiThere[] creates a mutable copy on the stack
  // - char *hiThere = "..." would point to read-only memory
  // - For write(), either works since we only read the data

  // SYSTEM CALL: write()
  write(1, hiThere, 33);

  // WRITE() PARAMETERS:
  // 1. File descriptor (1):
  //    - 0 = stdin (standard input - keyboard)
  //    - 1 = stdout (standard output - terminal)
  //    - 2 = stderr (standard error - terminal, for errors)
  //    - WHY 1? We're writing to standard output
  //
  // 2. Buffer pointer (hiThere):
  //    - Address of the data to write
  //    - Points to the first character 'H'
  //    - write() will read from this memory
  //
  // 3. Count (33):
  //    - Number of bytes to write
  //    - We write 33 bytes (not including the \0)
  //    - This includes the newline \n
  //    - WHY 33? The visible string is 33 chars long
  //      "Hey there, goofuses and goofuxen\n"
  //       ^                                ^
  //       0                               32
  //      (33 characters total)
  //
  // WHAT HAPPENS:
  // 1. Kernel reads 33 bytes from hiThere array
  // 2. Kernel writes those bytes to file descriptor 1 (stdout)
  // 3. Terminal displays: "Hey there, goofuses and goofuxen\n"
  // 4. write() returns 33 (number of bytes successfully written)
  //
  // RETURN VALUE (ignored here):
  // - On success: number of bytes written (33)
  // - On error: -1, and errno is set

  // WRITE() VS PRINTF():
  //
  // write():
  // - System call - direct to kernel
  // - No buffering - writes immediately
  // - No formatting - writes raw bytes
  // - Faster but less convenient
  // - Returns byte count or -1
  //
  // printf():
  // - Library function - calls write() internally
  // - Buffered - may delay output
  // - Formatted output - %d, %s, etc.
  // - Slower but more powerful
  // - Returns character count or negative value
  //
  // EQUIVALENT WITH printf():
  // printf("%s", hiThere);
  // // or simply:
  // printf("Hey there, goofuses and goofuxen\n");

  return 0;  // EXIT SUCCESS: Return 0 to indicate success
             // This is converted to exit() syscall by runtime
             // The shell sees exit status 0
}

// SYSTEM CALLS VS LIBRARY FUNCTIONS:
//
// System Call (write):
// - Direct kernel interface
// - Implemented by the OS kernel
// - Transitions from user mode to kernel mode
// - Very fast, minimal overhead
// - Low-level, less convenient
//
// Library Function (printf):
// - User-space code in libc
// - Eventually calls write() internally
// - Adds buffering, formatting, convenience
// - Slightly slower due to extra layers
// - High-level, more convenient

// FILE DESCRIPTORS:
//
// File descriptors are small integers that refer to open files
// Every process starts with three standard file descriptors:
//
// 0 = stdin  - Standard input  (usually keyboard)
// 1 = stdout - Standard output (usually terminal)
// 2 = stderr - Standard error  (usually terminal)
//
// When you open() a file, you get a new file descriptor (3, 4, 5, ...)
// You use these numbers with read(), write(), close(), etc.

// WHY 33 BYTES?
//
// Let's count the string: "Hey there, goofuses and goofuxen\n"
// H e y   t h e r e ,   g o o f u s e s   a n d   g o o f u x e n \n
// 1 2 3 4 5 6 7 8 9 10 11...                                  32 33
//
// Total: 33 characters including the \n
// We DON'T count the \0 because write() doesn't need it
// (write() writes exactly the number of bytes you specify)
//
// If we used printf("%s", hiThere), it would read until \0
// But write() uses the explicit byte count

// ASSEMBLY EQUIVALENT:
//
// This C program translates to assembly roughly as:
// (see readWriteASM1.s for the actual assembly version)
//
// .section .data
// hiThere: .ascii "Hey there, goofuses and goofuxen\n"
//
// .section .text
// _start:
//     mov $1,%rax        # syscall 1 = write
//     mov $1,%rdi        # fd = 1 (stdout)
//     lea hiThere(%rip),%rsi  # buffer = address of hiThere
//     mov $33,%rdx       # count = 33 bytes
//     syscall            # invoke kernel
//     mov $60,%rax       # syscall 60 = exit
//     mov $0,%rdi        # status = 0
//     syscall

// TO COMPILE AND RUN:
// gcc readWrite.c -o readWrite
// ./readWrite
// Output: Hey there, goofuses and goofuxen
//
// You can also redirect output:
// ./readWrite > output.txt
// cat output.txt
