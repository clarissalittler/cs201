// PEDAGOGICAL PURPOSE:
// This program demonstrates basic integer I/O operations in x86-64 assembly language.
// It shows how to compose external functions (readInt and writeInt) to create a simple
// echo program that reads an integer from stdin and writes it back to stdout, along with
// user-friendly prompts using Linux syscalls.
//
// Key learning objectives:
// 1. Understanding how to call external functions in assembly (.extern directive)
// 2. Learning the x86-64 calling convention (return values in %rax, params in %rdi, %rsi, etc.)
// 3. Understanding parameter passing for both syscalls and function calls
// 4. Seeing how data flows between function calls via registers
// 5. Learning string output with write syscall
// 6. Understanding the .rodata section for read-only string literals
// 7. Computing string lengths at assembly time using the . (current location) operator
// 8. Understanding register preservation (push/pop) when calling functions
// 9. Proper program termination using exit syscall
// 10. Understanding RIP-relative addressing for position-independent code

        .section .rodata            # READ-ONLY DATA SECTION
                                   # .rodata = Read-Only Data
                                   # This section contains constant data (strings, constants)
                                   # Cannot be modified at runtime (page is marked read-only)
                                   # WHY .rodata? Prevents accidental modification, allows sharing
                                   # Multiple processes can share the same .rodata pages

msg1:   .ascii "Enter a number: "  # STRING LITERAL: First prompt message
                                   # .ascii creates a byte array from the string
                                   # NOTE: .ascii does NOT add null terminator
                                   # For syscalls, we don't need null terminator
                                   # msg1 is a label pointing to the first byte

msg1len = . - msg1                 # COMPUTE STRING LENGTH at assembly time
                                   # . = current location counter (address)
                                   # msg1 = address of start of string
                                   # . - msg1 = length of string in bytes
                                   # This is calculated by the assembler (not at runtime)
                                   # Result: msg1len = 17 (bytes)

###  The difference between . (which signifies our current location in the program, as an address) - msg (the address of the start of the data) is the length of the message in bytes

msg2:   .ascii "Here's what you entered: "  # STRING LITERAL: Second message
                                            # Another string for output
msg2len = . - msg2                         # COMPUTE LENGTH: 25 bytes

msg3:   .ascii "\nGoodbye!\n"    # STRING LITERAL: Final message
                                 # \n = newline character (ASCII 10)
                                 # First \n moves to new line
                                 # Second \n adds blank line after
msg3len = . - msg3               # COMPUTE LENGTH: 11 bytes


        .section .text              # CODE SECTION: Executable instructions
        .global _start              # GLOBAL ENTRY POINT: Make _start visible to linker
                                   # The OS loader will jump here to start the program

        .extern readInt             # EXTERNAL FUNCTION DECLARATION
        .extern writeInt            # Tell assembler these symbols are defined elsewhere
                                   # The linker will resolve these references
                                   # readInt and writeInt are in separate .s files

