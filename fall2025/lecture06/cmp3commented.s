# PEDAGOGICAL PURPOSE:
# This program demonstrates comparing a register against an immediate value.
# It shows that cmp can compare against literal constants, not just other registers.
# This is extremely common in real programs for bounds checking, sentinel values, etc.
#
# Key learning objectives:
# 1. Comparing registers against immediate (constant) values
# 2. Understanding that immediate values can be source operands for cmp
# 3. Recognizing common patterns like "if (x < CONSTANT)"
# 4. Seeing how the same jl instruction works with immediate comparisons
# 5. Understanding when to use immediate vs register comparisons
#
# CONCEPTUAL OVERVIEW:
# This program tests if 2 < 3 (which is true)
# Unlike cmp2.s which compared two registers, this compares a register to a constant
# The syntax "cmp $3,%r9" compares %r9 against the literal value 3

        .section .text          # CODE SECTION: Executable instructions

        .global _start         # GLOBAL DIRECTIVE: Entry point

        # REFERENCE COMMENTS:
        ## the equivalent of <,==,!=, &c.
        ## is cmp
        ## cmp takes two arguments
        ## cmp %r1,%r2
        ## this calculates r2 - r1
        ## from here you have to choose
        ## one of the jmp instructions
        ## cmp sets the flags
        ## ZF (is the result = 0)
        ## SF (is the result negative)
        ## OF (was there overflow)
        ## CF (did an arithmetic carry happen)

_start:
        # SETUP: Initialize values
        mov $1,%r8             # LOAD VALUE 1: %r8 = 1
                               # This value is loaded but NOT used in the comparison!
                               # It's here from the previous example (cmp2.s)
                               # This demonstrates a common bug: initializing unused variables

        mov $2,%r9             # LOAD VALUE 2: %r9 = 2
                               # This is the value we will actually compare

        # THE KEY DIFFERENCE: Compare register against immediate value
        cmp $3,%r9             # COMPARE IMMEDIATE: Compute %r9 - 3 (2 - 3 = -1)
                               # IMPORTANT: The $ indicates an immediate (literal) value
                               # AT&T syntax: cmp source,destination means destination - source
                               # So this calculates: %r9 - 3 = 2 - 3 = -1
                               #
                               # FLAGS AFTER THIS OPERATION:
                               # ZF = 0 (result is not zero)
                               # SF = 1 (result is negative: -1 < 0)
                               # OF = 0 (no signed overflow)
                               # CF = 1 (unsigned borrow occurred: 2 < 3 in unsigned arithmetic)
                               #
                               # INTERPRETATION FOR jl:
                               # jl jumps if SF != OF (signed less-than)
                               # Here: SF=1, OF=0, so SF!=OF, therefore jl WILL jump
                               # This means: %r9 IS less than 3 (correct: 2 < 3)

        jl less                # CONDITIONAL JUMP: Jump if %r9 < 3 (signed comparison)
                               # jl = "jump if less" = "jump if SF != OF"
                               # Tests: Is %r9 < 3? Is 2 < 3? YES!
                               # Therefore, jump WILL occur
                               # Execution jumps to the 'less' label

        ## this code happens if the jump doesn't
        # NOT LESS PATH: Executes when %r9 >= 3
        mov $0,%rdi            # SET EXIT CODE: %rdi = 0 (not less than)
                               # This code would execute if %r9 >= 3
                               # In this program, this code WILL NOT execute

        ## jl means r9 < r8
        # ^^ MISLEADING COMMENT: After "cmp $3,%r9", jl tests %r9 < 3
        # The value in %r8 is IRRELEVANT to this comparison!

        jmp finish             # UNCONDITIONAL JUMP: Skip to finish
                               # This line is NEVER executed in this program
                               # because the jl jump happened first

        # LESS THAN PATH: Executes when %r9 < 3
