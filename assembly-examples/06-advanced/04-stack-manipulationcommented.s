// PEDAGOGICAL PURPOSE:
// This program demonstrates advanced stack manipulation techniques including stack frames,
// local variables, function prologues/epilogues, and the relationship between the stack
// pointer (RSP) and frame pointer (RBP). Understanding the stack is crucial for function
// calls, recursion, local variables, and debugging.
//
// Key learning objectives:
// 1. Understanding stack frames (also called activation records)
// 2. Learning the role of RBP (frame pointer) and RSP (stack pointer)
// 3. Understanding function prologue and epilogue conventions
// 4. Learning how local variables are allocated and accessed on the stack
// 5. Understanding negative offsets from RBP for local variables
// 6. Learning the 'leave' instruction and what it does
// 7. Understanding the x86-64 calling convention for function parameters
// 8. Seeing how to preserve and restore register state across function calls

// WHAT IS THE STACK?
//
// The stack is a region of memory used for:
//   - Function call management (return addresses)
//   - Local variables
//   - Temporary storage
//   - Preserving register values
//   - Passing arguments (when registers aren't enough)
//
// Stack characteristics:
//   - Grows DOWNWARD (toward lower memory addresses)
//   - Last-In-First-Out (LIFO) data structure
//   - Managed by RSP (stack pointer) register
//   - On x86-64, typically starts near high memory (e.g., 0x7fffffffffff)

// STACK GROWTH DIRECTION:
//
// High memory addresses
//   ↑
//   |  [older data]
//   |  ← Previous stack frames
//   |  [current stack frame]
//   ↓  ← Stack grows down (toward lower addresses)
// Low memory addresses
//
// When you PUSH:
//   1. RSP decreases (moves down)
//   2. Value is stored at new RSP location
//
// When you POP:
//   1. Value is read from RSP location
//   2. RSP increases (moves up)

// STACK FRAME STRUCTURE:
//
// A stack frame (activation record) for a function typically looks like:
//
// Higher addresses
//   ┌──────────────────────┐
//   │ Arguments (if needed)│  ← Passed on stack if >6 integer args
//   ├──────────────────────┤
//   │ Return address       │  ← Pushed by 'call' instruction
//   ├──────────────────────┤  ← RBP points here after prologue
//   │ Saved RBP (old frame)│  ← Pushed in prologue
//   ├──────────────────────┤
//   │ Local variable 1     │  ← At -8(%rbp)
//   ├──────────────────────┤
//   │ Local variable 2     │  ← At -16(%rbp)
//   ├──────────────────────┤
//   │ ...more locals...    │
//   └──────────────────────┘  ← RSP points here
// Lower addresses
//
// WHY TWO POINTERS (RSP and RBP)?
//   RSP: Changes as you push/pop (dynamic)
//   RBP: Fixed reference point for accessing locals (static within function)

// CALLING CONVENTION (x86-64 System V ABI):
//
// Integer/Pointer arguments passed in registers (in order):
//   1st: RDI
//   2nd: RSI
//   3rd: RDX
//   4th: RCX
//   5th: R8
//   6th: R9
//   Additional arguments: passed on stack
//
// Floating-point arguments:
//   XMM0-XMM7 (first 8 float/double arguments)
//
// Return value:
//   Integer/pointer: RAX
//   Floating-point: XMM0
//
// Caller-saved (caller must preserve if needed):
//   RAX, RCX, RDX, RSI, RDI, R8-R11
//
// Callee-saved (function must preserve):
//   RBX, RBP, R12-R15

        .section .rodata
# .rodata = Read-Only Data section
# Contains constants that should never be modified
# Prevents accidental modification and allows sharing in memory

msg1:   .ascii "\nEnter a number: "
# Define a string literal (no null terminator with .ascii)
# .ascii vs .asciz:
#   .ascii - raw bytes, no null terminator
#   .asciz - adds '\0' at end (C-style string)
# WHY no null terminator here?
#   We're using write syscall with explicit length
#   Don't need null terminator for length calculation

