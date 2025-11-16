# PEDAGOGICAL PURPOSE:
# This program demonstrates how to implement a while-loop in assembly language.
# It computes the same sum of squares as the previous program (01-for-loop.s),
# but uses a countdown approach: 5² + 4² + 3² + 2² + 1² = 55. This illustrates
# the while-loop pattern and shows that loop order doesn't affect the final sum.
#
# Key learning objectives:
# 1. Implementing while-loops with test-before-body structure
# 2. Countdown loops: decrementing from N down to 0
# 3. Using dec (decrement) instruction vs inc (increment)
# 4. Comparing to zero with cmp and testing with jg (jump if greater)
# 5. Understanding loop termination conditions
# 6. Seeing how addition is commutative (order doesn't matter for sum)
# 7. Contrasting while-loops with for-loops and do-while loops
#
# CONCEPTUAL OVERVIEW:
# This implements the following C code:
#   int i = 5;
#   int sum = 0;
#   while (i > 0) {
#       sum += i * i;
#       i--;
#   }
#   return sum;  // Returns 55
#
# LOOP STRUCTURE: This is a WHILE-STYLE loop (test before executing body)
# The condition is checked BEFORE each iteration, including the first one

        .section .text          # CODE SECTION: Executable instructions

        .global _start         # GLOBAL DIRECTIVE: Entry point for linker

