# ============================================================================
# Simple Exit - Guided Version
# ============================================================================
# This is the simplest possible assembly program that exits cleanly.
# It demonstrates the minimum required structure: a text section with _start
# that calls the exit syscall.
#
# WHAT IT DOES: Exits immediately with status -1 (error code)
# ============================================================================

.section .text          # Code section
.global _start          # Make entry point visible to linker

_start:
    # Exit syscall: exit(status=-1)
    mov $60, %rax       # syscall number: 60 = exit
    mov $-1, %rdi       # arg1: exit status = -1 (indicates error)
    syscall             # invoke kernel to terminate program

# ============================================================================
# KEY POINTS:
#
# - Minimal program: just 3 instructions
# - No data section needed (we don't use any data)
# - No output (doesn't print anything)
# - Exit status -1 typically indicates an error
#
# SYSTEM CALL: exit(int status)
# - Syscall number 60
# - Takes one argument in %rdi: the exit status code
# - Never returns (program terminates)
#
# ============================================================================
# TO BUILD AND RUN:
#
# as 02-simpleGuided.s -o simple.o
# ld simple.o -o simple
# ./simple
# echo $?                  # Should print: 255 (because -1 wraps to 255 in 8-bit)
#
# WHY 255?
# Exit codes are 8-bit unsigned (0-255). The value -1 wraps around to 255.
# ============================================================================
