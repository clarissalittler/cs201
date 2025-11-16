# PEDAGOGICAL PURPOSE:
# This program demonstrates how to use LOCAL VARIABLES on the stack within a function.
# It shows the complete stack frame setup (prologue), usage of local variables via base
# pointer offsets, and proper cleanup (epilogue). This is essential for understanding
# how compilers implement functions with local variables.
#
# Key learning objectives:
# 1. Understanding the difference between parameters (passed in registers) and local variables (on stack)
# 2. Learning the function prologue: saving %rbp, setting up stack frame
# 3. Understanding how to allocate space for local variables on the stack
# 4. Learning to access local variables using negative offsets from %rbp
# 5. Understanding the function epilogue: cleaning up stack frame
# 6. Learning the 'leave' instruction as shorthand for epilogue
# 7. Understanding why %rbp provides a stable reference point
# 8. Seeing how stack frames enable recursive and nested function calls

.section .text               # CODE SECTION: Executable instructions
        .global _start       # Make _start visible to linker (entry point)

        ## here's a thing that will
        ## use local variables on the stack
        ## long fun(long a, long b){
        ##   long c = a + b;
        ##   long d = 2+b;
        ##   return (c + d);
        ## }

# FUNCTION: fun
# Purpose: Compute (a + b) + (2 + b)
# Parameters:
#   %rdi = a (first parameter)
#   %rsi = b (second parameter)
# Returns:
#   %rax = result: (a + b) + (2 + b)
# Local Variables:
#   c at %rbp - 8  (stores a + b)
#   d at %rbp - 16 (stores 2 + b)
#
# C EQUIVALENT:
# long fun(long a, long b) {
#     long c = a + b;     // Local variable
#     long d = 2 + b;     // Local variable
#     return (c + d);
# }
#
# STACK FRAME STRUCTURE:
# High Memory
# +------------------+
# | caller's frame   |
# +------------------+
# | return address   | <- Pushed by 'call' instruction
# +------------------+
# | saved %rbp       | <- We push this in prologue
# +------------------+ <- %rbp points here after "mov %rsp,%rbp"
# | c (8 bytes)      | <- at %rbp - 8
# +------------------+
# | d (8 bytes)      | <- at %rbp - 16
# +------------------+ <- %rsp points here after "sub $16,%rsp"
# Low Memory
#
# WHY THIS STRUCTURE?
# - %rbp provides stable reference (doesn't change during function)
# - %rsp can move (push/pop operations)
# - Local variables accessed via fixed negative offsets from %rbp
# - This enables predictable access to local variables

