// PEDAGOGICAL PURPOSE:
// This is the classic "Hello World" program - the traditional first program
// when learning any programming language. It demonstrates the absolute minimum
// structure needed for a working C program.
// Key learning objectives:
// 1. Understanding the basic structure of a C program
// 2. The role of #include directives and header files
// 3. The main() function as the program entry point
// 4. Using printf() for output
// 5. Return values and exit codes
// 6. String literals and escape sequences

#include <stdio.h>      // HEADER FILE: Includes standard input/output functions
                        // Provides declaration for printf()
                        // Without this, compiler wouldn't know what printf is
                        // stdio.h is part of the C standard library

int main(){             // MAIN FUNCTION: Entry point of every C program
                        // Execution always begins here
                        // "int" means this function returns an integer
                        // Empty () means no command-line arguments (could be int argc, char* argv[])

  // PRINTF: Print formatted output to stdout (standard output = terminal)
  // "Hello, world!" is a string literal
  // String literals are stored in read-only memory
  // \n is an escape sequence representing a newline character
  // The newline moves the cursor to the next line after printing
  printf("Hello, world!\n");

  // RETURN VALUE: Exit code sent to the operating system
  // 0 conventionally means "success" or "no error"
  // Non-zero values indicate various error conditions
  // The OS and shell can check this value
  // In bash: echo $? shows the exit code of the last program
  return 0;
}

// COMPREHENSIVE EXPLANATION:
//
// WHAT HAPPENS WHEN YOU RUN THIS PROGRAM:
// 1. The operating system loads the executable into memory
// 2. The OS calls the main() function
// 3. printf() is called, which:
//    - Takes the string "Hello, world!\n"
//    - Writes it to file descriptor 1 (stdout)
//    - stdout is typically connected to your terminal
// 4. main() returns 0
// 5. The program exits with status code 0
// 6. Control returns to the shell

// THE #include DIRECTIVE:
//
// #include <stdio.h> is a preprocessor directive
// It runs BEFORE compilation
//
// What it does:
// - Finds the file stdio.h (in system include directories)
// - Copies its entire contents into this file
// - This happens textually, as if you copy-pasted the header
//
// What stdio.h contains:
// - Function declarations (prototypes) like: int printf(const char* format, ...);
// - Type definitions
// - Macro definitions
// - NOT the actual implementation of printf (that's in libc)
//
// Angle brackets vs quotes:
// - <stdio.h> searches system directories (/usr/include, etc.)
// - "myfile.h" searches current directory first, then system directories

// THE MAIN FUNCTION:
//
// main() is special:
// - It's the entry point (where execution begins)
// - It must exist in every C program
// - The OS calls it when your program starts
//
// Valid signatures:
// 1. int main()                          // No arguments
// 2. int main(void)                      // Explicitly no arguments
// 3. int main(int argc, char* argv[])    // Command-line arguments
// 4. int main(int argc, char** argv)     // Equivalent to #3
//
// Return type must be int (represents exit status)
//
// If you omit "return 0;", C99 and later automatically return 0 from main()
// But it's good practice to include it explicitly

// STRING LITERALS:
//
// "Hello, world!\n" is a string literal
//
// Properties:
// - Stored in read-only memory section (usually .rodata)
// - Type is "array of char" that decays to char*
// - Automatically null-terminated (ends with '\0')
// - Attempting to modify causes undefined behavior (often crashes)
//
// Memory layout of "Hello, world!\n":
//   'H' 'e' 'l' 'l' 'o' ',' ' ' 'w' 'o' 'r' 'l' 'd' '!' '\n' '\0'
//    ↑                                                        ↑
//    Start of string                                    Null terminator
//
// Size: 15 bytes (14 visible characters + 1 null terminator)

