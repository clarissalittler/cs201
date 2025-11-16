# PEDAGOGICAL PURPOSE:
# This program demonstrates loops in assembly by summing array elements.
# It combines array indexing, loops, conditionals, and accumulation patterns.
# Key learning objectives:
# 1. Implementing for-loops in assembly (initialization, condition, increment)
# 2. Using labels for loop control (loopStart, loopEnd)
# 3. Conditional jumps (jge) to exit loops
# 4. Unconditional jumps (jmp) to repeat loops
# 5. The accumulator pattern for computing sums
# 6. Combining indexed addressing with loop counters

.section .data               # DATA SECTION: Initialized global variables
arr:    .quad 1,2,3,4,5     # GLOBAL ARRAY: Five 64-bit integers
                            # Array contains: [1, 2, 3, 4, 5]
                            # Sum = 1 + 2 + 3 + 4 + 5 = 15
                            # Memory layout (8 bytes each):
                            #   arr[0]=1 at offset 0
                            #   arr[1]=2 at offset 8
                            #   arr[2]=3 at offset 16
                            #   arr[3]=4 at offset 24
                            #   arr[4]=5 at offset 32

        .section .text      # CODE SECTION
        .global _start      # Make _start visible to linker

_start:                     # PROGRAM ENTRY POINT

        lea arr(%rip),%r8   # LOAD ARRAY ADDRESS: %r8 = address of arr
                            # %r8 is our base pointer for array access
                            # C equivalent: long *r8 = arr;

        ## choose %r9 as our accumulator
        mov $0,%r9          # INITIALIZE ACCUMULATOR: %r9 = 0
                            # %r9 will hold the running sum
                            # We start at 0 and add each element to it
                            # C equivalent: long sum = 0;

        mov $0,%rcx         # INITIALIZE LOOP COUNTER: %rcx = 0
                            # %rcx holds the loop index (e.g. it is `i`)
                            # We'll use this to access arr[0], arr[1], etc.
                            # C equivalent: int i = 0;

        ## time to start the loop
loopStart:                  # LOOP LABEL: Start of loop body
                            # This is the target for "jmp loopStart"
                            # Execution jumps here to repeat the loop

        cmp $5,%rcx         # COMPARE: Is %rcx >= 5?
                            # Compare loop counter with array length
                            # Sets CPU flags based on (%rcx - 5)
                            # WHY 5? Because array has 5 elements (indices 0-4)
                            # C equivalent: if (i >= 5)

        jge loopEnd         # CONDITIONAL JUMP: Jump if Greater or Equal
                            # If %rcx >= 5, we've processed all elements, so exit
                            # "jge" checks the flags set by "cmp"
                            # If true: jump to loopEnd (exit loop)
                            # If false: continue to next instruction (loop body)
                            # C equivalent: if (i >= 5) goto loopEnd;
                            #
                            # WHY "jge" (signed) not "jae" (unsigned)?
                            # - jge = "jump if greater or equal" (signed comparison)
                            # - jae = "jump if above or equal" (unsigned comparison)
                            # - Both work here since we're comparing positive numbers
                            # - jge is conventional for loop counters

        add (%r8,%rcx,8),%r9  # ACCUMULATE: sum += arr[i]
                              # INDEXED ACCESS: (%r8,%rcx,8) = arr[i]
                              # - base = %r8 (array address)
                              # - index = %rcx (current index i)
                              # - scale = 8 (element size)
                              # - Address = %r8 + (%rcx * 8)
                              # - Load value at that address
                              # - Add it to %r9 (accumulator)
                              #
                              # ITERATION BY ITERATION:
                              # i=0: add arr[0]=1 to sum, sum becomes 1
                              # i=1: add arr[1]=2 to sum, sum becomes 3
                              # i=2: add arr[2]=3 to sum, sum becomes 6
                              # i=3: add arr[3]=4 to sum, sum becomes 10
                              # i=4: add arr[4]=5 to sum, sum becomes 15
                              #
                              # C equivalent: sum += arr[i];

        add $1,%rcx         # INCREMENT COUNTER: i++
                            # Move to the next array element
                            # %rcx goes from 0 to 1 to 2 to 3 to 4 to 5
                            # When %rcx reaches 5, we exit (5 >= 5)
                            # C equivalent: i++;

        jmp loopStart       # UNCONDITIONAL JUMP: Repeat loop
                            # Jump back to start of loop
                            # Check the condition again
                            # This creates the loop cycle
                            # C equivalent: goto loopStart; (end of loop body)

loopEnd:                    # LOOP EXIT LABEL: End of loop
                            # We arrive here when %rcx >= 5
                            # At this point, %r9 contains the sum (15)

        mov %r9,%rdi        # PREPARE EXIT CODE: %rdi = sum
                            # Move the accumulated sum into %rdi
                            # %rdi is the first argument to exit syscall
                            # The program will exit with status 15
                            # C equivalent: return sum;

        mov $60,%rax        # SYSCALL NUMBER: 60 = exit

        syscall             # EXIT: Terminate with status 15
                            # Exit code is 15 (sum of 1+2+3+4+5)
                            # Verify with: ./globalArray3; echo $?

