// PEDAGOGICAL PURPOSE:
// This program demonstrates stack frame management and local variables in x86-64 assembly.
// It shows how to implement a function with local variables, how parameters are passed,
// and how to maintain the stack frame properly. The 'fun' function demonstrates the
// translation of C-like local variables into assembly using stack-relative addressing.
//
// Key learning objectives:
// 1. Understanding stack frame structure (prologue/epilogue)
// 2. Learning how to allocate local variables on the stack
// 3. Understanding %rbp-relative addressing for local variables
// 4. Seeing how parameters are passed in registers (%rdi, %rsi)
// 5. Learning the relationship between C variables and stack offsets
// 6. Understanding stack alignment requirements (16-byte alignment)
// 7. Learning the 'leave' instruction as shorthand for epilogue
// 8. Seeing how arithmetic operations work with memory operands
// 9. Understanding the complete flow from input to computation to output
// 10. Learning how helper functions (printStr) reduce code duplication

        .section .rodata            # READ-ONLY DATA SECTION
msg1:   .ascii "\nEnter a number: " # Prompt for first input
msg1len = . - msg1                  # Compute length: 18 bytes

msg2:   .ascii "\nYour function calculated: " # Result message
msg2len = . - msg2                           # Compute length: 29 bytes

msg3:   .ascii "\nGoodbye!\n"      # Exit message
msg3len = . - msg3                 # Compute length: 11 bytes

        .section .text             # CODE SECTION
        .global _start             # Make _start symbol globally visible for the linker

        .extern readInt            # External function declarations
        .extern writeInt           # These are defined in separate files


        ## HELPER FUNCTION: printStr
        ## =========================
        ## Function that takes the address of a string in %rdi and length in %rsi,
        ## then prints it to stdout.
        ##
        ## This demonstrates code reuse - instead of repeating syscall setup
        ## three times, we encapsulate it in a function.
        ##
        ## PARAMETERS:
        ##   %rdi - Address of string
        ##   %rsi - Length of string
printStr:
        mov %rsi,%rdx               # STEP 1: Rearrange parameters for syscall
                                   # Write syscall needs length in %rdx
                                   # We received it in %rsi, so copy to %rdx

        mov %rdi,%rsi               # STEP 2: Move string address to %rsi
                                   # load message address
                                   # Write syscall expects buffer address in %rsi
                                   # We received it in %rdi, so copy to %rsi

        mov $1,%rax                 # SYSCALL NUMBER: 1 = write
        mov $0,%rdi                 # FILE DESCRIPTOR: 0 (should be 1 for stdout)
                                   # NOTE: Bug - should use fd 1 for stdout

        syscall                     # INVOKE WRITE: Output the string
        ret                         # RETURN to caller


