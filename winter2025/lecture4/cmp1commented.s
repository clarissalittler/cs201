# PEDAGOGICAL PURPOSE:
# This program demonstrates comparison and conditional branching with jne (jump if not equal).
# It explores the relationship between cmp, CPU flags, and conditional jumps, introducing
# students to the fundamental concept of conditional execution in assembly.
#
# Key learning objectives:
# 1. Understanding the cmp (compare) instruction and its syntax
# 2. Learning how cmp sets CPU flags through subtraction
# 3. Understanding jne (jump if not equal) - jumps when ZF=0
# 4. Implementing if-else logic using conditional and unconditional jumps
# 5. Understanding the importance of the jmp instruction to prevent fall-through
# 6. Clarifying the argument order for cmp in AT&T syntax
# 7. Using exit codes to verify program logic
#
# CONCEPTUAL OVERVIEW:
# This program implements:
#   if (rax != rbx) {    // if (1 != 2)
#       return 10;        // not equal path
#   } else {
#       return -1;        // equal path (won't execute)
#   }
#
# The comparison checks if %rax (1) is not equal to %rbx (2)

        .text                      # CODE SECTION: Executable instructions
        .global _start             # GLOBAL DIRECTIVE: Entry point visible to linker

_start:
        mov $1,%rax                # LOAD FIRST VALUE: %rax = 1
                                  # This is the first value we'll compare
                                  # Think of this as: int rax = 1;

        mov $2,%rbx                # LOAD SECOND VALUE: %rbx = 2
                                  # This is the second value we'll compare
                                  # Think of this as: int rbx = 2;
                                  # Since 1 ≠ 2, we expect the "not equal" path

        cmp %rbx,%rax              # COMPARE: Compute %rax - %rbx (1 - 2 = -1)
                                  # AT&T SYNTAX: cmp arg2,arg1 means arg1 - arg2
                                  # This can be confusing! Let's break it down:
                                  #
                                  # SYNTAX CLARIFICATION:
                                  # cmp %rbx,%rax  means  %rax - %rbx
                                  # (second operand) - (first operand)
                                  #
                                  # OPERATION PERFORMED:
                                  # 1 - 2 = -1
                                  #
                                  # CRITICAL: cmp does NOT modify %rax or %rbx!
                                  # It only sets the CPU flags based on the result
                                  #
                                  # FLAGS SET:
                                  # ZF (Zero Flag) = 0     because result is -1 (NOT zero)
                                  # SF (Sign Flag) = 1     because result is negative
                                  # CF (Carry Flag) = 1    because unsigned borrow occurred
                                  # OF (Overflow Flag) = 0 because no signed overflow
                                  #
                                  # The ZF=0 is the key for our jne instruction

        jne less                   # CONDITIONAL JUMP: Jump to 'less' if ZF=0 (not equal)
                                  # jne = "jump if not equal"
                                  # jne checks the Zero Flag (ZF)
                                  # If ZF=0 (values are NOT equal), jump to 'less' label
                                  # If ZF=1 (values ARE equal), continue to next instruction
                                  #
                                  # QUESTION FROM COMMENT: is this jump if arg2 <= arg1 or arg1 <= arg2?
                                  # ANSWER: Neither! This is jump if arg1 != arg2
                                  # jne only checks equality, not ordering
                                  # For ordering, you'd use: jl, jle, jg, jge, ja, jae, jb, jbe
                                  #
                                  # IN THIS CASE:
                                  # %rax (1) ≠ %rbx (2), so ZF=0
                                  # Therefore, the jump WILL occur
                                  # Execution continues at the 'less' label

        mov $-1,%rdi               # EQUAL PATH: %rdi = -1 (exit code for "equal")
                                  # This line only executes if jne did NOT jump
                                  # That would happen if %rax == %rbx (ZF=1)
                                  # Since 1 ≠ 2, this code is SKIPPED in this execution
                                  #
                                  # WHY -1?
                                  # This is just a distinctive exit code to indicate
                                  # "the values were equal" if we tested equal values

        jmp finish                 # UNCONDITIONAL JUMP: Jump to finish
                                  # jmp ALWAYS jumps (no condition checked)
                                  # This prevents "falling through" to the 'less' code
                                  #
                                  # WHY IS THIS NECESSARY?
                                  # Without this jmp, after setting %rdi = -1, execution
                                  # would continue to the next instruction (less:)
                                  # and overwrite %rdi with 10, giving wrong result!
                                  #
                                  # This implements the "end of the if-block" in C

        # NOT EQUAL PATH: Executed when values are different
less:
        mov $10,%rdi               # NOT EQUAL EXIT CODE: %rdi = 10
                                  # This label is the target of the jne instruction
                                  # This code executes when %rax ≠ %rbx
                                  # In this program, this WILL execute (since 1 ≠ 2)
                                  # Exit code 10 indicates "values were not equal"

        # COMMON EXIT PATH: Both branches converge here
