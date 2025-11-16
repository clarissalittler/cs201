### PEDAGOGICAL PURPOSE:
### This program demonstrates RECURSIVE FUNCTION CALLS in x86-64 assembly.
### It implements the Fibonacci sequence using recursion, showing how the stack
### enables multiple simultaneous function activations, how to preserve state
### across recursive calls, and how to handle base cases vs recursive cases.
###
### Key learning objectives:
### 1. Understanding recursive function calls and how they use the stack
### 2. Learning how to preserve register values across function calls
### 3. Understanding the importance of base cases to terminate recursion
### 4. Seeing how each function call gets its own stack frame
### 5. Learning to use local variables to save intermediate results
### 6. Understanding the overhead of function calls vs iteration
### 7. Visualizing the call stack growing and shrinking during recursion
### 8. Learning how external functions (readInt, writeInt) integrate with our code

    .section .text            # CODE SECTION: Where the instructions are stored
    .global _start            # GLOBAL DIRECTIVE: Define _start as entry point for linker
    .extern readInt           # EXTERNAL DECLARATION: readInt function defined elsewhere
    .extern writeInt          # EXTERNAL DECLARATION: writeInt function defined elsewhere
                             # These are typically provided by a library or separate object file

# FIBONACCI SEQUENCE REFRESHER:
# fib(0) = 0
# fib(1) = 1
# fib(n) = fib(n-1) + fib(n-2) for n > 1
#
# Example sequence: 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, ...
#
# C EQUIVALENT OF THIS FUNCTION:
# long fib(long n) {
#     if (n <= 1) {
#         return n;           // Base case
#     }
#     long temp1 = fib(n - 1);  // Recursive call 1
#     long temp2 = fib(n - 2);  // Recursive call 2
#     return temp1 + temp2;
# }

# FUNCTION: fib
# Purpose: Compute the nth Fibonacci number recursively
# Parameters:
#   %rdi = n (which Fibonacci number to compute)
# Returns:
#   %rax = fib(n)
# Local Variables:
#   -8(%rbp) = storage for fib(n-1) result
#
# RECURSION CALL TREE for fib(4):
#                    fib(4)
#                   /      \
#              fib(3)      fib(2)
#              /    \      /    \
#          fib(2) fib(1) fib(1) fib(0)
#          /   \
#      fib(1) fib(0)
#
# Each box represents a separate function call with its own stack frame