// DEMONSTRATION FUNCTION: fun
// ===========================
// This function demonstrates stack-based local variables and basic arithmetic.
// It's a direct translation of a simple C function to assembly.
//
// C EQUIVALENT:
//   long fun(long a, long b) {
//       long c = a + b;      // First local variable
//       long d = 2 + b;      // Second local variable
//       return (c + d);      // Return sum of local variables
//   }
//
// EXAMPLE EXECUTION:
//   fun(3, 4):
//     c = 3 + 4 = 7
//     d = 2 + 4 = 6
//     return 7 + 6 = 13
//
// This is a function that demonstrates:
// 1. Using local variables on the stack
// 2. Function call conventions
// 3. Basic arithmetic operations
//
// PARAMETERS:
//   %rdi - a (first parameter)
//   %rsi - b (second parameter)
//
// RETURNS:
//   %rax - result (c + d)
//
// LOCAL VARIABLES (on stack):
//   -8(%rbp)  - c (first local variable)
//   -16(%rbp) - d (second local variable)
//
fun:
        # FUNCTION PROLOGUE
        # =================
        # Standard function prologue establishes stack frame
        # Every function that uses local variables needs this

        # Save the old base pointer (frame pointer)
        push %rbp                   # Push caller's %rbp onto stack
                                   # %rbp is callee-saved (we must preserve it)
                                   # Stack: [..., old_rbp]
                                   # %rsp now points to old_rbp

        # Set up new base pointer for this stack frame
        mov %rsp,%rbp               # %rbp = %rsp
                                   # Establish our stack frame
                                   # %rbp now points to saved old_rbp
                                   # WHY? %rbp provides stable reference for locals
                                   # %rsp will change with push/pop
                                   # %rbp stays constant within function

        # Allocate 16 bytes of local stack space
        sub $16,%rsp                # %rsp -= 16
                                   # This creates space for two 8-byte (64-bit) local variables
                                   # WHY 16? Two variables (8 bytes each) + alignment
                                   # System V ABI requires 16-byte stack alignment
                                   #
                                   # STACK LAYOUT after prologue:
                                   # %rbp + 0  : saved old_rbp (from push)
                                   # %rbp - 8  : local variable c
                                   # %rbp - 16 : local variable d
                                   # %rsp points to %rbp - 16

        # COMPUTE FIRST LOCAL VARIABLE: c = a + b
        # ========================================
        # First parameter (a) is in %rdi (per calling convention)
        # Second parameter (b) is in %rsi (per calling convention)
        # We want: c = a + b, stored at -8(%rbp)

        mov %rdi,-8(%rbp)           # Store a at -8(%rbp)
                                   # First local variable will be at -8(%rbp)
                                   # We copy a there as starting point
                                   # Memory at -8(%rbp) now contains value of a

        add %rsi,-8(%rbp)           # Add b to the value at -8(%rbp)
                                   # This does: memory[-8(%rbp)] += %rsi
                                   # Result: memory[-8(%rbp)] = a + b = c
                                   # WHY THIS WORKS: x86 allows memory destination
                                   # add can directly modify memory location
                                   # Alternative would be:
                                   #   mov %rdi,%rax
                                   #   add %rsi,%rax
                                   #   mov %rax,-8(%rbp)

        # COMPUTE SECOND LOCAL VARIABLE: d = b + 2
        # =========================================
        # We want: d = b + 2, stored at -16(%rbp)

        mov %rsi,-16(%rbp)          # Store b at -16(%rbp)
                                   # Second local variable at -16(%rbp)
                                   # Copy b there as starting point
                                   # Memory at -16(%rbp) now contains value of b

        addq $2,-16(%rbp)           # Add 2 to the value at -16(%rbp)
                                   # This does: memory[-16(%rbp)] += 2
                                   # Result: memory[-16(%rbp)] = b + 2 = d
                                   # addq = add quadword (64-bit)
                                   # $2 is immediate operand (the constant 2)
                                   # Alternative would be:
                                   #   mov %rsi,%rax
                                   #   add $2,%rax
                                   #   mov %rax,-16(%rbp)

        # PREPARE RETURN VALUE: result = c + d
        # =====================================
        # Return value must be in %rax (calling convention)
        # We need to add c and d

        # Prepare return value by:
        # 1. Load d into %rax
        mov -16(%rbp),%rax          # %rax = d (load from memory)
                                   # Load the value of d into %rax
                                   # %rax now contains (b + 2)

        # 2. Add c to %rax
        add -8(%rbp),%rax           # %rax += c (add from memory)
                                   # This does: %rax = %rax + memory[-8(%rbp)]
                                   # Result: %rax = d + c = (b+2) + (a+b) = a + 2b + 2
                                   # This is our return value

        # FUNCTION EPILOGUE
        # =================
        # Cleanup stack frame and return

        # Function epilogue:
        # 'leave' is a single instruction that does:
        # mov %rbp,%rsp    (restore stack pointer, deallocate locals)
        # pop %rbp         (restore old base pointer)
        leave                       # Teardown stack frame
                                   # EQUIVALENT TO:
                                   #   mov %rbp,%rsp  # Deallocate local variables
                                   #   pop %rbp       # Restore caller's %rbp
                                   # 'leave' is an x86 convenience instruction
                                   # More concise than two separate instructions

        ret                         # Return to caller
                                   # Pops return address from stack
                                   # Jumps to that address
                                   # Return value is in %rax