finish:
        mov $60,%rax               # SYSCALL NUMBER: 60 = exit
                                  # Prepare to terminate the program

        syscall                    # INVOKE KERNEL: Execute exit(%rdi)
                                  # Program terminates with exit code from %rdi
                                  # In this case: exit(10) because values were not equal

# CONTROL FLOW ANALYSIS:
#
# Path when values are NOT equal (rax != rbx):
#   _start -> mov $1,%rax -> mov $2,%rbx -> cmp -> jne (JUMPS) -> less: -> mov $10,%rdi -> finish: -> syscall
#   Exit code: 10
#
# Path when values ARE equal (rax == rbx):
#   _start -> mov ... -> cmp -> jne (NO JUMP) -> mov $-1,%rdi -> jmp -> finish: -> syscall
#   Exit code: -1 (shown as 255 in shell due to 8-bit wrapping)

# AT&T vs INTEL SYNTAX COMPARISON:
#
# AT&T (what we're using):
#   cmp %rbx,%rax    means    %rax - %rbx
#   source,destination order
#
# Intel (alternative syntax):
#   cmp rax,rbx      means    rax - rbx
#   destination,source order (but same operation!)
#
# NOTE: In cmp, both syntaxes compute left - right in the instruction,
# but the operand order is reversed. Confusing but important to know!

# CPU FLAGS DETAILED EXPLANATION:
#
# The x86-64 RFLAGS register contains status flags:
#
# ZF (Zero Flag):
#   Set to 1 when an operation produces zero
#   Set to 0 when an operation produces non-zero
#   cmp sets ZF=1 when operands are EQUAL (subtraction gives 0)
#   cmp sets ZF=0 when operands are NOT EQUAL
#
# SF (Sign Flag):
#   Set to 1 when result is negative (MSB = 1)
#   Set to 0 when result is positive or zero
#   Used for signed comparisons
#
# CF (Carry Flag):
#   Set to 1 when unsigned arithmetic needs a borrow/carry
#   Used for unsigned comparisons
#   For cmp: CF=1 when first operand < second (unsigned)
#
# OF (Overflow Flag):
#   Set to 1 when signed arithmetic overflows
#   Used with signed comparison jumps

# CONDITIONAL JUMP INSTRUCTIONS:
#
# EQUALITY JUMPS:
#   je/jz   - Jump if Equal / Jump if Zero (ZF=1)
#   jne/jnz - Jump if Not Equal / Jump if Not Zero (ZF=0)
#
# SIGNED COMPARISONS (after cmp A,B which computes B - A):
#   jl/jnge - Jump if Less / Not Greater or Equal (SF != OF)
#   jle/jng - Jump if Less or Equal (ZF=1 OR SF != OF)
#   jg/jnle - Jump if Greater (ZF=0 AND SF == OF)
#   jge/jnl - Jump if Greater or Equal (SF == OF)
#
# UNSIGNED COMPARISONS (after cmp A,B which computes B - A):
#   jb/jnae/jc  - Jump if Below / Not Above or Equal / Carry (CF=1)
#   jbe/jna     - Jump if Below or Equal (CF=1 OR ZF=1)
#   ja/jnbe     - Jump if Above (CF=0 AND ZF=0)
#   jae/jnb/jnc - Jump if Above or Equal / Not Below / Not Carry (CF=0)
#
# OTHER JUMPS:
#   jmp     - Unconditional jump (always jumps)
#   js/jns  - Jump if Sign flag set/not set
#   jo/jno  - Jump if Overflow flag set/not set

# WHY USE SUBTRACTION FOR COMPARISON?
#
# Subtraction naturally reveals the relationship between two numbers:
#   If A - B = 0  → A == B (sets ZF)
#   If A - B < 0  → A < B  (sets SF for signed, CF for unsigned)
#   If A - B > 0  → A > B  (clears SF and CF appropriately)
#
# The cmp instruction leverages this by performing subtraction but
# DISCARDING the result and keeping only the flags. Benefits:
#   1. Original register values are preserved
#   2. We only care about the relationship, not the difference
#   3. CPU doesn't need to write back the result

# EQUIVALENT C CODE:
#
# int main() {
#     int rax = 1;
#     int rbx = 2;
#
#     if (rax != rbx) {
#         return 10;   // not equal path
#     } else {
#         return -1;   // equal path
#     }
# }

# TO ASSEMBLE, LINK, AND TEST:
# as cmp1.s -o cmp1.o
# ld cmp1.o -o cmp1
# ./cmp1
# echo $?                          # Should display 10 (values are not equal)
#
# TO TEST EQUAL PATH:
# Change "mov $2,%rbx" to "mov $1,%rbx" and reassemble
# Then echo $? should display 255 (which is -1 in unsigned 8-bit)

# EXPERIMENT:
# Try different comparison jumps:
#   Replace "jne less" with "je less"    - Now only jumps if equal
#   Replace "jne less" with "jl less"    - Only jumps if rax < rbx (signed)
#   Replace "jne less" with "jg less"    - Only jumps if rax > rbx (signed)
# See how the exit code changes!
