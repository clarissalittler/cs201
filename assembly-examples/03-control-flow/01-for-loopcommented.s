# PEDAGOGICAL PURPOSE:
# This program demonstrates how to implement a for-loop in assembly language.
# It computes the sum of squares from 0 to 5 (0² + 1² + 2² + 3² + 4² + 5² = 55)
# and returns the result as an exit code. This illustrates fundamental loop patterns
# and how high-level for-loops translate to low-level assembly instructions.
#
# Key learning objectives:
# 1. Implementing for-loops using labels, comparisons, and conditional jumps
# 2. Understanding loop counter initialization, condition testing, and increment
# 3. Using cmp instruction to test loop termination conditions
# 4. Conditional branching with jl (jump if less than) for signed comparisons
# 5. Loop body execution and accumulation patterns
# 6. Translating C for-loops to assembly control flow structures
# 7. Understanding CPU flags (ZF, SF, OF) and how they affect conditional jumps
#
# CONCEPTUAL OVERVIEW:
# This implements the following C code:
#   int sum = 0;
#   for (int i = 0; i < 6; i++) {
#       sum += i * i;  // Add square of i
#   }
#   return sum;  // Returns 55
#
# LOOP STRUCTURE: This is a DO-WHILE-STYLE loop (test after executing body)
# The condition is checked AFTER each iteration, so the body always executes at least once

        .section .text          # CODE SECTION: Executable instructions

        .global _start         # GLOBAL DIRECTIVE: Entry point for linker
                               # Makes _start visible to the linker

_start:
        # LOOP INITIALIZATION: Setup before entering loop
        mov $0,%rax            # INITIALIZE ACCUMULATOR: %rax = 0
                               # %rax will hold the running sum of squares
                               # Initialized to 0 because: sum = 0 + 0² + 1² + 2² + ...

        mov $0,%rcx            # INITIALIZE LOOP COUNTER: %rcx = 0
                               # %rcx is the loop index variable (i in C)
                               # Starting at 0 to compute: 0² + 1² + 2² + 3² + 4² + 5²
                               # %rcx chosen by CONVENTION (C for "Counter")
                               # setting up the loop

        # LOOP STRUCTURE: DO-WHILE-STYLE
        # This loop executes the body FIRST, then checks the condition
        # Pattern:
        #   .loopStart:
        #       body (compute and accumulate)
        #       increment counter
        #       test condition
        #       jump back if condition true

        .loopStart:            # LOOP LABEL: Entry point for each iteration
                               # This is where we jump back to after each iteration
                               # Also where we fall through on first iteration

        # LOOP BODY: Compute i² and add to sum
        mov %rcx,%rbx          # COPY COUNTER: %rbx = %rcx
                               # Copy loop counter to prepare for squaring
                               # We use %rbx as a temporary register
                               # Can't do imul %rcx,%rcx (would destroy original counter)

        imul %rcx,%rbx         # COMPUTE SQUARE: %rbx = %rcx × %rcx
                               # imul = signed integer multiply
                               # AT&T syntax: imul source,destination
                               # %rbx = %rbx × %rcx = i × i = i²
                               #
                               # TRACE OF SQUARES:
                               # i=0: %rbx = 0 × 0 = 0
                               # i=1: %rbx = 1 × 1 = 1
                               # i=2: %rbx = 2 × 2 = 4
                               # i=3: %rbx = 3 × 3 = 9
                               # i=4: %rbx = 4 × 4 = 16
                               # i=5: %rbx = 5 × 5 = 25

        add %rbx,%rax          # ACCUMULATE: %rax = %rax + %rbx
                               # Add current square to running sum
                               # This is the "sum += i*i" statement
                               #
                               # TRACE OF %rax:
                               # After i=0: %rax = 0 + 0 = 0
                               # After i=1: %rax = 0 + 1 = 1
                               # After i=2: %rax = 1 + 4 = 5
                               # After i=3: %rax = 5 + 9 = 14
                               # After i=4: %rax = 14 + 16 = 30
                               # After i=5: %rax = 30 + 25 = 55

        inc %rcx               # INCREMENT COUNTER: %rcx = %rcx + 1
                               # inc = increment by 1 (more compact than add $1,%rcx)
                               # This is the "i++" statement in the for-loop
                               # Moves to next iteration: 0→1→2→3→4→5→6

        # LOOP CONDITION TEST: Check if we should continue looping
        cmp $6,%rcx            # TEST TERMINATION: Compare %rcx to 6
                               # cmp performs: %rcx - 6
                               # AT&T syntax: cmp source,destination means destination - source
                               # The result is NOT stored, only CPU flags are set
                               #
                               # FLAGS SET (depends on %rcx value):
                               # When %rcx = 0: 0 - 6 = -6 (negative)
                               #   ZF = 0 (not zero), SF = 1 (negative), OF = 0
                               # When %rcx = 5: 5 - 6 = -1 (negative)
                               #   ZF = 0 (not zero), SF = 1 (negative), OF = 0
                               # When %rcx = 6: 6 - 6 = 0 (zero)
                               #   ZF = 1 (zero!), SF = 0 (not negative), OF = 0
                               #
                               # We want to loop WHILE %rcx < 6
                               # We exit WHEN %rcx >= 6

        jl .loopStart          # CONDITIONAL JUMP: Jump to .loopStart if %rcx < 6
                               # jl = "jump if less than" (signed comparison)
                               # jl checks: SF != OF (sign flag differs from overflow flag)
                               # For our case (no overflow): jl jumps when SF=1, meaning negative result
                               # If %rcx - 6 is negative, then %rcx < 6, so continue looping
                               #
                               # ITERATION TRACE:
                               # After i=0: %rcx=1, 1-6=-5<0, SF=1, JUMP back
                               # After i=1: %rcx=2, 2-6=-4<0, SF=1, JUMP back
                               # After i=2: %rcx=3, 3-6=-3<0, SF=1, JUMP back
                               # After i=3: %rcx=4, 4-6=-2<0, SF=1, JUMP back
                               # After i=4: %rcx=5, 5-6=-1<0, SF=1, JUMP back
                               # After i=5: %rcx=6, 6-6=0, SF=0, NO JUMP (exit loop)

        # PROGRAM TERMINATION: Exit with accumulated sum
        mov %rax,%rdi          # PREPARE EXIT CODE: %rdi = %rax (= 55)
                               # Transfer the sum to the syscall argument register
                               # %rdi holds the first argument to syscall (exit status)
                               # ending the program

        mov $60, %rax          # SYSCALL NUMBER: exit() = 60
                               # Overwrites the sum in %rax, but we saved it in %rdi
                               # 60 is the Linux syscall number for sys_exit

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
# This program uses DO-WHILE-STYLE (test after body):
#   init
# loop:
#   body
#   increment
#   test condition
#   jump to loop if condition true
#
# Our specific loop:
#   init:       mov $0,%rax; mov $0,%rcx
#   body:       mov %rcx,%rbx; imul %rcx,%rbx; add %rbx,%rax
#   increment:  inc %rcx
#   condition:  cmp $6,%rcx; jl .loopStart
#
# DO-WHILE-STYLE VS WHILE-STYLE LOOPS:
# This is a DO-WHILE-STYLE loop (test-after):
#   loop:
#     body
#     increment
#     if (condition) goto loop
#   Always executes at least once
#
# WHILE-STYLE (test-before) would be:
#   loop:
#     if (!condition) goto done
#     body
#     increment
#     goto loop
#   done:
#   Might execute 0 times if condition initially false
#
# For this program, both styles work because we start at 0 and 0 < 6