// MAIN PROGRAM
// ============
// Reads two integers, passes them to fun(), prints result
_start:
        ## PRINT FIRST PROMPT
        lea msg1(%rip),%rdi         # %rdi = address of msg1
        mov $msg1len,%rsi           # %rsi = length of msg1
        call printStr               # Print "\nEnter a number: "

        ## READ FIRST INTEGER
        call readInt                # Call readInt to get first number
                                   # User types a number
                                   # readInt returns it in %rax

        push %rax                   # Save first number on stack
                                   # We need to preserve it while reading second number
                                   # Stack: [first_number]

        ## PRINT SECOND PROMPT
        lea msg1(%rip),%rdi         # %rdi = address of msg1 (reuse same prompt)
        mov $msg1len,%rsi           # %rsi = length of msg1
        call printStr               # Print "\nEnter a number: " again

        ## READ SECOND INTEGER
        call readInt                # Call readInt to get second number
                                   # User types another number
                                   # readInt returns it in %rax
                                   # Now %rax = second number

        # Set up arguments for fun()
        # According to calling convention:
        # - First parameter goes in %rdi
        # - Second parameter goes in %rsi

        # Second arg (4) goes in %rsi
        mov %rax,%rsi               # %rsi = second number (just read)
                                   # This is parameter 'b' for fun

        pop %rax                    # Restore first number from stack
                                   # %rax = first number (read earlier)

        # First arg (3) goes in %rdi
        mov %rax,%rdi               # %rdi = first number
                                   # This is parameter 'a' for fun

        # Call the fun() function
        call fun                    # Call fun(first_number, second_number)
                                   # fun computes: (a+b) + (b+2) = a + 2b + 2
                                   # Example: fun(3, 4) = (3+4) + (4+2) = 7 + 6 = 13
                                   # Result returned in %rax

        push %rax                   # Save result on stack
                                   # We need to preserve it while printing message
                                   # Stack: [result]

        ## print the next message
        lea msg2(%rip),%rdi         # %rdi = address of msg2
        mov $msg2len,%rsi           # %rsi = length of msg2
        call printStr               # Print "\nYour function calculated: "

        # Return value from fun() is already in %rax
        pop %rax                    # Restore result from stack
                                   # %rax = result from fun()

        mov %rax,%rdi               # %rdi = result (parameter for writeInt)
        call writeInt               # Print the integer result
                                   # Converts to ASCII and outputs

        ## PRINT GOODBYE MESSAGE
        lea msg3(%rip),%rdi         # %rdi = address of msg3
        mov $msg3len,%rsi           # %rsi = length of msg3
        call printStr               # Print "\nGoodbye!\n"

        # EXIT PROGRAM
        # Syscall number for exit
        mov $60,%rax                # %rax = 60 (exit syscall)
        xor %rdi,%rdi               # %rdi = 0 (exit status: success)
                                   # xor trick: any value XOR itself = 0
        # Invoke system call to exit
        syscall                     # Exit program

// COMPLETE EXECUTION EXAMPLE:
//
// User inputs: 10 and 5
//
// 1. Print "Enter a number: "
// 2. Read first integer: %rax = 10
// 3. Push 10 onto stack
// 4. Print "Enter a number: " (again)
// 5. Read second integer: %rax = 5
// 6. Setup fun parameters: %rdi = 10 (a), %rsi = 5 (b)
// 7. Call fun(10, 5):
//    Prologue: setup stack frame
//    c = 10 + 5 = 15 (stored at -8(%rbp))
//    d = 5 + 2 = 7 (stored at -16(%rbp))
//    result = 15 + 7 = 22 (in %rax)
//    Epilogue: cleanup stack frame
//    Return with %rax = 22
// 8. Print "Your function calculated: "
// 9. Print 22
// 10. Print "Goodbye!"
// 11. Exit

