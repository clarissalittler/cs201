# PEDAGOGICAL PURPOSE:
# This program demonstrates the critical concept of stack frames and local variables
# in assembly. It introduces the function prologue/epilogue pattern essential for
# any non-trivial function that needs its own local storage.
#
# Key learning objectives:
# 1. Understanding stack frames and why they're necessary
# 2. Learning the function prologue (push %rbp, mov %rsp,%rbp, sub from %rsp)
# 3. Understanding the function epilogue (leave instruction)
# 4. Learning how local variables are stored on the stack with negative offsets
# 5. Understanding %rbp (base pointer) vs %rsp (stack pointer)
# 6. Seeing the difference between functions with and without stack frames
# 7. Understanding the leave instruction as a shorthand for epilogue
#
# CONCEPTUAL OVERVIEW:
# This program implements:
#   long fun(long a, long b) {
#       long c = a + b;
#       long d = 2 + b;
#       return (c + d);
#   }
#   int main() {
#       return fun(3, 4);  // Returns (3+4) + (2+4) = 7 + 6 = 13
#   }

        .text                      # CODE SECTION: Executable instructions
        .global _start             # GLOBAL DIRECTIVE: Entry point visible to linker

        ## FUNCTION COMMENT (from original):
        ## Here's a thing that will use local variables on the stack
        ##
        ## C EQUIVALENT:
        ## long fun(long a, long b) {
        ##     long c = a + b;
        ##     long d = 2 + b;
        ##     return (c + d);
        ## }