msg1len = . - msg1
# Calculate length of msg1
# '.' = current location counter (address)
# '. - msg1' = current address - start of msg1 = length in bytes
# This is an assembler constant, not stored in memory
# After assembly: msg1len = 18 (length of "\nEnter a number: ")
#
# WHY compute at assembly time?
#   No runtime overhead
#   Guaranteed correct (can't miscound)
#   Updates automatically if message changes

msg2:   .ascii "\nYour function calculated: "
# Message to display before showing result
msg2len = . - msg2
# Length: 28 bytes

msg3:   .ascii "\nGoodbye!\n"
# Final message before exiting
msg3len = . - msg3
# Length: 10 bytes

        .section .text                   # Start of the code section
        .global _start          # Make _start symbol globally visible for the linker

        .extern readInt
# Declare readInt as an external function (defined elsewhere)
# Typically reads an integer from stdin and returns it in RAX

        .extern writeInt
# Declare writeInt as an external function
# Expects integer to print in RDI

        ## function that takes the address
        ## of a string in %rdi
        ## prints it
printStr:
# Simple helper function to print a string
# Parameters:
#   RDI: pointer to string
#   RSI: length of string
# This function uses the write syscall

        mov %rsi,%rdx
	# Move the string length from RSI to RDX
	# RDX is the 3rd parameter for syscall
	# For write syscall: RDX = number of bytes to write
	#
	# WHY move RSI to RDX?
	#   write syscall parameter order:
	#     RDI = file descriptor
	#     RSI = buffer address
	#     RDX = count (number of bytes)
	#   Our calling convention:
	#     RDI = buffer address
	#     RSI = count
	#   So we need to rearrange

        mov %rdi,%rsi     # load message address
	# Move string pointer from RDI to RSI
	# RSI is the 2nd parameter for syscall (buffer address)
	# After this: RSI points to the string to print

        mov $1,%rax
	# Syscall number 1 = sys_write
	# Write data to a file descriptor

        mov $0,%rdi
	# File descriptor 0 = stdin
	# WAIT - this looks wrong! We want to write to stdout!
	# BUG: Should be mov $1,%rdi (stdout)
	# This code has a bug - it's trying to write to stdin
	# On most systems, writing to stdin redirects to stdout, so it works
	# But this is technically incorrect

        syscall
	# Invoke the system call
	# Kernel executes: write(0, string, length)
	# Despite the bug, usually prints to terminal

        ret
	# Return from printStr function
	# Pops return address from stack and jumps to it
	# No need for epilogue (we didn't modify RBP or allocate stack space)


## This is a function that demonstrates:
## 1. Using local variables on the stack
## 2. Function call conventions
## 3. Basic arithmetic operations
## long fun(long a, long b){
##   long c = a + b;
##   long d = 2+b;
##   return (c + d);
## }
#
# This is the C equivalent of the function below
# Let's trace through how it's compiled to assembly