// STACK EVOLUTION DURING fun(10, 5):
//
// Before call:
//   Stack: [..., 10, return_address_to_main]
//   %rdi = 10, %rsi = 5
//
// After 'push %rbp':
//   Stack: [..., 10, return_address, old_rbp]
//   %rbp -> old_rbp
//
// After 'mov %rsp,%rbp':
//   %rbp points to old_rbp location
//
// After 'sub $16,%rsp':
//   Stack: [..., 10, return_address, old_rbp, space_for_c, space_for_d]
//          %rbp points to old_rbp
//          %rbp-8 points to c's location
//          %rbp-16 points to d's location
//          %rsp points to %rbp-16
//
// After computing c and d:
//   -8(%rbp) = 15 (c = a + b)
//   -16(%rbp) = 7 (d = b + 2)
//
// After 'leave':
//   Stack: [..., 10, return_address]
//   %rbp restored to caller's value
//   %rsp points to return_address
//
// After 'ret':
//   Stack: [..., 10]
//   PC = return_address (back in _start)
//   %rax = 22

// IMPORTANT CONCEPTS:
//
// 1. STACK FRAME STRUCTURE:
//    - Saved %rbp links frames together (frame chain)
//    - Local variables at negative offsets from %rbp
//    - Parameters passed in registers (not on stack in our case)
//    - Return address pushed by 'call' instruction
//    - Each function call creates new frame on stack
//
// 2. LOCAL VARIABLES:
//    - Allocated by decreasing %rsp
//    - Accessed via %rbp-relative addressing
//    - Deallocated by 'leave' instruction
//    - Stack grows downward (toward lower addresses)
//
// 3. CALLING CONVENTION (System V AMD64 ABI):
//    - Parameters: %rdi, %rsi, %rdx, %rcx, %r8, %r9
//    - Return value: %rax
//    - Callee-saved: %rbp, %rbx, %r12-%r15
//    - Caller-saved: %rax, %rcx, %rdx, %rsi, %rdi, %r8-%r11
//    - Stack must be 16-byte aligned before 'call'
//
// 4. PROLOGUE/EPILOGUE PATTERN:
//    - Prologue: push %rbp, mov %rsp,%rbp, sub $N,%rsp
//    - Epilogue: leave (or mov %rbp,%rsp; pop %rbp), ret
//    - This pattern is standard for all functions with locals
//
// 5. MEMORY OPERANDS:
//    - Can read/write memory directly: add %rsi,-8(%rbp)
//    - No need to load to register first
//    - x86 is rich in addressing modes
//    - Makes code more concise

// WHY USE %RBP FOR LOCALS?
//
// Option 1: Use %rsp-relative (without %rbp):
//   - Would need to track %rsp changes (push/pop)
//   - Offsets change as stack grows/shrinks
//   - Hard to maintain, error-prone
//
// Option 2: Use %rbp-relative (with frame pointer):
//   - %rbp stays constant within function
//   - Offsets are fixed: -8(%rbp), -16(%rbp)
//   - Easy to understand and maintain
//   - Debuggers rely on this for backtraces
//
// Modern compilers sometimes omit frame pointer (-fomit-frame-pointer)
// to save register %rbp for other uses, but it makes debugging harder.

// COMPARISON WITH C COMPILATION:
//
// If you compile the C code:
//   long fun(long a, long b) {
//       long c = a + b;
//       long d = 2 + b;
//       return c + d;
//   }
//
// With: gcc -S -O0 fun.c
//
// You'll get assembly similar to our fun() function:
//   - Same prologue/epilogue
//   - Same stack layout
//   - Same %rbp-relative addressing
//   - Might have slight differences in register usage
//
// With optimization (-O2):
//   - Might eliminate stack frame
//   - Keep everything in registers
//   - Compute result directly: a + b + (2 + b) = a + 2b + 2
//   - Much faster but less readable