fun:                              # FUNCTION LABEL: Entry point for function with locals
                                  # This function needs LOCAL VARIABLES on the stack
                                  # Unlike fun1.s, we can't do everything in registers
                                  # We need storage for 'c' and 'd'

        ## FUNCTION PROLOGUE: Set up stack frame
        ## This is the standard pattern for functions with local variables

        push %rbp                  # SAVE OLD BASE POINTER: Push caller's %rbp onto stack
                                  # CRITICAL FIRST STEP of function prologue
                                  # WHY? Caller might be using %rbp, we need to preserve it
                                  # Stack before: [...]
                                  # Stack after:  [..., old_rbp]
                                  # %rsp now points to saved %rbp
                                  #
                                  # CALLING CONVENTION:
                                  # %rbp is a CALLEE-SAVED register
                                  # If we use it, we MUST save and restore it

        mov %rsp,%rbp              # ESTABLISH NEW BASE POINTER: %rbp = %rsp
                                  # Copy stack pointer to base pointer
                                  # WHY? Creates a stable reference point for local variables
                                  # %rsp will change as we push/pop, but %rbp stays constant
                                  # This makes addressing locals easier: -8(%rbp), -16(%rbp)
                                  #
                                  # AT THIS POINT:
                                  # %rbp points to the saved old %rbp
                                  # This is the BASE of our stack frame
                                  #
                                  # STACK LAYOUT NOW:
                                  # %rbp -> [old_rbp]  <- we're here
                                  # (higher addresses above)

        sub $16,%rsp               # ALLOCATE STACK SPACE: Reserve 16 bytes for locals
                                  # Subtract from stack pointer to make room
                                  # Stack grows DOWNWARD (toward lower addresses)
                                  # WHY 16 bytes for two longs?
                                  # - Each long is 8 bytes
                                  # - 2 longs = 16 bytes
                                  # - x86-64 ABI requires 16-byte stack alignment
                                  #
                                  # STACK LAYOUT NOW:
                                  # Higher addresses
                                  # %rbp + 0  -> [old_rbp]
                                  # %rbp - 8  -> [space for first variable 'c']
                                  # %rbp - 16 -> [space for second variable 'd']
                                  # %rsp ->      (top of stack, 16 bytes below %rbp)
                                  # Lower addresses

        ## COMMENTS FROM ORIGINAL:
        ## Set aside space for 16 bytes on the stack
        ## Using relative addressing:
        ## -8(%rbp) to be our first variable (c)
        ## -16(%rbp) to be our second variable (d)

        ## COMPUTE c = a + b
        mov %rdi,-8(%rbp)          # STORE PARAMETER: Copy 'a' to local variable 'c'
                                  # %rdi contains parameter 'a' (3 when called)
                                  # -8(%rbp) is our local variable 'c'
                                  # Address: %rbp - 8 (8 bytes below base pointer)
                                  # We're initializing c with the value of a
                                  # c = a; (first step toward c = a + b)

        add %rsi,-8(%rbp)          # ADD TO LOCAL: c = c + b
                                  # %rsi contains parameter 'b' (4 when called)
                                  # -8(%rbp) is our local variable 'c'
                                  # This adds b to c, completing: c = a + b
                                  # Memory at -8(%rbp) now contains: 3 + 4 = 7
                                  #
                                  # OPERATION BREAKDOWN:
                                  # 1. Read value at address (%rbp - 8) = 3
                                  # 2. Add %rsi (4) to it = 7
                                  # 3. Write result (7) back to address (%rbp - 8)
                                  #
                                  # WHY MEMORY OPERAND?
                                  # add can work directly with memory!
                                  # Saves us from: mov to register, add, mov back

        ## COMPUTE d = 2 + b
        mov %rsi,-16(%rbp)         # STORE PARAMETER: Copy 'b' to local variable 'd'
                                  # %rsi contains parameter 'b' (4)
                                  # -16(%rbp) is our local variable 'd'
                                  # Address: %rbp - 16 (16 bytes below base pointer)
                                  # We're initializing d with the value of b
                                  # d = b; (first step toward d = 2 + b)

        addq $2,-16(%rbp)          # ADD TO LOCAL: d = d + 2
                                  # Add immediate value 2 to variable 'd'
                                  # addq = add quad word (64-bit)
                                  # -16(%rbp) is our local variable 'd'
                                  # Memory at -16(%rbp) now contains: 4 + 2 = 6
                                  #
                                  # WHY addq NOT add?
                                  # Being explicit about 64-bit operation
                                  # Works on the full 8-byte long value
                                  #
                                  # AT THIS POINT:
                                  # -8(%rbp) contains c = 7
                                  # -16(%rbp) contains d = 6

        ## COMPUTE RETURN VALUE: return c + d
        mov -16(%rbp),%rax         # LOAD d INTO RESULT: %rax = d (6)
                                  # Read 'd' from stack into return value register
                                  # %rax will hold our final result
                                  # Start with d, then add c

        add -8(%rbp),%rax          # ADD c TO RESULT: %rax = %rax + c
                                  # Read 'c' from stack and add to %rax
                                  # %rax = 6 + 7 = 13
                                  # This is our final return value: c + d

        ## FUNCTION EPILOGUE: Tear down stack frame and return
        ## We need to:
        ## 1. Restore %rsp to its pre-allocation value
        ## 2. Restore caller's %rbp
        ## 3. Return to caller

        ## COMMENTED OUT MANUAL EPILOGUE (from original):
        ## mov %rbp,%rsp           # Restore stack pointer
        ## pop %rbp                # Restore base pointer
        ##
        ## This is the manual way to clean up the stack frame
        ## It's equivalent to the 'leave' instruction below

        leave                      # LEAVE INSTRUCTION: Shorthand for epilogue
                                  # EQUIVALENT TO:
                                  #   mov %rbp,%rsp    (restore stack pointer)
                                  #   pop %rbp         (restore base pointer)
                                  #
                                  # WHAT IT DOES:
                                  # 1. Copy %rbp to %rsp (deallocate local variables)
                                  #    Now %rsp points to saved %rbp
                                  # 2. Pop %rbp (restore caller's base pointer)
                                  #    Now %rbp has caller's value
                                  #    %rsp points to return address
                                  #
                                  # WHY USE leave?
                                  # - Single instruction vs two
                                  # - Clearer intent (epilogue pattern)
                                  # - Standard convention
                                  # - Slightly more efficient

        ret                        # RETURN: Pop return address and jump to caller
                                  # Stack now has return address on top
                                  # ret pops it and jumps back to _start
                                  # Return value (13) is in %rax

_start:                           # PROGRAM ENTRY POINT: Execution begins here

        mov $3,%rdi                # FIRST ARGUMENT: %rdi = 3 (parameter 'a')
                                  # Set up first parameter for fun(3, 4)

        mov $4,%rsi                # SECOND ARGUMENT: %rsi = 4 (parameter 'b')
                                  # Set up second parameter for fun(3, 4)

        call fun                   # CALL FUNCTION: Jump to fun, save return address
                                  # EXECUTION TRACE:
                                  # 1. Push return address onto stack
                                  # 2. Jump to fun
                                  # 3. fun prologue: push %rbp, mov %rsp,%rbp, sub $16,%rsp
                                  # 4. c = 3 + 4 = 7 (stored at %rbp-8)
                                  # 5. d = 2 + 4 = 6 (stored at %rbp-16)
                                  # 6. %rax = 6 + 7 = 13
                                  # 7. fun epilogue: leave (restore %rsp, restore %rbp)
                                  # 8. ret (return to here)
                                  # 9. %rax contains 13

        mov %rax,%rdi              # USE RETURN VALUE AS EXIT CODE: %rdi = %rax (13)
                                  # Transfer function result to exit code register
                                  # This will be our program's exit code

        mov $60,%rax               # SYSCALL NUMBER: 60 = exit
                                  # Prepare to terminate the program

        syscall                    # INVOKE EXIT: Execute exit(13)
                                  # Program terminates with exit code 13

