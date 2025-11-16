# PEDAGOGICAL PURPOSE:
# IMPORTANT NOTE: Despite the filename, this program uses INTEGER comparisons,
# not floating-point comparisons. It demonstrates the jl (jump if less than)
# conditional jump with a different control flow structure than 03-comparisons.s.
# This file is pedagogically valuable for understanding comparison logic and
# how the same conditional jumps can be used in different branching patterns.
#
# Key learning objectives:
# 1. Understanding cmp (compare) instruction with integer operands
# 2. Using jl (jump if less than) for signed integer comparisons
# 3. Implementing if-else logic with reversed branch structure
# 4. Understanding when conditional jumps do and don't occur
# 5. Seeing how SF (Sign Flag) and OF (Overflow Flag) control jl behavior
# 6. Contrasting this with actual floating-point comparisons (explained below)
# 7. Understanding AT&T syntax for cmp: cmp source,destination
#
# WHAT THIS PROGRAM ACTUALLY DOES:
# Tests if 10 < 5 (which is FALSE), so it takes the "not less" path
#
# CONCEPTUAL OVERVIEW:
# This implements the following C-like logic:
#   int value = 10;
#   if (value < 5) {      // This is FALSE (10 is not < 5)
#       exit(10);          // "Good" path - not taken
#   } else {
#       exit(-1);          // "Bad" path - this executes
#   }
#
# ACTUAL FLOATING-POINT COMPARISONS:
# Real floating-point comparisons use DIFFERENT instructions:
#   - comiss/comisd for scalar float/double comparison
#   - ucomiss/ucomisd for unordered comparison (handles NaN)
#   - These work with XMM registers (%xmm0-%xmm15), not general registers
#   - Example: comisd %xmm0,%xmm1  # Compare two doubles
#
# This program uses cmp with general-purpose registers (integer comparison)

	.section .text          # CODE SECTION: Executable instructions

	.global _start         # GLOBAL DIRECTIVE: Entry point for linker

	# ORIGINAL COMMENTS:
	#write a program that puts 10 in %rdi
	#if a 10 < 5
	#else it puts -1 in %rdi

_start:
	# INITIALIZE VALUE: Setup the value to test
	mov $10,%rcx           # LOAD TEST VALUE: %rcx = 10
                                # This is the value we'll compare against 5
                                # We're testing: is 10 < 5?
                                # (The answer is NO, so we'll take the else branch)

	# COMPARISON: Test the condition
	cmp $5,%rcx            # COMPARE: Compute %rcx - 5 (10 - 5 = 5)
                                # cmp performs SUBTRACTION but doesn't store result
                                # Only sets CPU flags based on the result
                                # AT&T syntax: cmp source,destination means destination - source
                                # cmp x,y => compares (y-x) and sets flags for if they are equal or if the result is positive
                                #
                                # CALCULATION: 10 - 5 = 5
                                #
                                # FLAGS SET BY THIS OPERATION:
                                # ZF (Zero Flag) = 0      because result is 5 (not zero)
                                # SF (Sign Flag) = 0      because result is positive (5)
                                # OF (Overflow Flag) = 0  because no signed overflow occurred
                                # CF (Carry Flag) = 0     because no borrow needed (10 >= 5)
                                #
                                # Since result is positive, %rcx > 5 (actually %rcx >= 5)
                                # Since result is not zero, %rcx != 5
                                # Combined: %rcx > 5, so NOT (%rcx < 5)

	jl .good               # CONDITIONAL JUMP: Jump to .good if %rcx < 5
                                # jl = "jump if less than" (signed comparison)
                                # jl checks: SF != OF (sign flag differs from overflow flag)
                                # In our case: SF=0, OF=0, so SF == OF (not different)
                                # Therefore, the jump WILL NOT occur
                                # Since 10 is NOT < 5, we don't jump
                                #
                                # WHY jl DOESN'T JUMP:
                                # For %rcx < 5, we need (%rcx - 5) < 0
                                # Our result is +5, which is > 0, not < 0
                                # So jl correctly identifies this and does NOT jump

	# ELSE BRANCH: This code executes if condition is FALSE (%rcx >= 5)
	mov $-1,%rdi           # SET EXIT CODE: %rdi = -1
                                # This line executes because jl did NOT jump
                                # Return value -1 indicates "value is NOT less than 5"
                                # In this program, this code WILL execute (10 is not < 5)
                                # Exit code -1 displays as 255 in shell (8-bit unsigned wrap)

	jmp .finish            # UNCONDITIONAL JUMP: Jump to finish
                                # jmp ALWAYS jumps (no condition)
                                # This prevents "falling through" into the .good code
                                # Without this, we'd execute both branches!
                                # This completes the else branch

	# IF BRANCH: This code executes if condition is TRUE (%rcx < 5)