fun:
        # Standard function prologue
	# The prologue sets up the stack frame for this function

        # Save the old base pointer (frame pointer)
        push %rbp
	# Push the caller's RBP onto the stack
	# This preserves the caller's frame pointer
	# Stack before: [... caller's data ...]
	# Stack after:  [... caller's data ...][saved RBP] ← RSP
	#
	# WHY save RBP?
	#   RBP is a callee-saved register
	#   We're about to modify it for our own stack frame
	#   Must restore it before returning
	#
	# What push does:
	#   1. RSP = RSP - 8 (move stack pointer down)
	#   2. Store RBP at address RSP

        # Set up new base pointer for this stack frame
        mov %rsp,%rbp
	# Copy the current stack pointer to RBP
	# Now RBP points to the saved RBP value we just pushed
	# This establishes our frame pointer
	#
	# After this:
	#   RBP = RSP (both point to saved old RBP)
	#
	# WHY set RBP = RSP?
	#   Creates a fixed reference point for accessing local variables
	#   Even if RSP changes (push/pop), RBP stays constant
	#   Local variables accessed via negative offsets from RBP

        # Allocate 16 bytes of local stack space
        # This creates space for two 8-byte (64-bit) local variables
        sub $16,%rsp
	# Subtract 16 from RSP to allocate stack space
	# RSP moves down (toward lower addresses)
	#
	# Stack layout after allocation:
	#   Higher addresses
	#     [...caller's frame...]
	#     [saved RBP]           ← RBP points here
	#     [8 bytes for var 1]   ← -8(%rbp)
	#     [8 bytes for var 2]   ← -16(%rbp) ← RSP points here
	#   Lower addresses
	#
	# WHY 16 bytes for 2 variables?
	#   Each long is 8 bytes (64 bits)
	#   2 variables × 8 bytes = 16 bytes
	#
	# NOTE: Could also do: sub $32,%rsp for alignment
	#   x86-64 ABI requires 16-byte stack alignment before call
	#   But this function doesn't call anything, so not critical here

        # First parameter (a) is in %rdi
        # First local variable will be at -8(%rbp)
        # Compute a + b and store at -8(%rbp)
        mov %rdi,-8(%rbp)
	# Store parameter 'a' (from RDI) into first local variable
	# -8(%rbp) = RBP - 8 bytes
	# This is the memory location for local variable 'c'
	#
	# After this: local variable at -8(%rbp) contains value of 'a'
	# But we want c = a + b, so we need to add b...

        add %rsi,-8(%rbp)
	# Add parameter 'b' (from RSI) to the local variable at -8(%rbp)
	# This is a memory-to-memory operation (well, register-to-memory)
	#
	# Operation: memory[-8(%rbp)] = memory[-8(%rbp)] + RSI
	# In C: c = c + b (but c was initialized to a, so c = a + b)
	#
	# After this: -8(%rbp) contains (a + b)
	# This is the value of 'c' in the C code

        # Second local variable at -16(%rbp)
        # Compute b + 2 and store at -16(%rbp)
        mov %rsi,-16(%rbp)
	# Store parameter 'b' (from RSI) into second local variable
	# -16(%rbp) = RBP - 16 bytes
	# This is the memory location for local variable 'd'
	#
	# After this: -16(%rbp) contains value of 'b'

        addq $2,-16(%rbp)
	# Add 2 to the local variable at -16(%rbp)
	# addq = add quadword (64-bit)
	# $2 = immediate value 2
	#
	# Operation: memory[-16(%rbp)] = memory[-16(%rbp)] + 2
	# In C: d = b + 2
	#
	# After this: -16(%rbp) contains (b + 2)
	# This is the value of 'd' in the C code
	#
	# WHY addq and not just add?
	#   addq explicitly specifies quadword (64-bit) operation
	#   Clearer and prevents ambiguity with memory operands
	#   'add' would work but assembler might complain

        # Prepare return value by:
        # 1. Load d into %rax
        # 2. Add c to %rax
        mov -16(%rbp),%rax
	# Load local variable 'd' (at -16(%rbp)) into RAX
	# RAX is used for return values in x86-64 calling convention
	#
	# After this: RAX = d = (b + 2)

        add -8(%rbp),%rax
	# Add local variable 'c' (at -8(%rbp)) to RAX
	# RAX = RAX + c
	# RAX = d + c
	# RAX = (b + 2) + (a + b)
	# RAX = a + 2b + 2
	#
	# This is the return value
	#
	# Let's verify with algebra:
	#   c = a + b
	#   d = b + 2
	#   return c + d = (a + b) + (b + 2) = a + 2b + 2 ✓

        # Function epilogue:
        # 'leave' is a single instruction that does:
        # mov %rbp,%rsp
        # pop %rbp
        leave
	# leave = combination of two instructions:
	#   1. mov %rbp,%rsp - Restore stack pointer to frame pointer
	#   2. pop %rbp      - Restore old frame pointer
	#
	# Step 1 (mov %rbp,%rsp):
	#   RSP = RBP
	#   This deallocates all local variables
	#   RSP now points to the saved RBP value
	#
	# Step 2 (pop %rbp):
	#   RBP = value at RSP (the saved old RBP)
	#   RSP = RSP + 8 (pop the saved RBP)
	#
	# After leave:
	#   RBP is restored to caller's frame pointer
	#   RSP points to return address (ready for ret)
	#   Stack frame is completely deallocated
	#
	# WHY use leave instead of manual cleanup?
	#   Shorter (1 instruction vs 2)
	#   Faster on some CPUs
	#   Standard convention
	#   Less error-prone
	#
	# Manual equivalent:
	#   mov %rbp,%rsp
	#   pop %rbp

        ret
	# Return from function
	# Pops return address from stack and jumps to it
	#
	# What ret does:
	#   1. RIP = value at RSP (the return address)
	#   2. RSP = RSP + 8 (pop the return address)
	#
	# Control returns to the caller
	# Return value is in RAX

