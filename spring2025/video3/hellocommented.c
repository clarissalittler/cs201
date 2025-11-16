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

#include <stdio.h>      // PREPROCESSOR DIRECTIVE: Include standard input/output library
                        // This provides the printf() function
                        // Without this, the compiler wouldn't know what printf is
                        // stdio.h contains declarations for I/O functions like printf, scanf, etc.

int main(){             // MAIN FUNCTION: The entry point of every C program
                        // When you run the program, execution starts here
                        // "int" means this function returns an integer to the OS
                        // By convention, 0 means success, non-zero means error
                        // The empty parentheses () mean no command-line arguments are expected
                        // Could also write: int main(int argc, char *argv[]) for arguments

  printf("Hello, world!\n");  // PRINTF: Print formatted output to stdout (the terminal)
                              // "Hello, world!" is a string literal
                              // String literals are stored in read-only memory
                              // \n is an ESCAPE SEQUENCE that means newline
                              // This moves the cursor to the next line after printing
                              // Without \n, the shell prompt would appear on the same line
                              // Note the comma after "world" - classic K&R style

  return 0;             // RETURN STATEMENT: Exit the program and report success
                        // The value 0 is returned to the operating system
                        // The OS uses this to determine if the program succeeded
                        // You can check this in the shell with: echo $?
                        // This is the exit code/status of the program
                        // Non-zero values (1, 2, etc.) indicate different types of errors
}

// TO COMPILE AND RUN:
// gcc hello.c -o hello     # Compile to create executable named "hello"
// ./hello                  # Run the program
// echo $?                  # Check the exit status (should be 0)

// WHAT HAPPENS WHEN THIS RUNS:
// 1. OS loads the program into memory
// 2. C runtime calls main()
// 3. printf writes "Hello, world!\n" to stdout (file descriptor 1)
// 4. main() returns 0
// 5. C runtime calls exit(0)
// 6. Program terminates with exit status 0

// WHY THIS PROGRAM IS IMPORTANT:
// - It's the traditional first program in any language
// - Demonstrates the minimum viable C program structure
// - Shows the relationship between source code and execution
// - Introduces fundamental concepts: functions, libraries, return values
// - Serves as a template for more complex programs

// COMPILING TO ASSEMBLY:
// You can see the assembly code that the compiler generates:
// gcc -S hello.c           # Creates hello.s (AT&T syntax)
// gcc -S -masm=intel hello.c  # Creates hello.s (Intel syntax)
// This shows you exactly what assembly instructions the C code becomes!

// WHAT THE COMPILER DOES:
// 1. Preprocessing: Expands #include directives
// 2. Compilation: Translates C to assembly
// 3. Assembly: Converts assembly to machine code (object file)
// 4. Linking: Links with C library to create executable
//
// You can see each stage:
// gcc -E hello.c           # Preprocessor output
// gcc -S hello.c           # Assembly output
// gcc -c hello.c           # Object file (hello.o)
// gcc hello.c -o hello     # Final executable

// COMPARING C TO ASSEMBLY:
// This simple C program becomes ~30+ lines of assembly!
// The compiler handles:
// - Function prologue/epilogue (stack frame setup)
// - Calling printf (via PLT for dynamic linking)
// - String literal storage in .rodata section
// - Proper argument passing (putting string address in %rdi)
// - Return value handling
// - Security features (stack canaries, etc.)
//
// See hello.s or hellocommented.s for the assembly version!
