# PEDAGOGICAL PURPOSE:
# This program demonstrates conditional jumps and comparison in x86-64 assembly,
# but with a BUG: the conditional code always executes (missing jmp after label).
# Key learning objectives:
# 1. The cmp instruction for comparing values
# 2. Conditional jumps (jl = jump if less than)
# 3. How comparisons set CPU flags
# 4. Common bug: falling through into conditional code
# 5. Why you need jmp to skip over conditional blocks
# 6. Control flow and labels in assembly

        .section .text  # CODE SECTION
        .global _start  # EXPORT ENTRY POINT

_start:
        mov $0,%rdi             # INITIALIZE EXIT CODE: %rdi = 0
                                # This is our default/initial value for the exit code

        mov $50,%r8             # LOAD FIRST VALUE: %r8 = 50
                                # %r8 will be the first value to compare

        mov $2,%r9              # LOAD SECOND VALUE: %r9 = 2
                                # %r9 will be the second value to compare

        cmp %r8,%r9             # COMPARE: Compare %r9 with %r8
                                # cmp source, destination (AT&T syntax)
                                # Computes: destination - source (but doesn't store result)
                                # Actually computes: %r9 - %r8 = 2 - 50 = -48
                                # really a subtract under the hood
                                ## calculates %r9 - %r8 and stores a result
                                #
                                # IMPORTANT: cmp doesn't store the result!
                                # It only sets CPU FLAGS based on the result:
                                # - ZF (Zero Flag): set if result is 0
                                # - SF (Sign Flag): set if result is negative
                                # - CF (Carry Flag): set if unsigned underflow
                                # - OF (Overflow Flag): set if signed overflow
                                #
                                # For 2 - 50 = -48:
                                # - ZF = 0 (result is not zero)
                                # - SF = 1 (result is negative)
                                # - CF = 1 (unsigned: 2 < 50, borrow needed)

        jl less                 # JUMP IF LESS: Jump to 'less' label if %r9 < %r8
                                # jl = jump if less than (signed comparison)
                                # Checks: SF != OF (sign flag != overflow flag)
                                # Since %r9 (2) < %r8 (50), the jump is taken
                                #
                                # CONDITION: Is %r9 < %r8?
                                # Is 2 < 50? YES!
                                # So we jump to the 'less' label

less:                           # LABEL: Target for conditional jump
                                # Execution arrives here if jump was taken
                                # But also falls through from above if jump wasn't taken!

        mov $10,%rdi            # SET EXIT CODE: %rdi = 10
                                #whoops! this line will always happen :(
                                ##  the above is if (r8 < r9) {rdi=10}
                                #
                                # BUG: This line ALWAYS executes!
                                # Even though the comment says "if (r8 < r9)",
                                # there's no jump to skip this code if condition is false
                                #
                                # WHAT SHOULD HAPPEN:
                                # - If %r9 < %r8 (true): jump to less, execute this
                                # - If %r9 >= %r8 (false): skip this code
                                #
                                # WHAT ACTUALLY HAPPENS:
                                # - If %r9 < %r8 (true): jump to less, execute this
                                # - If %r9 >= %r8 (false): fall through to less, execute this anyway!
                                #
                                # The problem: no "jmp end" before the less: label!

        mov $60,%rax            # SETUP EXIT SYSCALL: %rax = 60
        syscall                 # EXIT: Exit with status in %rdi (which is 10)

# WHAT THIS PROGRAM DOES:
# 1. Sets %rdi = 0 (initial exit code)
# 2. Sets %r8 = 50, %r9 = 2
# 3. Compares %r9 with %r8 (2 vs 50)
# 4. Since 2 < 50, jumps to 'less' label
# 5. Sets %rdi = 10
# 6. Exits with status 10

# THE BUG:
# The program exits with 10 regardless of the comparison result!
# In this case it doesn't matter because 2 < 50 is true,
# but if the values were different (e.g., %r9 = 100),
# it would still execute the code at 'less' because of fall-through.

# HOW TO FIX IT (see cmp2.s for the corrected version):
#   jl less
#   jmp end        # Skip the conditional code if condition is false
# less:
#   mov $10,%rdi
# end:
#   mov $60,%rax
#   syscall

# TO ASSEMBLE AND RUN:
# as cmp1.s -o cmp1.o
# ld cmp1.o -o cmp1
# ./cmp1
# echo $?  # Shows 10

# EXPECTED EXIT STATUS:
# 10 (because 2 < 50, so the conditional executes)

# CONDITIONAL JUMP INSTRUCTIONS:
#
# After cmp or test, you can use conditional jumps:
#
# Signed comparisons:
#   je  / jz   - Jump if Equal / Zero (ZF=1)
#   jne / jnz  - Jump if Not Equal / Not Zero (ZF=0)
#   jl  / jnge - Jump if Less / Not Greater or Equal (SF≠OF)
#   jle / jng  - Jump if Less or Equal / Not Greater (ZF=1 or SF≠OF)
#   jg  / jnle - Jump if Greater / Not Less or Equal (ZF=0 and SF=OF)
#   jge / jnl  - Jump if Greater or Equal / Not Less (SF=OF)
#
# Unsigned comparisons:
#   jb  / jnae / jc  - Jump if Below / Not Above or Equal / Carry (CF=1)
#   jbe / jna        - Jump if Below or Equal / Not Above (CF=1 or ZF=1)
#   ja  / jnbe       - Jump if Above / Not Below or Equal (CF=0 and ZF=0)
#   jae / jnb / jnc  - Jump if Above or Equal / Not Below / Not Carry (CF=0)

# HOW CMP SETS FLAGS:
#
# cmp %r8, %r9 computes: %r9 - %r8
#
# Example: cmp $50, $2 (computes 2 - 50 = -48)
#   ZF = 0 (result != 0)
#   SF = 1 (result < 0)
#   CF = 1 (unsigned: 2 < 50)
#   OF = 0 (no overflow)
#
# For jl (jump if less): checks SF != OF
#   SF=1, OF=0, so SF≠OF is true → jump taken

# COMPARISON TO C:
# This buggy code is like:
#   int main() {
#       int rdi = 0;
#       int r8 = 50;
#       int r9 = 2;
#       if (r9 < r8) {  // Always true, so jump taken
#           goto less;
#       }
#       // Missing: else { goto end; }
#   less:
#       rdi = 10;  // Always executes!
#   // end:
#       return rdi;  // Returns 10
#   }

# WHY THIS EXAMPLE IS VALUABLE:
# - Shows a common beginner mistake in assembly
# - Demonstrates the need for explicit control flow
# - High-level if-else needs TWO jumps in assembly!
# - Teaches debugging and understanding execution flow
