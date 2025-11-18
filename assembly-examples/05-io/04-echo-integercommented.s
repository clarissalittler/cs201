// PEDAGOGICAL PURPOSE:
// This program demonstrates function composition and modular programming in assembly.
// It combines readInt and writeInt to create a complete integer echo program that:
// 1. Prompts the user with messages
// 2. Reads an integer from stdin
// 3. Echoes it back to stdout with formatting
// This shows how to build complex programs from simpler, reusable functions.
//
// Key learning objectives:
// 1. Understanding how to compose multiple functions to solve a problem
// 2. Learning to use the .rodata section for read-only string data
// 3. Seeing how to declare and use external functions (.extern directive)
// 4. Understanding the calling convention for passing data between functions
// 5. Learning how to preserve values across function calls (using the stack)
// 6. Understanding the complete flow of a multi-function program
// 7. Seeing how to compute string lengths at assembly time
// 8. Proper program structure with entry point and clean exit

// CONCEPTUAL BACKGROUND - MODULAR PROGRAMMING:
//
// MODULAR DESIGN:
// - Break complex problems into smaller, manageable pieces
// - Each module (function) has a single, well-defined purpose
// - Modules can be developed and tested independently
// - Modules can be reused in different programs
//
// THIS PROGRAM'S MODULES:
// 1. readInt:  Reads an integer from stdin
// 2. writeInt: Writes an integer to stdout
// 3. _start:   Main program that orchestrates the modules
//
// BENEFITS:
// - Easier to understand (each function is simple)
// - Easier to test (test each function separately)
// - Easier to maintain (bugs are isolated to specific functions)
// - Easier to reuse (readInt/writeInt can be used in other programs)
//
// FUNCTION COMPOSITION:
// - Output of one function becomes input to another
// - readInt returns integer in %rax
// - We move it to %rdi for writeInt
// - This is the essence of function composition

// CONCEPTUAL BACKGROUND - READ-ONLY DATA:
//
// .rodata SECTION:
// - Stands for "read-only data"
// - Contains constant data that never changes
// - Examples: string literals, constant arrays, lookup tables
// - Placed in read-only memory pages by the OS
// - Attempting to modify causes a segmentation fault (good for safety!)
//
// WHY SEPARATE FROM .data?
// - .data is for mutable global variables
// - .rodata is for immutable constants
// - OS can optimize .rodata (share between processes, protect from writes)
// - Better security (prevents accidental modification)
//
// STRING LITERALS:
// - Strings that appear in the source code
// - Example: "Enter a number: "
// - Stored in .rodata section
// - Accessed by address (pointer to first character)

// CONCEPTUAL BACKGROUND - COMPUTING LENGTHS:
//
// THE DOT (.) OPERATOR:
// - Represents the current location in the program
// - Changes as assembler processes code/data
// - Can be used in expressions
//
// LENGTH CALCULATION:
// msg1:   .ascii "Hello"
// msg1len = . - msg1
//
// HOW IT WORKS:
// - msg1 = address of 'H' (start of string)
// - After .ascii, . points just past 'o' (end of string)
// - . - msg1 = end address - start address = length
// - Example: if msg1=0x1000, .=0x1005, then msg1len=5
//
// WHY THIS IS USEFUL:
// - Automatically computes string length
// - No need to count characters manually
// - If you change the string, length updates automatically
// - Less error-prone than hardcoding lengths

// CONCEPTUAL BACKGROUND - CALLING CONVENTION:
//
// System V AMD64 ABI (Linux x86-64):
//
// PARAMETER PASSING (first 6 parameters):
// 1st parameter: %rdi
// 2nd parameter: %rsi
// 3rd parameter: %rdx
// 4th parameter: %rcx
// 5th parameter: %r8
// 6th parameter: %r9
// 7+ parameters: pushed on stack (right to left)
//
// RETURN VALUE:
// - Integer/pointer: %rax
// - Second return value (if needed): %rdx
//
// REGISTER PRESERVATION:
// Caller-saved (caller must save if needed):
//   %rax, %rcx, %rdx, %rsi, %rdi, %r8-%r11
//   These may be clobbered by called function
//
// Callee-saved (callee must preserve):
//   %rbx, %rbp, %r12-%r15
//   Called function must restore these before returning
//
// EXAMPLE:
// To call: result = add(a, b)
//   mov a, %rdi       # First parameter
//   mov b, %rsi       # Second parameter
//   call add          # Call function
//   mov %rax, result  # Get return value

