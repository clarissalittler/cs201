# PEDAGOGICAL PURPOSE:
# This program demonstrates the FUNDAMENTALS of writing and calling functions in x86-64 assembly.
# It shows how to define a simple function, pass parameters via registers, perform computations,
# and return a value to the caller. This is the simplest possible function example.
#
# Key learning objectives:
# 1. Understanding how to define a function with a label
# 2. Learning the x86-64 calling convention for parameter passing
# 3. Understanding how to return values using %rax
# 4. Learning how call and ret instructions work together
# 5. Seeing the relationship between assembly functions and C functions
# 6. Understanding register usage for parameters and return values
# 7. Learning when stack frames are needed (vs when they're not)

.section .text               # CODE SECTION: Where executable instructions live
        .globl _start        # Make _start visible to the linker (entry point)

        ## this function adds two numbers
        ## then multiplies by 3
        ## then returns that

# FUNCTION: fun
# Purpose: Compute 3 * (a + b)
# Parameters:
#   %rdi = first parameter (a)
#   %rsi = second parameter (b)
# Returns:
#   %rax = result (3 * (a + b))
#
# C EQUIVALENT:
# int fun(int a, int b) {
#     return 3 * (a + b);
# }
#
# WHY NO STACK FRAME?
# This is a "leaf function" (doesn't call other functions) with no local variables.
# We don't need to save/restore %rbp or allocate stack space.
# All computation can be done directly in registers.
#
fun:
        mov $0,%rax          # INITIALIZE RESULT: Set %rax to 0
                            # %rax is the standard register for return values
                            # WHY clear first? Start with a clean slate for our computation
                            # Could skip this and use: mov %rdi,%rax (slightly more efficient)
                            # but this version is clearer for learning

        add %rdi,%rax        # ADD FIRST PARAMETER: %rax = 0 + %rdi
                            # %rdi contains first parameter (a)
                            # x86-64 calling convention: 1st integer arg in %rdi
                            # When called from _start: %rdi = 2
                            # After this: %rax = 2

        add %rsi,%rax        # ADD SECOND PARAMETER: %rax = %rax + %rsi
                            # %rsi contains second parameter (b)
                            # x86-64 calling convention: 2nd integer arg in %rsi
                            # When called from _start: %rsi = 3
                            # After this: %rax = 2 + 3 = 5
                            # Now %rax holds (a + b)

        imul $3,%rax         # MULTIPLY BY 3: %rax = %rax * 3
                            # imul is multiplication of SIGNED integers
                            # $3 is an immediate value (constant 3)
                            # After this: %rax = 5 * 3 = 15
                            # This is our final result: 3 * (a + b)
                            #
                            # NOTE: imul vs mul
                            # - imul: signed integer multiplication (use for signed values)
                            # - mul: unsigned integer multiplication (use for unsigned values)
                            # For positive numbers they give same result, but differ for negative

        ret                  # RETURN TO CALLER
                            # Pops return address from stack and jumps to it
                            # Return value (15) is in %rax
                            # Control transfers back to instruction after "call fun"
                            #
                            # WHAT RET DOES:
                            # 1. Pop the top of stack into %rip (instruction pointer)
                            # 2. Execution continues at that address
                            # This undoes what "call" did (call pushed return address)

# X86-64 CALLING CONVENTION DETAILS:
#
# INTEGER/POINTER PARAMETERS (in order):
#   1st parameter: %rdi
#   2nd parameter: %rsi
#   3rd parameter: %rdx
#   4th parameter: %rcx
#   5th parameter: %r8
#   6th parameter: %r9
#   7th+ parameters: passed on the stack
#
# RETURN VALUE:
#   %rax (for integers/pointers)
#   %xmm0 (for floating point)
#
# CALLER-SAVED REGISTERS (caller must save if needed):
#   %rax, %rcx, %rdx, %rsi, %rdi, %r8, %r9, %r10, %r11
#   These may be modified by called functions
#
# CALLEE-SAVED REGISTERS (callee must preserve):
#   %rbx, %rbp, %r12, %r13, %r14, %r15
#   If a function uses these, it must save and restore them

        ## int fun(int a,int b)
        ## ...
        ## return 3*(a + b);

