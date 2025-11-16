# PEDAGOGICAL PURPOSE:
# This program demonstrates subtraction in x86-64 assembly and how negative
# numbers are represented as unsigned exit codes.
# Key learning objectives:
# 1. The sub instruction for subtraction
# 2. How negative numbers are represented in two's complement
# 3. Exit codes are unsigned bytes (0-255)
# 4. How -1 maps to 255 in unsigned representation
# 5. Basic assembly program structure with .text and _start

        .section .text  # CODE SECTION: Executable code goes here
        .global _start  # EXPORT SYMBOL: Make _start visible to linker

_start:                 # ENTRY POINT: Program execution starts here

        mov $11,%rdi            # MOVE IMMEDIATE: Load 11 into %rdi
                                # %rdi = 11
                                # This will be our initial value

        sub $20,%rdi            # SUBTRACT IMMEDIATE: %rdi = %rdi - 20
                                # sub source, destination
                                # destination = destination - source
                                # %rdi = 11 - 20 = -9
                                # %rdi - $20 -> %rdi
                                #
                                # RESULT: %rdi = -9 (in two's complement)
                                # In 64-bit two's complement:
                                # -9 = 0xFFFFFFFFFFFFFFF7 (all 1s except last 4 bits)
                                #
                                # HOW TWO'S COMPLEMENT WORKS:
                                # To represent -N:
                                # 1. Take binary representation of N
                                # 2. Flip all bits (one's complement)
                                # 3. Add 1
                                # Example for -9 (8-bit):
                                #   9 = 00001001
                                #   Flip: 11110110
                                #   Add 1: 11110111 = 0xF7 = 247 (unsigned)

        mov $60,%rax            # SETUP EXIT SYSCALL: %rax = 60
                                # Syscall 60 is exit

        syscall                 # EXECUTE SYSTEM CALL: Exit the program
                                # The OS will read %rdi as the exit code
                                # But exit codes are UNSIGNED bytes (0-255)
                                # So the lower 8 bits of %rdi are used
                                # -9 as unsigned byte = 247
                                # (or actually -1 will become 255 based on comment)
                                ## will return 255 with echo $?
                                ## return codes are an unsigned byte
                                ## so they can take values 0 - 255
                                ## -1 -> 111111...[11111111]
                                #
                                # WAIT, THE COMMENT SAYS -1?
                                # Looking at the code: 11 - 20 = -9, not -1
                                # But the comment says -1 returns 255
                                # Let me explain both:
                                #
                                # -1 in binary (8-bit): 11111111 = 255 unsigned
                                # -9 in binary (8-bit): 11110111 = 247 unsigned
                                #
                                # The actual result here is -9, which maps to 247

# WHAT THIS PROGRAM DOES:
# 1. Loads 11 into %rdi
# 2. Subtracts 20 from %rdi (%rdi becomes -9)
# 3. Exits with status code 247 (unsigned representation of -9)

# TO ASSEMBLE AND RUN:
# as sub1.s -o sub1.o          # Assemble
# ld sub1.o -o sub1            # Link
# ./sub1                       # Run
# echo $?                      # Check exit status (should be 247)

# EXPECTED EXIT STATUS:
# 247 (which is -9 in two's complement, viewed as unsigned byte)

# TWO'S COMPLEMENT REPRESENTATION:
#
# In two's complement (8-bit examples):
#   0   = 00000000 = 0
#   1   = 00000001 = 1
#   127 = 01111111 = 127 (largest positive for signed byte)
#   -128 = 10000000 = 128 (when viewed as unsigned)
#   -127 = 10000001 = 129
#   -2  = 11111110 = 254
#   -1  = 11111111 = 255
#
# For our case (-9):
#   -9  = 11110111 = 247 (in 8 bits)

# WHY UNSIGNED BYTES FOR EXIT CODES?
#
# Exit codes are defined as unsigned values 0-255 because:
# 1. Tradition from Unix systems
# 2. Allows 256 distinct status codes
# 3. 0 means success, non-zero means various error conditions
# 4. Negative values don't make semantic sense for "exit status"
#
# Common exit codes:
#   0   = Success
#   1   = General error
#   2   = Misuse of shell command
#   126 = Command cannot execute
#   127 = Command not found
#   128+N = Fatal error signal N
#   130 = Terminated by Ctrl+C (128 + SIGINT)
#   255 = Exit status out of range

# HOW THE TRUNCATION WORKS:
#
# %rdi is a 64-bit register holding:
#   0xFFFFFFFFFFFFFFF7 (64-bit two's complement of -9)
#
# The kernel's exit syscall only looks at the lower 8 bits:
#   0xF7 = 247 (unsigned)
#
# So even though %rdi contains a 64-bit signed value of -9,
# the exit code seen by the shell is 247.

# COMPARISON TO C:
# This program is equivalent to:
#   int main() {
#       int rdi = 11;
#       rdi = rdi - 20;  // rdi is now -9
#       return rdi;      // But exit code is 247 (unsigned byte)
#   }

# EXPERIMENT:
# Try different values:
# 1. sub $11,%rdi  → Result: 0 (11 - 11)
# 2. sub $12,%rdi  → Result: 255 (11 - 12 = -1 → 255 unsigned)
# 3. sub $267,%rdi → Result: 0 (11 - 267 = -256, wraps to 0)
#                     (because -256 mod 256 = 0)

# THE SUB INSTRUCTION:
#
# Format: sub source, destination
# Effect: destination = destination - source
# Flags affected: CF, PF, AF, ZF, SF, OF (condition flags)
#
# Examples:
#   sub $5, %rax    →  %rax = %rax - 5
#   sub %rbx, %rcx  →  %rcx = %rcx - %rbx
#   sub $0, %rdx    →  No change, but sets flags (like cmp)