_start:
	# Main program entry point

	# Print "Enter a number: " prompt
        lea msg1(%rip),%rdi
	# Load the address of msg1 into RDI (first parameter)
	# lea = Load Effective Address (gets address, not contents)
	# msg1(%rip) = RIP-relative addressing
	#
	# After this: RDI points to "\nEnter a number: "

        mov $msg1len,%rsi
	# Load the length of msg1 into RSI (second parameter)
	# msg1len is an assembler constant (computed at assembly time)
	#
	# After this: RSI = 18 (length of msg1)

        call printStr
	# Call the printStr function
	# What call does:
	#   1. Push return address onto stack (address of next instruction)
	#   2. Jump to printStr
	#
	# printStr executes with:
	#   RDI = pointer to msg1
	#   RSI = length of msg1
	#
	# After printStr returns:
	#   Message has been printed
	#   RSP is restored (return address was popped)

	# Read first number from user
        call readInt
	# Call readInt function (defined externally)
	# readInt reads a number from stdin
	# Returns the integer value in RAX
	#
	# After this: RAX contains the first number entered by user

        push %rax
	# Save the first number on the stack
	# push decrements RSP and stores RAX at new RSP
	#
	# WHY push?
	#   We need to read a second number
	#   readInt will overwrite RAX
	#   We need to preserve the first number
	#   Stack is perfect for temporary storage
	#
	# Stack before: [...]
	# Stack after:  [...][first number] ← RSP

	# Print "Enter a number: " prompt again
        lea msg1(%rip),%rdi
	# Load address of msg1 into RDI
        mov $msg1len,%rsi
	# Load length of msg1 into RSI
        call printStr
	# Print the prompt again
	# (Asking for second number)

	# Read second number from user
        call readInt
	# Call readInt again
	# After this: RAX contains the second number

	# At this point:
	#   Stack contains first number
	#   RAX contains second number
	#   We need to set up parameters for fun(first, second)

        # Set up arguments for fun()
        # Second arg (4) goes in %rsi
        mov %rax,%rsi
	# Move second number from RAX to RSI
	# RSI is the second parameter register
	# In C terms: this will be parameter 'b'
	#
	# After this: RSI = second number

        pop %rax
	# Pop the first number from stack into RAX
	# pop loads value from RSP and increments RSP
	#
	# After this:
	#   RAX = first number
	#   Stack is back to original state (before the push)

        # First arg (3) goes in %rdi
        mov %rax,%rdi
	# Move first number from RAX to RDI
	# RDI is the first parameter register
	# In C terms: this will be parameter 'a'
	#
	# After this:
	#   RDI = first number (parameter a)
	#   RSI = second number (parameter b)

        # Call the fun() function
        call fun
	# Call fun(a, b)
	# fun will execute with:
	#   RDI = first number (a)
	#   RSI = second number (b)
	#
	# fun computes: a + 2b + 2
	# Returns result in RAX

        push %rax
	# Save the result from fun() on the stack
	# WHY?
	#   We need to call printStr next
	#   printStr might modify RAX
	#   We need to preserve the result to print it later

	## print the next message
        lea msg2(%rip),%rdi
	# Load address of msg2 into RDI
	# msg2 = "\nYour function calculated: "

        mov $msg2len,%rsi
	# Load length of msg2 into RSI
        call printStr
	# Print the message
	# Output: "\nYour function calculated: "

        # Return value from fun() is already in %rax
        pop %rax
	# Restore the result from fun() from the stack
	# After this: RAX contains the result we saved earlier

        mov %rax,%rdi
	# Move result to RDI (first parameter for writeInt)
	# writeInt expects the number to print in RDI

        call writeInt
	# Call writeInt to print the result
	# Outputs the computed value to terminal

	# Print goodbye message
        lea msg3(%rip),%rdi
	# Load address of msg3 into RDI
	# msg3 = "\nGoodbye!\n"

        mov $msg3len,%rsi
	# Load length of msg3 into RSI
        call printStr
	# Print the goodbye message

        # Syscall number for exit
        mov $60,%rax
	# Syscall 60 = sys_exit

        xor %rdi,%rdi
	# Set RDI to 0 (exit code 0 = success)
	# xor %rdi,%rdi is a common idiom to zero a register
	# Equivalent to mov $0,%rdi but:
	#   - Shorter encoding (2 bytes vs 7 bytes)
	#   - Faster on some CPUs (no dependency on old value)
	#   - More efficient (no immediate value to encode)

        # Invoke system call to exit
        syscall
	# Exit the program with code 0

