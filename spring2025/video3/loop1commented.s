# PEDAGOGICAL PURPOSE:
# This program demonstrates a countdown loop that sums numbers from N down to 1.
# It shows how to structure loops with the test at the beginning (like a while loop).
# Key learning objectives:
# 1. Loop structure with condition at the start
# 2. Countdown loops (decrementing counter)
# 3. Testing for less-than-or-equal-to-zero (jle)
# 4. Accumulator pattern (sum variable)
# 5. Using subtraction to decrement loop counter
# 6. How while-loops translate to assembly

        .section .text          # CODE SECTION
        .global _start

        ## let's have a function
        ## that will put a number in %rcx
        ## and then use labels to make a loop
        ## that adds all the numbers from
        ## 0 to the initial value of %rcx

loopstart:                      # LOOP CONDITION LABEL: Test before each iteration
                                # This is the entry point of the loop
                                # Testing condition at start = while-loop structure

        cmp $0,%rcx             # COMPARE WITH ZERO: Is %rcx <= 0?
                                # Computes: %rcx - 0
                                # Sets flags based on result

        jle end                 # EXIT LOOP IF: %rcx <= 0
                                # jle = jump if less than or equal (signed)
                                # If %rcx is 0 or negative, exit loop
                                # This implements: if (rcx <= 0) break;
                                # Or: while (rcx > 0) { ... }

        add %rcx,%rax           # ACCUMULATE: sum += rcx
                                # Add current value of %rcx to accumulator
                                # %rax += %rcx
                                #adds current value of rcx into rax, rax is the accumulator

        sub $1,%rcx             # DECREMENT: rcx--
                                # Subtract 1 from loop counter
                                # %rcx = %rcx - 1
                                # set rcx = rcx-1
                                # This counts down: 10 → 9 → 8 → ... → 1 → 0

        jmp loopstart           # REPEAT LOOP: Go back to test condition
                                # Unconditional jump back to loop start
                                # Creates the loop!

_start:                         # PROGRAM ENTRY POINT
                                # Note: _start is AFTER the loop code!
                                # This works because we jump directly to loopstart

        mov $0,%rax             # INITIALIZE ACCUMULATOR: sum = 0
                                # %rax will hold our running total

        mov $10,%rcx            # INITIALIZE COUNTER: n = 10
                                # %rcx is our loop counter
                                # We'll sum: 10 + 9 + 8 + ... + 2 + 1 = 55
                                # the sum should be 55

        jmp loopstart           # START LOOP: Jump to loop beginning
                                # Explicitly jump to the loop
                                # Could also have put loopstart after _start

end:                            # LOOP EXIT LABEL: After loop finishes
                                # Reached when %rcx <= 0

        mov %rax,%rdi           # SET EXIT CODE: %rdi = sum
                                # Move accumulated value to exit code register
                                #moves accumulated value into the return code

        mov $60,%rax            # SETUP EXIT SYSCALL: %rax = 60
                                # Overwrites sum, but we saved it in %rdi
                                #sets up exit system call (with %rdi as exit code)

        syscall                 # EXIT: Exit with status 55

# WHAT THIS PROGRAM DOES:
# 1. Initializes sum = 0, n = 10
# 2. Jumps to loop
# 3. Loop:
#    - If n <= 0, exit loop
#    - sum += n
#    - n--
#    - Repeat
# 4. Exits with sum = 55

# TO ASSEMBLE AND RUN:
# as loop1.s -o loop1.o
# ld loop1.o -o loop1
# ./loop1
# echo $?  # Shows 55

# EXPECTED EXIT STATUS:
# 55 (10 + 9 + 8 + 7 + 6 + 5 + 4 + 3 + 2 + 1)