fib:
    # FUNCTION PROLOGUE - Set up the stack frame
    # This is necessary because:
    # 1. We make recursive calls (need to preserve our state)
    # 2. We need local storage for intermediate results
    # 3. We must preserve %rbp for the caller

    push %rbp                 # SAVE CALLER'S BASE POINTER
                             # Push old %rbp onto stack
                             # This is required by calling convention
                             # Creates a chain of frame pointers for debugging
                             # %rsp decreases by 8

    mov %rsp, %rbp            # ESTABLISH NEW FRAME BASE
                             # Set %rbp to current stack position
                             # This creates a stable reference point for this frame
                             # Now %rbp points to saved old %rbp
                             # WHY? So we can access locals and params consistently
                             # even as %rsp changes during execution

    sub $16, %rsp             # ALLOCATE SPACE FOR LOCAL VARIABLES
                             # Reserve 16 bytes on the stack
                             # WHY 16? We need 8 bytes for storing fib(n-1)
                             # Extra 8 bytes maintains 16-byte stack alignment
                             # (required by x86-64 ABI for function calls)
                             # MEMORY LAYOUT:
                             # %rbp - 8  : will hold fib(n-1)
                             # %rbp - 16 : unused (for alignment)

    # BASE CASE CHECK: if (n <= 1)
    # This is CRITICAL - without base case, infinite recursion!

    cmp $1, %rdi              # COMPARE n WITH 1
                             # Set flags based on %rdi - 1
                             # If n=0: result is negative (n < 1)
                             # If n=1: result is zero (n = 1)
                             # If n>1: result is positive (n > 1)

    jle .base                 # JUMP IF LESS OR EQUAL
                             # If n <= 1, jump to base case handler
                             # This terminates the recursion
                             # jle checks the flags set by cmp
                             # Jumps if n-1 <= 0 (i.e., n <= 1)

    # RECURSIVE CASE: n > 1
    # We need to compute: fib(n-1) + fib(n-2)
    # Challenge: Making two recursive calls and preserving values

    # FIRST RECURSIVE CALL: fib(n - 1)

    push %rdi                 # SAVE n ON THE STACK
                             # WHY? We're about to modify %rdi for the recursive call
                             # Need to preserve n for later use
                             # %rdi is caller-saved, so recursive call will destroy it
                             # After this: stack has [..., saved_rbp, saved_n]

    dec %rdi                  # DECREMENT n BY 1
                             # %rdi = n - 1
                             # This is the parameter for fib(n-1)
                             # dec is shorthand for: sub $1,%rdi

    call fib                  # RECURSIVE CALL: fib(n - 1)
                             # WHAT HAPPENS:
                             # 1. Push return address onto stack
                             # 2. Jump to fib (recursively)
                             # 3. fib executes with parameter n-1
                             # 4. Result returns in %rax
                             # 5. Stack unwinds back to here
                             # THIS IS THE MAGIC OF RECURSION:
                             # Each call gets its own stack frame!

    mov %rax, -8(%rbp)        # SAVE fib(n-1) RESULT
                             # Store result in our local variable at %rbp-8
                             # WHY? We need to preserve this value across next call
                             # The next call to fib will overwrite %rax
                             # Now: local variable = fib(n-1)

    pop %rdi                  # RESTORE n FROM STACK
                             # Retrieve the original n value we saved earlier
                             # %rdi = n again
                             # Stack shrinks back to: [..., saved_rbp]

    # SECOND RECURSIVE CALL: fib(n - 2)

    push %rdi                 # SAVE n AGAIN
                             # Need to preserve n while we modify it
                             # Yes, we just popped it, but we need it again after this call

    dec %rdi                  # DECREMENT n BY 1
                             # %rdi = n - 1
                             # But we need n-2, so...

    dec %rdi                  # DECREMENT n BY 1 AGAIN
                             # %rdi = (n - 1) - 1 = n - 2
                             # This is the parameter for fib(n-2)
                             # Could use: sub $2,%rdi instead

    call fib                  # RECURSIVE CALL: fib(n - 2)
                             # Another recursive call
                             # Result returns in %rax
                             # This may trigger even more recursive calls
                             # The stack keeps growing with each recursive call

    pop %rdi                  # RESTORE n AGAIN
                             # Get original n back (though we don't actually use it)
                             # Good practice to clean up what we pushed
                             # Keeps stack balanced

    # COMBINE RESULTS: fib(n-1) + fib(n-2)

    add -8(%rbp), %rax        # ADD fib(n-1) TO fib(n-2)
                             # -8(%rbp) contains fib(n-1) that we saved earlier
                             # %rax contains fib(n-2) from the second call
                             # Result: %rax = fib(n-1) + fib(n-2) = fib(n)
                             # This is our final answer!

    # FUNCTION EPILOGUE - Restore the stack frame

    mov %rbp, %rsp            # DEALLOCATE LOCAL VARIABLES
                             # Restore stack pointer to base pointer
                             # This abandons the 16 bytes we allocated
                             # %rsp now points to saved old %rbp
                             # WHY? Quick way to deallocate all locals at once

    pop %rbp                  # RESTORE CALLER'S BASE POINTER
                             # Pop the old %rbp we saved in prologue
                             # This is required by calling convention
                             # %rsp now points to return address

    ret                       # RETURN TO CALLER
                             # Pop return address and jump to it
                             # Return value (fib(n)) is in %rax
                             # If this was a recursive call, we return to another fib call
                             # If this was the first call, we return to _start

# BASE CASE HANDLER
# When n <= 1, fib(n) = n

.base:
    mov %rdi, %rax            # RETURN n
                             # For base case: fib(0) = 0, fib(1) = 1
                             # Simply move n from %rdi to %rax
                             # %rax is the return value register

    mov %rbp, %rsp            # DEALLOCATE LOCALS
                             # Same cleanup as recursive case
                             # Even though we didn't use locals here
                             # We still allocated them in prologue

    pop %rbp                  # RESTORE BASE POINTER
                             # Restore caller's %rbp
                             # Maintains calling convention

    ret                       # RETURN TO CALLER
                             # Return n as the result
                             # Base case terminates the recursion

# PROGRAM ENTRY POINT
_start:
        call readInt              # READ INTEGER FROM USER
                                 # External function (defined elsewhere)
                                 # Reads integer from standard input
                                 # Result returned in %rax
                                 # Example: if user types "6", %rax = 6

        mov %rax, %rdi            # MOVE INPUT TO FIRST PARAMETER
                                 # %rax contains the number user entered
                                 # %rdi is first parameter for fib
                                 # Preparing to call: fib(user_input)

        call fib                  # COMPUTE FIBONACCI NUMBER
                                 # Call fib with n = user's input
                                 # Result (fib(n)) returns in %rax
                                 # This may trigger many recursive calls!
                                 # Example: fib(6) = 8

        mov %rax,%rdi             # PREPARE TO WRITE RESULT
                                 # %rax contains fib(n)
                                 # %rdi is first parameter for writeInt
                                 # writeInt will print this number

        call writeInt             # WRITE INTEGER TO SCREEN
                                 # External function
                                 # Prints the value in %rdi to stdout
                                 # User sees the Fibonacci number

        # CLEAN EXIT

        xor %rdi, %rdi            # SET EXIT CODE TO 0
                                 # xor register with itself = 0
                                 # %rdi = 0 means successful exit
                                 # More compact than: mov $0,%rdi

        mov $60, %rax             # SYSCALL NUMBER: 60 = sys_exit
                                 # Preparing to terminate program
                                 # Linux syscall interface

        syscall                   # INVOKE EXIT SYSTEM CALL
                                 # Transfer control to kernel
                                 # Program terminates with exit code 0


