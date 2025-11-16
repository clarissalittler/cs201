# PEDAGOGICAL PURPOSE:
# This program demonstrates less-than comparison using signed integer semantics.
# It builds on cmp1.s by showing a different conditional jump instruction (jl)
# and highlights the CRITICAL confusion point about operand order in AT&T syntax.
#
# Key learning objectives:
# 1. Understanding jl (jump if less) for signed comparisons
# 2. Recognizing the CONFUSING aspect of AT&T syntax for comparisons
# 3. Understanding how SF and OF flags work together for signed comparisons
# 4. Distinguishing between signed (jl) and unsigned (jb) comparisons
# 5. Debugging comparison logic by understanding operand order
# 6. The relationship between cmp operand order and jump conditions
#
# CONCEPTUAL OVERVIEW:
# This program compares 1 and 2, testing if 1 < 2 (which is true)
# The program exits with 1 to indicate "yes, less than" or 0 for "no, not less than"
#
# WARNING - COMMON CONFUSION:
# The comments in the original file contain CONTRADICTORY statements about jl!
# This is INTENTIONAL to teach students to verify their understanding
# Let's clarify what actually happens...

        .section .text          # CODE SECTION: Executable instructions

        .global _start         # GLOBAL DIRECTIVE: Entry point

        # REFERENCE COMMENTS (from cmp1.s):
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
        # SETUP: Initialize values to compare
        mov $1,%r8             # LOAD VALUE 1: %r8 = 1
                               # This is our first value

        mov $2,%r9             # LOAD VALUE 2: %r9 = 2
                               # This is our second value

        # THE CRITICAL COMPARISON:
        cmp %r8,%r9            # COMPARE: Compute %r9 - %r8 (2 - 1 = 1)
                               # AT&T syntax: cmp source,destination means destination - source
                               # So this calculates: %r9 - %r8 = 2 - 1 = 1
                               #
                               # FLAGS AFTER THIS OPERATION:
                               # ZF = 0 (result is not zero)
                               # SF = 0 (result is positive: 1 > 0)
                               # OF = 0 (no signed overflow)
                               # CF = 0 (no unsigned borrow)
                               #
                               # INTERPRETATION FOR jl:
                               # jl jumps if SF != OF (signed less-than condition)
                               # Here: SF=0, OF=0, so SF==OF, therefore jl will NOT jump
                               # This means: %r9 is NOT less than %r8 (correct: 2 is not < 1)

        jl less                # CONDITIONAL JUMP: Jump if %r9 < %r8 (signed comparison)
                               # jl = "jump if less" = "jump if SF != OF"
                               # IMPORTANT: This tests if DESTINATION < SOURCE
                               # So jl tests: Is %r9 < %r8? Is 2 < 1? NO!
                               # Therefore, jump WILL NOT occur
                               # Execution continues to the next line

        ## this code happens if the jump doesn't
        # NOT LESS PATH: Executes when %r9 >= %r8
        mov $0,%rdi            # SET EXIT CODE: %rdi = 0 (not less than)
                               # Since 2 >= 1, this code WILL execute
                               # Exit code 0 means "condition is false"

        ## jl means r9 < r8
        # ^^ CORRECT COMMENT: jl after "cmp %r8,%r9" tests if r9 < r8

        jmp finish             # UNCONDITIONAL JUMP: Skip to finish
                               # Prevents falling through to the less label

        # LESS THAN PATH: Executes when %r9 < %r8
less:
        mov $1,%rdi            # SET EXIT CODE: %rdi = 1 (less than)
                               # This code would execute if %r9 < %r8
                               # In this program, this code WILL NOT execute

        ## jl means r8 < r9
        # ^^ INCORRECT COMMENT: This is backwards!
        # After "cmp %r8,%r9", the jl instruction tests %r9 < %r8, NOT %r8 < %r9

        # COMMON EXIT: Both paths converge
finish:
        mov $60,%rax           # SYSCALL NUMBER: exit()

        syscall                # INVOKE KERNEL: Execute exit(0)
                               # Exit code is 0 because 2 is not < 1

