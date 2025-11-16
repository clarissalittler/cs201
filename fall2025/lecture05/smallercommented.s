# PEDAGOGICAL PURPOSE:
# This program demonstrates basic arithmetic in assembly language.
# It adds two numbers (1 + 2) and exits with the result as the exit code.
# Key learning objectives:
# 1. Using registers for computation (general-purpose registers like %r8, %r9)
# 2. Understanding the ADD instruction and how it works
# 3. Moving data between registers with MOV
# 4. How to use the exit syscall to return a value to the shell
# 5. The relationship between assembly operations and the CPU's execution
# 6. Introduction to the x86-64 instruction set

        .text                   # CODE SECTION: All executable code goes here
                               # This is where the CPU will read instructions from

        .global _start         # GLOBAL SYMBOL: Make _start visible to the linker
                               # _start is the entry point when using ld directly
                               # (gcc programs use main instead)

_start:
        # COMPUTATION: We're going to compute 1 + 2 = 3
        # We'll use %r8 and %r9 which are general-purpose 64-bit registers
        # These registers can hold integer values and be used for arithmetic

        mov $1, %r8              # MOVE IMMEDIATE: Put the value 1 into register %r8
                               # The $ means immediate value (literal constant)
                               # %r8 is a 64-bit register (part of the extended register set)
                               # After this: %r8 = 1

        mov $2, %r9              # MOVE IMMEDIATE: Put the value 2 into register %r9
                               # After this: %r9 = 2
                               # Now both operands are in registers, ready for computation

        add %r8, %r9             # ADD INSTRUCTION: Add %r8 to %r9, store result in %r9
                               # AT&T SYNTAX: source first, destination second
                               # This computes: %r9 = %r9 + %r8
                               # So: %r9 = 2 + 1 = 3
                               # The result OVERWRITES the destination register (%r9)
                               # After this: %r8 = 1 (unchanged), %r9 = 3

        mov %r9, %rdi            # PREPARE EXIT CODE: Move result into %rdi
                               # %rdi is the first argument register for system calls
                               # For the exit syscall, %rdi holds the exit status
                               # We're moving 3 into %rdi to exit with status 3
                               # After this: %rdi = 3

        # SYSTEM CALL SETUP: exit(3)
        # The exit syscall (number 60) terminates the program
        # and returns a status code to the parent process (usually the shell)

        mov $60, %rax            # SYSCALL NUMBER: 60 is the exit syscall on x86-64 Linux
                               # The kernel looks at %rax to determine which syscall to perform
                               # Different syscalls: 1=write, 60=exit, 57=fork, etc.

        syscall                  # SYSCALL INSTRUCTION: Transfer control to the kernel
                               # At this point:
                               #   - %rax = 60 (exit syscall number)
                               #   - %rdi = 3 (exit status)
                               # The kernel executes exit(3)
                               # The program terminates with exit code 3
                               # The shell can check this with: echo $?

# AFTER RUNNING:
# To assemble, link, and run:
#   as smaller.s -o smaller.o
#   ld smaller.o -o smaller
#   ./smaller
#   echo $?               # Will display: 3
#
# The exit code 3 proves our addition worked correctly!

# CONCEPTUAL NOTES:
#
# WHY USE MULTIPLE REGISTERS?
# We could have done this with fewer moves, but this demonstrates:
# - How to use general-purpose registers for computation
# - The two-operand form of ADD (source, destination)
# - Moving data between registers
#
# REGISTER CHOICE:
# %r8 and %r9 are chosen because:
# - They're general-purpose (can be used for any integer operations)
# - They're part of the extended x86-64 register set
# - They demonstrate that we have many registers available (not just %rax, %rbx, etc.)
#
# TWO-OPERAND INSTRUCTIONS:
# Notice that ADD has two operands: add %r8, %r9
# The destination (%r9) is both an input AND the output
# This is different from three-operand architectures like ARM
# where you might write: add r9, r9, r8 (destination is separate)
#
# COMPARISON TO C:
# This assembly program is roughly equivalent to:
#   int main() {
#     int a = 1;
#     int b = 2;
#     int c = a + b;
#     return c;
#   }
# But we're working directly with registers instead of variables!
