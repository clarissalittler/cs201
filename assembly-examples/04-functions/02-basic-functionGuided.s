# ============================================================================
# Basic Function - Guided Version
# ============================================================================
# This program demonstrates how to define and call a simple function in
# assembly, following the x86-64 calling convention.
#
# WHAT IT DOES: Calls fun(2, 3) which returns (2+3)*3 = 15, exits with 15
#
# C EQUIVALENT:
#   int fun(int a, int b) {
#       return 3 * (a + b);
#   }
#   int main() {
#       return fun(2, 3);  // returns 15
#   }
# ============================================================================

.text
.globl _start

# ============================================================================
# FUNCTION: fun(a, b) → returns 3*(a+b)
# Parameters:
#   %rdi = first parameter (a)
#   %rsi = second parameter (b)
# Returns:
#   %rax = result
# ============================================================================

fun:
    mov $0, %rax            # Initialize result to 0
    add %rdi, %rax          # result += a (first parameter)
    add %rsi, %rax          # result += b (second parameter)
                            # Now %rax = a + b

    imul $3, %rax           # result *= 3
                            # imul = signed integer multiplication
                            # Now %rax = 3 * (a + b)

    ret                     # Return to caller
                            # Result is in %rax (as per convention)

# ============================================================================
# MAIN PROGRAM
# ============================================================================

_start:
    # Prepare arguments for fun(2, 3)
    mov $2, %rdi            # First argument: a = 2
    mov $3, %rsi            # Second argument: b = 3

    # Call the function
    call fun                # Calls fun, pushes return address on stack
                            # Result comes back in %rax

    # Exit with function result as status code
    mov %rax, %rdi          # exit status = function result (15)
    mov $60, %rax           # syscall: exit
    syscall

# ============================================================================
# CALLING CONVENTION:
#
# ARGUMENTS (in order):
#   1st: %rdi
#   2nd: %rsi
#   3rd: %rdx
#   4th: %rcx
#   5th: %r8
#   6th: %r9
#   7+ : on the stack
#
# RETURN VALUE:
#   %rax (for integers and pointers)
#   %xmm0 (for floating-point values)
#
# WHAT 'call' DOES:
#   1. Push current address (next instruction) onto stack
#   2. Jump to the function label
#
# WHAT 'ret' DOES:
#   1. Pop return address from stack
#   2. Jump to that address
#
# ============================================================================
# WHY THIS IS SIMPLE:
#
# This function doesn't use the stack for local variables, so we don't need:
#   - push %rbp / mov %rsp, %rbp (stack frame setup)
#   - mov %rbp, %rsp / pop %rbp (stack frame teardown)
#   - sub $n, %rsp (local variable allocation)
#
# We only use registers, which makes this function very simple!
#
# For functions with local variables or that call other functions, you need
# a proper stack frame. See 01-fibonacci.s for a more complex example.
#
# ============================================================================
# TO BUILD AND RUN:
#
# as 02-basic-functionGuided.s -o func.o
# ld func.o -o func
# ./func
# echo $?                  # Should print: 15
#
# CALCULATION: fun(2, 3) = 3 * (2 + 3) = 3 * 5 = 15
# ============================================================================
