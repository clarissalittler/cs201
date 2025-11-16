// PEDAGOGICAL PURPOSE:
// This program demonstrates function abstraction and code reuse in assembly.
// It's an enhanced version of echoInt that factors out the repeated write syscall
// pattern into a reusable printStr function. This shows how to build helper functions
// to eliminate code duplication and improve maintainability.
//
// Key learning objectives:
// 1. Understanding function decomposition and abstraction in assembly
// 2. Learning how to create helper functions with multiple parameters
// 3. Understanding register usage for parameter passing (first param in %rdi, second in %rsi)
// 4. Seeing how function calls can simplify repetitive code
// 5. Understanding the benefits of code reuse and modularity
// 6. Learning proper function conventions (parameters, return, register usage)
// 7. Understanding that helper functions can use syscalls internally
// 8. Comparing refactored code with original to see the improvements

        .section .rodata            # READ-ONLY DATA SECTION
                                   # Contains string constants

msg1:   .ascii "Enter a number: "  # STRING LITERAL: First prompt
msg1len = . - msg1                 # LENGTH: 17 bytes

###  The difference between . (which signifies our current location in the program, as an address) - msg (the address of the start of the data) is the length of the message in bytes

msg2:   .ascii "Here's what you entered: "  # STRING LITERAL: Second prompt
msg2len = . - msg2                          # LENGTH: 25 bytes

msg3:   .ascii "\nGoodbye!\n"    # STRING LITERAL: Final message
msg3len = . - msg3               # LENGTH: 11 bytes


        .section .text              # CODE SECTION
        .global _start              # Entry point

        .extern readInt             # External function declarations
        .extern writeInt            # These are defined in separate files


## HELPER FUNCTION: printStr
## ========================
## Function that takes the address of a string in %rdi and its length in %rsi,
## then prints it to stdout using the write syscall.
##
## This is an example of FUNCTION ABSTRACTION - we're wrapping the write syscall
## in a reusable function to avoid repeating the same code pattern.
##
## WHY CREATE THIS FUNCTION?
## - Original code repeated write syscall 3 times
## - Each time: setup %rax, %rdi, %rsi, %rdx, then syscall
## - That's 5 instructions × 3 = 15 instructions total
## - With printStr: 2 instructions (setup params) + call × 3 = 9 instructions
## - Less code, easier to read, easier to maintain
## - If we need to change output method, change in ONE place
##
## PARAMETERS:
##   %rdi - Address of string to print (first parameter)
##   %rsi - Length of string in bytes (second parameter)
##
## RETURNS:
##   Nothing (void function)
##
## REGISTERS MODIFIED:
##   %rax, %rdi, %rsi, %rdx (used by syscall)
##   All caller-saved registers may be modified
##
## CALLING CONVENTION:
##   Parameters are passed in registers (System V AMD64 ABI)
##   First parameter in %rdi, second in %rsi
##
printStr:
        # STEP 1: SETUP PARAMETERS FOR WRITE SYSCALL
        # We need to rearrange registers because:
        # - printStr receives: %rdi=address, %rsi=length
        # - write syscall needs: %rdi=fd, %rsi=address, %rdx=length
        # So we need to move things around

        mov %rsi,%rdx               # MOVE LENGTH: %rdx = length
                                   # Write syscall expects length in %rdx
                                   # We received it in %rsi
                                   # Save it to %rdx before overwriting %rsi

        mov %rdi,%rsi               # MOVE ADDRESS: %rsi = string address
                                   # load message address
                                   # Write syscall expects buffer address in %rsi
                                   # We received it in %rdi
                                   # Move it to %rsi

        # STEP 2: SETUP SYSCALL-SPECIFIC REGISTERS
        mov $1,%rax                 # SYSCALL NUMBER: 1 = write
                                   # Tell kernel we want to write

        mov $0,%rdi                 # FILE DESCRIPTOR: 0 (should be 1 for stdout)
                                   # NOTE: Bug - should be stdout (1), not stdin (0)
                                   # 0 = stdin, 1 = stdout, 2 = stderr

        # At this point:
        # %rax = 1 (write syscall)
        # %rdi = 0 (file descriptor - should be 1)
        # %rsi = string address (from original %rdi)
        # %rdx = string length (from original %rsi)

        # STEP 3: INVOKE SYSCALL
        syscall                     # WRITE: Execute write(0, address, length)
                                   # Outputs the string to file descriptor
                                   # Kernel copies %rdx bytes from %rsi to stdout

        # STEP 4: RETURN TO CALLER
        ret                         # RETURN: Pop return address and jump to it
                                   # Function is complete
                                   # Caller continues execution