// ESCAPE SEQUENCES:
//
// \n is an escape sequence - a special character represented by backslash + letter
//
// Common escape sequences:
// \n  - newline (LF, ASCII 10)
// \t  - tab
// \r  - carriage return (CR, ASCII 13)
// \\  - backslash itself
// \"  - double quote
// \'  - single quote
// \0  - null character (string terminator)
// \a  - alert (bell)
// \b  - backspace
// \xHH - character with hex value HH (e.g., \x41 is 'A')
//
// Why \n matters:
// Without it: "Hello, world!" is printed and cursor stays on same line
//             Next shell prompt appears right after: Hello, world!$
// With it:    "Hello, world!\n" prints and moves to next line
//             Shell prompt appears on its own line

// THE PRINTF FUNCTION:
//
// printf is a variadic function (takes variable number of arguments)
// It's part of the C standard library
//
// Basic usage:
//   printf("format string", arg1, arg2, ...);
//
// Format specifiers (used in later programs):
//   %d - signed decimal integer
//   %u - unsigned decimal integer
//   %f - floating point
//   %c - character
//   %s - string
//   %p - pointer address
//   %x - hexadecimal
//
// In this program, there are no format specifiers
// Just a plain string to print

// RETURN VALUE AND EXIT CODES:
//
// "return 0;" sets the exit status of the program
//
// Exit code conventions:
//   0   - Success (by convention)
//   1   - Generic error
//   2   - Misuse of command
//   126 - Command cannot execute
//   127 - Command not found
//   128+N - Program terminated by signal N
//
// The shell can access this:
//   $ ./hello
//   Hello, world!
//   $ echo $?
//   0
//
// You could return different values:
//   return 1;   # Indicates error
//   return 42;  # Custom error code
//
// Scripts can check exit codes:
//   if ./hello; then
//     echo "Success!"
//   else
//     echo "Failed!"
//   fi

// COMPILATION PROCESS:
//
// To turn hello.c into an executable:
//
// 1. PREPROCESSING:
//    gcc -E hello.c
//    - Processes #include, #define, etc.
//    - Outputs expanded source with stdio.h contents
//
// 2. COMPILATION:
//    gcc -S hello.c
//    - Translates C code to assembly language
//    - Outputs hello.s
//
// 3. ASSEMBLY:
//    gcc -c hello.c
//    - Translates assembly to machine code
//    - Outputs hello.o (object file)
//
// 4. LINKING:
//    gcc hello.o -o hello
//    - Combines object file with C standard library
//    - Resolves printf symbol to libc implementation
//    - Outputs executable "hello"
//
// Or all at once:
//    gcc hello.c -o hello

// COMPARISON TO OTHER LANGUAGES:
//
// Python:
//   print("Hello, world!")
//   # No includes, no main, no types, no return
//
// Java:
//   public class Hello {
//     public static void main(String[] args) {
//       System.out.println("Hello, world!");
//     }
//   }
//   # More verbose, needs class structure
//
// C++:
//   #include <iostream>
//   int main() {
//     std::cout << "Hello, world!" << std::endl;
//     return 0;
//   }
//   # Similar to C, but uses streams instead of printf

// WHY THIS PROGRAM MATTERS:
//
// Despite its simplicity, this demonstrates:
// 1. How to structure any C program
// 2. The preprocessor (#include)
// 3. The compilation model (separate compilation and linking)
// 4. The runtime model (OS calls main, program returns to OS)
// 5. Basic I/O (printf to terminal)
// 6. The minimalism of C (no built-in I/O, everything is a library)

// TO COMPILE AND RUN:
// $ gcc hello.c -o hello
// $ ./hello
// Hello, world!
//
// TO CHECK EXIT CODE:
// $ ./hello
// Hello, world!
// $ echo $?
// 0
//
// TO SEE PREPROCESSED OUTPUT:
// $ gcc -E hello.c | less
// (You'll see the entire stdio.h contents!)
//
// TO SEE ASSEMBLY OUTPUT:
// $ gcc -S hello.c
// $ cat hello.s
// (You'll see the assembly code generated)