.good:
	mov $10,%rdi           # SET EXIT CODE: %rdi = 10
                                # This label is the target of the jl instruction
                                # Return value 10 indicates "value is less than 5"
                                # In this program, this code is NEVER executed (10 is not < 5)

	# COMMON EXIT PATH: Both branches converge here
.finish:
	mov $60,%rax           # SYSCALL NUMBER: exit() = syscall 60
                                # Prepare to terminate the program
                                # 60 is the Linux syscall number for sys_exit

	syscall                # INVOKE KERNEL: Execute exit(%rdi)
                                # Program terminates with exit code from %rdi
                                # In this case: exit(-1) → displays as exit(255)
                                # Because 10 is not less than 5

# CONTROL FLOW ANALYSIS:
# This program has TWO possible execution paths:
#
# PATH 1: When %rcx < 5 (NOT taken in this program):
#   _start → mov $10,%rcx → cmp $5,%rcx → jl (JUMPS) → .good: → mov $10,%rdi →
#   .finish: → mov $60,%rax → syscall
#   Exit code: 10
#
# PATH 2: When %rcx >= 5 (TAKEN in this program because 10 >= 5):
#   _start → mov $10,%rcx → cmp $5,%rcx → jl (NO JUMP) → mov $-1,%rdi →
#   jmp .finish → .finish: → mov $60,%rax → syscall
#   Exit code: -1 (displayed as 255)

# COMPARING WITH 03-comparisons.s:
# File 03-comparisons.s:
#   - Tests: 0 <= 5 (true)
#   - Uses: jle (jump if less or equal)
#   - Result: jumps to .less, returns 10
#
# This file (04-float-comparisons.s):
#   - Tests: 10 < 5 (false)
#   - Uses: jl (jump if less, NOT equal)
#   - Result: doesn't jump, returns -1
#
# Key difference:
#   - jle includes equality (<=)
#   - jl excludes equality (<)

# CPU FLAGS DETAILED EXPLANATION:
# The x86-64 CPU maintains a FLAGS register (RFLAGS) with individual status bits:
#
# ZF (Zero Flag, bit 6):
#   Set to 1 when an arithmetic operation produces zero
#   Set to 0 when result is non-zero
#   After "cmp $5,%rcx" with %rcx=10: result is 5, so ZF=0
#   Used by: je/jz (jump if equal/zero), jne/jnz (jump if not equal/zero)
#
# SF (Sign Flag, bit 7):
#   Set to 1 when result is negative (MSB = 1 in two's complement)
#   Set to 0 when result is positive or zero
#   After "cmp $5,%rcx" with %rcx=10: result is 5 (positive), so SF=0
#   Used by: js (jump if sign), jns (jump if not sign), and signed comparisons
#
# OF (Overflow Flag, bit 11):
#   Set to 1 when signed arithmetic overflows
#   Set to 0 when no overflow occurs
#   After "cmp $5,%rcx" with %rcx=10: no overflow, so OF=0
#   Example of overflow: 0x7FFFFFFFFFFFFFFF + 1 = negative (overflow!)
#   Used by: jo (jump if overflow), jno (jump if not overflow)
#
# CF (Carry Flag, bit 0):
#   Set to 1 when unsigned arithmetic produces a carry or borrow
#   Set to 0 when no carry/borrow occurs
#   After "cmp $5,%rcx" with %rcx=10: 10-5 needs no borrow, so CF=0
#   Used by: jc (jump if carry), jnc (jump if not carry), and unsigned comparisons

