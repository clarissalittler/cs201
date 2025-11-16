# PEDAGOGICAL PURPOSE:
# This program demonstrates how to implement a for-loop in assembly language.
# It computes the sum of integers from 1 to 10 (1+2+3+...+10 = 55) and returns
# the result as an exit code. This is a fundamental pattern for iteration.
#
# Key learning objectives:
# 1. Implementing for-loops using labels and conditional jumps
# 2. Understanding loop variables (index/counter) and accumulators
# 3. Register conventions: %rcx for loop counter (from x86 string operations)
# 4. Loop patterns: initialization, condition check, body, increment
# 5. While-style loops (test-before) vs do-while-style (test-after)
# 6. Translating high-level control flow to assembly
#
# CONCEPTUAL OVERVIEW:
# This implements the following C code:
#   int sum = 0;
#   for (int i = 1; i < 11; i++) {
#       sum += i;
#   }
#   return sum;  // Returns 55
#
# LOOP STYLE: This is a WHILE-STYLE loop (test before executing body)
# The condition is checked BEFORE each iteration, including the first one

        .section .text          # CODE SECTION: Executable instructions

        .global _start         # GLOBAL DIRECTIVE: Entry point

        # ORIGINAL COMMENTS:
        ## let's have a loop that adds all the numbers between 1 and 10 and puts that as the exit code

_start:
        # ORIGINAL COMMENTS:
        ## let's store our computations in %r8
        ## where do we store the index of our for-loop?
        ## By convention! Use %rcx for that
        ## for(%rcx = 1; %rcx<11; %rcx++){
        ##   %r8 = %r8 + %rcx
        ## }

        # LOOP INITIALIZATION: Setup before entering loop
        mov $1,%rcx            # INITIALIZE LOOP COUNTER: %rcx = 1
                               # %rcx is the loop index variable (i in C)
                               # %rcx is chosen by CONVENTION from x86 history:
                               #   - String operations use %rcx as a counter
                               #   - Loop instruction (rarely used) uses %rcx
                               #   - It's the "C" in "CouNter" (mnemonic)
                               # Starting at 1 because we want to sum 1..10

        mov $0,%r8             # INITIALIZE ACCUMULATOR: %r8 = 0
                               # %r8 will hold the running sum
                               # Initialized to 0 because: sum = 0 + 1 + 2 + ...
                               # %r8 is used for computation (any general register works)

        # LOOP STRUCTURE:
        # This is a WHILE-STYLE loop:
        #   1. Check condition FIRST
        #   2. If false, exit immediately (may never execute body)
        #   3. If true, execute body
        #   4. Increment and jump back to step 1

loop:                          # LOOP LABEL: Entry point for each iteration
                               # This is where we jump back to after each iteration
                               # Also where we fall through on first iteration

        #while loop style, tests before the loop body
        # ^^^ IMPORTANT COMMENT: Test happens BEFORE body execution

        cmp $11,%rcx           # TEST LOOP CONDITION: Is %rcx >= 11?
                               # Computes: %rcx - 11
                               # This tests the TERMINATION condition
                               # We want to loop WHILE %rcx < 11
                               # So we exit WHEN %rcx >= 11
                               # AT&T syntax: cmp $11,%rcx tests "Is %rcx compared to 11?"

        jge finish             # CONDITIONAL EXIT: Jump to finish if %rcx >= 11
                               # jge = "jump if greater or equal" (signed)
                               # If %rcx >= 11, we're done looping
                               # Otherwise, fall through to loop body
                               #
                               # ITERATION TRACE:
                               # Iteration 1: %rcx=1, 1<11, don't jump, continue
                               # Iteration 2: %rcx=2, 2<11, don't jump, continue
                               # ...
                               # Iteration 10: %rcx=10, 10<11, don't jump, continue
                               # After iteration 10: %rcx=11, 11>=11, JUMP to finish

        # LOOP BODY: The work done each iteration
        add %rcx,%r8           # ACCUMULATE: %r8 = %r8 + %rcx
                               # Add current loop counter to running sum
                               # This is the "sum += i" statement
                               #
                               # TRACE OF %r8:
                               # After iteration 1: %r8 = 0 + 1 = 1
                               # After iteration 2: %r8 = 1 + 2 = 3
                               # After iteration 3: %r8 = 3 + 3 = 6
                               # After iteration 4: %r8 = 6 + 4 = 10
                               # After iteration 5: %r8 = 10 + 5 = 15
                               # After iteration 6: %r8 = 15 + 6 = 21
                               # After iteration 7: %r8 = 21 + 7 = 28
                               # After iteration 8: %r8 = 28 + 8 = 36
                               # After iteration 9: %r8 = 36 + 9 = 45
                               # After iteration 10: %r8 = 45 + 10 = 55

        add $1,%rcx            # INCREMENT COUNTER: %rcx = %rcx + 1
                               # This is the "i++" or "i += 1" statement
                               # Could also use: inc %rcx (same effect, shorter encoding)
                               # Moves to next iteration: 1->2->3->...->10->11

        jmp loop               # UNCONDITIONAL JUMP: Go back to loop label
                               # This creates the loop!
                               # Jump back to test the condition again
                               # Equivalent to closing brace } in C for-loop

        # LOOP EXIT: Execution continues here when %rcx >= 11