// CONCEPTUAL BACKGROUND - STACK USAGE:
//
// WHY USE THE STACK?
// - Preserve register values across function calls
// - Store temporary values
// - Allocate local variables
//
// PUSH/POP OPERATIONS:
// push %rax  ->  subq $8, %rsp; movq %rax, (%rsp)
// pop %rax   ->  movq (%rsp), %rax; addq $8, %rsp
//
// STACK GROWS DOWN:
//   Higher addresses
//   +----------------+
//   | older data     |
//   +----------------+
//   | pushed value   | <- was here before push
//   +----------------+ <- %rsp after push
//   | new value      | <- newly pushed
//   +----------------+ <- %rsp now
//   Lower addresses
//
// LIFO (Last In, First Out):
// - Last value pushed is first value popped
// - Must pop in reverse order of push
// - Unbalanced push/pop causes crashes

        .section .rodata           # Begin read-only data section
                                   # .rodata = read-only data
                                   # This section contains constant data
                                   # OS typically maps this as read-only (prevents modification)
                                   # Good for security and optimization

msg1:   .ascii "Enter a number: "  # First prompt message
                                   # .ascii directive creates a string (no null terminator)
                                   # Creates a label 'msg1' pointing to first character
                                   # String: "Enter a number: " (16 bytes)
                                   # These bytes will be in read-only memory

msg1len = . - msg1                 # Compute length of msg1
                                   # . = current location (just after the string)
                                   # msg1 = start of string
                                   # . - msg1 = length in bytes
                                   # Result: msg1len = 16
                                   # WHY? E n t e r   a   n u m b e r :   (16 chars)
                                   # This is computed at assembly time (not runtime)

###  The difference between . (which signifies our current location in the program, as an address) - msg (the address of the start of the data) is the length of the message in bytes

msg2:   .ascii "Here's what you entered: "
                                   # Second message (displayed before echoing the number)
                                   # .ascii creates string without null terminator
                                   # Label 'msg2' points to 'H'
                                   # String: "Here's what you entered: " (25 bytes)

msg2len = . - msg2                 # Compute length of msg2
                                   # Same technique as msg1len
                                   # Result: msg2len = 25

msg3:   .ascii "\nGoodbye!\n"     # Final message
                                   # \n = newline character (ASCII 10)
                                   # String: newline, G, o, o, d, b, y, e, !, newline (10 bytes)
                                   # Two newlines for formatting

msg3len = . - msg3                 # Compute length of msg3
                                   # Result: msg3len = 10


        .section .text             # Begin code section
                                   # .text contains executable instructions

        .global _start             # Make _start visible to linker
                                   # _start is the entry point for the program
                                   # OS loader jumps here to begin execution

        .extern readInt            # Declare readInt as external function
                                   # .extern tells assembler this symbol is defined elsewhere
                                   # readInt will be resolved by the linker
                                   # Must link with object file containing readInt

        .extern writeInt           # Declare writeInt as external function
                                   # .extern tells assembler this symbol is defined elsewhere
                                   # writeInt will be resolved by the linker
                                   # Must link with object file containing writeInt

        # IMPORTANT: .extern declares symbols that exist in OTHER object files
        # The linker will connect our calls to the actual function code
        # This enables separate compilation and code reuse