less:
        mov $1,%rdi            # SET EXIT CODE: %rdi = 1 (less than)
                               # This code WILL execute because 2 < 3
                               # Exit code 1 means "condition is true"

        ## jl means r8 < r9
        # ^^ INCORRECT AND IRRELEVANT: After "cmp $3,%r9", jl tests %r9 < 3
        # Neither r8 nor r9 are being compared to each other!

        # COMMON EXIT: Both paths converge
finish:
        mov $60,%rax           # SYSCALL NUMBER: exit()

        syscall                # INVOKE KERNEL: Execute exit(1)
                               # Exit code is 1 because 2 < 3

# IMMEDIATE VS REGISTER COMPARISONS:
# Register to Register:
#   cmp %rax,%rbx    -> compares %rbx to %rax
#   Used when both values are variables
#
# Immediate to Register:
#   cmp $100,%rax    -> compares %rax to 100
#   Used when comparing against a constant
#   Very common for: bounds checking, sentinel values, magic numbers
#
# Memory to Register (not shown):
#   cmp (%rax),%rbx  -> compares %rbx to value at memory address in %rax
#   Used when one value is in memory

# COMMON PATTERNS WITH IMMEDIATE COMPARISONS:
# 1. Array bounds checking:
#    cmp $ARRAY_SIZE,%rax
#    jge out_of_bounds
#
# 2. Loop termination:
#    cmp $0,%rcx
#    je loop_done
#
# 3. Null pointer checking:
#    cmp $0,%rax
#    je is_null
#
# 4. Character comparison:
#    cmp $'\n',%al    # Check if character is newline
#    je found_newline
#
# 5. Range checking:
#    cmp $MIN_VALUE,%rax
#    jl below_min
#    cmp $MAX_VALUE,%rax
#    jg above_max

# WHY THE UNUSED %r8?
# This code initializes %r8 but never uses it in the comparison
# This might be:
#   1. A teaching moment about dead code
#   2. Copy-paste from cmp2.s that wasn't cleaned up
#   3. Preparation for future code that's commented out
#   4. Demonstrating that only the compared registers matter
#
# The assembler doesn't optimize this out - it faithfully
# generates the "mov $1,%r8" instruction even though it's unused

# TRACE OF EXECUTION:
#   mov $1,%r8       -> %r8 = 1 (unused)
#   mov $2,%r9       -> %r9 = 2
#   cmp $3,%r9       -> Compute 2-3=-1, set SF=1,OF=0,ZF=0,CF=1
#   jl less          -> SF!=OF, so JUMP to less
#   less:            -> Execution continues here
#   mov $1,%rdi      -> %rdi = 1
#   finish:          -> Fall through to finish
#   mov $60,%rax     -> %rax = 60
#   syscall          -> exit(1)

# COMPARISON WITH cmp2.s:
# cmp2.s:  cmp %r8,%r9  -> Compares two registers (%r9 vs %r8)
# cmp3.s:  cmp $3,%r9   -> Compares register to constant (%r9 vs 3)
#
# Both use jl, but compare different things
# cmp2.s: Tests if one variable is less than another
# cmp3.s: Tests if a variable is less than a constant

# EQUIVALENT C CODE:
# int main() {
#     int r8 = 1;           // Initialized but unused
#     int r9 = 2;
#     if (r9 < 3) {         // Is 2 < 3? Yes!
#         return 1;         // less than
#     } else {
#         return 0;         // not less than
#     }
# }

# ALTERNATIVE INTERPRETATION:
# You could also think of this as testing a condition:
#   const int THRESHOLD = 3;
#   int value = 2;
#   if (value < THRESHOLD) {
#       return 1;  // Below threshold
#   } else {
#       return 0;  // At or above threshold
#   }

# TO ASSEMBLE, LINK, AND TEST:
# as cmp3.s -o cmp3.o
# ld cmp3.o -o cmp3
# ./cmp3
# echo $?                      # Should display 1 (2 is less than 3)
#
# EXPERIMENT:
# Change "mov $2,%r9" to "mov $5,%r9" and reassemble
# Then echo $? should display 0 (because 5 is not < 3)
#
# Or change "cmp $3,%r9" to "cmp $1,%r9"
# Then echo $? should display 0 (because 2 is not < 1)