# LOOP EXECUTION TRACE:
#
# Initial: %rcx=10, %rax=0
#
# Iteration 1: cmp $0,%rcx → 10 > 0, continue
#              add %rcx,%rax → %rax = 0 + 10 = 10
#              sub $1,%rcx → %rcx = 9
# Iteration 2: cmp $0,%rcx → 9 > 0, continue
#              add %rcx,%rax → %rax = 10 + 9 = 19
#              sub $1,%rcx → %rcx = 8
# Iteration 3: %rax = 19 + 8 = 27, %rcx = 7
# Iteration 4: %rax = 27 + 7 = 34, %rcx = 6
# Iteration 5: %rax = 34 + 6 = 40, %rcx = 5
# Iteration 6: %rax = 40 + 5 = 45, %rcx = 4
# Iteration 7: %rax = 45 + 4 = 49, %rcx = 3
# Iteration 8: %rax = 49 + 3 = 52, %rcx = 2
# Iteration 9: %rax = 52 + 2 = 54, %rcx = 1
# Iteration 10: %rax = 54 + 1 = 55, %rcx = 0
# Iteration 11: cmp $0,%rcx → 0 <= 0, jump to end!

# FORMULA VERIFICATION:
# Sum of 1 to N = N × (N + 1) / 2
# Sum of 1 to 10 = 10 × 11 / 2 = 110 / 2 = 55 ✓

# COMPARISON TO C:
# This program implements:
#   int main() {
#       int sum = 0;
#       int n = 10;
#       while (n > 0) {   // or: for (; n > 0; n--)
#           sum += n;
#           n--;
#       }
#       return sum;  // Returns 55
#   }

# WHILE-LOOP STRUCTURE:
#
# C while-loop:
#   while (condition) {
#       body;
#   }
#
# Assembly pattern:
# loop:
#   test condition
#   jump if false to done
#   body
#   jump to loop
# done:
#
# Our loop:
# loopstart:
#   cmp $0,%rcx      # test: rcx > 0?
#   jle end          # exit if rcx <= 0
#   add %rcx,%rax    # body: sum += rcx
#   sub $1,%rcx      # body: rcx--
#   jmp loopstart    # repeat
# end:

# COUNTDOWN vs COUNTUP LOOPS:
#
# Countdown (this program):
#   mov $10,%rcx
# loop:
#   cmp $0,%rcx
#   jle done
#   # use %rcx
#   sub $1,%rcx
#   jmp loop
# done:
#
# Countup (like data2.s):
#   mov $0,%rcx
# loop:
#   cmp $10,%rcx
#   jge done
#   # use %rcx
#   add $1,%rcx
#   jmp loop
# done:
#
# Both styles are equivalent; choice depends on context.

# WHY TEST AT START VS END?
#
# Test at start (while):
# loop:
#   test
#   je done
#   body
#   jmp loop
# done:
# - Handles zero iterations correctly
# - Common for while and for loops
#
# Test at end (do-while):
# loop:
#   body
#   test
#   jne loop
# - Always runs at least once
# - One fewer jump per iteration (faster!)
# - Common for do-while loops

# CODE ORGANIZATION:
#
# Notice that loopstart comes BEFORE _start!
# This works because:
# 1. Execution starts at _start (linker's entry point)
# 2. _start jumps to loopstart explicitly
# 3. Label order in file doesn't matter for jumps
#
# Alternative organization:
# _start:
#   mov $0,%rax
#   mov $10,%rcx
# loopstart:
#   cmp $0,%rcx
#   jle end
#   add %rcx,%rax
#   sub $1,%rcx
#   jmp loopstart
# end:
#   mov %rax,%rdi
#   mov $60,%rax
#   syscall
#
# Both are equivalent!

# ACCUMULATOR PATTERN:
#
# 1. Initialize accumulator to identity value:
#    - For addition: 0
#    - For multiplication: 1
#    - For max: -infinity
#    - For min: +infinity
#
# 2. Loop through values:
#    add value to accumulator (or other operation)
#
# 3. Result is in accumulator
#
# Our example:
#   mov $0,%rax        # Initialize sum to 0
# loop:
#   add %rcx,%rax      # Add current value
#   ...
# end:
#   # %rax has final sum
