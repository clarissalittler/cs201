# PEDAGOGICAL PURPOSE:
# This program demonstrates conditional branching using the cmp instruction
# and jle (jump if less than or equal). It implements a simple if-else statement
# that tests whether a value is less than or equal to another value, then takes
# different execution paths accordingly. This is fundamental to understanding
# how high-level if-else translates to assembly language.
#
# Key learning objectives:
# 1. Understanding the cmp (compare) instruction and how it sets CPU flags
# 2. Learning about conditional jumps - specifically jle (jump if less or equal)
# 3. Implementing if-else logic using labels and jumps
# 4. Understanding unconditional jumps (jmp) for control flow
# 5. How CPU flags (ZF, SF, OF, CF) determine program flow
# 6. Translating C if-else statements to assembly
# 7. Understanding branch paths and control flow convergence
#
# CONCEPTUAL OVERVIEW:
# This program implements the following C-like logic:
#   int value = 0;
#   if (value <= 5) {
#       exit(10);    // Value is less than or equal to 5
#   } else {
#       exit(-1);    // Value is greater than 5
#   }
#
# The comparison is performed by SUBTRACTION (value - 5)
# If the result is negative or zero, then value <= 5

	.text                   # CODE SECTION: Executable instructions
                                # Equivalent to .section .text

	.global _start         # GLOBAL DIRECTIVE: Entry point for linker
                                # Makes _start visible to the linker

	# ORIGINAL COMMENTS:
	# Okay so we want a program that
	# will let us test out how the compare operator works
	# in assembly

_start:
	# INITIALIZE VALUE: Setup the value to test
	mov $0,%rdi            # LOAD TEST VALUE: %rdi = 0
                                # This is the value we'll compare against 5
                                # Using %rdi here (it will later hold exit code)
                                # We're testing: is 0 <= 5?

	# COMPARISON: Test the condition
	cmp $5,%rdi            # COMPARE: Compute %rdi - 5 (0 - 5 = -5)
                                # cmp performs SUBTRACTION but doesn't store result
                                # Only sets CPU flags based on the result
                                # AT&T syntax: cmp source,destination means destination - source
                                #
                                # CALCULATION: 0 - 5 = -5
                                #
                                # FLAGS SET BY THIS OPERATION:
                                # ZF (Zero Flag) = 0      because result is -5 (not zero)
                                # SF (Sign Flag) = 1      because result is negative (-5)
                                # OF (Overflow Flag) = 0  because no signed overflow occurred
                                # CF (Carry Flag) = 1     because unsigned borrow occurred
                                #                         (0 - 5 requires borrow in unsigned)
                                #
                                # The combination of these flags tells us the relationship

	jle .less              # CONDITIONAL JUMP: Jump to .less if %rdi <= 5
                                # jle = "jump if less than or equal" (signed comparison)
                                # jle checks: (ZF=1) OR (SF != OF)
                                # In our case: ZF=0, SF=1, OF=0, so SF != OF is true
                                # Therefore, the jump WILL occur
                                # Since 0 <= 5, we jump to .less label
                                #
                                # WHY jle JUMPS:
                                # For %rdi <= 5, we need (%rdi - 5) <= 0
                                # Our result is -5, which is indeed <= 0
                                # So jle correctly identifies this and jumps

	# ELSE BRANCH: This code executes if condition is FALSE (%rdi > 5)
	mov $-1,%rax           # SET EXIT CODE: %rax = -1
                                # This line only executes if jle did NOT jump
                                # Return value -1 indicates "value is greater than 5"
                                # In this program, this code is NEVER executed (0 is not > 5)

	jmp .finish            # UNCONDITIONAL JUMP: Jump to finish
                                # jmp ALWAYS jumps (no condition)
                                # This prevents "falling through" into the .less code
                                # Without this, we'd execute both branches!
                                # This is the "end of else block" in C

	# IF BRANCH: This code executes if condition is TRUE (%rdi <= 5)
