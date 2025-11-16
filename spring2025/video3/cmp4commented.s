# PEDAGOGICAL PURPOSE:
# This program demonstrates comparing a register with an immediate value
# instead of two registers, and tests for inequality.
# Key learning objectives:
# 1. Comparing register with immediate value (cmp $-1,%r9)
# 2. Immediate values can be negative
# 3. Same jne/jmp pattern as previous examples
# 4. How comparisons work with negative numbers
# 5. Testing register content against a constant

        .section .text  # CODE SECTION
        .global _start  # EXPORT ENTRY POINT

_start:
        mov $0,%rdi             # INITIALIZE EXIT CODE: %rdi = 0

        mov $0,%r8              # LOAD VALUE: %r8 = 0
                                # This value is loaded but not used in comparison!
                                # It's here perhaps for consistency with previous examples

        mov $0,%r9              # LOAD TEST VALUE: %r9 = 0
                                # This is the value we'll compare against -1

        cmp $-1,%r9             # COMPARE WITH IMMEDIATE: Compare %r9 with -1
                                # cmp source, destination (AT&T syntax)
                                # Computes: %r9 - (-1) = 0 - (-1) = 0 + 1 = 1
                                # really a subtract under the hood
                                ## calculates %r9 - %r8 and stores a result
                                # (comment is wrong: actually computes %r9 - (-1))
                                #
                                # FLAGS AFTER COMPARISON:
                                # Result = 1 (not zero)
                                # - ZF = 0 (result is not zero: 0 != -1)
                                # - SF = 0 (result is positive)
                                # - CF = 0 (no borrow needed)
                                # - OF = 0 (no overflow)
                                #
                                # KEY CONCEPT - IMMEDIATE COMPARISONS:
                                # cmp $-1,%r9 compares %r9 against the literal value -1
                                # No need to load -1 into a register first!
                                # This is more efficient than:
                                #   mov $-1,%r8
                                #   cmp %r8,%r9

        jne notequal            # JUMP IF NOT EQUAL: Jump if %r9 != -1
                                # jne checks: ZF == 0
                                # Is %r9 != -1? Is 0 != -1? YES!
                                # So this jump IS taken
                                # Execution jumps to 'notequal' label

        jmp end                 # SKIP CODE: Would skip conditional block
                                # This line is never reached in this program
                                # because the jne above is always taken

notequal:                       # LABEL: "Not equal" code block
                                # Execution arrives here because 0 != -1

        mov $10,%rdi            # SET EXIT CODE: %rdi = 10
                                # This executes because %r9 (0) != -1

end:                            # LABEL: End of conditional logic
        mov $60,%rax            # SETUP EXIT SYSCALL: %rax = 60
        syscall                 # EXIT: Exit with status in %rdi (10)

# WHAT THIS PROGRAM DOES:
# 1. Sets %rdi = 0
# 2. Sets %r8 = 0 (unused)
# 3. Sets %r9 = 0
# 4. Compares %r9 (0) with -1
# 5. Since 0 != -1, jumps to 'notequal'
# 6. Sets %rdi = 10
# 7. Exits with status 10

# TO ASSEMBLE AND RUN:
# as cmp4.s -o cmp4.o
# ld cmp4.o -o cmp4
# ./cmp4
# echo $?  # Shows 10

# EXPECTED EXIT STATUS:
# 10 (because 0 != -1, so the conditional code executes)

# IMMEDIATE VALUES IN COMPARISONS:
#
# You can compare registers with:
# 1. Other registers: cmp %r8,%r9
# 2. Immediate values: cmp $-1,%r9
# 3. Memory locations: cmp num1,%r9
#
# Using immediate values is efficient:
# - No need to waste a register
# - Shorter code
# - Faster execution (no extra mov needed)

# NEGATIVE IMMEDIATE VALUES:
#
# $-1 is a negative immediate value
# In 64-bit two's complement:
#   -1 = 0xFFFFFFFFFFFFFFFF (all bits set)
#
# When compared:
#   cmp $-1, %r9 where %r9=0
#   Computes: 0 - (-1) = 0 + 1 = 1
#   Result is positive, non-zero
#   ZF=0 (not equal)

# COMMON USE CASE:
#
# Testing for -1 is common because -1 is often used as:
# - Error indicator (e.g., file descriptor -1 means error)
# - End-of-input marker
# - "Invalid" or "not found" value
#
# Example from systems programming:
#   mov ..., %rax       # Call some function that returns fd
#   cmp $-1, %rax       # Check if it returned -1 (error)
#   je error_handler    # Jump to error handling if -1

# CONTROL FLOW:
#
#   _start:
#       mov $0,%rdi
#       mov $0,%r8
#       mov $0,%r9
#       cmp $-1,%r9      (0 != -1, ZF=0)
#       jne notequal  ────┐ (taken: ZF=0)
#       jmp end       ────┼─────(not reached)
#                         │
#   notequal:          ←──┘
#       mov $10,%rdi
#       (fall through)
#
#   end:
#       mov $60,%rax
#       syscall

# COMPARISON TO C:
# This program implements:
#   int main() {
#       int rdi = 0;
#       int r8 = 0;      // unused
#       int r9 = 0;
#       if (r9 != -1) {  // if (0 != -1) - TRUE
#           rdi = 10;
#       }
#       return rdi;      // Returns 10
#   }

# COMPARING IMMEDIATE VS REGISTER:
#
# Register-to-register comparison:
#   mov $-1,%r8
#   cmp %r8,%r9
# Advantages: None really
# Disadvantages: Wastes a register, extra instruction
#
# Register-to-immediate comparison:
#   cmp $-1,%r9
# Advantages: Shorter, faster, saves register
# Disadvantages: None (unless you need to reuse the value)

# WHEN TO USE EACH METHOD:
#
# Use immediate:
#   cmp $0,%rax     # Testing for zero
#   cmp $-1,%rax    # Testing for -1
#   cmp $100,%rcx   # Testing against constant
#
# Use register:
#   cmp %rbx,%rax   # Comparing two variables
#   cmp limit,%rax  # Where limit changes at runtime

# EXPERIMENT:
# Try changing %r9 to different values:
# 1. mov $-1,%r9  → Exit 0  (-1 == -1, not equal is false)
# 2. mov $1,%r9   → Exit 10 (1 != -1, not equal is true)
# 3. mov $255,%r9 → Exit 10 (255 != -1, not equal is true)
#
# Note: On x86-64, $255 and $-1 are different when viewed as signed!
# But in an unsigned byte context (exit codes), they're the same.