# CONDITIONAL JUMP INSTRUCTIONS FOR SIGNED COMPARISONS:
# After cmp X,Y (which computes Y - X):
#
#   jl/jnge - Jump if Less / Jump if Not Greater or Equal
#             Condition: SF != OF (sign differs from overflow)
#             After cmp X,Y: jumps if Y < X (signed)
#             THIS IS THE ONE WE USE IN THIS PROGRAM!
#
#   jle/jng - Jump if Less or Equal / Jump if Not Greater
#             Condition: (ZF = 1) OR (SF != OF)
#             After cmp X,Y: jumps if Y <= X (signed)
#
#   jg/jnle - Jump if Greater / Jump if Not Less or Equal
#             Condition: (ZF = 0) AND (SF == OF)
#             After cmp X,Y: jumps if Y > X (signed)
#
#   jge/jnl - Jump if Greater or Equal / Jump if Not Less
#             Condition: SF == OF
#             After cmp X,Y: jumps if Y >= X (signed)
#
#   je/jz   - Jump if Equal / Jump if Zero
#             Condition: ZF = 1
#             After cmp X,Y: jumps if Y == X
#
#   jne/jnz - Jump if Not Equal / Jump if Not Zero
#             Condition: ZF = 0
#             After cmp X,Y: jumps if Y != X

# HOW jl WORKS:
# After "cmp $5,%rcx" with %rcx=10:
#   Computation: 10 - 5 = 5
#   Flags: ZF=0, SF=0, OF=0
#
# jl jumps if: SF != OF
#   SF = 0 (result is positive)
#   OF = 0 (no overflow)
#   SF != OF? No (0 == 0)
#   Therefore: jl DOES NOT JUMP
#
# Why this logic works:
#   - If result is negative without overflow (SF=1, OF=0), then Y < X ✓
#   - If result is negative WITH overflow (SF=1, OF=1), that's actually Y > X
#     (Example: comparing 0x8000000000000000 - 0x7FFFFFFFFFFFFFFF overflows)
#   - If result is positive (SF=0), then Y >= X, so NOT Y < X
#   - The SF != OF condition correctly handles overflow cases

# ACTUAL FLOATING-POINT COMPARISONS (what the filename suggests):
# If this were REALLY a floating-point comparison program, it would look like:
#
#   .section .data
#   val1: .float 10.0        # Define floating-point constant
#   val2: .float 5.0
#
#   .section .text
#   .global _start
# _start:
#   movss val1(%rip),%xmm0   # Load float into XMM register
#   movss val2(%rip),%xmm1   # Load second float
#   comiss %xmm1,%xmm0       # Compare floats: %xmm0 - %xmm1
#   jb .less                  # Jump if below (for float <)
#   mov $-1,%rdi
#   jmp .finish
# .less:
#   mov $10,%rdi
# .finish:
#   mov $60,%rax
#   syscall
#
# KEY DIFFERENCES FOR FLOATING-POINT:
# 1. Use XMM registers (%xmm0-%xmm15), not general registers
# 2. Use movss/movsd to load floats/doubles
# 3. Use comiss/comisd or ucomiss/ucomisd to compare
# 4. Use unsigned comparison jumps (jb, jbe, ja, jae) after comiss/comisd
# 5. Handle special cases: NaN, Infinity, -0 vs +0
#
# FLOATING-POINT COMPARISON INSTRUCTIONS:
#   comiss  - Compare Ordered Scalar Single-Precision (float)
#   comisd  - Compare Ordered Scalar Double-Precision (double)
#   ucomiss - Compare Unordered Scalar Single-Precision (handles NaN)
#   ucomisd - Compare Unordered Scalar Double-Precision (handles NaN)
#
# These set ZF, PF, CF flags (NOT the same as integer cmp!)
#
# FLOATING-POINT COMPARISON FLAGS:
# After comiss/comisd:
#   ZF=1, PF=0, CF=0 → operands equal
#   ZF=0, PF=0, CF=0 → first > second
#   ZF=0, PF=0, CF=1 → first < second
#   ZF=1, PF=1, CF=1 → unordered (NaN involved)
#
# FLOATING-POINT CONDITIONAL JUMPS:
# After comiss/comisd, use:
#   je  - equal
#   jne - not equal
#   ja  - above (greater, unordered = false)
#   jae - above or equal (>=, unordered = false)
#   jb  - below (less than, unordered = false)
#   jbe - below or equal (<=, unordered = false)
#
# For handling NaN (unordered results):
#   jp  - jump if parity (PF=1, unordered/NaN)
#   jnp - jump if not parity (PF=0, ordered)

