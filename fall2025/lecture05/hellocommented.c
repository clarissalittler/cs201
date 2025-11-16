// PEDAGOGICAL PURPOSE:
// This is a minimal "Hello World" program in C that demonstrates
// the absolute basics of C programming structure.
// Key learning objectives:
// 1. Understanding the basic structure of a C program
// 2. Introduction to the stdio.h library and printf function
// 3. Understanding the main() function as the entry point
// 4. Return values and program exit codes
// 5. String literals and escape sequences (\n for newline)
// 6. The minimal elements needed for a working C program

#include "stdio.h"      // PREPROCESSOR DIRECTIVE: Include standard input/output library
                        // This provides the printf() function
                        // Without this, the compiler wouldn't know what printf is
                        // The quotes (not angle brackets) mean look in current directory first

int main(){             // MAIN FUNCTION: The entry point of every C program
                        // When you run the program, execution starts here
                        // "int" means this function returns an integer to the OS
                        // By convention, 0 means success, non-zero means error

  printf("Hello world\n");  // PRINTF: Print formatted output to stdout (the terminal)
                            // "Hello world" is a string literal
                            // \n is an ESCAPE SEQUENCE that means newline
                            // This moves the cursor to the next line after printing
                            // Without \n, the shell prompt would appear on the same line

  return 0;             // RETURN STATEMENT: Exit the program and report success
                        // The value 0 is returned to the operating system
                        // The OS uses this to determine if the program succeeded
                        // You can check this in the shell with: echo $?
                        // This is the exit code/status of the program
}

// TO COMPILE AND RUN:
// gcc hello.c -o hello     # Compile to create executable named "hello"
// ./hello                  # Run the program
// echo $?                  # Check the exit status (should be 0)

// WHAT HAPPENS WHEN THIS RUNS:
// 1. OS loads the program into memory
// 2. OS calls main()
// 3. printf writes "Hello world\n" to stdout
// 4. main() returns 0
// 5. Program exits with status 0
