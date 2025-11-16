// PEDAGOGICAL PURPOSE:
// This is a minimal "Hello World" variant that demonstrates the basic structure
// of a C program. It's used as a source file for studying compiler-generated assembly.
// Key learning objectives:
// 1. The minimal structure needed for a working C program
// 2. Understanding main() as the entry point
// 3. Using printf() for output
// 4. Return values and exit codes
// 5. How this simple C code translates to assembly (see hello.s)
// 6. The compilation process from C to machine code

#include <stdio.h>      // PREPROCESSOR DIRECTIVE: Include standard I/O library
                        // Provides the declaration for printf()
                        // Without this, compiler wouldn't know about printf

int main(){             // MAIN FUNCTION: Program entry point
                        // The operating system calls this function when program starts
                        // "int" means it returns an integer (exit code) to the OS

  // PRINTF CALL:
  // Prints "Hello CS201!" followed by a newline to standard output
  // The compiler will optimize this to use puts() instead of printf()
  // because there are no format specifiers
  printf("Hello CS201!\n");

  // RETURN STATEMENT:
  // Returns 0 to the operating system
  // 0 conventionally means "success" or "no errors"
  // Non-zero values indicate error conditions
  return 0;
}

// COMPREHENSIVE EXPLANATION:
//
// RELATIONSHIP TO ASSEMBLY CODE (hello.s):
//
// This simple C program compiles to assembly code (hello.s in the same directory)
// The assembly version shows:
// 1. How the compiler sets up the stack frame for main()
// 2. How string literals are stored in the .rodata (read-only data) section
// 3. How printf is called (or optimized to puts)
// 4. How the return value is placed in register %eax
// 5. How the function epilogue restores the stack and returns
//
// Comparing this .c file with hello.s reveals the compilation process

// THE COMPILATION PROCESS:
//
// C source → Preprocessor → Compiler → Assembler → Linker → Executable
//
// 1. PREPROCESSING (gcc -E hello.c):
//    - Processes #include directives
//    - Expands macros
//    - Removes comments
//    - Output: expanded C source
//
// 2. COMPILATION (gcc -S hello.c):
//    - Translates C code to assembly language
//    - Output: hello.s (assembly source)
//    - This is what you see in hello.s
//
// 3. ASSEMBLY (gcc -c hello.c):
//    - Translates assembly to machine code
//    - Output: hello.o (object file, binary)
//    - Contains machine instructions, not human-readable
//
// 4. LINKING (gcc hello.o -o hello):
//    - Combines object files
//    - Resolves external symbols (like printf)
//    - Links with C standard library
//    - Output: hello (executable)

// WHY "Hello CS201!" INSTEAD OF "Hello, world!":
//
// This is a course-specific variant
// CS201 is likely the course code
// The message doesn't matter functionally
// What matters is understanding the underlying mechanics

// COMPILER OPTIMIZATIONS:
//
// The compiler typically optimizes printf("Hello CS201!\n") to puts("Hello CS201!")
// Why?
// - puts is simpler (no format string parsing)
// - puts is faster for plain strings
// - puts automatically adds newline (printf's \n becomes redundant, so removed)
//
// You can see this optimization in hello.s:
//   call puts@PLT
// instead of:
//   call printf@PLT
//
// To disable optimization:
//   gcc -O0 hello.c -o hello

// INTEL vs AT&T SYNTAX:
//
// The hello.s file uses Intel syntax (.intel_syntax noprefix)
// This is specified by a directive at the top of hello.s
//
// Intel syntax:
//   mov rax, rbx      (destination first)
//
// AT&T syntax (default):
//   movq %rbx, %rax   (source first, % prefix on registers)
//
// Intel syntax is generally considered more readable for beginners

// KEY ASSEMBLY CONCEPTS (from hello.s):
//
// 1. STACK FRAME:
//    Every function has a stack frame
//    Stores local variables, saved registers, return address
//
// 2. BASE POINTER (%rbp):
//    Points to the base of current function's stack frame
//    Provides stable reference for accessing local variables
//
// 3. STACK POINTER (%rsp):
//    Points to top of stack
//    Moves as values are pushed/popped
//
// 4. CALLING CONVENTION:
//    Arguments passed in registers: %rdi, %rsi, %rdx, %rcx, %r8, %r9
//    Return value in %rax (or %eax for 32-bit)
//    Caller-saved vs callee-saved registers
//
// 5. PLT (Procedure Linkage Table):
//    Used for dynamic linking
//    puts@PLT resolves the actual address of puts at runtime

// WHAT HAPPENS AT RUNTIME:
//
// 1. Operating system loads executable into memory
// 2. OS sets up initial stack and registers
// 3. OS calls _start (C runtime initialization)
// 4. C runtime calls main()
// 5. main() executes:
//    a. Sets up stack frame (push %rbp, mov %rbp, %rsp)
//    b. Loads address of string into %rdi (first argument to puts)
//    c. Calls puts@PLT
//    d. puts() writes "Hello CS201!\n" to stdout
//    e. Sets %eax to 0 (return value)
//    f. Restores stack frame (pop %rbp)
//    g. Returns (ret instruction)
// 6. C runtime receives return value
// 7. C runtime calls exit() with that value
// 8. OS reclaims resources and terminates process

// DIFFERENCES FROM lecture1/hello.c:
//
// This is almost identical to lecture1/hello.c
// Main difference: "Hello CS201!" vs "Hello, world!"
// Both serve the same pedagogical purpose
// This version (lecture3) is paired with assembly code (hello.s)
// Lecture3 focuses on assembly language translation

// TO COMPILE AND RUN:
// gcc hello.c -o hello
// ./hello
// Output: Hello CS201!
//
// TO GENERATE ASSEMBLY:
// gcc -S hello.c
// This creates hello.s (assembly language version)
//
// TO GENERATE ASSEMBLY WITH INTEL SYNTAX:
// gcc -S -masm=intel hello.c
// This matches the syntax in the provided hello.s
//
// TO SEE PREPROCESSOR OUTPUT:
// gcc -E hello.c
// Shows the expanded source after #include processing
//
// TO COMPILE WITHOUT OPTIMIZATION:
// gcc -O0 hello.c -o hello
// Generates less optimized but more readable assembly
//
// TO COMPILE WITH OPTIMIZATION:
// gcc -O2 hello.c -o hello
// Generates highly optimized code (may inline, eliminate code, etc.)

// STUDYING THE ASSEMBLY:
//
// After compiling this file, examine hello.s to see:
// 1. How the string literal is stored
// 2. How the function prologue/epilogue work
// 3. How function calls are made
// 4. How return values are passed
// 5. Security features (endbr64, stack canaries, etc.)
// 6. Position-independent code (PIC) for ASLR
// 7. Metadata sections (.note.GNU-stack, etc.)