# CPU FLAGS DETAILED EXPLANATION:
# The x86-64 CPU maintains a FLAGS register (RFLAGS) with individual status bits:
#
# ZF (Zero Flag, bit 6):
#   Set to 1 when an arithmetic operation produces zero
#   Set to 0 when result is non-zero
#   Example: cmp $6,%rcx when %rcx=6 sets ZF=1
#   Used by: je/jz (jump if equal/zero), jne/jnz (jump if not equal/zero)
#
# SF (Sign Flag, bit 7):
#   Set to 1 when result is negative (MSB = 1 in two's complement)
#   Set to 0 when result is positive or zero
#   Example: cmp $6,%rcx when %rcx=5 gives -1, sets SF=1
#   Used by: js (jump if sign), jns (jump if not sign), jl, jge, etc.
#
# OF (Overflow Flag, bit 11):
#   Set to 1 when signed arithmetic overflows
#   Example: adding 0x7FFFFFFF + 0x7FFFFFFF gives negative (overflow!)
#   Set to 0 when no overflow occurs
#   Used by: jo (jump if overflow), jno (jump if not overflow), jl, jge, etc.
#
# CF (Carry Flag, bit 0):
#   Set to 1 when unsigned arithmetic produces a carry out or borrow
#   Example: adding 0xFFFFFFFF + 1 produces carry
#   Used by: jc (jump if carry), jnc (jump if not carry), jb, jae, etc.