finish:
        mov %r8,%rdi           # PREPARE RETURN VALUE: %rdi = %r8 (= 55)
                               # Transfer the sum to the exit code register
                               # %rdi holds the first argument to syscall (exit status)

        mov $60,%rax           # SYSCALL NUMBER: exit() = 60

        syscall                # INVOKE KERNEL: Execute exit(55)
                               # Program terminates with exit code 55
                               # You can verify with: echo $?

# FOR-LOOP ANATOMY:
# A for-loop has four parts: initialization, condition, increment, body
#
# C code:
#   for (init; condition; increment) {
#       body;
#   }
#
# Assembly translation (while-style):
#   init
# loop:
#   test condition
#   jump to finish if condition false
#   body
#   increment
#   jump to loop
# finish:
#
# This program:
#   init:       mov $1,%rcx; mov $0,%r8
#   condition:  cmp $11,%rcx; jge finish
#   body:       add %rcx,%r8
#   increment:  add $1,%rcx
#   jump back:  jmp loop

# WHILE-STYLE VS DO-WHILE-STYLE LOOPS:
# This is a WHILE-STYLE loop (test-before):
#   loop:
#     if (condition) {
#       body
#       goto loop
#     }
#   Might execute 0 times if condition initially false
#
# DO-WHILE-STYLE (test-after) would be:
#   loop:
#     body
#     if (condition) goto loop
#   Always executes at least once
#
# For-loops are typically while-style (test before body)

# REGISTER CHOICE CONVENTIONS:
# %rcx for loop counter:
#   - Historical: x86 "loop" instruction uses %rcx
#   - String operations (rep movsb, etc.) use %rcx as count
#   - "C" mnemonic for "Counter"
#
# %rax for accumulator:
#   - "A" for "Accumulator" (historical)
#   - Used for return values
#   - Used for syscall numbers
#
# In this program:
#   %rcx = loop index (conventional)
#   %r8 = accumulator (arbitrary choice, any register works)

# WHY TEST %rcx < 11 instead of %rcx <= 10?
# Both are equivalent, but "i < 11" is more idiomatic for:
#   for (i = 1; i < 11; i++)
# than:
#   for (i = 1; i <= 10; i++)
#
# In assembly, "i < 11" becomes:
#   cmp $11,%rcx; jge finish  (jump if NOT less than)
# While "i <= 10" would become:
#   cmp $10,%rcx; jg finish   (jump if greater than)
#
# The first is slightly more natural in assembly

# VERIFYING THE RESULT:
# Sum of 1 to 10 = 1+2+3+4+5+6+7+8+9+10
# Formula: n*(n+1)/2 = 10*11/2 = 110/2 = 55
# Exit code will be 55 (modulo 256 for 8-bit exit codes)

# LOOP INVARIANTS AND CORRECTNESS:
# Loop invariant: %r8 = sum of all %rcx values from 1 to current-1
# At start: %rcx=1, %r8=0 = sum from 1 to 0 (empty sum)
# After first iteration: %rcx=2, %r8=1 = sum from 1 to 1
# After second iteration: %rcx=3, %r8=3 = sum from 1 to 2
# ...
# After tenth iteration: %rcx=11, %r8=55 = sum from 1 to 10
# At this point, %rcx >= 11, so we exit

# EQUIVALENT C CODE:
# int main() {
#     int sum = 0;           // %r8
#     for (int i = 1; i < 11; i++) {  // %rcx
#         sum += i;
#     }
#     return sum;            // Returns 55
# }

# ALTERNATIVE LOOP STYLES:
# This program could also be written as:
#
# Do-while style (test after):
#   mov $1,%rcx
#   mov $0,%r8
# loop:
#   add %rcx,%r8
#   add $1,%rcx
#   cmp $11,%rcx
#   jl loop         # Jump if less than (continue looping)
#
# Countdown style (more efficient on some CPUs):
#   mov $10,%rcx
#   mov $0,%r8
# loop:
#   add %rcx,%r8
#   sub $1,%rcx     # or: dec %rcx
#   jnz loop        # Jump if not zero (very efficient!)

# TO ASSEMBLE, LINK, AND RUN:
# as for1.s -o for1.o
# ld for1.o -o for1
# ./for1
# echo $?                      # Should display 55
#
# EXPERIMENT:
# Change "cmp $11,%rcx" to "cmp $101,%rcx" to sum 1..100
# Result: 5050 -> exit code will be 5050 % 256 = 162
# (Exit codes are only 8 bits, so values wrap around)