# PROGRAM ENTRY POINT
_start:
        mov $2,%rdi          # SET FIRST ARGUMENT: %rdi = 2
                            # Preparing to call fun(2, 3)
                            # %rdi will be the first parameter (a)

        mov $3,%rsi          # SET SECOND ARGUMENT: %rsi = 3
                            # %rsi will be the second parameter (b)
                            # Both parameters are now set up according to calling convention

        call fun             # CALL THE FUNCTION
                            # WHAT HAPPENS DURING "call fun":
                            # 1. Push return address onto stack (%rsp decreases by 8)
                            #    Return address = address of next instruction (mov %rax,%rdi)
                            # 2. Jump to the 'fun' label
                            # 3. Function executes: computes 3 * (2 + 3) = 15
                            # 4. 'ret' pops return address and jumps back
                            # 5. %rax now contains 15 (the return value)

        mov %rax,%rdi        # USE RETURN VALUE AS EXIT CODE
                            # %rax contains 15 (result from fun)
                            # Move it to %rdi to use as program exit code
                            # %rdi is the first parameter for syscalls
                            # For exit syscall, %rdi is the exit status
                            #
                            # VERIFICATION: After running, check with:
                            # echo $?
                            # Should display: 15

        mov $60,%rax         # SYSCALL NUMBER: 60 = sys_exit
                            # Setting up to terminate the program
                            # Linux syscall numbers defined in <sys/syscall.h>

        syscall              # INVOKE SYSTEM CALL
                            # Transfers control to kernel
                            # Kernel performs exit with status code 15
                            # Program terminates

# STACK BEHAVIOR WALKTHROUGH:
#
# BEFORE call fun (in _start):
#   %rsp points to: [initial stack position]
#   Stack: [...]
#
# DURING call fun (executing "call fun"):
#   %rsp decreases by 8
#   Stack: [..., return_address]
#   return_address = address of "mov %rax,%rdi"
#   %rip (instruction pointer) = address of "mov $0,%rax" (first instruction in fun)
#
# INSIDE fun (before ret):
#   %rsp unchanged (we don't modify it)
#   Stack: [..., return_address]
#   %rax = 15 (computed result)
#
# DURING ret (executing "ret"):
#   Pop return_address from stack into %rip
#   %rsp increases by 8 (back to original position)
#   Execution jumps to "mov %rax,%rdi"
#
# AFTER ret (back in _start):
#   Stack: [...]
#   %rax = 15 (return value from fun)
#   Continue executing at "mov %rax,%rdi"

# MEMORY DIAGRAM:
#
# When "call fun" executes:
#
# High Memory
# +------------------+
# | ...              |
# +------------------+
# | return address   | <- %rsp points here (pushed by call)
# +------------------+    Value: address of "mov %rax,%rdi"
# | ...              |
# +------------------+
# Low Memory
#
# When "ret" executes:
# - Pops return address from stack
# - Jumps to that address
# - %rsp moves back up (stack shrinks)

# STEP-BY-STEP EXECUTION TRACE:
#
# _start:
#   mov $2,%rdi         -> %rdi = 2
#   mov $3,%rsi         -> %rsi = 3
#   call fun            -> Push return address, jump to fun
#
# fun:
#   mov $0,%rax         -> %rax = 0
#   add %rdi,%rax       -> %rax = 0 + 2 = 2
#   add %rsi,%rax       -> %rax = 2 + 3 = 5
#   imul $3,%rax        -> %rax = 5 * 3 = 15
#   ret                 -> Return to _start, %rax = 15
#
# _start (continued):
#   mov %rax,%rdi       -> %rdi = 15
#   mov $60,%rax        -> %rax = 60
#   syscall             -> Exit with status 15