_start:
        # LOOP INITIALIZATION: Setup before entering loop
        ## Here's how we write a loop as a while loop that will count
        ## the same thing as the previous program but backwards

        mov $5,%rcx            # INITIALIZE LOOP COUNTER: %rcx = 5
                               # %rcx is the loop index variable (i in C)
                               # Starting at 5 to compute: 5² + 4² + 3² + 2² + 1²
                               # We count DOWN from 5 to 1
                               # %rcx chosen by CONVENTION (C for "Counter")

        mov $0,%rax            # INITIALIZE ACCUMULATOR: %rax = 0
                               # %rax will hold the running sum of squares
                               # Initialized to 0 because: sum = 0 + 5² + 4² + ...
                               # %rax for "Accumulator" (A in register naming)

        # WHILE-LOOP STRUCTURE:
        # A while-loop tests the condition BEFORE executing the body:
        #   while (condition) {
        #       body;
        #   }
        #
        # In assembly:
        # .loopStart:
        #   test condition
        #   jump to exit if condition false
        #   execute body
        #   jump back to .loopStart
        #
        # This is different from do-while, which tests AFTER the body

        .loopStart:            # LOOP LABEL: Entry point for each iteration
                               # This is where we jump back to after each iteration
                               # Also where we fall through on first iteration

        # LOOP BODY: Compute i² and add to sum
        mov %rcx,%rbx          # COPY COUNTER: %rbx = %rcx
                               # Copy loop counter to prepare for squaring
                               # We need a copy because imul modifies its destination
                               # Can't do imul %rcx,%rcx (would destroy the counter!)

        imul %rbx,%rbx         # COMPUTE SQUARE: %rbx = %rbx × %rbx = i²
                               # imul = signed integer multiply
                               # AT&T syntax: imul source,destination
                               # %rbx = %rbx × %rbx (self-multiplication for squaring)
                               #
                               # TRACE OF SQUARES (counting backwards):
                               # i=5: %rbx = 5 × 5 = 25
                               # i=4: %rbx = 4 × 4 = 16
                               # i=3: %rbx = 3 × 3 = 9
                               # i=2: %rbx = 2 × 2 = 4
                               # i=1: %rbx = 1 × 1 = 1

        add %rbx,%rax          # ACCUMULATE: %rax = %rax + %rbx
                               # Add current square to running sum
                               # This is the "sum += i*i" statement
                               #
                               # TRACE OF %rax (counting backwards):
                               # After i=5: %rax = 0 + 25 = 25
                               # After i=4: %rax = 25 + 16 = 41
                               # After i=3: %rax = 41 + 9 = 50
                               # After i=2: %rax = 50 + 4 = 54
                               # After i=1: %rax = 54 + 1 = 55
                               #
                               # Note: Same final result as forward counting!
                               # This demonstrates commutativity of addition

        dec %rcx               # DECREMENT COUNTER: %rcx = %rcx - 1
                               # dec = decrement by 1 (more compact than sub $1,%rcx)
                               # Opposite of inc (increment)
                               # This is the "i--" statement in the while-loop
                               # Counts down: 5→4→3→2→1→0

        # LOOP CONDITION TEST: Check if we should continue looping
        cmp $0,%rcx            # TEST TERMINATION: Compare %rcx to 0
                               # cmp performs: %rcx - 0 = %rcx
                               # AT&T syntax: cmp source,destination means destination - source
                               # The result is NOT stored, only CPU flags are set
                               #
                               # FLAGS SET (depends on %rcx value):
                               # When %rcx = 5: 5 - 0 = 5 (positive)
                               #   ZF = 0 (not zero), SF = 0 (positive), OF = 0
                               # When %rcx = 1: 1 - 0 = 1 (positive)
                               #   ZF = 0 (not zero), SF = 0 (positive), OF = 0
                               # When %rcx = 0: 0 - 0 = 0 (zero)
                               #   ZF = 1 (zero!), SF = 0 (not negative), OF = 0
                               #
                               # We want to loop WHILE %rcx > 0
                               # We exit WHEN %rcx <= 0

        jg .loopStart          # CONDITIONAL JUMP: Jump to .loopStart if %rcx > 0
                               # jg = "jump if greater than" (signed comparison)
                               # jg checks: (ZF=0) AND (SF == OF)
                               # For positive values: ZF=0, SF=0, OF=0, so jg jumps
                               # For zero: ZF=1, so jg does NOT jump (exits loop)
                               # For negative: SF=1, OF=0, so jg does NOT jump
                               #
                               # ITERATION TRACE:
                               # Start: %rcx=5, 5-0=5>0, ZF=0 SF=0, JUMP (execute body)
                               # After i=5: %rcx=4, 4-0=4>0, ZF=0 SF=0, JUMP
                               # After i=4: %rcx=3, 3-0=3>0, ZF=0 SF=0, JUMP
                               # After i=3: %rcx=2, 2-0=2>0, ZF=0 SF=0, JUMP
                               # After i=2: %rcx=1, 1-0=1>0, ZF=0 SF=0, JUMP
                               # After i=1: %rcx=0, 0-0=0, ZF=1, NO JUMP (exit loop)

        # PROGRAM TERMINATION: Exit with accumulated sum
        ## end of the program

        mov %rax,%rdi          # PREPARE EXIT CODE: %rdi = %rax (= 55)
                               # Transfer the sum to the syscall argument register
                               # %rdi holds the first argument to syscall (exit status)

        mov $60,%rax           # SYSCALL NUMBER: exit() = 60
                               # Overwrites the sum in %rax, but we saved it in %rdi
                               # 60 is the Linux syscall number for sys_exit

        syscall                # INVOKE KERNEL: Execute exit(55)
                               # Program terminates with exit code 55
                               # You can verify with: echo $?

# WHILE-LOOP ANATOMY:
# A while-loop has three key parts: initialization, condition, body
#
# C code:
#   init;
#   while (condition) {
#       body;
#       update;
#   }
#
# Assembly translation:
#   init
# loop:
#   test condition
#   jump to finish if condition false
#   body
#   update
#   jump to loop
# finish:
#
# This program:
#   init:       mov $5,%rcx; mov $0,%rax
#   condition:  cmp $0,%rcx; jg .loopStart (jump if greater)
#   body:       mov %rcx,%rbx; imul %rbx,%rbx; add %rbx,%rax
#   update:     dec %rcx
#   jump back:  (implicit, jg jumps back)

# LOOP STYLE COMPARISON:
#
# WHILE-STYLE (this program):
#   Tests condition BEFORE body
#   May execute 0 times if condition initially false
#   Pattern:
#     loop:
#       if (!condition) goto done
#       body
#       goto loop
#     done:
#
# DO-WHILE-STYLE (previous program):
#   Tests condition AFTER body
#   Always executes at least once
#   Pattern:
#     loop:
#       body
#       if (condition) goto loop
#
# FOR-LOOP-STYLE:
#   Similar to while, but with explicit init and update
#   Pattern:
#     init
#     loop:
#       if (!condition) goto done
#       body
#       update
#       goto loop
#     done:

# WHY COUNT BACKWARDS?
# Counting backwards (countdown loops) can be more efficient:
# 1. Compare to zero is simpler than compare to arbitrary value
# 2. Some CPUs can optimize "compare to zero" better
# 3. Testing flags after dec/sub is sometimes more efficient
# 4. Common pattern in optimized assembly code
#
# Alternative countdown pattern (even simpler):
#   mov $5,%rcx
# loop:
#   mov %rcx,%rbx
#   imul %rbx,%rbx
#   add %rbx,%rax
#   dec %rcx
#   jnz loop       # Jump if Not Zero - very efficient!
#                  # dec automatically sets ZF flag
#                  # No need for separate cmp instruction

# CPU FLAGS DETAILED EXPLANATION:
# The x86-64 CPU maintains a FLAGS register (RFLAGS) with individual status bits:
#
# ZF (Zero Flag, bit 6):
#   Set to 1 when an arithmetic operation produces zero
#   Set to 0 when result is non-zero
#   Example: cmp $0,%rcx when %rcx=0 sets ZF=1
#   Used by: je/jz (jump if equal/zero), jne/jnz (jump if not equal/zero)
#
# SF (Sign Flag, bit 7):
#   Set to 1 when result is negative (MSB = 1 in two's complement)
#   Set to 0 when result is positive or zero
#   Example: cmp $0,%rcx when %rcx=-1 gives -1, sets SF=1
#   Used by: js (jump if sign), jns (jump if not sign), jl, jge, jg, jle
#
# OF (Overflow Flag, bit 11):
#   Set to 1 when signed arithmetic overflows
#   Example: adding two large positive numbers gives negative
#   Set to 0 when no overflow occurs
#   Used by: jo (jump if overflow), jno (jump if not overflow)
#
# CF (Carry Flag, bit 0):
#   Set to 1 when unsigned arithmetic produces a carry or borrow
#   Example: subtracting larger from smaller unsigned number
#   Used by: jc (jump if carry), jnc (jump if not carry), jb, jae, jbe, ja

# CONDITIONAL JUMP INSTRUCTIONS FOR SIGNED COMPARISONS:
# After cmp X,Y (which computes Y - X):
#
#   jg/jnle - Jump if Greater / Jump if Not Less or Equal
#             Condition: (ZF = 0) AND (SF == OF)
#             Meaning: Y > X (signed)
#             This is what we use in this program!
#
#   jge/jnl - Jump if Greater or Equal / Jump if Not Less
#             Condition: SF == OF
#             Meaning: Y >= X (signed)
#
#   jl/jnge - Jump if Less / Jump if Not Greater or Equal
#             Condition: SF != OF
#             Meaning: Y < X (signed)
#
#   jle/jng - Jump if Less or Equal / Jump if Not Greater
#             Condition: (ZF = 1) OR (SF != OF)
#             Meaning: Y <= X (signed)
#
#   je/jz   - Jump if Equal / Jump if Zero
#             Condition: ZF = 1
#             Meaning: Y == X
#
#   jne/jnz - Jump if Not Equal / Jump if Not Zero
#             Condition: ZF = 0
#             Meaning: Y != X

# WHY WE USE jg (JUMP IF GREATER):
# After "cmp $0,%rcx", we want to continue looping if %rcx > 0
# The jg instruction tests if the destination operand (%rcx) is greater
# than the source operand (0) using SIGNED comparison.
#
# How jg works:
# - After subtraction (%rcx - 0), if result is positive and non-zero, then %rcx > 0
# - Positive non-zero: ZF=0 (not zero), SF=0 (positive), OF=0 (no overflow)
# - jg checks: (ZF=0) AND (SF==OF), which is true for positive values
# - This correctly implements "continue looping while i > 0"