fun:
        # FUNCTION PROLOGUE - Setting up the stack frame
        # This is STANDARD for functions that need local variables

        ## first we need to save the old stack pointer into the base pointer
        ## the stack pointer is %rsp
        ## the base pointer is %rbp

        push %rbp            # SAVE OLD BASE POINTER
                            # Push caller's %rbp onto stack
                            # %rsp decreases by 8 (stack grows down)
                            # WHY? %rbp is a callee-saved register
                            # We must preserve it for the caller
                            # Also creates the frame pointer chain for debugging

        mov %rsp,%rbp        # ESTABLISH NEW BASE POINTER
                            # Copy current stack pointer to base pointer
                            # %rbp now points to the saved old %rbp
                            # This creates a STABLE REFERENCE POINT
                            # WHY? Even if we push/pop (changing %rsp),
                            # %rbp stays constant, making local variable offsets predictable
                            # AFTER THIS: %rbp = address of saved old %rbp

        sub $16,%rsp         # ALLOCATE SPACE FOR LOCAL VARIABLES
                            # Move stack pointer down 16 bytes
                            # This reserves space for our local variables
                            # WHY 16? We need 2 local variables × 8 bytes each
                            # Could allocate exactly what we need, but often align to 16 bytes
                            # AFTER THIS: %rsp points 16 bytes below %rbp

        ## set aside space for 16 bytes on the stack
        ## relative addressing
        ## -8(%rbp) to be our first variable
        ## -16(%rbp) to be our second variable

        # STACK FRAME NOW LOOKS LIKE:
        # %rbp + 8  : return address
        # %rbp + 0  : saved old %rbp  <- %rbp points here
        # %rbp - 8  : c (first local variable)
        # %rbp - 16 : d (second local variable)  <- %rsp points here

        # COMPUTE c = a + b
        # In C: long c = a + b;

        mov %rdi,-8(%rbp)    # STORE a IN LOCAL VARIABLE c
                            # %rdi contains first parameter (a)
                            # -8(%rbp) is the address of local variable c
                            # ADDRESSING MODE: displacement(%base)
                            # Effective address = %rbp - 8
                            # This initializes c with the value of a
                            # In C: c = a;

        add %rsi,-8(%rbp)    # ADD b TO LOCAL VARIABLE c
                            # %rsi contains second parameter (b)
                            # -8(%rbp) is still local variable c
                            # This adds b to c (which contained a)
                            # AFTER THIS: c contains (a + b)
                            # In C: c += b; (or equivalently, c = a + b)
                            #
                            # WHY DIRECT MEMORY OPERATION?
                            # x86-64 allows add with memory operands
                            # More efficient than load-add-store sequence

        # COMPUTE d = 2 + b
        # In C: long d = 2 + b;

        mov %rsi,-16(%rbp)   # STORE b IN LOCAL VARIABLE d
                            # %rsi still contains parameter b
                            # -16(%rbp) is the address of local variable d
                            # This initializes d with the value of b
                            # In C: d = b;

        addq $2,-16(%rbp)    # ADD 2 TO LOCAL VARIABLE d
                            # $2 is an immediate value (constant 2)
                            # -16(%rbp) is local variable d
                            # addq specifies quadword (8-byte) operation
                            # AFTER THIS: d contains (b + 2) or (2 + b)
                            # In C: d += 2; (or equivalently, d = 2 + b)
                            #
                            # NOTE: addq vs add
                            # addq explicitly specifies quadword size
                            # add would work too (assembler infers size)
                            # addq is clearer about the size being operated on

        # COMPUTE RETURN VALUE: c + d
        # In C: return (c + d);

        mov -16(%rbp),%rax   # LOAD d INTO RETURN REGISTER
                            # -16(%rbp) is local variable d (contains 2 + b)
                            # Move it to %rax (return value register)
                            # %rax now contains d

        add -8(%rbp),%rax    # ADD c TO RETURN REGISTER
                            # -8(%rbp) is local variable c (contains a + b)
                            # Add it to %rax
                            # AFTER THIS: %rax = d + c = (2 + b) + (a + b)
                            # Simplifies to: a + 2b + 2
                            # This is our return value

        # FUNCTION EPILOGUE - Cleaning up the stack frame
        # This restores the stack to its state before the function

        ## mov %rbp,%rsp
        ## pop %rbp

        leave                # SHORTHAND FOR EPILOGUE
                            # The 'leave' instruction does TWO things:
                            # 1. mov %rbp,%rsp  - Restore stack pointer from base pointer
                            #                     This deallocates local variables
                            # 2. pop %rbp       - Restore old base pointer
                            #                     This retrieves caller's %rbp
                            #
                            # WHY USE leave?
                            # - More concise than two instructions
                            # - Common idiom in x86-64 assembly
                            # - Compiler-generated code often uses it
                            # - Equivalent to the commented-out two instructions above
                            #
                            # AFTER leave:
                            # - %rsp points to return address
                            # - %rbp contains caller's base pointer
                            # - Local variables are abandoned (not erased, just unreachable)

        ret                  # RETURN TO CALLER
                            # Pop return address from stack and jump to it
                            # Return value in %rax: (a + b) + (2 + b)
                            # When called from _start with a=3, b=4:
                            # %rax = (3 + 4) + (2 + 4) = 7 + 6 = 13

# WHY DO WE NEED BOTH %rbp AND %rsp?
#
# PROBLEM WITH USING ONLY %rsp:
# If we only used %rsp for accessing locals:
#   sub $16,%rsp        # Allocate space
#   mov %rdi,0(%rsp)    # c at %rsp + 0
#   mov %rsi,8(%rsp)    # d at %rsp + 8
#   push %rcx           # Save a register - %rsp changes!
#   # Now c is at %rsp + 8, d is at %rsp + 16 - offsets changed!
#   # Hard to track! Error-prone!
#
# SOLUTION WITH %rbp:
#   %rbp never changes during function execution
#   c is ALWAYS at %rbp - 8
#   d is ALWAYS at %rbp - 16
#   Even if we push/pop, offsets from %rbp remain constant
#   Easy to track, reliable, used by compilers

# PROGRAM ENTRY POINT
_start:
        mov $3,%rdi          # SET FIRST ARGUMENT: a = 3
                            # Preparing to call fun(3, 4)
                            # %rdi is the first integer parameter

        mov $4,%rsi          # SET SECOND ARGUMENT: b = 4
                            # %rsi is the second integer parameter
                            # Parameters are now set up

        call fun             # CALL THE FUNCTION
                            # What happens:
                            # 1. Push return address (address of next instruction)
                            # 2. Jump to fun
                            # 3. fun executes:
                            #    c = 3 + 4 = 7
                            #    d = 2 + 4 = 6
                            #    return 7 + 6 = 13
                            # 4. ret returns here
                            # 5. %rax = 13

        mov %rax,%rdi        # USE RETURN VALUE AS EXIT CODE
                            # %rax contains 13 (result from fun)
                            # Move it to %rdi for exit syscall
                            # Verification: echo $? should show 13

        mov $60,%rax         # SYSCALL NUMBER: 60 = sys_exit

        syscall              # EXIT WITH STATUS 13

