// PEDAGOGICAL PURPOSE:
// This minimal program demonstrates the fundamental stack operations: PUSH and POP.
// It shows how the stack is used to temporarily store values and how those values
// can be retrieved and used as function parameters.
//
// Key learning objectives:
// 1. Understanding the PUSH instruction - how it stores values on the stack
// 2. Understanding the POP instruction - how it retrieves values from the stack
// 3. Seeing that stack operations affect the stack pointer (%rsp)
// 4. Learning LIFO (Last In, First Out) behavior of the stack
// 5. Using stack values as function parameters (exit code)
// 6. Understanding that PUSH decrements %rsp, POP increments %rsp

.section .text
        .global _start          # Entry point for the program

_start:
        # PUSH OPERATION:
        # push $10 does TWO things:
        # 1. Decrements %rsp by 8 bytes (subq $8, %rsp)
        # 2. Stores the value 10 at the new location (%rsp)
        #
        # BEFORE: %rsp points to address X
        # AFTER:  %rsp points to address X-8, and memory[X-8] contains 10
        #
        # WHY PUSH?
        # - The stack grows DOWNWARD (toward lower memory addresses)
        # - We need to make room BEFORE storing the value
        # - This is why we subtract from %rsp first
        push $10                # Push immediate value 10 onto the stack
                                # Equivalent to:
                                #   subq $8, %rsp      # Make room (move stack pointer down)
                                #   movq $10, (%rsp)   # Store 10 at top of stack

        # STACK STATE AFTER PUSH:
        # High Memory
        # +------------------+
        # | (previous data)  |
        # +------------------+
        # | 10               | <- %rsp points here now
        # +------------------+
        # Low Memory

        # POP OPERATION:
        # pop %rdi does TWO things:
        # 1. Reads the value at (%rsp) and stores it in %rdi
        # 2. Increments %rsp by 8 bytes (addq $8, %rsp)
        #
        # BEFORE: %rsp points to address Y (where we stored 10)
        # AFTER:  %rdi contains 10, %rsp points to address Y+8
        #
        # WHY POP?
        # - Retrieve the value we just stored
        # - Remove it from the stack (by moving stack pointer back up)
        # - The value is MOVED from stack to register
        pop %rdi                # Pop the top stack value into %rdi
                                # Equivalent to:
                                #   movq (%rsp), %rdi  # Read value from top of stack
                                #   addq $8, %rsp      # Remove item (move stack pointer up)

        # AFTER POP:
        # - %rdi now contains 10
        # - %rsp is back to its original position (before the push)
        # - The stack is back to its original state

        # WHY USE %rdi?
        # The exit syscall expects its parameter (exit code) in %rdi
        # By popping into %rdi, we're setting up the exit code
        # This demonstrates using the stack to prepare function arguments

        # EXIT SYSCALL:
        mov $60,%rax            # Syscall number 60 = exit
                                # %rax always holds the syscall number

        syscall                 # Invoke the exit syscall
                                # Parameters:
                                #   %rdi = exit code (10, from our pop operation)
                                # The program terminates with exit code 10

// STACK OPERATION DETAILS:
//
// The x86-64 stack:
// - Grows DOWNWARD (from high addresses to low addresses)
// - %rsp (stack pointer) always points to the TOP of the stack
// - TOP means the LOWEST address currently in use
// - PUSH decrements %rsp (moves toward lower addresses)
// - POP increments %rsp (moves toward higher addresses)

// MEMORY LAYOUT EXAMPLE:
//
// Assume %rsp = 0x7fffffffe000 before push
//
// After push $10:
//   Address          Value
//   0x7fffffffdfff8  10        <- %rsp now points here
//   0x7fffffffe000   (old data)
//
// After pop %rdi:
//   %rdi = 10
//   %rsp = 0x7fffffffe000      <- back to original position

// LIFO (Last In, First Out):
//
// The stack follows LIFO ordering:
// - The last value pushed is the first value popped
// - Like a stack of plates: you add/remove from the top only
//
// Example with multiple pushes:
//   push $10        # Stack: [10]
//   push $20        # Stack: [20, 10]  (20 is on top)
//   push $30        # Stack: [30, 20, 10]  (30 is on top)
//   pop %rax        # %rax = 30, Stack: [20, 10]
//   pop %rbx        # %rbx = 20, Stack: [10]
//   pop %rcx        # %rcx = 10, Stack: []

// WHY THIS EXAMPLE?
//
// This program demonstrates the most basic use of the stack:
// 1. Temporarily storing a value (push)
// 2. Retrieving that value later (pop)
//
// In real programs, the stack is used for:
// - Saving register values that need to be preserved
// - Passing function parameters (when more than 6 parameters)
// - Storing local variables
// - Saving return addresses for function calls

// REGISTER SIZES AND STACK:
//
// Important: push and pop work with 64-bit (8-byte) values
// - push $10 stores 8 bytes on the stack (with zero-extension)
// - pop %rdi loads 8 bytes from the stack into %rdi
// - %rsp changes by 8 bytes with each push/pop
//
// Variants:
// - pushq/popq: explicitly 64-bit (8 bytes) - this is the default
// - pushw/popw: 16-bit (2 bytes) - rarely used
// - No 32-bit push/pop in x86-64 mode

// STACK ALIGNMENT:
//
// The x86-64 ABI requires:
// - %rsp must be 16-byte aligned before a CALL instruction
// - Our push makes %rsp = (original - 8)
// - This might violate alignment for some calls
// - For this simple program with just exit, it doesn't matter

// CHECKING THE EXIT CODE:
//
// After running this program, you can check the exit code:
//   ./pushpop1
//   echo $?
//
// Output: 10
//
// This confirms that:
// 1. The value 10 was successfully pushed onto the stack
// 2. The value 10 was successfully popped into %rdi
// 3. The exit syscall received 10 as the exit code

// TRY IT:
//
// Compile and run:
//   as pushpop1.s -o pushpop1.o
//   ld pushpop1.o -o pushpop1
//   ./pushpop1
//   echo $?     # Shows: 10
//
// Experiment:
// - Change push $10 to push $42
// - Run again and check: echo $?  (should show 42)
// - This proves the stack operations are working

// WHAT IF WE SKIP THE POP?
//
// Try commenting out the pop instruction:
//   push $10
//   # pop %rdi         <- commented out
//   mov $60,%rax
//   syscall
//
// What happens?
// - %rdi contains whatever value it had before (undefined)
// - The exit code will be unpredictable
// - The stack still has 10 on it (memory leak, though harmless here)
// - This shows that pop is necessary to retrieve pushed values

// COMMON MISTAKES:
//
// 1. PUSH/POP MISMATCH:
//    push $10
//    push $20
//    pop %rdi      # Gets 20, not 10!
//    # Forgot to pop the second value
//
// 2. POPPING WHEN STACK IS EMPTY:
//    pop %rdi      # Undefined behavior if nothing was pushed
//
// 3. FORGETTING STACK GROWS DOWNWARD:
//    Thinking push increases %rsp (wrong!)
//    push decreases %rsp (correct)
//
// 4. ASSUMING PUSH/POP PRESERVE VALUES:
//    push $10
//    (lots of code that uses the stack)
//    pop %rdi
//    # %rdi might NOT be 10 if other code pushed/popped