// PRINTSTR FUNCTION FLOW:
//
// Call: printStr with %rdi = address of "Hello", %rsi = 5
//
// Inside printStr:
//   mov %rsi,%rdx     -> %rdx = 5 (length)
//   mov %rdi,%rsi     -> %rsi = address of "Hello"
//   mov $1,%rax       -> %rax = 1 (write syscall)
//   mov $0,%rdi       -> %rdi = 0 (fd, should be 1)
//   syscall           -> write(0, "Hello", 5)
//   ret               -> return to caller
//
// Output: "Hello" appears on screen


_start:
        ## STEP 1: PRINT FIRST PROMPT
        ## Use printStr helper function instead of inline syscall

        lea msg1(%rip),%rdi         # PARAMETER 1: Load address of msg1 into %rdi
                                   # load message address
                                   # %rdi is the first parameter register
                                   # printStr expects string address in %rdi

        mov $msg1len,%rsi           # PARAMETER 2: Load length into %rsi
                                   # %rsi is the second parameter register
                                   # printStr expects string length in %rsi

        call printStr               # CALL: Invoke printStr(msg1, msg1len)
                                   # printStr will print "Enter a number: "
                                   # Returns here after printing

        ## STEP 2: READ INTEGER FROM USER
        call readInt                # CALL: Invoke readInt()
                                   # Reads input from stdin
                                   # Parses ASCII to binary integer
                                   # Returns integer in %rax
        ## now %rax holds the integer we read in
        ## why %rax? Because that's generally what
        ## things are returned into (calling conv)

        ## STEP 3: SAVE THE INTEGER
        ## We need to preserve it across the next function calls
        push %rax                   # SAVE: Push integer onto stack
                                   # whoops better save rax quick
                                   # Stack: [integer]
                                   # WHY? printStr will modify %rax
                                   # We need the integer for writeInt later

        ## STEP 4: PRINT SECOND PROMPT
        lea msg2(%rip),%rdi         # PARAMETER 1: Address of msg2
        mov $msg2len,%rsi           # PARAMETER 2: Length of msg2
        call printStr               # CALL: Print "Here's what you entered: "

        ## STEP 5: RESTORE AND PRINT THE INTEGER
        pop %rax                    # RESTORE: Pop integer from stack
                                   # we got it back!!!
                                   # %rax contains original integer again

        mov %rax,%rdi               # PARAMETER: Move integer to %rdi
                                   # setup the writeInt call
                                   # writeInt expects integer in %rdi
                                   # This is the first (and only) parameter

        call writeInt               # CALL: Invoke writeInt(integer)
                                   # Converts integer to ASCII
                                   # Outputs to stdout

        ## STEP 6: PRINT FINAL MESSAGE
        lea msg3(%rip),%rdi         # PARAMETER 1: Address of msg3
        mov $msg3len,%rsi           # PARAMETER 2: Length of msg3
        call printStr               # CALL: Print "\nGoodbye!\n"

        ## STEP 7: EXIT PROGRAM
        mov $60,%rax                # SYSCALL NUMBER: 60 = exit
        xor %rdi,%rdi               # EXIT CODE: 0 (success)
                                   # fast way of making 0 be in rdi
        syscall                     # EXIT: Terminate program

// EXECUTION FLOW:
//
// 1. _start begins
// 2. Setup params: %rdi=msg1_addr, %rsi=msg1_len
// 3. Call printStr -> outputs "Enter a number: "
// 4. Call readInt -> reads and parses integer, returns in %rax
// 5. Push %rax -> save integer on stack
// 6. Setup params: %rdi=msg2_addr, %rsi=msg2_len
// 7. Call printStr -> outputs "Here's what you entered: "
// 8. Pop %rax -> restore integer
// 9. Setup param: %rdi=integer
// 10. Call writeInt -> outputs the integer
// 11. Setup params: %rdi=msg3_addr, %rsi=msg3_len
// 12. Call printStr -> outputs "\nGoodbye!\n"
// 13. Exit with status 0

