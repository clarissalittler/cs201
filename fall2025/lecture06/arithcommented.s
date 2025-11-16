# PEDAGOGICAL PURPOSE:
# This program demonstrates basic arithmetic operations in x86-64 assembly.
# It shows how to perform addition with both immediate values and register values,
# and how the result is passed as an exit code.
#
# Key learning objectives:
# 1. Understanding arithmetic instructions (add)
# 2. Distinguishing between immediate values ($10) and register values (%r8)
# 3. How registers hold intermediate computational results
# 4. The difference between "add immediate to register" vs "add register to register"
# 5. Data flow through registers in a computation
# 6. Returning computed values via exit codes
#
# CONCEPTUAL OVERVIEW:
# This program performs: result = (5 + 10) + (5 + 10) = 15 + 15 = 30
# It demonstrates the two forms of the add instruction:
#   - add $immediate, %register  (adds a literal number)
#   - add %register, %register   (adds two register values)

        .section .text          # CODE SECTION: All executable instructions go here
                               # The .text section is loaded into memory as read-only executable code

        .global _start         # GLOBAL DIRECTIVE: Makes _start visible to the linker
                               # Required for the linker to find the program entry point

_start:
        # COMPUTATION STEP 1: Initialize r8 to 5
        mov $5,%r8             # LOAD IMMEDIATE: %r8 = 5
                               # The $ indicates this is an immediate value (literal 5)
                               # mov copies data from source to destination
                               # %r8 is one of the general-purpose 64-bit registers (r8-r15)
                               # AT&T syntax: source first, destination second

        # COMPUTATION STEP 2: Add 10 to r8
        add $10,%r8            # ADD IMMEDIATE TO REGISTER: %r8 = %r8 + 10
                               # This is the FIRST FORM of add: immediate + register
                               # After this instruction: %r8 = 5 + 10 = 15
                               # The add instruction modifies the destination operand in-place
                               # This also updates CPU flags (ZF, SF, OF, CF) based on result

        # COMPUTATION STEP 3: Double the value in r8
        add %r8,%r8            # ADD REGISTER TO REGISTER: %r8 = %r8 + %r8
                               # This is the SECOND FORM of add: register + register
                               # After this instruction: %r8 = 15 + 15 = 30
                               # BOTH operands reference the same register, effectively doubling it
                               # This demonstrates that add can use any combination of:
                               #   - add immediate to register
                               #   - add register to register
                               #   - add memory to register (not shown here)

        # PREPARE EXIT CODE: Copy result to proper register
        mov %r8,%rdi           # COPY RESULT: %rdi = %r8 (= 30)
                               # %rdi is used for the first argument to system calls
                               # For exit(), this argument is the exit status code
                               # The shell can read this value with: echo $?

        # EXIT SYSTEM CALL: Terminate the program
        mov $60,%rax           # SYSCALL NUMBER: Load 60 into %rax
                               # Syscall 60 is exit() on Linux x86-64
                               # The kernel reads %rax to determine which syscall to execute

        syscall                # INVOKE KERNEL: Execute the syscall
                               # Transfers control to the kernel
                               # Kernel executes exit() with status code from %rdi (30)
                               # Program terminates and returns 30 to the shell

# ARITHMETIC INSTRUCTION SUMMARY:
# The add instruction has several forms:
#   1. add $immediate, %register    - Adds a constant to a register
#   2. add %reg1, %reg2             - Adds reg1 to reg2, stores result in reg2
#   3. add memory, %register         - Adds memory value to register
#   4. add %register, memory         - Adds register to memory location
#
# Other arithmetic instructions follow similar patterns:
#   - sub (subtraction)
#   - imul (signed multiplication)
#   - mul (unsigned multiplication)
#   - idiv (signed division)
#   - div (unsigned division)
#   - inc (increment by 1)
#   - dec (decrement by 1)
#   - neg (negate - two's complement)

# REGISTER USAGE NOTES:
# %r8-%r15 are "new" registers added in x86-64 (not available in 32-bit x86)
# They are general-purpose and can be used freely in most contexts
# %rdi, %rsi, %rdx, %rcx, %r8, %r9 are used for function arguments (in that order)
# %rax is used for return values and syscall numbers

# TRACE OF REGISTER VALUES:
#   After mov $5,%r8:      %r8 = 5
#   After add $10,%r8:     %r8 = 15
#   After add %r8,%r8:     %r8 = 30
#   After mov %r8,%rdi:    %rdi = 30, %r8 = 30
#   After syscall:         Program exits with code 30

# TO ASSEMBLE, LINK, AND RUN:
# as arith.s -o arith.o         # Assemble to object file
# ld arith.o -o arith           # Link to executable
# ./arith                       # Run the program
# echo $?                       # Display exit code (should show 30)
