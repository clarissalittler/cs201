# PEDAGOGICAL PURPOSE:
# This program demonstrates testing for equality using cmp and jne (jump if not equal).
# Key learning objectives:
# 1. Using jne (jump if not equal) for equality testing
# 2. The Zero Flag (ZF) and how it's set by comparisons
# 3. Testing if two values are equal vs not equal
# 4. Proper if-statement structure with jne and jmp
# 5. How cmp sets flags based on equality

        .section .text  # CODE SECTION
        .global _start  # EXPORT ENTRY POINT

_start:
        mov $0,%rdi             # INITIALIZE EXIT CODE: %rdi = 0
                                # Default exit code if condition is false

        mov $0,%r8              # LOAD FIRST VALUE: %r8 = 0

        mov $0,%r9              # LOAD SECOND VALUE: %r9 = 0
                                # Now %r8 == %r9 (both are 0)

        cmp %r8,%r9             # COMPARE FOR EQUALITY: Compute %r9 - %r8
                                # %r9 - %r8 = 0 - 0 = 0
                                # really a subtract under the hood
                                ## calculates %r9 - %r8 and stores a result
                                #
                                # FLAGS AFTER COMPARISON:
                                # - ZF = 1 (result IS zero: 0 == 0)
                                # - SF = 0 (result is not negative)
                                # - CF = 0 (no borrow)
                                # - OF = 0 (no overflow)
                                #
                                # KEY: ZF=1 means the values are EQUAL

        jne notequal            # JUMP IF NOT EQUAL: Jump if %r9 != %r8
                                # jne = jump if not equal
                                # jne checks: ZF == 0 (zero flag is not set)
                                # Is %r9 != %r8? Is 0 != 0? NO!
                                # So this jump is NOT taken
                                # Execution falls through to next instruction
                                #
                                # EQUIVALENT TO: if (%r9 != %r8) goto notequal;

        jmp end                 # SKIP CONDITIONAL CODE: Jump to end
                                # Since we didn't take jne (values ARE equal),
                                # we skip the "not equal" code block
                                # This preserves %rdi = 0

notequal:                       # LABEL: Code for "not equal" case
                                # Only executes if jne jumped here
                                # (i.e., when %r9 != %r8)

        mov $10,%rdi            # SET EXIT CODE: %rdi = 10
                                #whoops! this line will always happen :(
                                ##  the above is if (r9 < r8) {rdi=10}
                                #
                                # NOTE: Comments are outdated/wrong
                                # Should say: if (r9 != r8) {rdi=10}
                                # This does NOT always happen (fixed version)
                                # Only happens when values are not equal

end:                            # LABEL: Continue execution
                                # Reached either by:
                                # 1. jmp end (when values are equal)
                                # 2. Fall-through from notequal (when values are not equal)

        mov $60,%rax            # SETUP EXIT SYSCALL: %rax = 60
        syscall                 # EXIT: Exit with status in %rdi

# WHAT THIS PROGRAM DOES:
# 1. Sets %rdi = 0
# 2. Sets %r8 = 0, %r9 = 0
# 3. Compares them (0 == 0? YES)
# 4. jne is not taken (because they ARE equal)
# 5. Jumps to end, keeping %rdi = 0
# 6. Exits with status 0

# TO ASSEMBLE AND RUN:
# as cmp3.s -o cmp3.o
# ld cmp3.o -o cmp3
# ./cmp3
# echo $?  # Shows 0 (because 0 == 0)

# EXPECTED EXIT STATUS:
# 0 (because %r8 == %r9, so the conditional code is skipped)

# THE ZERO FLAG (ZF):
#
# The Zero Flag is set (ZF=1) when an arithmetic operation produces zero.
# For cmp A, B (which computes B - A):
#   - If B == A, result is 0, so ZF=1
#   - If B != A, result is non-zero, so ZF=0
#
# Examples:
#   cmp $5, $5   → 5-5=0   → ZF=1 (equal)
#   cmp $3, $7   → 7-3=4   → ZF=0 (not equal)
#   cmp $10, $2  → 2-10=-8 → ZF=0 (not equal)

# EQUALITY-RELATED JUMPS:
#
# je  (jump if equal)     - jumps if ZF=1
# jz  (jump if zero)      - same as je, jumps if ZF=1
# jne (jump if not equal) - jumps if ZF=0
# jnz (jump if not zero)  - same as jne, jumps if ZF=0
#
# Note: je and jz are the same instruction!
# Use je after cmp (testing equality)
# Use jz after test or arithmetic (testing for zero)

# CONTROL FLOW FOR THIS PROGRAM:
#
#   _start:
#       mov $0,%rdi
#       mov $0,%r8
#       mov $0,%r9
#       cmp %r8,%r9      (0 == 0, ZF=1)
#       jne notequal  ────┐ (not taken: ZF=1)
#       jmp end       ────┼────────┐
#                         │        │
#   notequal:          ←──┘        │
#       mov $10,%rdi              │
#       (fall through)             │
#                                  │
#   end:               ←───────────┘
#       mov $60,%rax
#       syscall

# COMPARISON TO C:
# This program implements:
#   int main() {
#       int rdi = 0;
#       int r8 = 0;
#       int r9 = 0;
#       if (r9 != r8) {     // if (0 != 0) - FALSE
#           rdi = 10;
#       }
#       return rdi;         // Returns 0
#   }

# ALTERNATIVE IMPLEMENTATION:
# Could use je instead of jne for clearer logic:
#   cmp %r8,%r9
#   je end              # Jump to end if equal (skip the code)
#   mov $10,%rdi        # Only execute if not equal
# end:
#   mov $60,%rax
#   syscall

# WHY USE jne vs je?
#
# Using jne (jump if NOT equal):
#   cmp %r8,%r9
#   jne notequal    # Jump to "not equal" block
#   jmp end         # Skip "not equal" block
# notequal:
#   // code for not equal case
# end:
#
# Using je (jump if equal):
#   cmp %r8,%r9
#   je end          # Jump over "not equal" block
#   // code for not equal case
# end:
#
# The je approach is simpler (one jump instead of two)!

# EXPERIMENT:
# Try different values for %r8 and %r9:
# 1. mov $0,%r8 and mov $1,%r9   → Exit 10 (0 != 1)
# 2. mov $5,%r8 and mov $5,%r9   → Exit 0  (5 == 5)
# 3. mov $-1,%r8 and mov $-1,%r9 → Exit 0  (-1 == -1)
# 4. mov $0,%r8 and mov $0,%r9   → Exit 0  (0 == 0) - current

# TESTING FOR EQUALITY:
#
# To test if A == B:
#   cmp A, B
#   je equal_label     # Jump if equal
#   jne not_equal_label # Jump if not equal
#
# To test if A != B:
#   cmp A, B
#   jne not_equal_label # Jump if not equal
#   je equal_label      # Jump if equal