_start:
        # ================================================================
        # STEP 1: Display first prompt ("Enter a number: ")
        # ================================================================
        # We're calling: sys_write(fd=1, buf=msg1, count=msg1len)

        mov $1,%rax                # Syscall number 1 = sys_write
                                   # sys_write writes bytes to a file descriptor
                                   # This is how we output text to the screen

        mov $1,%rdi                # file descriptor 1 = stdout
                                   # stdout is standard output (terminal screen)
                                   # Writing to file descriptor 1 displays on screen

        lea msg1(%rip),%rsi        # load message address
                                   # lea = load effective address
                                   # msg1(%rip) = RIP-relative addressing
                                   # %rsi will point to "Enter a number: "
                                   # This is the buffer parameter for sys_write

        mov $msg1len,%rdx          # Number of bytes to write
                                   # msg1len was computed as . - msg1 = 16
                                   # Tells kernel to write exactly 16 bytes
                                   # This is the count parameter for sys_write

        syscall                    # Execute the system call
                                   # Kernel writes 16 bytes from msg1 to stdout
                                   # User sees: "Enter a number: " (with cursor after it)
                                   # Control returns to our program

        # ================================================================
        # STEP 2: Read integer from user
        # ================================================================
        # We're calling: integer = readInt()
        # No parameters needed (readInt reads from stdin)

        ## first we call readInt
        call readInt               # Call the readInt function
                                   # WHAT HAPPENS:
                                   # 1. Push return address onto stack
                                   # 2. Jump to readInt function
                                   # 3. readInt executes (reads user input, parses it)
                                   # 4. readInt returns with integer in %rax
                                   # 5. Execution continues here

        ## now %rax holds the integer we read in
        ## why %rax? Because that's generally what
        ## things are returned into (calling conv)

        # After call returns:
        # - %rax contains the integer that was read
        # - Example: if user typed "42", %rax = 42 (binary)
        # - %rax is a caller-saved register (may be clobbered by next call)
        # - PROBLEM: We need this value later, but we're about to make more syscalls

        # ================================================================
        # STEP 3: Preserve the integer value
        # ================================================================
        # We need to save %rax because:
        # 1. We're about to make a syscall (which clobbers %rax)
        # 2. We need the integer value for writeInt later
        # SOLUTION: Save it on the stack

        push %rax                  # whoops better save rax quick
                                   # Push the integer onto the stack
                                   # Stack now has our value safely preserved
                                   # Stack pointer (%rsp) decremented by 8
                                   # CRITICAL: Must pop later to balance the stack!

        # Stack state:
        # +----------------+
        # | ...            |
        # +----------------+
        # | integer value  | <- %rsp points here
        # +----------------+

        # ================================================================
        # STEP 4: Display second prompt ("Here's what you entered: ")
        # ================================================================
        # We're calling: sys_write(fd=1, buf=msg2, count=msg2len)

        mov $1,%rax                # Syscall number 1 = sys_write
                                   # Setting up for another write syscall

        mov $1,%rdi                # file descriptor 1 = stdout
                                   # Output goes to the terminal

        lea msg2(%rip),%rsi        # load message address
                                   # %rsi points to "Here's what you entered: "

        mov $msg2len,%rdx          # Number of bytes to write (25)
                                   # msg2len was computed as . - msg2

        syscall                    # Execute the system call
                                   # User sees: "Here's what you entered: "
                                   # Cursor is after the message, waiting for the number

        # ================================================================
        # STEP 5: Restore the integer and display it
        # ================================================================
        # Now we need to print the integer we read earlier

        pop %rax                   # we got it back!!!
                                   # Restore the integer from the stack
                                   # %rax now has the value we saved earlier
                                   # Stack pointer (%rsp) incremented by 8
                                   # Stack is balanced again (push/pop matched)

        mov %rax,%rdi              # setup the writeInt call
                                   # Move integer from %rax to %rdi
                                   # WHY? writeInt expects its parameter in %rdi
                                   # CALLING CONVENTION: First parameter goes in %rdi
                                   # We're preparing to call writeInt(integer)

        call writeInt              # Call writeInt to display the integer
                                   # WHAT HAPPENS:
                                   # 1. Push return address onto stack
                                   # 2. Jump to writeInt function
                                   # 3. writeInt converts integer to ASCII
                                   # 4. writeInt calls sys_write to output the digits
                                   # 5. writeInt returns
                                   # 6. Execution continues here
                                   # User now sees the integer on screen
                                   # Example: if %rdi was 42, user sees "42"

        # ================================================================
        # STEP 6: Display goodbye message ("\nGoodbye!\n")
        # ================================================================
        # We're calling: sys_write(fd=1, buf=msg3, count=msg3len)

        mov $1,%rax                # Syscall number 1 = sys_write
                                   # One more write syscall

        mov $1,%rdi                # file descriptor 1 = stdout
                                   # Output to terminal

        lea msg3(%rip),%rsi        # load message address
                                   # %rsi points to "\nGoodbye!\n"

        mov $msg3len,%rdx          # Number of bytes to write (10)
                                   # msg3len computed as . - msg3

        syscall                    # Execute the system call
                                   # User sees:
                                   # [newline]
                                   # Goodbye!
                                   # [newline]

        # ================================================================
        # STEP 7: Exit the program
        # ================================================================
        # We're calling: sys_exit(status=0)

        mov $60,%rax               # Syscall number 60 = sys_exit
                                   # sys_exit terminates the process
                                   # This is REQUIRED - we must explicitly exit
                                   # Unlike C's main(), there's no implicit return

        xor %rdi,%rdi              # fast way of making 0 be in rdi
                                   # xor %rdi,%rdi sets %rdi to 0
                                   # WHY? x XOR x = 0 for any value x
                                   # This is faster and smaller than mov $0,%rdi
                                   # Exit status 0 means "success"
                                   # Shell can check with: echo $?

        syscall                    # Execute exit syscall
                                   # Program terminates here
                                   # Control returns to the shell
                                   # This syscall NEVER returns!