# COMPARING dec vs sub $1:
# Both decrease the register by 1, but there are subtle differences:
#
# dec %rcx:
#   - Shorter encoding (1-2 bytes)
#   - Sets ZF, SF, OF, PF, AF flags
#   - Does NOT affect CF (carry flag)
#   - Preferred when you don't care about CF
#
# sub $1,%rcx:
#   - Slightly longer encoding (3-4 bytes)
#   - Sets ALL flags including CF
#   - More general (can subtract any value)
#   - Use when you need CF to be updated

# REGISTER USAGE:
# %rcx - Loop counter (convention: C for "Counter")
# %rax - Accumulator for sum (convention: A for "Accumulator")
# %rbx - Temporary for squaring (general purpose, preserves %rcx)

# EQUIVALENT C CODE:
# int main() {
#     int i = 5;             // %rcx
#     int sum = 0;           // %rax
#     while (i > 0) {
#         int square = i * i;  // %rbx
#         sum += square;
#         i--;
#     }
#     return sum;            // Returns 55
# }

# VERIFYING THE RESULT:
# Sum of squares from 5 down to 1:
# 5² + 4² + 3² + 2² + 1²
# = 25 + 16 + 9 + 4 + 1
# = 55
#
# Same result as 01-for-loop.s because addition is commutative:
# (0² + 1² + 2² + 3² + 4² + 5²) = (5² + 4² + 3² + 2² + 1² + 0²)

# LOOP INVARIANTS AND CORRECTNESS:
# Loop invariant: %rax = sum of squares from %rcx+1 to 5
#
# Before first iteration: %rcx=5, %rax=0 = sum from 6 to 5 (empty sum)
# After iteration 1: %rcx=4, %rax=25 = 5²
# After iteration 2: %rcx=3, %rax=41 = 5² + 4²
# After iteration 3: %rcx=2, %rax=50 = 5² + 4² + 3²
# After iteration 4: %rcx=1, %rax=54 = 5² + 4² + 3² + 2²
# After iteration 5: %rcx=0, %rax=55 = 5² + 4² + 3² + 2² + 1²
# At this point, %rcx <= 0, so we exit the loop

# ALTERNATIVE IMPLEMENTATIONS:
#
# Using jnz instead of cmp+jg (more efficient):
#   mov $5,%rcx
#   mov $0,%rax
# .loop:
#   mov %rcx,%rbx
#   imul %rbx,%rbx
#   add %rbx,%rax
#   dec %rcx
#   jnz .loop          # Jump if Not Zero (dec sets ZF)
#
# Forward counting (like 01-for-loop.s):
#   mov $0,%rcx
#   mov $0,%rax
# .loop:
#   cmp $6,%rcx
#   jge .done
#   mov %rcx,%rbx
#   imul %rcx,%rbx
#   add %rbx,%rax
#   inc %rcx
#   jmp .loop
# .done:

# COMPARING FORWARD vs BACKWARD LOOPS:
#
# Forward (01-for-loop.s):
#   Pros: Natural order (0,1,2,3,4,5), matches typical for-loop
#   Cons: Need to compare against specific value (6)
#
# Backward (this program):
#   Pros: Can use "compare to zero" which may be more efficient
#   Cons: Less intuitive, backwards from typical for-loop
#
# Both produce same result due to commutativity of addition!

# TO ASSEMBLE, LINK, AND RUN:
# as 02-while-loop.s -o 02-while-loop.o
# ld 02-while-loop.o -o 02-while-loop
# ./02-while-loop
# echo $?                      # Should display 55
#
# COMPARE WITH 01-for-loop:
# as 01-for-loop.s -o 01-for-loop.o
# ld 01-for-loop.o -o 01-for-loop
# ./01-for-loop
# echo $?                      # Also displays 55!
#
# DEBUGGING TIPS:
# Use gdb to trace execution:
# gdb ./02-while-loop
# (gdb) break _start
# (gdb) run
# (gdb) info registers rcx rax rbx  # View register values
# (gdb) stepi                        # Step one instruction
# (gdb) display/i $pc                # Show current instruction
#
# EXPERIMENT:
# Change "mov $5,%rcx" to "mov $10,%rcx" to sum squares 10..1
# Result: 10² + 9² + ... + 1² = 385 -> exit code will be 385 % 256 = 129