# STACK FRAME ANATOMY:
#
# When fun is executing:
#
# Higher addresses
#   ...
#   [caller's stack]
#   [return address]      <- pushed by 'call'
#   [saved %rbp]          <- pushed by 'push %rbp'  <- %rbp points here
#   [local var c (8 bytes)]  <- at %rbp - 8
#   [local var d (8 bytes)]  <- at %rbp - 16        <- %rsp points here
# Lower addresses
#
# Accessing locals:
#   c is at -8(%rbp)
#   d is at -16(%rbp)

# WHY USE %rbp (BASE POINTER)?
#
# Without %rbp (using only %rsp):
#   - Must track %rsp changes carefully
#   - Every push/pop changes offsets to locals
#   - Example: if we push %rax, locals move 8 bytes
#   - Fragile and error-prone
#
# With %rbp (base pointer):
#   - %rbp stays constant during function
#   - Locals always at same offset from %rbp
#   - Can push/pop freely without affecting local access
#   - Much easier to write and maintain

# FUNCTION PROLOGUE PATTERN:
#
# Standard prologue for functions with locals:
#   push %rbp           # Save caller's base pointer
#   mov %rsp,%rbp       # Establish our base pointer
#   sub $N,%rsp         # Allocate N bytes for locals
#
# Where N is:
#   - Size of all local variables
#   - Rounded up to 16-byte alignment
#   - May be larger for alignment requirements

# FUNCTION EPILOGUE PATTERNS:
#
# Manual epilogue:
#   mov %rbp,%rsp       # Deallocate locals
#   pop %rbp            # Restore caller's base pointer
#   ret                 # Return
#
# Using leave:
#   leave               # Equivalent to above two lines
#   ret                 # Return
#
# Both are equivalent; leave is more common in modern code.

# STACK ALIGNMENT:
#
# x86-64 ABI requires:
#   - Stack 16-byte aligned before 'call'
#   - If you allocate locals, round to 16 bytes
#
# Example:
#   1 long (8 bytes) -> allocate 16 bytes (8 unused)
#   2 longs (16 bytes) -> allocate 16 bytes (perfect)
#   3 longs (24 bytes) -> allocate 32 bytes (8 unused)
#
# WHY?
#   - SSE instructions require aligned access
#   - Performance optimization
#   - ABI requirement for calling external functions

# MEMORY OPERATIONS WITH OFFSETS:
#
# Reading:
#   mov -8(%rbp),%rax       # %rax = value at (%rbp - 8)
#
# Writing:
#   mov %rax,-8(%rbp)       # Store %rax at (%rbp - 8)
#
# Arithmetic in memory:
#   add $2,-8(%rbp)         # Add 2 to value at (%rbp - 8)
#   add %rax,-8(%rbp)       # Add %rax to value at (%rbp - 8)
#
# This is more efficient than:
#   mov -8(%rbp),%rax
#   add $2,%rax
#   mov %rax,-8(%rbp)

# CALLEE-SAVED REGISTERS:
#
# These must be preserved by the function:
#   %rbp, %rbx, %r12-r15
#
# If a function uses these, it must:
#   1. Save them (push) in prologue
#   2. Restore them (pop) in epilogue
#
# This function uses %rbp, so we save/restore it.

# EQUIVALENT C CODE:
#
# long fun(long a, long b) {
#     long c = a + b;     // -8(%rbp)
#     long d = 2 + b;     // -16(%rbp)
#     return (c + d);
# }
#
# int main() {
#     long result = fun(3, 4);
#     return result;  // returns 13
# }
#
# Calculation:
#   a = 3, b = 4
#   c = 3 + 4 = 7
#   d = 2 + 4 = 6
#   return 7 + 6 = 13

# COMPARISON WITH fun1.s:
#
# fun1.s (no stack frame):
#   - No local variables
#   - All work in registers
#   - No prologue/epilogue
#   - Simpler but limited
#
# fun2.s (with stack frame):
#   - Has local variables
#   - Uses stack for storage
#   - Full prologue/epilogue
#   - More complex but powerful

# TO ASSEMBLE, LINK, AND RUN:
# as fun2.s -o fun2.o
# ld fun2.o -o fun2
# ./fun2
# echo $?                          # Should display 13

# EXPERIMENT:
# Try different values:
#   mov $10,%rdi
#   mov $5,%rsi
# Result: c = 10+5 = 15, d = 2+5 = 7, return = 15+7 = 22
#
# Modify the computation:
#   addq $10,-16(%rbp)    # d = b + 10
# With (3,4): c = 7, d = 14, return = 21