# FIBONACCI VALUES FOR REFERENCE:
        ## fib(0) = 0
        ## fib(1) = 1
        ## fib(2) = 1    (fib(1) + fib(0) = 1 + 0)
        ## fib(3) = 2    (fib(2) + fib(1) = 1 + 1)
        ## fib(4) = 3    (fib(3) + fib(2) = 2 + 1)
        ## fib(5) = 5    (fib(4) + fib(3) = 3 + 2)
        ## fib(6) = 8    (fib(5) + fib(4) = 5 + 3)

# DETAILED RECURSION TRACE FOR fib(3):
#
# CALL STACK EVOLUTION:
#
# 1. _start calls fib(3)
#    Stack: [fib(3)]
#    fib(3): n=3, need fib(2) and fib(1)
#
# 2. fib(3) calls fib(2)
#    Stack: [fib(3), fib(2)]
#    fib(2): n=2, need fib(1) and fib(0)
#
# 3. fib(2) calls fib(1)
#    Stack: [fib(3), fib(2), fib(1)]
#    fib(1): n=1, BASE CASE, return 1
#
# 4. fib(2) resumes, saves result (1)
#    Stack: [fib(3), fib(2)]
#    fib(2): fib(1)=1, now need fib(0)
#
# 5. fib(2) calls fib(0)
#    Stack: [fib(3), fib(2), fib(0)]
#    fib(0): n=0, BASE CASE, return 0
#
# 6. fib(2) resumes, adds results
#    Stack: [fib(3), fib(2)]
#    fib(2): return 1+0 = 1
#
# 7. fib(3) resumes, saves result (1)
#    Stack: [fib(3)]
#    fib(3): fib(2)=1, now need fib(1)
#
# 8. fib(3) calls fib(1)
#    Stack: [fib(3), fib(1)]
#    fib(1): n=1, BASE CASE, return 1
#
# 9. fib(3) resumes, adds results
#    Stack: [fib(3)]
#    fib(3): return 1+1 = 2
#
# 10. _start resumes
#     Stack: []
#     Result: fib(3) = 2

# STACK FRAME DIAGRAM FOR NESTED CALLS:
#
# When fib(3) calls fib(2) calls fib(1):
#
# High Memory
# +------------------+
# | _start's frame   |
# +------------------+
# | ret to _start    | <- pushed by call fib(3)
# +------------------+
# | saved %rbp (1)   | <- fib(3)'s frame
# +------------------+ <- fib(3)'s %rbp
# | fib(2) result    | <- at fib(3)'s %rbp - 8
# +------------------+
# | (alignment)      |
# +------------------+
# | ret to fib(3)    | <- pushed by call fib(2)
# +------------------+
# | saved %rbp (2)   | <- fib(2)'s frame
# +------------------+ <- fib(2)'s %rbp
# | fib(1) result    | <- at fib(2)'s %rbp - 8
# +------------------+
# | (alignment)      |
# +------------------+
# | ret to fib(2)    | <- pushed by call fib(1)
# +------------------+
# | saved %rbp (3)   | <- fib(1)'s frame
# +------------------+ <- fib(1)'s %rbp, %rsp
# | (locals)         |
# +------------------+
# Low Memory
#
# Each function call adds a new frame!
# Stack grows DOWN (toward lower addresses)
# Each frame is independent with its own locals

# WHY RECURSION IS EXPENSIVE:
#
# For fib(6), the call tree is:
#                    fib(6)
#                   /      \
#              fib(5)      fib(4)
#              /    \      /    \
#          fib(4) fib(3) fib(3) fib(2)
#          ...many more calls...
#
# Total calls for fib(6): 25 function calls!
# Total calls for fib(10): 177 function calls!
# Total calls for fib(20): 21,891 function calls!
#
# EACH CALL:
# - Allocates stack frame (push, sub)
# - Saves/restores registers
# - Performs comparisons and jumps
# - Deallocates stack frame (mov, pop)
#
# BETTER APPROACH: Iterative solution
# Can compute fib(n) in O(n) time with O(1) space
# But recursion demonstrates important concepts!

