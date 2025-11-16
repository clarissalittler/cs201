// PEDAGOGICAL PURPOSE:
// This program demonstrates basic I/O operations in x86-64 assembly language.
// It shows how to compose two separate functions (readInt and writeInt) to
// create a simple echo program that reads an integer from stdin and writes it back.
//
// Key learning objectives:
// 1. Understanding how to call external functions in assembly
// 2. Learning the calling convention for x86-64 (return values in %rax)
// 3. Understanding parameter passing (%rdi is first parameter)
// 4. Seeing how data flows between function calls
// 5. Proper program termination using exit syscall
// 6. Composition of modular assembly functions

.section .text
        .extern readInt,writeInt    # Declare external functions we'll use
                                    # .extern tells the assembler these symbols
                                    # are defined in other object files
                                    # The linker will resolve these references

        .global _start              # Make _start visible to the linker
                                    # This is the entry point for the program

_start:
        # STEP 1: Read an integer from standard input
        call readInt                # Call the readInt function
                                    # readInt reads from stdin and converts ASCII to integer
                                    # CALLING CONVENTION: Return value is placed in %rax
                                    # After this call, %rax contains the integer that was read

        # STEP 2: Prepare the integer for output
        ## the integer read from stdin
        ## is in %rax
        ## we put that in %rdi
        ## and call writeInt to print
        mov %rax,%rdi               # Move the integer from %rax to %rdi
                                    # WHY? x86-64 calling convention:
                                    # - %rdi holds the FIRST parameter to a function
                                    # - writeInt expects its parameter (the int to print) in %rdi
                                    # This is part of the System V AMD64 ABI calling convention

        # STEP 3: Write the integer to standard output
        call writeInt               # Call writeInt to print the integer
                                    # writeInt converts the integer to ASCII and writes to stdout
                                    # The value to print is taken from %rdi

        # STEP 4: Exit the program cleanly
        mov $0,%rdi                 # Set exit code to 0 (success)
                                    # %rdi holds the first parameter to the syscall
                                    # For exit, this is the status code returned to the OS

        mov $60,%rax                # Syscall number 60 is exit
                                    # %rax always holds the syscall number
                                    # Common syscalls:
                                    #   0 = read, 1 = write, 60 = exit

        syscall                     # Invoke the kernel to exit the program
                                    # This transfers control to the OS
                                    # The program terminates here

// EXECUTION FLOW:
//
// 1. Program starts at _start
// 2. readInt is called
//    - Reads bytes from stdin (syscall)
//    - Converts ASCII digits to binary integer
//    - Returns integer in %rax
// 3. Integer is moved from %rax to %rdi
// 4. writeInt is called
//    - Converts binary integer to ASCII digits
//    - Writes bytes to stdout (syscall)
// 5. Program exits with status 0

// DATA FLOW EXAMPLE:
// If user types "123<enter>":
//
// readInt:
//   - Reads bytes: '1', '2', '3', '\n'
//   - Converts to integer: 123 (binary: 0x7B)
//   - Returns 123 in %rax
//
// mov %rax,%rdi:
//   - Copies 123 from %rax to %rdi
//
// writeInt:
//   - Takes 123 from %rdi
//   - Converts to ASCII: '1', '2', '3'
//   - Writes to stdout
//   - User sees: 123

// IMPORTANT CONCEPTS:
//
// 1. CALLING CONVENTION:
//    - Functions receive parameters in: %rdi, %rsi, %rdx, %rcx, %r8, %r9
//    - Functions return values in: %rax
//    - This is the System V AMD64 ABI standard
//
// 2. FUNCTION COMPOSITION:
//    - Output of readInt (in %rax) becomes input of writeInt (via %rdi)
//    - This shows modular programming in assembly
//
// 3. EXTERNAL SYMBOLS:
//    - .extern declares symbols defined elsewhere
//    - The linker resolves these at link time
//    - Allows separate compilation of modules
//
// 4. ENTRY POINT:
//    - _start is the default entry point for programs
//    - The OS loader jumps here when program begins
//    - Unlike C, there's no implicit return from _start - we must explicitly exit

// TRY IT:
// Compile and link:
//   as echoInt.s -o echoInt.o
//   as readInt.s -o readInt.o
//   as writeInt.s -o writeInt.o
//   ld echoInt.o readInt.o writeInt.o -o echoInt
//
// Run:
//   ./echoInt
//   123         (type this)
//   123         (program echoes it back)
//
// Test with different values:
//   ./echoInt
//   -42         (negative numbers work)
//   -42
