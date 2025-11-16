# ============================================================================
# For Loop - Sum of Squares - Guided Version
# ============================================================================
# This program demonstrates how to implement a for-loop in assembly.
# It computes the sum of squares from 0 to 5: 0² + 1² + 2² + 3² + 4² + 5²
#
# WHAT IT DOES: Calculates 0+1+4+9+16+25 = 55, exits with 55
#
# C EQUIVALENT:
#   int sum = 0;
#   for (int i = 0; i < 6; i++) {
#       sum += i * i;
#   }
#   return sum;
# ============================================================================

.section .text
.global _start

_start:
    # ========================================================================
    # LOOP SETUP: Initialize accumulator and counter
    # ========================================================================

    mov $0, %rax            # %rax = sum accumulator (starts at 0)
    mov $0, %rcx            # %rcx = loop counter i (starts at 0)

    # ========================================================================
    # FOR LOOP: for (i = 0; i < 6; i++)
    # ========================================================================

.loopStart:
    # LOOP BODY: Compute i² and add to sum
    mov %rcx, %rbx          # %rbx = i (copy counter to temp register)
    imul %rcx, %rbx         # %rbx = i * i (compute square)
    add %rbx, %rax          # sum += i²

    # LOOP INCREMENT and TEST
    inc %rcx                # i++ (increment counter)
    cmp $6, %rcx            # Compare i with 6
    jl .loopStart           # if (i < 6) continue loop

    # ========================================================================
    # EXIT: Return sum as exit status
    # ========================================================================

    mov %rax, %rdi          # exit status = sum
    mov $60, %rax           # syscall: exit
    syscall

# ============================================================================
# LOOP STRUCTURE EXPLAINED:
#
# This is a DO-WHILE style loop (test at end):
#
# .loopStart:              ← Loop body starts here
#     [compute i²]         ← Do the work
#     [add to sum]
#     inc %rcx             ← Increment counter
#     cmp $6, %rcx         ← Test condition
#     jl .loopStart        ← Jump if Less than (i < 6)
#
# ITERATION TRACE:
# i=0: sum = 0 + (0×0) = 0
# i=1: sum = 0 + (1×1) = 1
# i=2: sum = 1 + (2×2) = 5
# i=3: sum = 5 + (3×3) = 14
# i=4: sum = 14 + (4×4) = 30
# i=5: sum = 30 + (5×5) = 55
# i=6: exit loop (6 is not < 6)
#
# ============================================================================
# KEY INSTRUCTIONS:
#
# imul src, dest   → dest = dest * src (signed multiplication)
# inc reg          → reg = reg + 1 (increment)
# cmp val, reg     → compare reg with val (sets flags)
# jl label         → jump if less than (signed comparison)
#
# REGISTERS USED:
# %rax - accumulator (sum)
# %rcx - loop counter (i)
# %rbx - temporary for i² calculation
#
# ============================================================================
# TO BUILD AND RUN:
#
# as 01-for-loopGuided.s -o loop.o
# ld loop.o -o loop
# ./loop
# echo $?                  # Should print: 55
#
# RESULT: 0² + 1² + 2² + 3² + 4² + 5² = 55
# ============================================================================