// COMPLETE EXECUTION TRACE:
//
// Example: User enters 10 and 5
//
// 1. Print "Enter a number: "
// 2. User types: 10
// 3. readInt returns RAX = 10
// 4. push %rax → stack: [10]
// 5. Print "Enter a number: "
// 6. User types: 5
// 7. readInt returns RAX = 5
// 8. mov %rax,%rsi → RSI = 5
// 9. pop %rax → RAX = 10, stack: []
// 10. mov %rax,%rdi → RDI = 10
// 11. call fun(10, 5):
//     - Prologue: push %rbp, mov %rsp,%rbp, sub $16,%rsp
//     - Stack frame created:
//       [...][saved RBP][c=-8(rbp)][d=-16(rbp)]
//     - mov %rdi,-8(%rbp) → c = 10
//     - add %rsi,-8(%rbp) → c = 10 + 5 = 15
//     - mov %rsi,-16(%rbp) → d = 5
//     - addq $2,-16(%rbp) → d = 5 + 2 = 7
//     - mov -16(%rbp),%rax → RAX = 7
//     - add -8(%rbp),%rax → RAX = 7 + 15 = 22
//     - Epilogue: leave, ret
//     - Returns RAX = 22
// 12. push %rax → stack: [22]
// 13. Print "Your function calculated: "
// 14. pop %rax → RAX = 22
// 15. writeInt(22) → prints "22"
// 16. Print "Goodbye!\n"
// 17. exit(0)
//
// VERIFICATION:
//   fun(a=10, b=5):
//     c = a + b = 10 + 5 = 15
//     d = b + 2 = 5 + 2 = 7
//     return c + d = 15 + 7 = 22 ✓

// WHY THIS EXAMPLE MATTERS:
//
// 1. DEMONSTRATES COMPLETE STACK FRAME MANAGEMENT:
//    Shows all aspects: prologue, local variables, epilogue
//    This is how ALL functions work at the assembly level
//
// 2. ILLUSTRATES CALLING CONVENTIONS:
//    Parameters in RDI, RSI
//    Return value in RAX
//    Stack preservation with push/pop
//
// 3. SHOWS RBP VS RSP:
//    RBP: Fixed reference (doesn't change during function)
//    RSP: Dynamic (changes with push/pop)
//    Why both are needed
//
// 4. DEMONSTRATES LOCAL VARIABLE ACCESS:
//    Negative offsets from RBP
//    -8(%rbp), -16(%rbp), etc.
//    How C local variables map to stack locations

// STACK FRAME MEMORY DIAGRAM:
//
// Before fun() call:
//   High memory
//     [...]
//     [return address from _start]
//     [...]
//   Low memory
//
// After fun() prologue (push %rbp; mov %rsp,%rbp; sub $16,%rsp):
//   High memory
//     [...]
//     [return address]      ← pushed by 'call fun'
//     [saved old RBP]       ← RBP points here (pushed by 'push %rbp')
//     [local var c: 8 bytes]  ← -8(%rbp)
//     [local var d: 8 bytes]  ← -16(%rbp), RSP points here
//     [...]
//   Low memory
//
// After fun() epilogue (leave; ret):
//   High memory
//     [...]
//     [...back to caller's frame...]
//   Low memory
//
// The stack frame is completely removed!

