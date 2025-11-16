# PEDAGOGICAL PURPOSE:
# This program demonstrates creating a custom function with arithmetic operations
# including multiplication. It builds on basic function concepts by introducing
# the imul (signed multiply) instruction and showing how to perform multiple
# operations before returning a result.
#
# Key learning objectives:
# 1. Understanding function definition and calling in assembly
# 2. Learning the x86-64 calling convention for parameters (%rdi, %rsi)
# 3. Understanding the imul instruction for signed multiplication
# 4. Learning the difference between imul (signed) and mul (unsigned)
# 5. Seeing how to chain operations: add, add, multiply
# 6. Understanding return values via %rax
# 7. Implementing the formula: return 3 * (a + b)
#
# CONCEPTUAL OVERVIEW:
# This program implements:
#   int fun(int a, int b) {
#       return 3 * (a + b);
#   }
#   int main() {
#       return fun(2, 3);  // Returns 3 * (2 + 3) = 15
#   }

        .text                      # CODE SECTION: Executable instructions
        .globl _start              # GLOBAL DIRECTIVE: Entry point visible to linker

        ## FUNCTION COMMENT (from original):
        ## this function adds two numbers
        ## then multiplies by 3
        ## then returns that

fun:                              # FUNCTION LABEL: Entry point for our custom function
                                  # This implements: 3 * (a + b)
                                  # Parameters arrive in %rdi (a) and %rsi (b)
                                  # Result will be returned in %rax

        mov $0,%rax                # INITIALIZE RESULT: %rax = 0
                                  # Clear the return value register
                                  # %rax is the standard register for return values
                                  # WHY clear first?
                                  # - Ensures we start with a known state
                                  # - Makes the subsequent additions clear
                                  # - Good practice to avoid leftover values

        add %rdi,%rax              # ADD FIRST PARAMETER: %rax = %rax + %rdi
                                  # %rdi contains the first parameter 'a' (2 when called)
                                  # x86-64 calling convention: first arg in %rdi
                                  # Now %rax = 0 + 2 = 2

        add %rsi,%rax              # ADD SECOND PARAMETER: %rax = %rax + %rsi
                                  # %rsi contains the second parameter 'b' (3 when called)
                                  # x86-64 calling convention: second arg in %rsi
                                  # Now %rax = 2 + 3 = 5
                                  # We've computed (a + b)

        imul $3,%rax               # MULTIPLY BY 3: %rax = %rax * 3
                                  # imul = Integer MULtiply (signed)
                                  # This is SIGNED multiplication
                                  # Syntax: imul immediate,%destination
                                  # Operation: destination = destination * immediate
                                  #
                                  # CALCULATION:
                                  # %rax = 5 * 3 = 15
                                  # This is the final result: 3 * (a + b)
                                  #
                                  # NOTE: Comment says "imul is multiplication of signed numbers"
                                  # This is correct! imul handles negative numbers properly
                                  # Example: -2 * 3 = -6 (imul gives correct result)
                                  #         -2 * 3 with mul would give wrong result

        ret                        # RETURN: Pop return address and jump to caller
                                  # The return value (15) is in %rax
                                  # Caller will find the result there
                                  # Stack has return address from "call fun"
                                  # ret pops it and jumps back to _start

        ## C EQUIVALENT (from original comment):
        ## int fun(int a, int b) {
        ##     return 3 * (a + b);
        ## }

_start:                           # PROGRAM ENTRY POINT: Execution begins here
                                  # We'll call our function and use its return value as exit code

        mov $2,%rdi                # FIRST ARGUMENT: %rdi = 2
                                  # Set up first parameter for fun(2, 3)
                                  # Calling convention: first integer arg in %rdi

        mov $3,%rsi                # SECOND ARGUMENT: %rsi = 3
                                  # Set up second parameter for fun(2, 3)
                                  # Calling convention: second integer arg in %rsi
                                  # Both arguments are now in place

        call fun                   # CALL FUNCTION: Jump to fun, save return address
                                  # WHAT HAPPENS:
                                  # 1. Push address of next instruction onto stack
                                  # 2. Jump to 'fun' label
                                  # 3. fun executes: 0 + 2 + 3 = 5, then 5 * 3 = 15
                                  # 4. ret pops return address and jumps back here
                                  # 5. %rax now contains 15 (the return value)
                                  #
                                  # CALCULATION TRACE:
                                  # fun(2, 3):
                                  # - %rax = 0
                                  # - %rax = 0 + 2 = 2
                                  # - %rax = 2 + 3 = 5
                                  # - %rax = 5 * 3 = 15
                                  # - return 15

        mov %rax,%rdi              # USE RETURN VALUE AS EXIT CODE: %rdi = %rax (15)
                                  # Transfer function result to exit code register
                                  # WHY? To demonstrate that the function worked correctly
                                  # You can verify with: ./fun1; echo $?
                                  # This will show 15 (the result of 3 * (2 + 3))

        mov $60,%rax               # SYSCALL NUMBER: 60 = exit
                                  # Prepare to terminate the program
                                  # Note: This overwrites the function return value in %rax
                                  # But we already saved it in %rdi!

        syscall                    # INVOKE EXIT: Execute exit(15)
                                  # Program terminates with exit code 15
                                  # Shell can check with: echo $?