// COMPLETE EXECUTION EXAMPLE:
//
// User runs the program and types "42"
//
// TRACE:
//
// STEP 1: First prompt
//   Setup: %rax=1, %rdi=1, %rsi=msg1, %rdx=16
//   syscall -> sys_write(1, "Enter a number: ", 16)
//   Screen: "Enter a number: "
//
// STEP 2: Read integer
//   call readInt
//   [readInt executes:]
//     - Calls sys_read to get input from keyboard
//     - User types: "42\n"
//     - Parses ASCII "42" to binary 42 (0x2A)
//     - Returns with %rax = 42
//   [back in _start]
//   %rax = 42
//
// STEP 3: Save integer
//   push %rax
//   Stack now contains: [42]
//
// STEP 4: Second prompt
//   Setup: %rax=1, %rdi=1, %rsi=msg2, %rdx=25
//   syscall -> sys_write(1, "Here's what you entered: ", 25)
//   Screen: "Enter a number: Here's what you entered: "
//
// STEP 5: Display integer
//   pop %rax -> %rax = 42
//   mov %rax,%rdi -> %rdi = 42
//   call writeInt
//   [writeInt executes:]
//     - Converts 42 to ASCII "42"
//     - Calls sys_write to output "42"
//   [back in _start]
//   Screen: "Enter a number: Here's what you entered: 42"
//
// STEP 6: Goodbye message
//   Setup: %rax=1, %rdi=1, %rsi=msg3, %rdx=10
//   syscall -> sys_write(1, "\nGoodbye!\n", 10)
//   Screen: "Enter a number: Here's what you entered: 42
//           Goodbye!
//           "
//
// STEP 7: Exit
//   Setup: %rax=60, %rdi=0
//   syscall -> sys_exit(0)
//   [Program terminates]
//   [Control returns to shell]
//   Shell sees exit status 0 (success)

// EXECUTION FLOW DIAGRAM:
//
// _start
//   |
//   v
// [Write msg1: "Enter a number: "]
//   |
//   v
// call readInt -----> readInt function
//   |                   |
//   |                   v
//   |                 [Read from stdin]
//   |                   |
//   |                   v
//   |                 [Parse ASCII to int]
//   |                   |
//   |                   v
//   |                 ret (return %rax=integer)
//   v                   |
// [Save %rax on stack] <-
//   |
//   v
// [Write msg2: "Here's what you entered: "]
//   |
//   v
// [Restore %rax from stack]
//   |
//   v
// [Move %rax to %rdi]
//   |
//   v
// call writeInt ----> writeInt function
//   |                   |
//   |                   v
//   |                 [Convert int to ASCII]
//   |                   |
//   |                   v
//   |                 [Write to stdout]
//   |                   |
//   |                   v
//   |                 ret
//   v                   |
// [Write msg3: "\nGoodbye!\n"] <-
//   |
//   v
// [Exit with status 0]
//   |
//   v
// [Program terminates]

// WHY THIS DESIGN?
//
// 1. MODULARITY:
//    - readInt handles all integer input logic
//    - writeInt handles all integer output logic
//    - _start handles program flow and user interaction
//    - Each module has a single, clear responsibility
//
// 2. REUSABILITY:
//    - readInt and writeInt can be used in other programs
//    - No need to rewrite integer I/O code for each program
//    - Can be compiled into a library
//
// 3. TESTABILITY:
//    - Each function can be tested independently
//    - Bugs are isolated to specific functions
//    - Easier to verify correctness
//
// 4. READABILITY:
//    - High-level flow is clear in _start
//    - Implementation details hidden in functions
//    - Easier to understand program logic
//
// 5. CALLING CONVENTION:
//    - Following standard convention enables interoperability
//    - Can call C functions, or be called from C
//    - Tools (debuggers, profilers) understand the convention