.less:
	mov $10,%rax           # SET EXIT CODE: %rax = 10
                                # This label is the target of the jle instruction
                                # Return value 10 indicates "value is less than or equal to 5"
                                # In this program, this code WILL execute (0 <= 5)

	# COMMON EXIT PATH: Both branches converge here
.finish:
	mov %rax,%rdi          # PREPARE SYSCALL ARG: %rdi = %rax
                                # Transfer the exit code to the syscall argument register
                                # %rdi holds the first argument to syscall (exit status)

	mov $60,%rax           # SYSCALL NUMBER: exit() = syscall 60
                                # 60 is the Linux syscall number for sys_exit
                                # This overwrites our exit code in %rax, but we saved it in %rdi

	syscall                # INVOKE KERNEL: Execute exit(%rdi)
                                # Program terminates with exit code from %rdi
                                # In this case: exit(10) because 0 <= 5

# CONTROL FLOW ANALYSIS:
# This program has TWO possible execution paths:
#
# PATH 1: When %rdi <= 5 (TRUE in this program):
#   _start → mov $0,%rdi → cmp $5,%rdi → jle (JUMPS) → .less: → mov $10,%rax →
#   .finish: → mov %rax,%rdi → mov $60,%rax → syscall
#   Exit code: 10
#
# PATH 2: When %rdi > 5 (NOT taken in this program):
#   _start → mov $0,%rdi → cmp $5,%rdi → jle (NO JUMP) → mov $-1,%rax →
#   jmp .finish → .finish: → mov %rax,%rdi → mov $60,%rax → syscall
#   Exit code: -1 (displayed as 255 in shell because exit codes are unsigned 8-bit)

# IF-ELSE STRUCTURE IN ASSEMBLY:
# High-level C code:
#   if (condition) {
#       then_branch;
#   } else {
#       else_branch;
#   }
#
# Assembly pattern:
#   test condition
#   jump_if_true to then_label
#   else_branch          # Falls through if condition false
#   jmp finish
# then_label:
#   then_branch
# finish:
#
# Our specific implementation:
#   cmp $5,%rdi          # Test: is %rdi <= 5?
#   jle .less            # Jump if true
#   mov $-1,%rax         # Else branch
#   jmp .finish
# .less:
#   mov $10,%rax         # Then branch
# .finish:

# CPU FLAGS DETAILED EXPLANATION:
# The x86-64 CPU maintains a FLAGS register (RFLAGS) with individual status bits:
#
# ZF (Zero Flag, bit 6):
#   Set to 1 when an arithmetic operation produces zero
#   Set to 0 when result is non-zero
#   After "cmp $5,%rdi" with %rdi=0: result is -5, so ZF=0
#   Used by: je/jz (jump if equal/zero), jne/jnz (jump if not equal/zero)
#
# SF (Sign Flag, bit 7):
#   Set to 1 when result is negative (MSB = 1 in two's complement)
#   Set to 0 when result is positive or zero
#   After "cmp $5,%rdi" with %rdi=0: result is -5 (negative), so SF=1
#   Used by: js (jump if sign), jns (jump if not sign), and signed comparisons
#
# OF (Overflow Flag, bit 11):
#   Set to 1 when signed arithmetic overflows
#   Set to 0 when no overflow occurs
#   After "cmp $5,%rdi" with %rdi=0: no overflow, so OF=0
#   Example of overflow: 0x7FFFFFFFFFFFFFFF + 1 = negative (overflow!)
#   Used by: jo (jump if overflow), jno (jump if not overflow)
#
# CF (Carry Flag, bit 0):
#   Set to 1 when unsigned arithmetic produces a carry or borrow
#   Set to 0 when no carry/borrow occurs
#   After "cmp $5,%rdi" with %rdi=0: 0-5 requires borrow, so CF=1
#   Used by: jc (jump if carry), jnc (jump if not carry), and unsigned comparisons

