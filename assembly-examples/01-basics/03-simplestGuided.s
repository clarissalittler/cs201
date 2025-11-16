# ============================================================================
# Simplest Program - Guided Version
# ============================================================================
# This is literally the simplest assembly program that will assemble.
# However, it will CRASH when run because it doesn't exit properly!
#
# WHAT IT DOES: Loads 60 into %rax, then falls through to whatever is next
# in memory, causing a segmentation fault.
#
# PURPOSE: Educational - shows what happens when you forget to exit
# ============================================================================

.section .text          # Code section
.global _start          # Entry point

_start:
    mov $60, %rax       # Load 60 into %rax (but never call syscall!)
    # BUG: Missing syscall instruction!
    # BUG: Missing exit code in %rdi!
    # Program will crash here by executing random memory

# ============================================================================
# WHAT GOES WRONG:
#
# After executing "mov $60, %rax", the CPU continues to the next instruction.
# But there is no next instruction in our code! So the CPU tries to execute
# whatever bytes happen to be in memory after our program.
#
# This causes a SEGMENTATION FAULT because:
# 1. Those bytes aren't valid instructions, OR
# 2. The program tries to access memory it doesn't own
#
# ============================================================================
# TO BUILD AND RUN:
#
# as 03-simplestGuided.s -o simplest.o
# ld simplest.o -o simplest
# ./simplest
#
# Expected output:
# Segmentation fault (core dumped)
#
# Exit status:
# echo $?                  # Will be 139 (128 + 11, where 11 = SIGSEGV)
#
# ============================================================================
# HOW TO FIX:
#
# Add these two lines after "mov $60, %rax":
#     xor %rdi, %rdi      # Set exit status to 0
#     syscall             # Actually invoke the exit syscall
#
# Or see 02-simple.s for a working minimal program.
# ============================================================================