# EQUIVALENT C CODE (for this integer program):
# int main() {
#     int value = 10;        // %rcx
#     if (value < 5) {
#         return 10;         // "good" branch
#     } else {
#         return -1;         // "bad" branch - executes
#     }
# }

# EQUIVALENT C CODE (for actual float comparison):
# int main() {
#     float value = 10.0f;
#     if (value < 5.0f) {
#         return 10;
#     } else {
#         return -1;
#     }
# }

# WHY THE FILENAME IS MISLEADING:
# This file is named "04-float-comparisons.s" but uses INTEGER comparisons.
# Possible reasons:
#   1. Originally intended to demonstrate float comparisons, simplified to int
#   2. Teaching progression: master int comparisons before float
#   3. Naming error
#
# Pedagogical value:
#   - Shows jl (strict less than) vs jle (less or equal)
#   - Demonstrates negative exit codes
#   - Provides contrast with 03-comparisons.s

# TESTING DIFFERENT VALUES:
# To test different conditions, change "mov $10,%rcx" to:
#
# mov $0,%rcx   → 0 < 5 → true → exit(10)
# mov $4,%rcx   → 4 < 5 → true → exit(10)
# mov $5,%rcx   → 5 < 5 → false → exit(-1) [shows as 255]
# mov $6,%rcx   → 6 < 5 → false → exit(-1) [shows as 255]
# mov $-1,%rcx  → -1 < 5 → true → exit(10)

# SIGNED vs UNSIGNED COMPARISONS:
# This program uses jl (signed comparison).
#
# SIGNED interpretation (jl):
#   Treats values as two's complement signed integers
#   Range: -2^63 to 2^63-1 for 64-bit registers
#   10 and 5 are both positive, comparison is straightforward
#
# UNSIGNED interpretation (jb - Jump if Below):
#   Treats values as unsigned integers
#   Range: 0 to 2^64-1 for 64-bit registers
#   For small positive numbers like 10 and 5, result is same as signed
#
# When it matters:
#   Compare -1 (0xFFFFFFFFFFFFFFFF) to 5:
#   Signed: -1 < 5 (jl jumps)
#   Unsigned: 18446744073709551615 > 5 (jb doesn't jump)

# TO ASSEMBLE, LINK, AND TEST:
# as 04-float-comparisons.s -o 04-float-comparisons.o
# ld 04-float-comparisons.o -o 04-float-comparisons
# ./04-float-comparisons
# echo $?                      # Should display 255 (because -1 wraps to 255)
#
# TO TEST WITH DIFFERENT VALUES:
# Edit line "mov $10,%rcx" to "mov $3,%rcx" and reassemble
# Then echo $? should display 10 (because 3 < 5)

# DEBUGGING WITH GDB:
# gdb ./04-float-comparisons
# (gdb) break _start
# (gdb) run
# (gdb) info registers rcx rdi rax      # View register values
# (gdb) info registers eflags           # View CPU flags
# (gdb) stepi                            # Step one instruction
# (gdb) print/t $eflags                  # Print flags in binary
#
# Watch the flags change after cmp:
# (gdb) watch $eflags

# COMMON MISTAKES AND DEBUGGING:
# 1. Confusing jl (less than) with jle (less or equal)
#    → jl jumps for <, jle jumps for <=
#
# 2. Thinking this program uses floats (it doesn't!)
#    → Real float comparison needs comiss/comisd and XMM registers
#
# 3. Expecting positive exit code (program returns -1)
#    → -1 is valid but displays as 255 (unsigned 8-bit)
#
# 4. Forgetting jmp .finish after else branch
#    → Would fall through and execute both branches
#
# 5. Using unsigned jump (jb) instead of signed (jl)
#    → Works for positive numbers, breaks for negative

# EXPERIMENT:
# 1. Change jl to jle (add equality):
#    jle .good
#    Now test with mov $5,%rcx
#    5 <= 5 is true, so exits with 10
#
# 2. Change jl to jge (reverse condition):
#    jge .good
#    Now 10 >= 5 is true, so exits with 10
#
# 3. Try actual float comparison (advanced):
#    See the floating-point example code above
#    Requires understanding XMM registers and SSE instructions
