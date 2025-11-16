# PEDAGOGICAL PURPOSE:
# This program demonstrates conditional branching based on equality comparison.
# It introduces the fundamental concept of compare-and-branch, which is how
# assembly implements if-else statements and other conditional logic.
#
# Key learning objectives:
# 1. Understanding the cmp (compare) instruction and how it works
# 2. Learning about CPU flags (ZF, SF, OF, CF) and how they're set
# 3. Conditional jumps - specifically je (jump if equal)
# 4. Implementing if-else logic using labels and jumps
# 5. Understanding unconditional jumps (jmp) for control flow
# 6. How comparison results affect program flow
#
# CONCEPTUAL OVERVIEW:
# This program implements the following C-like logic:
#   if (r9 == r8) {
#       exit(1);    // They're equal
#   } else {
#       exit(0);    // They're not equal
#   }
#
# The comparison is performed by SUBTRACTION (r9 - r8)
# If the result is zero, the values are equal (ZF flag is set)

        .section .text          # CODE SECTION: Executable instructions

        .global _start         # GLOBAL DIRECTIVE: Entry point for linker

        # INTRODUCTORY COMMENTS FROM ORIGINAL:
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
        mov $2,%r8             # LOAD VALUE 1: %r8 = 2
                               # First value for comparison

        mov $2,%r9             # LOAD VALUE 2: %r9 = 2
                               # Second value for comparison
                               # Note: Both registers now contain the same value (2)

        # COMPARISON: Test if r8 and r9 are equal
        cmp %r8,%r9            # COMPARE: Compute %r9 - %r8 (2 - 2 = 0)
                               # CRITICAL: cmp does NOT store the result anywhere!
                               # It ONLY sets the CPU flags based on the result
                               # AT&T syntax: cmp source,destination means destination - source
                               #
                               # FLAGS SET BY THIS OPERATION:
                               # ZF (Zero Flag) = 1     because result is 0 (2 - 2 = 0)
                               # SF (Sign Flag) = 0     because result is not negative
                               # OF (Overflow Flag) = 0 because no signed overflow occurred
                               # CF (Carry Flag) = 0    because no unsigned borrow occurred
                               #
                               # The ZF=1 is the KEY flag for equality testing

        je equal               # CONDITIONAL JUMP: Jump to 'equal' label if ZF=1
                               # je = "jump if equal" = "jump if ZF=1"
                               # Since %r9 - %r8 = 0, ZF is set, so jump WILL occur
                               # If the jump happens, the next instruction is SKIPPED

        ## this code happens if the jump doesn't
        # NOT EQUAL PATH: This code executes if ZF=0 (values are different)
        mov $0,%rdi            # SET EXIT CODE: %rdi = 0 (indicating not equal)
                               # This line only executes if je did NOT jump
                               # Return value 0 means "they're not equal"

        jmp finish             # UNCONDITIONAL JUMP: Jump to finish
                               # jmp ALWAYS jumps (no condition)
                               # This prevents "falling through" into the equal code
                               # Without this, we'd execute the equal code even when not equal!

        # EQUAL PATH: This code executes if the comparison was equal
equal:
        mov $1,%rdi            # SET EXIT CODE: %rdi = 1 (indicating equal)
                               # This label is the target of the je instruction
                               # Return value 1 means "they're equal"
                               # In this program, this code WILL execute (since 2 == 2)

        # COMMON EXIT PATH: Both branches converge here
finish:
        mov $60,%rax           # SYSCALL NUMBER: exit() = syscall 60
                               # Prepare to terminate the program

        syscall                # INVOKE KERNEL: Execute exit(%rdi)
                               # Program terminates with exit code from %rdi
                               # In this case: exit(1) because values were equal

# CONTROL FLOW ANALYSIS:
# Path when values ARE equal (r8 == r9):
#   _start -> mov $2,%r8 -> mov $2,%r9 -> cmp -> je (JUMPS) -> equal: -> mov $1,%rdi -> finish: -> syscall
#   Exit code: 1
#
# Path when values are NOT equal (r8 != r9):
#   _start -> mov ... -> cmp -> je (NO JUMP) -> mov $0,%rdi -> jmp -> finish: -> syscall
#   Exit code: 0

# CPU FLAGS DETAILED EXPLANATION:
# The x86-64 CPU maintains a FLAGS register (also called RFLAGS) with individual bits:
#
# ZF (Zero Flag, bit 6):
#   Set to 1 when an operation produces zero
#   Set to 0 when an operation produces non-zero
#   Used by: je, jne, jz, jnz
#
# SF (Sign Flag, bit 7):
#   Set to 1 when result is negative (MSB = 1 in two's complement)
#   Set to 0 when result is positive or zero
#   Used by: js, jns
#
# OF (Overflow Flag, bit 11):
#   Set to 1 when signed arithmetic overflows
#   Example: adding two positive numbers gives negative
#   Used by: jo, jno, jl, jge, jle, jg
#
# CF (Carry Flag, bit 0):
#   Set to 1 when unsigned arithmetic produces a carry/borrow
#   Used by: jc, jnc, jb, jae, jbe, ja

# CONDITIONAL JUMP INSTRUCTIONS:
# For equality:
#   je/jz   - Jump if Equal / Jump if Zero (ZF=1)
#   jne/jnz - Jump if Not Equal / Jump if Not Zero (ZF=0)
#
# For signed comparisons (interprets as two's complement):
#   jl/jnge - Jump if Less / Jump if Not Greater or Equal (SF != OF)
#   jle/jng - Jump if Less or Equal / Jump if Not Greater (ZF=1 OR SF != OF)
#   jg/jnle - Jump if Greater / Jump if Not Less or Equal (ZF=0 AND SF == OF)
#   jge/jnl - Jump if Greater or Equal / Jump if Not Less (SF == OF)
#
# For unsigned comparisons (interprets as unsigned integers):
#   jb/jnae - Jump if Below / Jump if Not Above or Equal (CF=1)
#   jbe/jna - Jump if Below or Equal / Jump if Not Above (CF=1 OR ZF=1)
#   ja/jnbe - Jump if Above / Jump if Not Below or Equal (CF=0 AND ZF=0)
#   jae/jnb - Jump if Above or Equal / Jump if Not Below (CF=0)

# WHY CMP USES SUBTRACTION:
# Comparison is implemented as subtraction because subtraction naturally reveals
# the relationship between two numbers:
#   - If A - B = 0, then A == B (sets ZF)
#   - If A - B < 0, then A < B (sets SF for signed, CF for unsigned)
#   - If A - B > 0, then A > B (clears both SF and CF appropriately)
#
# The cmp instruction performs this subtraction but DISCARDS the result,
# keeping only the flags. This is more efficient than sub because:
#   1. We don't need to store the result
#   2. We preserve the original register values
#   3. We only care about the relationship, not the difference

# EQUIVALENT C CODE:
# int main() {
#     int r8 = 2;
#     int r9 = 2;
#     if (r9 == r8) {
#         return 1;  // equal
#     } else {
#         return 0;  // not equal
#     }
# }

# TO ASSEMBLE, LINK, AND TEST:
# as cmp1.s -o cmp1.o
# ld cmp1.o -o cmp1
# ./cmp1
# echo $?                      # Should display 1 (values are equal)
#
# TO TEST WITH DIFFERENT VALUES:
# Change line "mov $2,%r9" to "mov $3,%r9" and reassemble
# Then echo $? should display 0 (values are not equal)