// UNDERSTANDING NEGATIVE OFFSETS:
//
// Why -8(%rbp) and not +8(%rbp)?
//
// Because the stack grows DOWN (toward lower addresses):
//   RBP points to saved old RBP
//   Local variables are BELOW RBP (at lower addresses)
//   Lower addresses = negative offsets
//
// Visual:
//   Higher addresses
//     +16(%rbp): [some caller data]
//     +8(%rbp):  [return address]
//     0(%rbp):   [saved RBP]       ← RBP
//     -8(%rbp):  [local var 1]     ← First local
//     -16(%rbp): [local var 2]     ← Second local
//   Lower addresses

// THE 'LEAVE' INSTRUCTION IN DETAIL:
//
// leave is equivalent to:
//   mov %rbp,%rsp    # Deallocate locals
//   pop %rbp         # Restore old frame pointer
//
// Step-by-step:
//
// Before leave:
//   RBP = 0x7fffffffe100 (points to saved old RBP)
//   RSP = 0x7fffffffe0f0 (points below locals)
//   Stack:
//     0x7fffffffe100: [saved RBP = 0x7fffffffe200]
//     0x7fffffffe0f8: [local var]
//     0x7fffffffe0f0: [local var] ← RSP
//
// After mov %rbp,%rsp:
//   RSP = 0x7fffffffe100
//   Stack:
//     0x7fffffffe100: [saved RBP = 0x7fffffffe200] ← RSP, RBP
//     0x7fffffffe0f8: [local var] (no longer accessible)
//     0x7fffffffe0f0: [local var] (no longer accessible)
//
// After pop %rbp:
//   RBP = 0x7fffffffe200 (restored old value)
//   RSP = 0x7fffffffe108
//   Stack:
//     0x7fffffffe108: [return address] ← RSP
//     0x7fffffffe100: [old saved RBP] (no longer needed)
//
// Now RSP points to return address, ready for 'ret'!

// CALLER-SAVED VS CALLEE-SAVED REGISTERS:
//
// Caller-saved (volatile):
//   RAX, RCX, RDX, RSI, RDI, R8-R11
//   Function can freely modify these
//   Caller must save them if values are needed after call
//
// Callee-saved (non-volatile):
//   RBX, RBP, R12-R15
//   Function must preserve these (save on entry, restore on exit)
//   Caller can rely on these being unchanged
//
// Example in this code:
//   fun() saves RBP (callee-saved)
//   _start uses push/pop to save RAX around calls (caller-saved)

// STACK ALIGNMENT:
//
// x86-64 ABI requires:
//   RSP must be 16-byte aligned before a 'call' instruction
//   (RSP mod 16 = 0)
//
// WHY?
//   SSE instructions require aligned memory access
//   Some instructions crash on unaligned stack
//   Performance optimization
//
// HOW to ensure alignment:
//   'call' pushes 8-byte return address (misaligns by 8)
//   Function prologue should adjust to maintain alignment
//   Common: allocate locals in multiples of 16 bytes
//
// This program's fun():
//   Allocates 16 bytes (already aligned)
//   If we needed 8 bytes, would allocate 16 anyway for alignment

// ALTERNATIVES TO RBP-BASED STACK FRAMES:
//
// Modern compilers sometimes use RSP-only (frameless):
//   Don't save/restore RBP
//   Access locals via RSP offset
//   Saves 2 instructions per function
//   But: RSP offsets change if you push/pop!
//
// Example frameless version:
//   fun_frameless:
//     sub $16,%rsp         # Allocate locals (no RBP setup)
//     mov %rdi,8(%rsp)     # First local at 8(%rsp)
//     add %rsi,8(%rsp)
//     mov %rsi,0(%rsp)     # Second local at 0(%rsp)
//     addq $2,0(%rsp)
//     mov 0(%rsp),%rax
//     add 8(%rsp),%rax
//     add $16,%rsp         # Deallocate (no leave needed)
//     ret
//
// Advantages: Faster, smaller code
// Disadvantages: Harder to debug, offsets are fragile