_start:
        ## STEP 1: PRINT "Enter a number: " PROMPT
        ## We use the write syscall to output a string to stdout

        mov $1,%rax                 # SYSCALL NUMBER: 1 = write
                                   # write(fd, buffer, count)
                                   # Linux syscall numbers are in %rax

        mov $0,%rdi                 # FILE DESCRIPTOR: 0 (SHOULD BE 1 for stdout)
                                   # NOTE: This appears to be a bug - should be stdout (1)
                                   # 0 = stdin, 1 = stdout, 2 = stderr
                                   # CORRECT: mov $1,%rdi

        lea msg1(%rip),%rsi         # BUFFER ADDRESS: Load address of msg1 into %rsi
                                   # lea = Load Effective Address
                                   # msg1(%rip) = RIP-relative addressing
                                   # WHY RIP-relative? Position-independent code
                                   # Works regardless of where program is loaded in memory
                                   # %rsi holds the pointer to the string

        mov $msg1len,%rdx           # BYTE COUNT: Number of bytes to write (17)
                                   # msg1len was computed by assembler
                                   # %rdx tells syscall how many bytes to output

        syscall                     # INVOKE WRITE: Execute write(0, msg1, 17)
                                   # Control transfers to kernel
                                   # Kernel outputs the string to file descriptor 0
                                   # User sees: "Enter a number: "

        ## STEP 2: READ INTEGER FROM USER
        ## Call external readInt function
        call readInt                # FUNCTION CALL: Call the readInt function
                                   # readInt reads bytes from stdin
                                   # Parses ASCII digits into binary integer
                                   # CALLING CONVENTION: Return value is in %rax
                                   # After this call, %rax contains the integer
                                   # Example: User types "123" -> %rax = 123 (0x7B)

        ## now %rax holds the integer we read in
        ## why %rax? Because that's generally what
        ## things are returned into (calling conv)

        ## STEP 3: SAVE THE INTEGER
        ## We need to preserve %rax because the next syscall will overwrite it
        push %rax                   # SAVE %rax: Push integer onto stack
                                   # whoops better save rax quick
                                   # Stack grows downward: %rsp -= 8
                                   # The integer is now saved at (%rsp)
                                   # WHY? The write syscall will modify %rax
                                   # We need the integer for writeInt later

        ## STEP 4: PRINT "Here's what you entered: " MESSAGE
        mov $1,%rax                 # SYSCALL NUMBER: 1 = write
        mov $0,%rdi                 # FILE DESCRIPTOR: 0 (should be 1)
        lea msg2(%rip),%rsi         # BUFFER ADDRESS: Load address of msg2
        mov $msg2len,%rdx           # BYTE COUNT: 25 bytes
        syscall                     # INVOKE WRITE: Output message

        ## STEP 5: RESTORE AND PRINT THE INTEGER
        pop %rax                    # RESTORE %rax: Pop integer from stack
                                   # we got it back!!!
                                   # Stack pointer moves up: %rsp += 8
                                   # %rax now contains the original integer again

        mov %rax,%rdi               # PREPARE PARAMETER: Move integer to %rdi
                                   # setup the writeInt call
                                   # WHY %rdi? x86-64 calling convention:
                                   # First parameter goes in %rdi
                                   # writeInt expects the integer in %rdi
                                   # This is System V AMD64 ABI standard

        call writeInt               # FUNCTION CALL: Call writeInt
                                   # writeInt converts integer to ASCII digits
                                   # Writes the digits to stdout
                                   # Example: 123 -> outputs "123"

        ## STEP 6: PRINT "Goodbye!" MESSAGE
        mov $1,%rax                 # SYSCALL NUMBER: 1 = write
        mov $0,%rdi                 # FILE DESCRIPTOR: 0 (should be 1)
        lea msg3(%rip),%rsi         # BUFFER ADDRESS: Load address of msg3
        mov $msg3len,%rdx           # BYTE COUNT: 11 bytes
        syscall                     # INVOKE WRITE: Output final message

        ## STEP 7: EXIT THE PROGRAM
        mov $60,%rax                # SYSCALL NUMBER: 60 = exit
                                   # Prepare to terminate the program

        xor %rdi,%rdi               # EXIT CODE: Set %rdi to 0 (success)
                                   # fast way of making 0 be in rdi
                                   # xor %rdi,%rdi is faster/shorter than mov $0,%rdi
                                   # Any value XOR itself = 0
                                   # %rdi holds the exit status returned to OS

        syscall                     # INVOKE EXIT: Execute exit(0)
                                   # Program terminates
                                   # Exit code 0 indicates success

// EXECUTION FLOW:
//
// 1. Program starts at _start
// 2. Prints "Enter a number: " (write syscall)
// 3. Calls readInt
//    - readInt uses read syscall to get input from stdin
//    - Parses ASCII digits into binary integer
//    - Returns integer in %rax
// 4. Saves integer on stack (push)
// 5. Prints "Here's what you entered: " (write syscall)
// 6. Restores integer from stack (pop)
// 7. Moves integer to %rdi for writeInt
// 8. Calls writeInt
//    - writeInt converts integer to ASCII digits
//    - Uses write syscall to output to stdout
// 9. Prints "\nGoodbye!\n" (write syscall)
// 10. Exits with status 0 (exit syscall)

// DATA FLOW EXAMPLE:
// If user types "42<enter>":
//
// Initial prompt:
//   write(0, "Enter a number: ", 17)
//   Output: "Enter a number: "
//
// readInt:
//   read(0, buffer, 128) reads "42\n"
//   Parses: '4' -> 4, multiply by 10 -> 40, '2' -> 2, add -> 42
//   Returns: %rax = 42 (0x2A)
//
// push %rax:
//   Stack: [42]
//   %rsp points to 42
//
// Second message:
//   write(0, "Here's what you entered: ", 25)
//   Output: "Here's what you entered: "
//
// pop %rax:
//   %rax = 42 again
//   Stack: []
//
// mov %rax,%rdi:
//   %rdi = 42
//
// writeInt:
//   Converts 42 to "42"
//   42 / 10 = 4 remainder 2 -> '2'
//   4 / 10 = 0 remainder 4 -> '4'
//   Builds string: "42"
//   write(1, "42", 2)
//   Output: "42"
//
// Final message:
//   write(0, "\nGoodbye!\n", 11)
//   Output: blank line, "Goodbye!", blank line
//
// exit(0):
//   Program terminates