# STEP-BY-STEP EXECUTION TRACE:
#
# INITIAL STATE (in _start):
#   %rsp = 0x7fffffffe000 (example)
#   %rbp = 0x0 (not used yet in _start)
#
# _start:
#   mov $3,%rdi         -> %rdi = 3
#   mov $4,%rsi         -> %rsi = 4
#   call fun            -> Push return address to 0x7fffffffdff8
#                          Jump to fun
#
# fun - PROLOGUE:
#   push %rbp           -> Save old %rbp at 0x7fffffffdff0
#                          %rsp = 0x7fffffffdff0
#   mov %rsp,%rbp       -> %rbp = 0x7fffffffdff0
#   sub $16,%rsp        -> %rsp = 0x7fffffffdfe0
#
# STACK NOW:
#   0x7fffffffdfe0: (space for d)      <- %rsp
#   0x7fffffffdfe8: (space for c)      <- %rbp - 8
#   0x7fffffffdff0: saved old %rbp     <- %rbp
#   0x7fffffffdff8: return address
#
# fun - BODY:
#   mov %rdi,-8(%rbp)   -> memory[0x7fffffffdfe8] = 3 (c = a)
#   add %rsi,-8(%rbp)   -> memory[0x7fffffffdfe8] = 3 + 4 = 7 (c = a + b)
#   mov %rsi,-16(%rbp)  -> memory[0x7fffffffdfe0] = 4 (d = b)
#   addq $2,-16(%rbp)   -> memory[0x7fffffffdfe0] = 4 + 2 = 6 (d = 2 + b)
#   mov -16(%rbp),%rax  -> %rax = 6
#   add -8(%rbp),%rax   -> %rax = 6 + 7 = 13
#
# fun - EPILOGUE:
#   leave               -> mov %rbp,%rsp: %rsp = 0x7fffffffdff0
#                          pop %rbp: %rbp = old value, %rsp = 0x7fffffffdff8
#   ret                 -> Pop return address, jump back to _start
#                          %rsp = 0x7fffffffe000 (back to original)
#
# _start (continued):
#   mov %rax,%rdi       -> %rdi = 13
#   mov $60,%rax        -> %rax = 60
#   syscall             -> Exit with code 13

# DETAILED STACK FRAME DIAGRAM:
#
# BEFORE call fun:
# +------------------+ <- %rsp (0x7fffffffe000)
# | ...              |
# +------------------+
#
# AFTER call fun (before fun's prologue):
# +------------------+
# | ...              |
# +------------------+
# | return address   | <- %rsp (0x7fffffffdff8)
# +------------------+
#
# AFTER push %rbp:
# +------------------+
# | ...              |
# +------------------+
# | return address   | (0x7fffffffdff8)
# +------------------+
# | saved %rbp       | <- %rsp (0x7fffffffdff0)
# +------------------+
#
# AFTER mov %rsp,%rbp:
# +------------------+
# | ...              |
# +------------------+
# | return address   | (0x7fffffffdff8)
# +------------------+
# | saved %rbp       | <- %rbp, %rsp (0x7fffffffdff0)
# +------------------+
#
# AFTER sub $16,%rsp (FINAL FRAME):
# +------------------+
# | ...              |
# +------------------+
# | return address   | <- %rbp + 8 (0x7fffffffdff8)
# +------------------+
# | saved %rbp       | <- %rbp (0x7fffffffdff0)
# +------------------+
# | c = a + b        | <- %rbp - 8 (0x7fffffffdfe8)
# +------------------+
# | d = 2 + b        | <- %rbp - 16, %rsp (0x7fffffffdfe0)
# +------------------+

# UNDERSTANDING 'leave' INSTRUCTION:
#
# Manual epilogue (two instructions):
#   mov %rbp,%rsp    # Deallocate locals by resetting %rsp
#   pop %rbp         # Restore caller's base pointer
#
# Using leave (one instruction):
#   leave            # Does both operations above
#
# WHEN TO USE leave:
# - Common in functions with stack frames
# - Cleaner, more readable
# - Slight performance difference (negligible on modern CPUs)
# - Standard practice in x86-64 assembly
#
# WHEN NOT TO USE leave:
# - In functions without stack frames (leaf functions)
# - When you want to show each step for educational purposes
# - When you need different cleanup (rare)

