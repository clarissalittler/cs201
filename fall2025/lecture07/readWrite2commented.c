// PEDAGOGICAL PURPOSE:
// This program demonstrates both read() and write() system calls,
// creating a simple echo program that reads from stdin and writes to stdout.
// Key learning objectives:
// 1. Understanding the read() system call for input
// 2. Using file descriptor 0 (stdin) for reading
// 3. Handling variable-length input with return values
// 4. Combining read() and write() for I/O operations
// 5. Buffer management and safety considerations

#include "unistd.h"     // For read() and write() system calls
                        // Provides POSIX operating system API
                        // Both read() and write() are declared here

int main(){

  // BUFFER DECLARATION:
  // Allocate space to store input data
  char buff[30];        // Local array of 30 bytes on the stack
                        // IMPORTANT: No initialization - contains garbage
                        // This is our INPUT BUFFER - where read() stores data
                        //
                        // SIZE CHOICE:
                        // - 30 bytes is arbitrary (could be larger/smaller)
                        // - Larger = can read more at once
                        // - Smaller = less memory, more read() calls needed
                        // - Must be big enough for expected input

  // SYSTEM CALL: read()
  int charsRead = read(0,buff,30);

  // READ() PARAMETERS:
  // 1. File descriptor (0):
  //    - 0 = stdin (standard input)
  //    - This reads from the terminal (or redirected input)
  //    - User types text and presses Enter
  //
  // 2. Buffer pointer (buff):
  //    - Address where read() should store the input
  //    - Points to our 30-byte array
  //    - read() will write directly to this memory
  //
  // 3. Count (30):
  //    - Maximum number of bytes to read
  //    - read() will NOT read more than this
  //    - Prevents buffer overflow (reading beyond buff)
  //    - WHY 30? That's the size of our buffer
  //
  // RETURN VALUE (charsRead):
  // - On success: number of bytes actually read
  //   - May be less than 30 if user types less
  //   - Includes the newline character if present
  // - On EOF: 0 (e.g., Ctrl+D on Unix)
  // - On error: -1, and errno is set
  //
  // WHAT HAPPENS:
  // 1. Program blocks (waits) until user types something
  // 2. User types text and presses Enter
  // 3. Kernel copies input into buff[]
  // 4. read() returns the number of bytes copied
  // 5. charsRead now contains that count
  //
  // EXAMPLE:
  // If user types "Hello\n" and presses Enter:
  // - buff contains: "Hello\n" (6 bytes used, 24 unused)
  // - charsRead = 6
  // - buff[0]='H', buff[1]='e', ..., buff[5]='\n'

  // SYSTEM CALL: write()
  write(1,buff,charsRead);

  // WRITE() PARAMETERS:
  // 1. File descriptor (1):
  //    - 1 = stdout (standard output)
  //    - This writes to the terminal
  //
  // 2. Buffer pointer (buff):
  //    - Address of the data to write
  //    - Same buffer we just read into
  //    - We're echoing what we read
  //
  // 3. Count (charsRead):
  //    - Number of bytes to write
  //    - WHY charsRead? Write exactly what we read!
  //    - If user typed 6 chars, write 6 chars
  //    - If user typed 20 chars, write 20 chars
  //    - This ensures we don't write garbage bytes
  //
  // WHAT HAPPENS:
  // 1. Kernel reads charsRead bytes from buff
  // 2. Kernel writes those bytes to stdout
  // 3. Terminal displays them
  // 4. Result: user sees their input echoed back
  //
  // WHY NOT write(1,buff,30)?
  // - That would write all 30 bytes, including garbage!
  // - If user typed "Hi\n" (3 bytes), we'd write 3 bytes + 27 garbage bytes
  // - Using charsRead ensures we only write valid data

  return 0;  // EXIT SUCCESS: Return 0 to indicate success
             // Runtime converts this to exit(0) syscall
}

// ECHO PROGRAM BEHAVIOR:
//
// This creates a simple echo program:
// 1. User types: "Hello World\n"
// 2. read() stores it in buff, returns 12
// 3. write() outputs those 12 bytes
// 4. User sees: "Hello World\n"
//
// Try it:
// $ ./readWrite2
// Hello World     ← user types this
// Hello World     ← program echoes it
// $