# IMUL vs MUL - CRITICAL DIFFERENCE:
#
# imul (Integer MULtiply - SIGNED):
#   - Treats operands as signed two's complement integers
#   - Handles negative numbers correctly
#   - Example: imul $3,%rax where %rax = -2
#     Result: %rax = -6 (correct for signed)
#   - This is what we use for most integer arithmetic
#
# mul (MULtiply - UNSIGNED):
#   - Treats operands as unsigned integers
#   - Negative numbers interpreted as large positive
#   - Different syntax: mul %rbx (multiplies %rax by %rbx)
#   - Example: %rax = -2 (0xFFFFFFFFFFFFFFFE)
#     mul by 3 gives huge positive number (wrong for signed!)
#   - Used for unsigned arithmetic (like array sizes, byte counts)

# IMUL INSTRUCTION FORMS:
#
# One operand (uses %rax implicitly):
#   imul %rbx           # %rdx:%rax = %rax * %rbx (128-bit result)
#
# Two operands (destination is also source):
#   imul %rbx,%rax      # %rax = %rax * %rbx
#
# Three operands (immediate constant):
#   imul $3,%rsi,%rax   # %rax = %rsi * 3
#
# Two operands with immediate (used in this program):
#   imul $3,%rax        # %rax = %rax * 3
#   This is shorthand for: imul $3,%rax,%rax

# SIGNED vs UNSIGNED MULTIPLICATION EXAMPLE:
#
# Consider: -2 * 3
#
# With imul (signed):
#   %rax = -2 (0xFFFFFFFFFFFFFFFE in two's complement)
#   imul $3,%rax
#   %rax = -6 (0xFFFFFFFFFFFFFFFA in two's complement)
#   CORRECT!
#
# With mul (unsigned):
#   %rax = -2 (interpreted as 18446744073709551614)
#   mul with 3 (complex syntax, not shown)
#   Result: huge positive number
#   WRONG for signed arithmetic!

# CALLING CONVENTION SUMMARY:
#
# x86-64 System V ABI (used on Linux):
#
# Integer/pointer arguments (in order):
#   1st: %rdi
#   2nd: %rsi
#   3rd: %rdx
#   4th: %rcx
#   5th: %r8
#   6th: %r9
#   7th+: on stack
#
# Return value:
#   Integer/pointer: %rax
#   (128-bit: %rdx:%rax)
#
# Caller-saved registers (may be modified by callee):
#   %rax, %rcx, %rdx, %rsi, %rdi, %r8, %r9, %r10, %r11
#
# Callee-saved registers (must be preserved):
#   %rbx, %rbp, %r12-r15
#   (If function uses these, must save/restore them)

# WHY THIS FUNCTION DOESN'T SAVE REGISTERS:
#
# This function only uses:
#   - %rax (return value, caller-saved)
#   - %rdi (parameter, caller-saved)
#   - %rsi (parameter, caller-saved)
#
# Since all are caller-saved, we don't need to preserve them!
# The caller (_start) knows these might be modified.

# ARITHMETIC OPERATION CHAIN:
#
# To compute 3 * (a + b):
#
# Method 1 (used here):
#   %rax = 0
#   %rax += a
#   %rax += b        # Now have (a + b)
#   %rax *= 3        # Now have 3 * (a + b)
#
# Alternative method:
#   %rax = a
#   %rax += b        # Now have (a + b)
#   imul $3,%rax     # Now have 3 * (a + b)
#
# Alternative method (more efficient):
#   lea (%rdi,%rsi),%rax   # %rax = a + b (no flags affected)
#   imul $3,%rax           # %rax = 3 * (a + b)

# STACK BEHAVIOR:
#
# Before "call fun":
#   Stack: [...]
#   %rsp points to top
#
# After "call fun" (inside fun):
#   Stack: [..., return_address]
#   %rsp points to return_address
#   (return_address points to "mov %rax,%rdi" in _start)
#
# After "ret" (back in _start):
#   Stack: [...]
#   %rsp back to original position
#   Execution continues at "mov %rax,%rdi"

# EQUIVALENT C CODE:
#
# int fun(int a, int b) {
#     int result = 0;
#     result += a;
#     result += b;
#     result *= 3;
#     return result;
# }
#
# // Or more simply:
# int fun(int a, int b) {
#     return 3 * (a + b);
# }
#
# int main() {
#     int result = fun(2, 3);
#     return result;  // returns 15
# }

# TO ASSEMBLE, LINK, AND RUN:
# as fun1.s -o fun1.o
# ld fun1.o -o fun1
# ./fun1
# echo $?                          # Should display 15

# EXPERIMENT:
# Try different values:
#   mov $5,%rdi
#   mov $7,%rsi
# Result should be 3 * (5 + 7) = 36
#
# Try negative numbers:
#   mov $-2,%rdi
#   mov $3,%rsi
# Result should be 3 * (-2 + 3) = 3 * 1 = 3
#
# Try different multiplier:
#   imul $10,%rax
# Result with (2,3) would be 10 * 5 = 50