# MEMORY ACCESS MODES USED:
#
# 1. IMMEDIATE: $2, $16, $60
#    A constant value embedded in the instruction
#    Example: addq $2,-16(%rbp)
#
# 2. REGISTER DIRECT: %rax, %rdi, %rsi
#    Value is in the register itself
#    Example: mov %rdi,-8(%rbp)
#
# 3. BASE + DISPLACEMENT: -8(%rbp), -16(%rbp)
#    Address = base register + signed displacement
#    Example: mov -16(%rbp),%rax
#    Effective address = %rbp - 16

# TO ASSEMBLE, LINK, AND RUN:
#
# as 03-function-parameters.s -o function-parameters.o
# ld function-parameters.o -o function-parameters
# ./function-parameters
# echo $?
# 13
#
# EXPECTED OUTPUT:
# The program exits with status 13
# Calculation: (3 + 4) + (2 + 4) = 7 + 6 = 13

# EXPERIMENTS TO TRY:
#
# 1. Change input values:
#    mov $10,%rdi  # a = 10
#    mov $5,%rsi   # b = 5
#    Expected: (10 + 5) + (2 + 5) = 15 + 7 = 22
#
# 2. Add a third local variable:
#    sub $24,%rsp  # Allocate 24 bytes for 3 variables
#    movq $100,-24(%rbp)  # e = 100
#    add -24(%rbp),%rax   # Add e to result
#
# 3. Trace with GDB:
#    gdb ./function-parameters
#    break fun
#    run
#    info registers rbp rsp
#    x/2gx $rbp-16  # Examine c and d
#    stepi
#    print $rax

# COMMON MISTAKES TO AVOID:
#
# 1. POSITIVE OFFSETS FROM %rbp:
#    mov %rdi,8(%rbp)  # WRONG! This writes into caller's frame
#    Local variables MUST use negative offsets
#
# 2. NOT ALLOCATING ENOUGH SPACE:
#    sub $8,%rsp       # Only 8 bytes
#    mov %rdi,-8(%rbp)   # OK
#    mov %rsi,-16(%rbp)  # WRONG! Writing beyond allocated space
#
# 3. FORGETTING EPILOGUE:
#    # Missing leave or manual cleanup
#    ret  # WRONG! Stack corrupted, %rbp not restored
#
# 4. WRONG OFFSET CALCULATIONS:
#    # If you allocate sub $16,%rsp
#    # First variable is at -8, not -16
#    # Second variable is at -16, not -8
#
# 5. MIXING UP %rsp AND %rbp:
#    mov %rdi,-8(%rsp)  # FRAGILE! %rsp can change
#    Should use: mov %rdi,-8(%rbp)
#
# 6. NOT PRESERVING %rbp:
#    # Forgetting "push %rbp" in prologue
#    # Caller's %rbp gets lost - hard to debug!

# COMPARISON: FUNCTION WITH vs WITHOUT LOCAL VARIABLES:
#
# WITHOUT locals (02-basic-function.s):
#   fun:
#       mov $0,%rax
#       add %rdi,%rax
#       add %rsi,%rax
#       imul $3,%rax
#       ret
#   No prologue, no epilogue, simple and fast
#
# WITH locals (this file):
#   fun:
#       push %rbp          # Prologue
#       mov %rsp,%rbp
#       sub $16,%rsp
#       # ... use locals ...
#       leave              # Epilogue
#       ret
#   More overhead, but enables complex logic with locals

# REAL-WORLD USAGE:
#
# Compilers generate this pattern for functions that:
# - Have local variables
# - Need temporary storage beyond registers
# - Make multiple function calls (saving state)
# - Use arrays or structures
# - Need addressable variables (&var in C)
#
# Modern optimizations:
# - Use registers for locals when possible (faster)
# - Omit frame pointer with -fomit-frame-pointer (saves %rbp)
# - Inline small functions (eliminate call overhead)
# - But the CONCEPT remains: locals go on stack when needed

# STACK ALIGNMENT NOTES:
#
# The x86-64 ABI requires 16-byte stack alignment before 'call'
# Our allocation of 16 bytes maintains this alignment:
# - Before call: %rsp aligned to 16 bytes
# - call pushes 8 bytes (return address): misaligned
# - push %rbp pushes 8 bytes: aligned again
# - sub $16,%rsp: still aligned
#
# If we allocated 8 or 24 bytes, we'd violate alignment
# Some functions require proper alignment (e.g., SSE instructions)