# WHY THIS EXAMPLE IS IMPORTANT:
#
# 1. SIMPLEST FUNCTION POSSIBLE
#    - No stack frame needed
#    - No local variables
#    - Pure computation in registers
#    - Easy to understand the basics
#
# 2. CALLING CONVENTION
#    - Shows parameter passing via %rdi, %rsi
#    - Shows return value via %rax
#    - Foundation for more complex functions
#
# 3. CALL/RET MECHANICS
#    - Demonstrates how call pushes return address
#    - Shows how ret pops and jumps back
#    - Stack grows and shrinks automatically
#
# 4. LEAF FUNCTION
#    - Doesn't call other functions
#    - Doesn't need to save registers
#    - No prologue/epilogue required
#    - Efficient and simple

# COMPARISON WITH MORE COMPLEX FUNCTIONS:
#
# This simple function:
#   fun:
#       mov $0,%rax
#       add %rdi,%rax
#       add %rsi,%rax
#       imul $3,%rax
#       ret
#
# vs. Function with local variables (would need stack frame):
#   complexFun:
#       push %rbp           # Save old base pointer
#       mov %rsp,%rbp       # Set up new base pointer
#       sub $16,%rsp        # Allocate space for locals
#       # ... use local variables ...
#       mov %rbp,%rsp       # Deallocate locals
#       pop %rbp            # Restore base pointer
#       ret
#
# Our function is simpler because:
# - No local variables needed
# - No function calls made
# - All work done in registers

# TO ASSEMBLE, LINK, AND RUN:
#
# as 02-basic-function.s -o basic-function.o
# ld basic-function.o -o basic-function
# ./basic-function
# echo $?
# 15
#
# EXPECTED OUTPUT:
# The program exits with status 15 (which is 3 * (2 + 3))
# No text output - result is in the exit code

# EXPERIMENTS TO TRY:
#
# 1. Change the parameters:
#    mov $5,%rdi
#    mov $10,%rsi
#    Expected result: 3 * (5 + 10) = 45
#
# 2. Change the multiplier:
#    imul $7,%rax
#    Expected result: 7 * (2 + 3) = 35
#
# 3. Add more operations:
#    After imul, add:
#    add $100,%rax
#    Expected result: 3 * (2 + 3) + 100 = 115
#
# 4. Trace with GDB:
#    gdb ./basic-function
#    break fun
#    run
#    stepi (step through each instruction)
#    info registers rax rdi rsi (check register values)

# COMMON MISTAKES TO AVOID:
#
# 1. WRONG RETURN REGISTER:
#    mov %rax,%rbx; ret  # WRONG! Return value must be in %rax
#    Caller expects result in %rax, not %rbx
#
# 2. FORGETTING TO SET PARAMETERS:
#    call fun  # WRONG! %rdi and %rsi have garbage values
#    Must set up parameters before call
#
# 3. OVERWRITING RETURN VALUE:
#    call fun
#    mov $0,%rax  # WRONG! Destroys return value
#    mov %rax,%rdi  # Now %rdi gets 0, not the function result
#
# 4. USING WRONG REGISTERS FOR PARAMETERS:
#    mov $2,%rax  # WRONG! First parameter goes in %rdi
#    mov $3,%rbx  # WRONG! Second parameter goes in %rsi
#    call fun
#
# 5. NOT PRESERVING CALLER-SAVED REGISTERS:
#    # If you need %rdi after the call:
#    mov $2,%rdi
#    # ... need %rdi later ...
#    call fun  # WRONG! fun might modify %rdi
#    # Should save %rdi first:
#    push %rdi
#    call fun
#    pop %rdi

# EXTENDING THIS EXAMPLE:
#
# To call with different values at runtime:
# - Read input from user (using readInt syscall)
# - Call fun with those values
# - Print result (using writeInt syscall)
#
# Example:
#   call readInt    # Get first number in %rax
#   mov %rax,%rdi   # Move to first parameter
#   call readInt    # Get second number in %rax
#   mov %rax,%rsi   # Move to second parameter
#   call fun        # Compute result
#   mov %rax,%rdi   # Set up for writeInt
#   call writeInt   # Display result