# CONDITIONAL JUMP INSTRUCTIONS FOR SIGNED COMPARISONS:
# After cmp X,Y (which computes Y - X):
#
#   jl/jnge - Jump if Less / Jump if Not Greater or Equal
#             Condition: SF != OF (sign differs from overflow)
#             Meaning: Y < X (signed)
#
#   jle/jng - Jump if Less or Equal / Jump if Not Greater
#             Condition: (SF != OF) OR (ZF = 1)
#             Meaning: Y <= X (signed)
#
#   jg/jnle - Jump if Greater / Jump if Not Less or Equal
#             Condition: (SF == OF) AND (ZF = 0)
#             Meaning: Y > X (signed)
#
#   jge/jnl - Jump if Greater or Equal / Jump if Not Less
#             Condition: SF == OF
#             Meaning: Y >= X (signed)
#
#   je/jz   - Jump if Equal / Jump if Zero
#             Condition: ZF = 1
#             Meaning: Y == X
#
#   jne/jnz - Jump if Not Equal / Jump if Not Zero
#             Condition: ZF = 0
#             Meaning: Y != X

# WHY WE USE jl (JUMP IF LESS):
# After "cmp $6,%rcx", we want to continue looping if %rcx < 6
# The jl instruction tests if the destination operand (%rcx) is less than
# the source operand (6) using SIGNED comparison.
#
# How jl works:
# - After subtraction (%rcx - 6), if result is negative, then %rcx < 6
# - Negative result means SF=1 (sign flag set)
# - If no overflow occurred, OF=0, so SF != OF, and jl jumps
# - This correctly implements "continue looping while i < 6"

# REGISTER CHOICE CONVENTIONS:
# %rcx for loop counter:
#   - Historical: x86 "loop" instruction uses %rcx
#   - String operations (rep movsb, etc.) use %rcx as count
#   - "C" mnemonic for "Counter"
#
# %rax for accumulator:
#   - "A" for "Accumulator" (historical)
#   - Used for return values and syscall numbers
#   - Natural choice for sum/result
#
# %rbx for temporary:
#   - General-purpose register
#   - "B" register in historical x86 naming (after A)
#   - Used here to compute squares without destroying loop counter

# EQUIVALENT C CODE:
# int main() {
#     int sum = 0;           // %rax
#     for (int i = 0; i < 6; i++) {  // %rcx
#         int square = i * i;         // %rbx
#         sum += square;
#     }
#     return sum;            // Returns 55
# }

# VERIFYING THE RESULT:
# Sum of squares from 0 to 5:
# 0² + 1² + 2² + 3² + 4² + 5²
# = 0 + 1 + 4 + 9 + 16 + 25
# = 55
#
# Formula: Sum of squares from 0 to n = n(n+1)(2n+1)/6
# For n=5: 5×6×11/6 = 330/6 = 55 ✓

# LOOP INVARIANTS AND CORRECTNESS:
# Loop invariant: %rax = sum of squares from 0 to (%rcx - 1)
#
# Before first iteration: %rcx=0, %rax=0 = sum from 0 to -1 (empty sum)
# After iteration 1: %rcx=1, %rax=0 = 0²
# After iteration 2: %rcx=2, %rax=1 = 0² + 1²
# After iteration 3: %rcx=3, %rax=5 = 0² + 1² + 2²
# After iteration 4: %rcx=4, %rax=14 = 0² + 1² + 2² + 3²
# After iteration 5: %rcx=5, %rax=30 = 0² + 1² + 2² + 3² + 4²
# After iteration 6: %rcx=6, %rax=55 = 0² + 1² + 2² + 3² + 4² + 5²
# At this point, %rcx >= 6, so we exit the loop

# ALTERNATIVE IMPLEMENTATIONS:
# While-style (test before):
#   mov $0,%rax
#   mov $0,%rcx
# .loop:
#   cmp $6,%rcx
#   jge .done          # Exit if %rcx >= 6
#   mov %rcx,%rbx
#   imul %rcx,%rbx
#   add %rbx,%rax
#   inc %rcx
#   jmp .loop
# .done:
#
# Using dec instead of inc (countdown):
#   mov $0,%rax
#   mov $5,%rcx        # Start from 5
# .loop:
#   mov %rcx,%rbx
#   imul %rcx,%rbx
#   add %rbx,%rax
#   dec %rcx           # Count down
#   cmp $0,%rcx
#   jge .loop          # Continue while %rcx >= 0

# TO ASSEMBLE, LINK, AND RUN:
# as 01-for-loop.s -o 01-for-loop.o
# ld 01-for-loop.o -o 01-for-loop
# ./01-for-loop
# echo $?                      # Should display 55
#
# DEBUGGING TIPS:
# Use gdb to trace execution:
# gdb ./01-for-loop
# (gdb) break _start
# (gdb) run
# (gdb) info registers rcx rax rbx  # View register values
# (gdb) stepi                        # Step one instruction
# (gdb) continue                     # Continue to next breakpoint
#
# EXPERIMENT:
# Change "cmp $6,%rcx" to "cmp $10,%rcx" to sum squares 0..9
# Result: 0² + 1² + ... + 9² = 285 -> exit code will be 285 % 256 = 29
# (Exit codes are only 8 bits, so values wrap around)