# LOOP STRUCTURE IN ASSEMBLY:
#
# C for-loop:
#   for (int i = 0; i < 5; i++) {
#       sum += arr[i];
#   }
#
# Assembly equivalent:
#   mov $0,%rcx           # i = 0 (initialization)
# loopStart:              # (loop label)
#   cmp $5,%rcx           # i < 5? (condition check)
#   jge loopEnd           # if (i >= 5) exit loop
#   add (%r8,%rcx,8),%r9  # sum += arr[i] (loop body)
#   add $1,%rcx           # i++ (increment)
#   jmp loopStart         # repeat
# loopEnd:                # (after loop)

# LOOP CONTROL FLOW:
#
# Execution path for this program:
#
# 1. Initialize: %r8=arr, %r9=0, %rcx=0
# 2. Jump to loopStart
# 3. Compare %rcx (0) with 5: 0 < 5, don't jump
# 4. Add arr[0]=1 to %r9, now %r9=1
# 5. Increment %rcx to 1
# 6. Jump to loopStart
# 7. Compare %rcx (1) with 5: 1 < 5, don't jump
# 8. Add arr[1]=2 to %r9, now %r9=3
# 9. Increment %rcx to 2
# 10. Jump to loopStart
# ... (continue for i=2,3,4) ...
# N. Compare %rcx (5) with 5: 5 >= 5, jump to loopEnd
# N+1. Execute loopEnd section, exit with status 15

# CONDITIONAL JUMP INSTRUCTIONS:
#
# After "cmp $5,%rcx" (computes %rcx - 5):
#
# jge  loopEnd    # Jump if Greater or Equal (signed)
# jg   loopEnd    # Jump if Greater (signed)
# jle  loopEnd    # Jump if Less or Equal (signed)
# jl   loopEnd    # Jump if Less (signed)
# je   loopEnd    # Jump if Equal
# jne  loopEnd    # Jump if Not Equal
#
# For unsigned comparisons:
# jae  loopEnd    # Jump if Above or Equal (unsigned >= )
# ja   loopEnd    # Jump if Above (unsigned >)
# jbe  loopEnd    # Jump if Below or Equal (unsigned <=)
# jb   loopEnd    # Jump if Below (unsigned <)
#
# WHY different instructions for signed/unsigned?
# - 0xFF as signed byte = -1
# - 0xFF as unsigned byte = 255
# - So "is 0xFF < 0x01?" depends on signedness!

# ACCUMULATOR PATTERN:
#
# The accumulator pattern is fundamental in programming:
#
# 1. Initialize accumulator to identity value:
#    - For sum: start at 0
#    - For product: start at 1
#    - For max: start at -∞ or first element
#
# 2. Loop through data:
#    - Add each element to accumulator (for sum)
#    - Multiply each element with accumulator (for product)
#    - Compare each element with accumulator (for max)
#
# 3. Accumulator holds final result
#
# This program uses sum accumulator:
#   %r9 = 0          (identity for addition)
#   %r9 += arr[0]    (1)
#   %r9 += arr[1]    (3)
#   %r9 += arr[2]    (6)
#   %r9 += arr[3]    (10)
#   %r9 += arr[4]    (15)
#   return %r9       (15)

# REGISTER USAGE SUMMARY:
#
# %r8  = Base address of array (constant during loop)
# %r9  = Accumulator for sum (changes each iteration)
# %rcx = Loop counter/index (0, 1, 2, 3, 4, 5)
# %rdi = Exit status (sum at end)
# %rax = Syscall number (60 = exit)

# LOOP TERMINATION:
#
# IMPORTANT: We use "cmp $5,%rcx" and "jge"
# This means we check BEFORE accessing the array
# If we checked AFTER, we'd access arr[5] which doesn't exist!
#
# Safe (what we do):
#   cmp $5,%rcx           # Check if i >= 5
#   jge loopEnd           # Exit if true
#   add (%r8,%rcx,8),%r9  # Safe: only executes if i < 5
#
# UNSAFE (don't do this):
#   add (%r8,%rcx,8),%r9  # Might access arr[5]!
#   cmp $5,%rcx           # Check too late
#   jge loopEnd

# DIFFERENT LOOP STYLES:
#
# Pre-test loop (what we use - check before body):
# loopStart:
#   cmp $5,%rcx
#   jge loopEnd
#   # loop body
#   add $1,%rcx
#   jmp loopStart
# loopEnd:
#
# Post-test loop (check after body - like do-while):
# loopStart:
#   # loop body
#   add $1,%rcx
#   cmp $5,%rcx
#   jl loopStart
#
# Pre-test is safer - won't execute body if condition false initially
# Post-test always executes body at least once

# TO ASSEMBLE, LINK, AND RUN:
# as globalArray3.s -o globalArray3.o
# ld globalArray3.o -o globalArray3
# ./globalArray3
# echo $?                  # Should show 15 (1+2+3+4+5)