# CLARIFYING THE CONFUSION:
# The original code has contradictory comments on lines 24 and 29:
#   Line 24: "## jl means r9 < r8"  <- CORRECT
#   Line 29: "## jl means r8 < r9"  <- INCORRECT
#
# Let's prove which is correct:
#   %r8 = 1, %r9 = 2
#   cmp %r8,%r9 computes: %r9 - %r8 = 2 - 1 = 1 (positive)
#   jl tests: SF != OF
#   Result is positive, so SF=0, OF=0, therefore SF==OF
#   jl does NOT jump
#   Exit code: 0
#
# If jl meant "%r8 < %r9" (as line 29 incorrectly claims):
#   Then jl would jump (because 1 < 2)
#   Exit code would be 1
#   But that's NOT what happens!
#
# Therefore, line 24 is correct: jl means %r9 < %r8

# HOW SIGNED LESS-THAN WORKS:
# For signed comparisons, we need both SF and OF:
#   SF tells us if the result is negative
#   OF tells us if signed overflow occurred
#
# After cmp %r8,%r9 (which computes %r9 - %r8):
#   If %r9 < %r8: result is negative, SF=1 (unless overflow)
#   If overflow occurs, SF might be flipped
#   So: %r9 < %r8 when SF != OF
#
# Examples:
#   cmp 5, 10:  10-5=5 (positive), SF=0, OF=0 -> SF==OF -> NOT less
#   cmp 10, 5:  5-10=-5 (negative), SF=1, OF=0 -> SF!=OF -> IS less
#   cmp -128, 127: 127-(-128)=255 (overflow!), SF=0, OF=1 -> SF!=OF -> IS less (correct!)

# SIGNED vs UNSIGNED COMPARISONS:
# Signed (two's complement):
#   jl/jnge  - Jump if Less (SF != OF)
#   jle/jng  - Jump if Less or Equal (ZF=1 OR SF != OF)
#   jg/jnle  - Jump if Greater (ZF=0 AND SF == OF)
#   jge/jnl  - Jump if Greater or Equal (SF == OF)
#
# Unsigned (binary):
#   jb/jnae  - Jump if Below (CF=1)
#   jbe/jna  - Jump if Below or Equal (CF=1 OR ZF=1)
#   ja/jnbe  - Jump if Above (CF=0 AND ZF=0)
#   jae/jnb  - Jump if Above or Equal (CF=0)
#
# WHY THE DISTINCTION MATTERS:
#   -1 as signed = -1 (less than 0)
#   -1 as unsigned = 0xFFFFFFFFFFFFFFFF (greater than 0)
#   Use jl for signed, jb for unsigned!

# AT&T SYNTAX GOTCHA:
# In AT&T syntax, "cmp A,B" computes B-A and sets flags
# Then conditional jumps test relationships of B to A:
#   cmp %r8,%r9; jl -> jumps if %r9 < %r8
#   cmp %r8,%r9; jg -> jumps if %r9 > %r8
#   cmp %r8,%r9; je -> jumps if %r9 == %r8
#
# This is OPPOSITE of the natural reading order!
# To test "if A < B", you write: cmp B,A; jl
# Many beginners get this backwards!

# MENTAL MODEL:
# Think of cmp as asking a question about the second operand:
#   cmp %r8,%r9 asks: "How does %r9 relate to %r8?"
#   Then jl answers: "It's less than"
#   Then jg answers: "It's greater than"
#   Then je answers: "It's equal to"

# EQUIVALENT C CODE:
# int main() {
#     int r8 = 1;
#     int r9 = 2;
#     if (r9 < r8) {        // Is 2 < 1? No!
#         return 1;         // less than
#     } else {
#         return 0;         // not less than
#     }
# }

# TO ASSEMBLE, LINK, AND TEST:
# as cmp2.s -o cmp2.o
# ld cmp2.o -o cmp2
# ./cmp2
# echo $?                      # Should display 0 (2 is not less than 1)
#
# EXPERIMENT:
# Swap the mov instructions to set r8=2, r9=1, then:
# echo $? should display 1 (because 1 < 2)