// IMPORTANT CONCEPTS:
//
// 1. FUNCTION CALLS:
//    - call pushes return address and jumps to function
//    - ret pops return address and returns
//    - Must balance stack (push/pop, call/ret)
//
// 2. PARAMETER PASSING:
//    - readInt: no parameters (reads from stdin)
//    - writeInt: one parameter in %rdi (integer to print)
//    - Follows System V AMD64 ABI
//
// 3. RETURN VALUES:
//    - readInt returns integer in %rax
//    - writeInt doesn't return a value (void function)
//    - %rax is standard return value register
//
// 4. REGISTER PRESERVATION:
//    - %rax is caller-saved (may be clobbered by calls)
//    - We save it on stack between readInt and writeInt
//    - push/pop used for temporary storage
//
// 5. EXTERNAL SYMBOLS:
//    - .extern declares symbols defined in other files
//    - Linker resolves these references at link time
//    - Enables separate compilation
//
// 6. READ-ONLY DATA:
//    - String literals in .rodata section
//    - Protected from modification by OS
//    - Can be shared between processes

// DATA FLOW:
//
// Keyboard -> readInt -> %rax -> stack -> %rax -> %rdi -> writeInt -> Screen
//
// DETAILED:
// 1. User types on keyboard: "42\n"
// 2. readInt reads it: sys_read(0, buffer, 128)
// 3. readInt parses it: ASCII "42" -> binary 42
// 4. readInt returns: %rax = 42
// 5. _start saves it: push %rax (stack now has 42)
// 6. _start writes prompt: sys_write(...)
// 7. _start restores it: pop %rax (42)
// 8. _start passes to writeInt: mov %rax,%rdi (42)
// 9. writeInt converts: binary 42 -> ASCII "42"
// 10. writeInt outputs: sys_write(1, "42", 2)
// 11. Screen displays: 42

// COMPARISON WITH C CODE:
//
// This assembly is equivalent to:
//
// extern long readInt(void);
// extern void writeInt(long n);
//
// int main() {
//     write(1, "Enter a number: ", 16);
//     long num = readInt();
//     write(1, "Here's what you entered: ", 25);
//     writeInt(num);
//     write(1, "\nGoodbye!\n", 10);
//     return 0;
// }
//
// Or with printf/scanf:
//
// int main() {
//     printf("Enter a number: ");
//     long num;
//     scanf("%ld", &num);
//     printf("Here's what you entered: %ld\n", num);
//     printf("Goodbye!\n");
//     return 0;
// }

// TRY IT:
//
// Compile and link all three files:
//   as 04-echo-integer.s -o echoInt.o
//   as 02-read-integer.s -o readInt.o
//   as 03-write-integer.s -o writeInt.o
//   ld echoInt.o readInt.o writeInt.o -o echoInt
//
// Run:
//   ./echoInt
//   Enter a number: 42         (you type "42" and press Enter)
//   Here's what you entered: 42
//   Goodbye!
//
// Test with different values:
//   ./echoInt
//   Enter a number: 0          (zero)
//   Here's what you entered: 0
//   Goodbye!
//
//   ./echoInt
//   Enter a number: 999        (large number)
//   Here's what you entered: 999
//   Goodbye!
//
// Check exit status:
//   ./echoInt
//   Enter a number: 123
//   Here's what you entered: 123
//   Goodbye!
//   echo $?
//   0                          (exit status is 0 = success)

// LINKING EXPLAINED:
//
// Why three separate files?
// - Modularity: Each function in its own file
// - Separate compilation: Change one, recompile only that one
// - Reusability: readInt.o and writeInt.o can be used in other programs
//
// What does the linker do?
// 1. Reads all object files: echoInt.o, readInt.o, writeInt.o
// 2. Resolves external symbols:
//    - _start calls readInt -> finds readInt in readInt.o
//    - _start calls writeInt -> finds writeInt in writeInt.o
// 3. Combines all code and data sections
// 4. Fixes up addresses (relocations)
// 5. Produces executable: echoInt
//
// Symbol resolution:
// - echoInt.o has undefined symbols: readInt, writeInt
// - readInt.o defines: readInt
// - writeInt.o defines: writeInt
// - Linker matches them up
//
// Entry point:
// - Linker looks for _start symbol
// - OS loader will jump there to begin execution