// IMPORTANT CONCEPTS:
//
// 1. CALLING CONVENTION (System V AMD64 ABI):
//    - Integer/pointer parameters: %rdi, %rsi, %rdx, %rcx, %r8, %r9
//    - Return value: %rax
//    - Caller-saved registers: %rax, %rcx, %rdx, %rsi, %rdi, %r8-r11
//      (These can be modified by called function)
//    - Callee-saved registers: %rbx, %rbp, %r12-r15
//      (Called function must preserve these)
//
// 2. SYSCALL CONVENTION:
//    - Syscall number: %rax
//    - Parameters: %rdi, %rsi, %rdx, %r10, %r8, %r9
//    - Return value: %rax
//    - Syscalls can modify all caller-saved registers
//
// 3. EXTERNAL FUNCTIONS:
//    - .extern declares symbols defined in other files
//    - Linker resolves these at link time
//    - Allows modular programming (separate compilation)
//    - Each module can be compiled independently
//
// 4. .RODATA SECTION:
//    - Contains read-only constant data
//    - Cannot be modified (page protection)
//    - String literals go here
//    - More efficient than .data (can be shared between processes)
//
// 5. POSITION-INDEPENDENT CODE (PIC):
//    - RIP-relative addressing: lea msg(%rip),%rsi
//    - Works regardless of load address
//    - Required for shared libraries
//    - Modern default for security (ASLR - Address Space Layout Randomization)
//
// 6. STACK FOR TEMPORARY STORAGE:
//    - Stack grows downward (toward lower addresses)
//    - push decrements %rsp by 8, stores value
//    - pop loads value, increments %rsp by 8
//    - Used here to preserve %rax across syscall
//    - Must balance pushes and pops!
//
// 7. ASSEMBLY-TIME COMPUTATION:
//    - msg1len = . - msg1 computed by assembler
//    - No runtime overhead
//    - . (dot) is the location counter
//    - Difference gives string length

// WHY PUSH/POP THE INTEGER?
//
// Without push/pop:
//   call readInt              # %rax = 42
//   mov $1,%rax               # %rax = 1 (lost the integer!)
//   syscall                   # write syscall
//   # Now we've lost the integer we read!
//
// With push/pop:
//   call readInt              # %rax = 42
//   push %rax                 # Save 42 on stack
//   mov $1,%rax               # %rax = 1 (OK, integer is saved)
//   syscall                   # write syscall
//   pop %rax                  # %rax = 42 (restored!)
//   mov %rax,%rdi             # %rdi = 42
//   call writeInt             # Can now print it!
//
// The syscall overwrites %rax with return value (bytes written)
// Without saving, we'd lose the integer we read

// FILE DESCRIPTOR BUG:
//
// This code uses fd 0 for write syscalls:
//   mov $0,%rdi
//   syscall
//
// Correct usage:
//   read uses fd 0 (stdin)
//   write uses fd 1 (stdout)
//
// Should be:
//   mov $1,%rdi  # for write syscalls
//
// Why does it work anyway?
// - In some environments, stdin is bidirectional
// - Or the code might be used in special contexts
// - But it's not standard/portable

// EQUIVALENT C CODE:
//
// #include <unistd.h>
//
// extern long readInt();
// extern void writeInt(long n);
//
// int main() {
//     write(1, "Enter a number: ", 17);
//     long n = readInt();
//     write(1, "Here's what you entered: ", 25);
//     writeInt(n);
//     write(1, "\nGoodbye!\n", 11);
//     return 0;
// }

// TRY IT:
//
// Compile and link:
//   as echoInt.s -o echoInt.o
//   as readInt.s -o readInt.o
//   as writeInt.s -o writeInt.o
//   ld echoInt.o readInt.o writeInt.o -o echoInt
//
// Run:
//   ./echoInt
//   Enter a number: 123         (type this)
//   Here's what you entered: 123
//   Goodbye!
//
// Test with different values:
//   ./echoInt
//   Enter a number: -456        (negative numbers)
//   Here's what you entered: -456
//   Goodbye!
//
//   ./echoInt
//   Enter a number: 0           (zero)
//   Here's what you entered: 0
//   Goodbye!

// EXTENDING THIS CODE:
//
// 1. Add input validation:
//    - Check if readInt returns error
//    - Validate range
//    - Handle non-numeric input
//
// 2. Read multiple integers:
//    - Loop to read N integers
//    - Store in array
//    - Process and output
//
// 3. Add arithmetic:
//    - Read two numbers
//    - Add/subtract/multiply/divide
//    - Output result
//
// 4. Format output:
//    - Add thousands separators
//    - Pad with zeros
//    - Right/left align