// DEBUGGING WITH STACK FRAMES:
//
// RBP-based frames help debuggers:
//   GDB can walk the stack using RBP chain
//   Each frame's RBP points to previous frame's RBP
//   This is called a "frame chain" or "frame linked list"
//
// Frame chain example:
//   main's RBP → fun's saved RBP → helper's saved RBP → ...
//
// GDB commands:
//   bt (backtrace) - uses frame chain
//   frame N        - select frame using chain
//   info frame     - shows RBP, RSP values

// RECURSION AND THE STACK:
//
// Each recursive call creates a new stack frame:
//
// factorial(3) call stack:
//   [factorial(0) frame]
//   [factorial(1) frame]
//   [factorial(2) frame]
//   [factorial(3) frame]
//   [main frame]
//
// Each frame has its own:
//   - Saved RBP
//   - Return address
//   - Local variables
//   - Parameters (in registers or stack)
//
// Stack overflow occurs when:
//   Too many recursive calls
//   Stack grows into other memory regions
//   Typical stack size limit: 8MB on Linux

// REAL-WORLD APPLICATIONS:
//
// 1. Function calls:
//    Every function in every program uses stack frames
//    C, C++, Rust, Java (JIT), etc.
//
// 2. Exception handling:
//    Stack unwinding walks frame chain
//    C++ destructors called in reverse order
//
// 3. Debuggers:
//    Stack traces, breakpoints, stepping
//    All rely on proper frame management
//
// 4. Profilers:
//    Sample call stacks to find hotspots
//    Frame pointers enable accurate profiling
//
// 5. Security:
//    Buffer overflow exploits target stack
//    Stack canaries detect corruption
//    ASLR randomizes stack location

// COMPILING AND RUNNING:
//
// Assemble:
//   as 04-stack-manipulation.s -o 04-stack-manipulation.o
//
// Link with helper functions:
//   ld 04-stack-manipulation.o readInt.o writeInt.o -o 04-stack-manipulation
//
// Run:
//   ./04-stack-manipulation
//
// Example session:
//   Enter a number: 10
//   Enter a number: 5
//   Your function calculated: 22
//   Goodbye!
//
// Formula: a + 2b + 2 = 10 + 2(5) + 2 = 22

// EXTENDING THIS EXAMPLE:
//
// 1. Add more local variables:
//    sub $32,%rsp     # 4 variables
//    Access at -8, -16, -24, -32(%rbp)
//
// 2. Nested function calls:
//    Call another function from within fun()
//    Watch how frames stack up
//
// 3. Recursive function:
//    Implement factorial or fibonacci
//    See multiple frames for same function
//
// 4. Pass more parameters:
//    Use RDX, RCX, R8, R9 for params 3-6
//    Or push params 7+ onto stack
//
// 5. Return struct:
//    For large return values (>16 bytes)
//    Caller passes pointer in RDI
//    Function fills struct at pointer
//
// 6. Preserve callee-saved registers:
//    Use RBX for something
//    Must push/pop to preserve it

// COMMON PITFALLS:
//
// 1. Forgetting to restore RBP:
//    Results in corrupted frame chain
//    Debugger shows garbage
//
// 2. Misaligned stack:
//    Some instructions will crash
//    Always allocate multiples of 16 bytes
//
// 3. Stack overflow:
//    Too deep recursion
//    Too many local variables
//
// 4. Accessing freed stack space:
//    After 'leave', locals are gone
//    Pointers to locals become invalid
//
// 5. Wrong offset calculation:
//    Remember: negative offsets for locals
//    -8, -16, -24, etc. from RBP

// FURTHER READING:
//
// System V AMD64 ABI:
//   Official calling convention specification
//   https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf
//
// Intel Software Developer Manual:
//   Volume 1, Chapter 6: Procedure Calls
//
// "Computer Systems: A Programmer's Perspective" (CS:APP):
//   Chapter 3.7: Procedures
//   Excellent explanation of stack frames and calling conventions