// EQUIVALENT C CODE FOR ENTIRE PROGRAM:
//
// extern long readInt();
// extern void writeInt(long n);
// extern void printStr(const char* s, size_t len);
//
// long fun(long a, long b) {
//     long c = a + b;
//     long d = 2 + b;
//     return c + d;
// }
//
// int main() {
//     printStr("\nEnter a number: ", 18);
//     long a = readInt();
//     printStr("\nEnter a number: ", 18);
//     long b = readInt();
//     long result = fun(a, b);
//     printStr("\nYour function calculated: ", 29);
//     writeInt(result);
//     printStr("\nGoodbye!\n", 11);
//     return 0;
// }

// TRY IT:
//
// Compile and link:
//   as stacky.s -o stacky.o
//   as readInt.s -o readInt.o
//   as writeInt.s -o writeInt.o
//   ld stacky.o readInt.o writeInt.o -o stacky
//
// Run:
//   ./stacky
//   Enter a number: 10
//   Enter a number: 5
//   Your function calculated: 22
//   Goodbye!
//
// Verify calculation: fun(10, 5) = (10+5) + (5+2) = 15 + 7 = 22
//
// Try other inputs:
//   fun(3, 4) = (3+4) + (4+2) = 7 + 6 = 13
//   fun(0, 0) = (0+0) + (0+2) = 0 + 2 = 2
//   fun(7, 3) = (7+3) + (3+2) = 10 + 5 = 15

// DEBUGGING WITH GDB:
//
// Compile with debug symbols:
//   as -g stacky.s -o stacky.o
//   ld stacky.o readInt.o writeInt.o -o stacky
//
// Debug:
//   gdb ./stacky
//   break fun           # Set breakpoint at fun
//   run                 # Start program
//   (enter inputs when prompted)
//   info registers      # See register values
//   info frame          # See stack frame info
//   x/2xg $rbp-16       # Examine local variables
//   stepi               # Step through instructions
//   backtrace           # See call stack
//
// This shows:
//   - Values of a and b in %rdi and %rsi
//   - Local variables c and d at -8(%rbp) and -16(%rbp)
//   - Stack frame structure
//   - Return value in %rax

// EXERCISES:
//
// 1. Modify fun to compute different formula:
//    - Try: c = a * b, d = a - b, return c + d
//    - Verify with different inputs
//
// 2. Add a third local variable:
//    - Increase stack space: sub $24,%rsp
//    - Allocate at -24(%rbp)
//    - Use in calculation
//
// 3. Add error checking:
//    - Check if inputs are positive
//    - Return error code if not
//    - Print error message
//
// 4. Optimize:
//    - Eliminate local variables
//    - Compute directly in registers
//    - Compare code size and clarity
//
// 5. Trace execution:
//    - Add print statements showing intermediate values
//    - Print c and d before computing result
//    - Verify calculations manually

// COMMON PITFALLS:
//
// 1. Forgetting prologue/epilogue:
//    - Corrupts caller's stack frame
//    - Crash or wrong results
//
// 2. Wrong stack alignment:
//    - System V ABI requires 16-byte alignment
//    - Some functions (like printf) require it
//    - Use multiples of 16 for stack allocation
//
// 3. Accessing beyond allocated space:
//    - If you allocate 16 bytes, only use -8(%rbp) and -16(%rbp)
//    - Don't access -24(%rbp) unless you allocated it
//    - Can corrupt other stack data
//
// 4. Mismatched push/pop:
//    - Every push needs a pop (or leave)
//    - Stack must be balanced
//    - Unbalanced stack causes wrong returns
//
// 5. Clobbering parameters:
//    - If you need parameters after a call, save them
//    - %rdi and %rsi are caller-saved
//    - Push them before calls that might modify them
