# PEDAGOGICAL PURPOSE:
# This program demonstrates CORRECT conditional execution using cmp and jumps.
# It fixes the bug from cmp1.s by adding a jump to skip the conditional code.
# Key learning objectives:
# 1. Proper implementation of if-statement in assembly
# 2. Using jmp (unconditional jump) to skip code
# 3. How to structure if-else logic with labels
# 4. The importance of explicit control flow
# 5. Fall-through vs explicit jumps

        .section .text  # CODE SECTION
        .global _start  # EXPORT ENTRY POINT

_start:
        mov $0,%rdi             # INITIALIZE EXIT CODE: %rdi = 0
                                # This will be our exit code if condition is false

        mov $0,%r8              # LOAD FIRST VALUE: %r8 = 0
                                # Changed from 50 in cmp1.s to demonstrate false condition

        mov $50,%r9             # LOAD SECOND VALUE: %r9 = 50
                                # Now %r9 > %r8 instead of %r9 < %r8

        cmp %r8,%r9             # COMPARE: Compute %r9 - %r8
                                # %r9 - %r8 = 50 - 0 = 50
                                # really a subtract under the hood
                                ## calculates %r9 - %r8 and stores a result
                                #
                                # FLAGS AFTER COMPARISON:
                                # - ZF = 0 (result is not zero: 50 ≠ 0)
                                # - SF = 0 (result is positive: 50 > 0)
                                # - CF = 0 (no borrow needed)
                                # - OF = 0 (no overflow)

        jl less                 # JUMP IF LESS: Jump to 'less' if %r9 < %r8
                                # jl checks if SF != OF (signed less than)
                                # Is %r9 < %r8? Is 50 < 0? NO!
                                # So this jump is NOT taken
                                # Execution falls through to next instruction

        jmp end                 # UNCONDITIONAL JUMP: Skip the conditional code
                                # THIS IS THE FIX!
                                # If we didn't take the conditional jump (condition false),
                                # we skip over the code that should only execute when true
                                # This jumps directly to the 'end' label
                                #
                                # WHY NEEDED?
                                # Without this, execution would fall through into 'less'
                                # even though the condition was false!

less:                           # LABEL: Conditional code block
                                # This code only executes if jl jumped here
                                # (i.e., when %r9 < %r8)

        mov $10,%rdi            # SET EXIT CODE: %rdi = 10
                                #whoops! this line will always happen :(
                                ##  the above is if (r9 < r8) {rdi=10}
                                #
                                # NOTE: The comment is outdated (from cmp1.s)
                                # This does NOT always happen anymore!
                                # It only happens if the jl jump was taken
                                # In this program: %r9 (50) is NOT < %r8 (0)
                                # So this code is skipped!

end:                            # LABEL: End of conditional logic
                                # Execution arrives here either:
                                # 1. Via 'jmp end' (condition was false)
                                # 2. By falling through from 'less' (condition was true)

        mov $60,%rax            # SETUP EXIT SYSCALL: %rax = 60
        syscall                 # EXIT: Exit with status in %rdi

# WHAT THIS PROGRAM DOES:
# 1. Sets %rdi = 0 (initial exit code)
# 2. Sets %r8 = 0, %r9 = 50
# 3. Compares %r9 with %r8 (50 vs 0)
# 4. Since 50 is NOT < 0, doesn't jump to 'less'
# 5. Executes 'jmp end', skipping the conditional code
# 6. Exits with status 0 (original value of %rdi)

# TO ASSEMBLE AND RUN:
# as cmp2.s -o cmp2.o
# ld cmp2.o -o cmp2
# ./cmp2
# echo $?  # Shows 0 (because 50 is not < 0)

# EXPECTED EXIT STATUS:
# 0 (because 50 is not less than 0, so %rdi stays 0)

# CONTROL FLOW DIAGRAM:
#
#   _start:
#       mov $0,%rdi
#       mov $0,%r8
#       mov $50,%r9
#       cmp %r8,%r9
#       jl less          ──┐ (not taken: 50 >= 0)
#       jmp end          ──┼─────────┐
#                          │         │
#   less:                ←─┘         │
#       mov $10,%rdi                 │
#       (fall through)               │
#                                    │
#   end:                  ←──────────┘
#       mov $60,%rax
#       syscall

# IF-STATEMENT TRANSLATION PATTERN:
#
# C code:
#   if (condition) {
#       // true_block
#   }
#   // continue
#
# Assembly pattern:
#   cmp ...               # Set up comparison
#   j<opposite> end       # Jump if condition is FALSE
#   // true_block
# end:
#   // continue
#
# OR (using our pattern):
#   cmp ...               # Set up comparison
#   j<condition> true     # Jump if condition is TRUE
#   jmp end               # Skip true block
# true:
#   // true_block
# end:
#   // continue

# COMPARING cmp1.s and cmp2.s:
#
# cmp1.s (BUGGY):
#   cmp %r8,%r9
#   jl less
# less:             # Falls through even if condition false!
#   mov $10,%rdi
#   mov $60,%rax
#   syscall
#
# cmp2.s (FIXED):
#   cmp %r8,%r9
#   jl less
#   jmp end         # Explicitly skip conditional code!
# less:
#   mov $10,%rdi
# end:
#   mov $60,%rax
#   syscall

# COMPARISON TO C:
# This program correctly implements:
#   int main() {
#       int rdi = 0;
#       int r8 = 0;
#       int r9 = 50;
#       if (r9 < r8) {      // if (50 < 0) - FALSE
#           rdi = 10;
#       }
#       return rdi;         // Returns 0
#   }

# WHY TWO JUMPS ARE NEEDED:
#
# High-level languages have implicit control flow:
#   if (condition) { code }
# The "skip if false" is automatic.
#
# In assembly, control flow is explicit:
# - First jump: "go to true block if condition is true"
# - Second jump: "skip true block if we didn't go there"
#
# Both jumps are needed to implement if-statement correctly!

# EXPERIMENT:
# Try changing the values:
# 1. mov $50,%r8 and mov $0,%r9  → Exit code: 0 (50 > 0)
# 2. mov $0,%r8 and mov $0,%r9   → Exit code: 0 (0 == 0, not <)
# 3. mov $10,%r8 and mov $5,%r9  → Exit code: 10 (5 < 10)