// DEBUGGING TIPS:
//
// Use GDB to trace execution:
//   gdb ./echoInt
//   break _start
//   run
//
// Step through:
//   ni                         # Next instruction (doesn't enter calls)
//   si                         # Step instruction (enters calls)
//
// Examine values:
//   info registers             # View all registers
//   p/d $rax                   # Print %rax in decimal
//   x/s $rsi                   # Examine string at %rsi
//
// Breakpoints:
//   break readInt              # Break when entering readInt
//   break writeInt             # Break when entering writeInt
//
// Watch stack:
//   x/16xg $rsp                # Examine 16 quadwords from stack pointer
//   info frame                 # Show stack frame info
//
// Trace syscalls with strace:
//   strace ./echoInt
//   (shows all syscalls with arguments and return values)
//
// Example strace output:
//   write(1, "Enter a number: ", 16) = 16
//   read(0, "42\n", 128) = 3
//   write(1, "Here's what you entered: ", 25) = 25
//   write(1, "42", 2) = 2
//   write(1, "\nGoodbye!\n", 10) = 10
//   exit(0) = ?

// EXTENDING THIS CODE:
//
// Add error handling:
//   # After readInt, check if input was valid
//   test %rax, %rax
//   js error_handler           # Jump if negative (error flag)
//
// Read multiple integers:
//   loop:
//       call readInt
//       # Check for EOF or error
//       cmp $-1, %rax
//       je done
//       call writeInt
//       jmp loop
//   done:
//
// Perform computation:
//   call readInt               # Get first number
//   mov %rax, %rbx             # Save it
//   call readInt               # Get second number
//   add %rbx, %rax             # Add them
//   mov %rax, %rdi
//   call writeInt              # Display result
//
// Format output:
//   # Print result with label
//   lea result_msg(%rip), %rsi
//   mov $result_msglen, %rdx
//   mov $1, %rax
//   mov $1, %rdi
//   syscall
//   pop %rax
//   mov %rax, %rdi
//   call writeInt

// COMMON MISTAKES:
//
// 1. Forgetting to save %rax:
//    - Result: writeInt gets wrong value (clobbered by syscall)
//    - Fix: push %rax before syscalls, pop before writeInt
//
// 2. Unbalanced stack:
//    - push without matching pop
//    - Result: ret jumps to wrong address (crash)
//    - Fix: ensure every push has a matching pop
//
// 3. Wrong register for parameter:
//    - Using %rax instead of %rdi for writeInt
//    - Result: writeInt gets garbage
//    - Fix: move value to %rdi (first parameter register)
//
// 4. Missing .extern:
//    - Assembler can't find readInt/writeInt
//    - Result: "undefined reference" error at link time
//    - Fix: add .extern declarations
//
// 5. Wrong linking order:
//    - Not including all object files
//    - Result: linker can't resolve symbols
//    - Fix: link all three .o files
//
// 6. Modifying .rodata:
//    - Trying to write to msg1, msg2, msg3
//    - Result: segmentation fault (access violation)
//    - Fix: don't modify read-only data
//
// ===========================================================================
// EXPECTED OUTPUT AND TESTING
// ===========================================================================
//
// TO ASSEMBLE, LINK, AND RUN:
// (Assuming you have readInt.o and writeInt.o already assembled)
//
// as 04-echo-integer.s -o 04-echo-integer.o
// ld 04-echo-integer.o readInt.o writeInt.o -o 04-echo-integer
// ./04-echo-integer
//
// EXPECTED BEHAVIOR:
// The program prompts for an integer, reads it, and echoes it back.
//
// EXAMPLE SESSION 1:
// $ ./04-echo-integer
// 42         <- You type this and press Enter
// 42         <- Program echoes it back
// $ echo $?
// 0
//
// EXAMPLE SESSION 2:
// $ ./04-echo-integer
// -100       <- Negative number
// -100       <- Echoed back
// $
//
// EXAMPLE SESSION 3:
// $ ./04-echo-integer
// 0          <- Zero
// 0          <- Echoed back
// $
//
// NOTES:
// - Reads one integer from standard input
// - Handles positive and negative integers
// - Typical range: -2^63 to 2^63-1 (64-bit signed)
// - Non-numeric input may cause unexpected behavior
//
// EXIT CODE:
// $ echo $?
// 0          (Always successful if linking is correct)
//
// TESTING WITH PIPES:
// $ echo "123" | ./04-echo-integer
// 123
//
// $ echo "-456" | ./04-echo-integer
// -456
//
// COMMON ISSUES:
// 1. Undefined reference to readInt or writeInt
//    Solution: Make sure you link with readInt.o and writeInt.o
//
// 2. Incorrect output for large numbers
//    Check: Is the number within 64-bit signed range?
//
// 3. Garbage output
//    Problem: Likely buffer or parsing issue in readInt
//    Solution: Verify readInt implementation