# REGISTER PRESERVATION STRATEGY:
#
# CALLER-SAVED (we must save if we need them):
#   %rax, %rcx, %rdx, %rsi, %rdi, %r8-r11
#   We push %rdi (parameter n) because recursive calls destroy it
#
# CALLEE-SAVED (fib must preserve for its caller):
#   %rbp, %rbx, %r12-r15
#   We save/restore %rbp in prologue/epilogue
#   We don't use %rbx or %r12-r15, so don't need to save them
#
# RETURN VALUE:
#   %rax holds fib(n)
#   This is standard across all functions

# COMPARISON: RECURSIVE vs ITERATIVE FIBONACCI:
#
# RECURSIVE (this implementation):
# - Clean, matches mathematical definition
# - Easy to understand conceptually
# - Exponential time complexity: O(2^n)
# - Lots of function call overhead
# - Demonstrates recursion principles
#
# ITERATIVE (alternative):
# fib_iterative:
#     cmp $1,%rdi
#     jle .base
#     mov $0,%rax    # fib(0)
#     mov $1,%rbx    # fib(1)
#   .loop:
#     dec %rdi
#     jz .done
#     mov %rbx,%rcx
#     add %rax,%rbx  # fib(i) = fib(i-1) + fib(i-2)
#     mov %rcx,%rax
#     jmp .loop
#   .done:
#     mov %rbx,%rax
#     ret
# - Linear time complexity: O(n)
# - More efficient
# - No recursion overhead
# - But less intuitive

# TO ASSEMBLE, LINK, AND RUN:
#
# You need to provide readInt and writeInt implementations
# (These are external functions)
#
# Assuming you have readInt.o and writeInt.o:
#
# as 01-fibonacci.s -o fibonacci.o
# ld fibonacci.o readInt.o writeInt.o -o fibonacci
# ./fibonacci
# 6        <- user inputs this
# 8        <- program outputs fib(6) = 8

# DEBUGGING WITH GDB:
#
# gdb ./fibonacci
# break fib
# run
# 5        <- input 5
#
# In GDB:
#   backtrace              # See the call stack
#   info registers rdi     # See parameter n
#   stepi                  # Step through instructions
#   finish                 # Run until return
#   print $rax             # See return value
#
# Watch the stack grow and shrink as recursion proceeds!

# COMMON MISTAKES TO AVOID:
#
# 1. FORGETTING BASE CASE:
#    Without "cmp $1,%rdi; jle .base", infinite recursion!
#    Stack overflow crash
#
# 2. NOT SAVING n BEFORE RECURSIVE CALLS:
#    Without "push %rdi", n is lost after first recursive call
#    Can't compute fib(n-2)
#
# 3. NOT SAVING fib(n-1) RESULT:
#    Without "mov %rax,-8(%rbp)", result is lost
#    Second recursive call overwrites %rax
#
# 4. INCORRECT DECREMENTS:
#    Using "dec %rdi; call fib" twice gives fib(n-1) and fib(n-2)? NO!
#    Second dec operates on already-decremented value
#    Must restore n between calls
#
# 5. STACK IMBALANCE:
#    Every push must have matching pop
#    Every prologue must have matching epilogue
#    Otherwise: stack corruption, crashes
#
# 6. WRONG RETURN VALUE:
#    Returning in wrong register (%rbx instead of %rax)
#    Caller expects result in %rax
#
# 7. MODIFYING CALLEE-SAVED REGISTERS:
#    Using %rbx without saving/restoring
#    Violates calling convention, corrupts caller's data

# ADVANCED TOPICS:
#
# 1. TAIL CALL OPTIMIZATION:
#    This code can't use it (need both results before returning)
#    But some recursive functions can be optimized to iteration
#
# 2. MEMOIZATION:
#    Cache results to avoid recomputation
#    fib(4) computed multiple times in fib(6)
#    Could store in array/hash table
#
# 3. STACK OVERFLOW:
#    fib(1000) would cause stack overflow
#    Each call uses stack space
#    Eventually run out of stack
#    Iterative solution doesn't have this problem
#
# 4. REGISTER ALLOCATION:
#    Could use %rbx to hold n (callee-saved)
#    Avoid push/pop overhead
#    But must save/restore %rbx in prologue/epilogue

# LEARNING PROGRESSION:
#
# This example builds on:
# 1. 02-basic-function.s: Simple function calls
# 2. 03-function-parameters.s: Local variables
# 3. Now: Recursion with multiple calls
#
# Key new concepts:
# - Multiple calls to same function
# - Each call has independent state
# - Stack manages the complexity
# - Base case prevents infinite recursion
# - Preserving values across calls