// COMPLETE EXAMPLE:
//
// User input: "789"
//
// Execution trace:
//   lea msg1(%rip),%rdi          # %rdi = address of "Enter a number: "
//   mov $17,%rsi                 # %rsi = 17
//   call printStr
//     printStr:
//       mov %rsi,%rdx            # %rdx = 17
//       mov %rdi,%rsi            # %rsi = msg1_addr
//       mov $1,%rax              # write syscall
//       mov $0,%rdi              # fd 0
//       syscall                  # write(0, msg1, 17)
//       ret
//   Output: "Enter a number: "
//
//   call readInt
//     readInt reads "789\n"
//     Parses to 789 (0x315)
//     Returns %rax = 789
//
//   push %rax                    # Stack: [789]
//
//   lea msg2(%rip),%rdi          # %rdi = address of msg2
//   mov $25,%rsi                 # %rsi = 25
//   call printStr
//   Output: "Here's what you entered: "
//
//   pop %rax                     # %rax = 789
//   mov %rax,%rdi                # %rdi = 789
//   call writeInt
//   Output: "789"
//
//   lea msg3(%rip),%rdi          # %rdi = address of msg3
//   mov $11,%rsi                 # %rsi = 11
//   call printStr
//   Output: "\nGoodbye!\n"
//
//   exit(0)

// CODE COMPARISON:
//
// ORIGINAL (echoInt.s) - Inline syscalls:
//   mov $1,%rax
//   mov $0,%rdi
//   lea msg1(%rip),%rsi
//   mov $msg1len,%rdx
//   syscall                      # 5 instructions for first write
//
//   call readInt
//   push %rax
//
//   mov $1,%rax
//   mov $0,%rdi
//   lea msg2(%rip),%rsi
//   mov $msg2len,%rdx
//   syscall                      # 5 instructions for second write
//
//   pop %rax
//   mov %rax,%rdi
//   call writeInt
//
//   mov $1,%rax
//   mov $0,%rdi
//   lea msg3(%rip),%rsi
//   mov $msg3len,%rdx
//   syscall                      # 5 instructions for third write
//
// REFACTORED (echoInt2.s) - Using printStr:
//   lea msg1(%rip),%rdi
//   mov $msg1len,%rsi
//   call printStr                # 3 instructions for first write
//
//   call readInt
//   push %rax
//
//   lea msg2(%rip),%rdi
//   mov $msg2len,%rsi
//   call printStr                # 3 instructions for second write
//
//   pop %rax
//   mov %rax,%rdi
//   call writeInt
//
//   lea msg3(%rip),%rdi
//   mov $msg3len,%rsi
//   call printStr                # 3 instructions for third write
//
// SAVINGS:
// - Original: 15 instructions for writes (5 × 3)
// - Refactored: 9 instructions for writes (3 × 3)
// - Plus printStr function: 5 instructions (amortized across calls)
// - Clearer intent: "print string" vs low-level syscall setup
// - DRY principle: Don't Repeat Yourself

// IMPORTANT CONCEPTS:
//
// 1. FUNCTION ABSTRACTION:
//    - Wrap common patterns in functions
//    - Reduces code duplication
//    - Improves readability and maintainability
//    - Single point of change if implementation needs updating
//
// 2. PARAMETER PASSING:
//    - First 6 integer/pointer params: %rdi, %rsi, %rdx, %rcx, %r8, %r9
//    - printStr takes 2 params: %rdi (address), %rsi (length)
//    - Additional params go on stack (not used here)
//    - Return value in %rax (not used by printStr)
//
// 3. REGISTER REMAPPING:
//    - printStr receives: %rdi=addr, %rsi=len
//    - syscall needs: %rdi=fd, %rsi=addr, %rdx=len
//    - Must rearrange: save %rsi to %rdx, move %rdi to %rsi, set new %rdi
//    - Order matters! Save %rsi first, or it gets lost
//
// 4. CALLER VS CALLEE RESPONSIBILITIES:
//    - Caller: setup parameters in correct registers
//    - Caller: save registers it wants to preserve (if caller-saved)
//    - Callee: perform the task
//    - Callee: save/restore callee-saved registers (if used)
//    - Callee: return value in %rax (if applicable)
//    - Return: callee pops return address, jumps back
//
// 5. STACK USAGE:
//    - Still need to save %rax around printStr calls
//    - push/pop for temporary storage
//    - Stack pointer must be balanced
//    - Each push must have matching pop
//
// 6. CODE REUSE BENEFITS:
//    - Less code to write
//    - Less code to debug
//    - Less code to maintain
//    - Consistent behavior
//    - Self-documenting: "call printStr" is clearer than raw syscall