// BUFFER SAFETY:
//
// IMPORTANT: We read at most 30 bytes
// If user types more than 30 chars:
// - Only first 30 are read into buff
// - Rest remain in input buffer
// - Next read() would get them
//
// WHY is this safe?
// - read() respects the count parameter (30)
// - It will NEVER write beyond buff[29]
// - This prevents buffer overflow vulnerabilities
//
// UNSAFE version (don't do this):
// read(0, buff, 1000);  // buff is only 30 bytes!
// // This would overflow the buffer if read() gets 1000 bytes

// READ() BLOCKING BEHAVIOR:
//
// read() is a BLOCKING call:
// - If no input is available, program waits
// - CPU puts process to sleep
// - When input arrives, kernel wakes the process
// - read() returns with the data
//
// Non-blocking I/O is possible with fcntl() or O_NONBLOCK

// NULL TERMINATION:
//
// IMPORTANT: read() does NOT null-terminate!
// - If user types "Hi\n", buff contains: 'H','i','\n',<garbage>
// - There's NO '\0' at buff[3]
// - This is fine for write() (uses explicit count)
// - But unsafe for printf("%s", buff) - might read past end!
//
// To use with string functions, you'd need:
// buff[charsRead] = '\0';  // Manually add null terminator
// printf("%s", buff);       // Now safe

// COMPARISON: read() VS scanf() VS fgets():
//
// read():
// - System call, direct to kernel
// - Reads raw bytes (no interpretation)
// - No null termination
// - Returns byte count
// - Example: read(0, buff, 30)
//
// scanf():
// - Library function (calls read() internally)
// - Parses formatted input (%d, %s, etc.)
// - Adds null termination for strings
// - Returns number of items matched
// - Example: scanf("%s", buff)
//
// fgets():
// - Library function (calls read() internally)
// - Reads until newline or limit
// - Includes newline in buffer
// - Adds null termination
// - Returns pointer or NULL
// - Example: fgets(buff, 30, stdin)

// FILE DESCRIPTOR REDIRECTION:
//
// You can redirect input/output:
//
// $ echo "Test input" | ./readWrite2
// Test input
//
// $ ./readWrite2 < input.txt
// (contents of input.txt are echoed)
//
// $ ./readWrite2 > output.txt
// (user types input, it's saved to output.txt)
//
// The program doesn't change - just the meaning of fd 0 and 1

// ASSEMBLY EQUIVALENT:
//
// This C program translates to assembly roughly as:
// (see readWriteASM2.s for the actual assembly version)
//
// .section .bss
// buff: .skip 30         # Reserve 30 uninitialized bytes
//
// .section .text
// _start:
//     # read(0, buff, 30)
//     mov $0,%rax        # syscall 0 = read
//     mov $0,%rdi        # fd = 0 (stdin)
//     lea buff(%rip),%rsi   # buffer address
//     mov $30,%rdx       # count = 30
//     syscall            # invoke kernel, returns count in %rax
//
//     # write(1, buff, charsRead)
//     mov %rax,%rdx      # count = charsRead (from read's return)
//     mov $1,%rax        # syscall 1 = write
//     mov $1,%rdi        # fd = 1 (stdout)
//     lea buff(%rip),%rsi   # same buffer
//     syscall
//
//     # exit(0)
//     mov $60,%rax
//     mov $0,%rdi
//     syscall

// TO COMPILE AND RUN:
// gcc readWrite2.c -o readWrite2
// ./readWrite2
//
// Try typing various inputs:
// $ ./readWrite2
// Hello!          ← type this
// Hello!          ← program echoes
// $ ./readWrite2
// Testing 123     ← type this
// Testing 123     ← program echoes

// POTENTIAL ENHANCEMENTS:
//
// 1. Error checking:
//    if (charsRead < 0) {
//        perror("read failed");
//        return 1;
//    }
//
// 2. Loop to keep reading:
//    while ((charsRead = read(0, buff, 30)) > 0) {
//        write(1, buff, charsRead);
//    }
//
// 3. Handle partial writes:
//    ssize_t written = write(1, buff, charsRead);
//    if (written < charsRead) {
//        // Handle error or retry
//    }