# CONDITIONAL JUMP INSTRUCTIONS - COMPREHENSIVE GUIDE:
#
# FOR EQUALITY:
#   je/jz   - Jump if Equal / Jump if Zero
#             Condition: ZF = 1
#             After cmp X,Y: jumps if Y == X
#
#   jne/jnz - Jump if Not Equal / Jump if Not Zero
#             Condition: ZF = 0
#             After cmp X,Y: jumps if Y != X
#
# FOR SIGNED COMPARISONS (treats numbers as two's complement):
#   jl/jnge - Jump if Less / Jump if Not Greater or Equal
#             Condition: SF != OF
#             After cmp X,Y: jumps if Y < X (signed)
#
#   jle/jng - Jump if Less or Equal / Jump if Not Greater
#             Condition: (ZF = 1) OR (SF != OF)
#             After cmp X,Y: jumps if Y <= X (signed)
#             THIS IS THE ONE WE USE IN THIS PROGRAM!
#
#   jg/jnle - Jump if Greater / Jump if Not Less or Equal
#             Condition: (ZF = 0) AND (SF == OF)
#             After cmp X,Y: jumps if Y > X (signed)
#
#   jge/jnl - Jump if Greater or Equal / Jump if Not Less
#             Condition: SF == OF
#             After cmp X,Y: jumps if Y >= X (signed)
#
# FOR UNSIGNED COMPARISONS (treats numbers as unsigned integers):
#   jb/jnae/jc  - Jump if Below / Jump if Not Above or Equal / Jump if Carry
#                 Condition: CF = 1
#                 After cmp X,Y: jumps if Y < X (unsigned)
#
#   jbe/jna     - Jump if Below or Equal / Jump if Not Above
#                 Condition: (CF = 1) OR (ZF = 1)
#                 After cmp X,Y: jumps if Y <= X (unsigned)
#
#   ja/jnbe     - Jump if Above / Jump if Not Below or Equal
#                 Condition: (CF = 0) AND (ZF = 0)
#                 After cmp X,Y: jumps if Y > X (unsigned)
#
#   jae/jnb/jnc - Jump if Above or Equal / Jump if Not Below / Jump if Not Carry
#                 Condition: CF = 0
#                 After cmp X,Y: jumps if Y >= X (unsigned)

# WHY CMP USES SUBTRACTION:
# Comparison is implemented as subtraction because subtraction naturally reveals
# the relationship between two numbers:
#
#   If A - B = 0, then A == B (sets ZF=1)
#   If A - B < 0, then A < B (sets SF=1 for signed, CF=1 for unsigned)
#   If A - B > 0, then A > B (clears SF and CF appropriately)
#
# The cmp instruction performs this subtraction but DISCARDS the result,
# keeping only the flags. This is more efficient than sub because:
#   1. We don't need to store the result
#   2. We preserve the original register values
#   3. We only care about the relationship, not the difference
#
# Example with our values:
#   cmp $5,%rdi    where %rdi=0
#   Computes: 0 - 5 = -5
#   Result is negative, so %rdi < 5
#   Also, result is not zero, so %rdi != 5
#   Combined: %rdi < 5, therefore %rdi <= 5

# HOW jle WORKS:
# After "cmp $5,%rdi" with %rdi=0:
#   Computation: 0 - 5 = -5
#   Flags: ZF=0, SF=1, OF=0
#
# jle jumps if: (ZF = 1) OR (SF != OF)
#   ZF = 1?  No (result is not zero)
#   SF != OF? Yes (SF=1, OF=0, so 1 != 0)
#   Therefore: jle JUMPS
#
# Why this logic works:
#   - If result is zero (ZF=1), then Y == X, so Y <= X ✓
#   - If result is negative without overflow (SF=1, OF=0), then Y < X, so Y <= X ✓
#   - If result is negative WITH overflow (SF=1, OF=1), then Y > X actually!
#     (Example: large positive - small negative = negative with overflow)
#   - The SF != OF condition correctly handles the overflow case