// WHY NOT INLINE?
//
// Some might ask: why not just repeat the syscall code?
//
// Problems with repetition:
// 1. More typing, more chance for errors
// 2. If write syscall interface changes, must update 3 places
// 3. If fd bug needs fixing (0 -> 1), must fix 3 places
// 4. Code is harder to read (syscall details obscure intent)
// 5. Violates DRY principle (Don't Repeat Yourself)
//
// Benefits of printStr function:
// 1. Single implementation of write logic
// 2. Change in one place affects all callers
// 3. Clear intent: "print this string"
// 4. Can add features (error checking) in one place
// 5. Easier to unit test
// 6. More professional/maintainable code

// FUNCTION DESIGN PRINCIPLES:
//
// 1. SINGLE RESPONSIBILITY:
//    - printStr does one thing: print a string
//    - Clear, focused purpose
//    - Easy to understand and test
//
// 2. MINIMAL INTERFACE:
//    - Takes exactly what it needs: address and length
//    - No extra parameters
//    - No hidden dependencies
//
// 3. CONSISTENT CONVENTIONS:
//    - Follows System V AMD64 ABI
//    - Parameters in standard registers
//    - Predictable behavior
//
// 4. REUSABILITY:
//    - Works with any string/length
//    - No hardcoded values
//    - Can be used throughout program
//
// 5. ENCAPSULATION:
//    - Hides syscall details from caller
//    - Caller doesn't need to know about %rax/%rdx setup
//    - Implementation can change without affecting callers

// EQUIVALENT C CODE:
//
// #include <unistd.h>
//
// extern long readInt();
// extern void writeInt(long n);
//
// // Helper function - wraps write syscall
// void printStr(const char* str, size_t len) {
//     write(1, str, len);  // Should use fd 1, not 0
// }
//
// int main() {
//     printStr("Enter a number: ", 17);
//     long n = readInt();
//     printStr("Here's what you entered: ", 25);
//     writeInt(n);
//     printStr("\nGoodbye!\n", 11);
//     return 0;
// }
//
// Notice how much cleaner the C version looks with the helper function!
// Same benefit applies to assembly.

// TRY IT:
//
// Compile and link:
//   as echoInt2.s -o echoInt2.o
//   as readInt.s -o readInt.o
//   as writeInt.s -o writeInt.o
//   ld echoInt2.o readInt.o writeInt.o -o echoInt2
//
// Run:
//   ./echoInt2
//   Enter a number: 999
//   Here's what you entered: 999
//   Goodbye!

// EXERCISES:
//
// 1. Add error checking to printStr:
//    - Check syscall return value
//    - Return error code if write fails
//    - Handle partial writes
//
// 2. Create printStrLn function:
//    - Prints string + newline
//    - Simplifies printing lines
//
// 3. Create readStr function:
//    - Wrapper for read syscall
//    - Takes buffer address and size
//    - Returns number of bytes read
//
// 4. Fix the file descriptor bug:
//    - Change mov $0,%rdi to mov $1,%rdi
//    - Verify correct behavior
//
// 5. Add printInt helper:
//    - Wrapper for writeInt
//    - Could add formatting options
//    - Could add newline option

// PERFORMANCE CONSIDERATIONS:
//
// Does the function call add overhead?
// - Yes, call/ret instructions have cost
// - But it's minimal (a few CPU cycles)
// - Compared to syscall overhead (thousands of cycles), negligible
// - Benefits of clean code far outweigh tiny performance cost
//
// Could we inline printStr?
// - Yes, compiler could inline it
// - But for clarity, keep as function
// - Modern CPUs are very fast at function calls
// - Branch prediction makes calls cheap
//
// When to inline vs function?
// - Inline: extremely performance-critical inner loops
// - Function: everything else (readability matters)
// - For syscalls, the syscall itself dominates cost
// - Organization and maintainability are more important