# SIGNED vs UNSIGNED COMPARISONS:
# This program uses jle (signed comparison).
#
# SIGNED interpretation (jle):
#   Treats values as two's complement signed integers
#   Range: -2^63 to 2^63-1 for 64-bit registers
#   0 and 5 are both positive, so comparison is straightforward
#
# UNSIGNED interpretation (jbe - Jump if Below or Equal):
#   Treats values as unsigned integers
#   Range: 0 to 2^64-1 for 64-bit registers
#   For small positive numbers like 0 and 5, result is the same as signed
#
# When it matters:
#   Compare -1 (0xFFFFFFFFFFFFFFFF) to 5:
#   Signed: -1 < 5 (jl jumps, jg doesn't)
#   Unsigned: 18446744073709551615 > 5 (jb doesn't, ja jumps)
#
# Always use the right comparison for your data type!

# EQUIVALENT C CODE:
# int main() {
#     int value = 0;         // %rdi
#     if (value <= 5) {
#         return 10;         // less or equal branch
#     } else {
#         return -1;         // greater branch
#     }
# }

# ALTERNATIVE C CODE (more explicit):
# int main() {
#     int value = 0;
#     int result;
#     if (value <= 5) {
#         result = 10;
#     } else {
#         result = -1;
#     }
#     return result;
# }

# WHY WE NEED THE jmp .finish:
# Without the jmp, execution would "fall through" into the .less label:
#
# BAD CODE (without jmp):
#   cmp $5,%rdi
#   jle .less
#   mov $-1,%rax      # Else branch
# .less:               # Falls through here! BAD!
#   mov $10,%rax      # This would execute even for else branch!
# .finish:
#
# GOOD CODE (with jmp):
#   cmp $5,%rdi
#   jle .less
#   mov $-1,%rax      # Else branch
#   jmp .finish       # Skip over then branch
# .less:
#   mov $10,%rax      # Only executed if jumped here
# .finish:
#
# The jmp ensures mutual exclusion of if and else branches

# TESTING DIFFERENT VALUES:
# To test different conditions, change "mov $0,%rdi" to:
#
# mov $3,%rdi   → 3 <= 5 → true → exit(10)
# mov $5,%rdi   → 5 <= 5 → true → exit(10)
# mov $6,%rdi   → 6 <= 5 → false → exit(-1) [shows as 255]
# mov $10,%rdi  → 10 <= 5 → false → exit(-1) [shows as 255]
# mov $-1,%rdi  → -1 <= 5 → true → exit(10)

# TO ASSEMBLE, LINK, AND TEST:
# as 03-comparisons.s -o 03-comparisons.o
# ld 03-comparisons.o -o 03-comparisons
# ./03-comparisons
# echo $?                      # Should display 10 (because 0 <= 5)
#
# TO TEST WITH DIFFERENT VALUES:
# Edit line "mov $0,%rdi" to "mov $10,%rdi" and reassemble
# Then echo $? should display 255 (because -1 exit code wraps to 255)

# DEBUGGING WITH GDB:
# gdb ./03-comparisons
# (gdb) break _start
# (gdb) run
# (gdb) info registers rdi rax          # View register values
# (gdb) info registers eflags           # View CPU flags
# (gdb) stepi                            # Step one instruction
# (gdb) disassemble                      # View current code
#
# After the cmp instruction, use:
# (gdb) print $eflags
# To see the actual flag values

# COMMON MISTAKES AND DEBUGGING:
# 1. Forgetting the jmp after else branch
#    → Both branches execute (wrong!)
#
# 2. Using wrong jump instruction
#    → jle for "less or equal", not jl (just "less")
#
# 3. Reversing cmp operands
#    → cmp $5,%rdi tests "%rdi compared to 5"
#    → cmp %rdi,$5 would test "5 compared to %rdi" (backwards!)
#
# 4. Using unsigned jump (jbe) when you mean signed (jle)
#    → Works for positive numbers, breaks for negative
#
# 5. Not understanding AT&T syntax
#    → cmp source,destination means destination - source
#    → This is opposite of Intel syntax!

# EXPERIMENT:
# Change jle to jl (remove the "equal" part):
#   jl .less        # Jump if Less (not equal)
# Now: 0 < 5 → true → still exit(10)
# But: 5 < 5 → false → would exit(-1)
# The "equal" in jle makes it inclusive
