# PEDAGOGICAL PURPOSE:
# This program demonstrates writing and calling a custom function in x86-64 assembly.
# It introduces fundamental concepts of function calls, parameter passing, and return values.
# Key learning objectives:
# 1. Understanding how to define and call functions in assembly
# 2. The x86-64 calling convention for passing parameters (%rdi, %rsi)
# 3. How to return values from functions (using %rax)
# 4. The ret instruction and how it returns control to the caller
# 5. Combining function calls with system calls
# 6. Using the exit status to verify program results

.section .text               # CODE SECTION: All executable instructions go here
        .global _start       # GLOBAL DIRECTIVE: Make _start visible to linker
                            # _start is the entry point when linking with ld

        ## silly add takes the argument in %rdi
        ## and the argument in %rsi
        ## and adds them together in %rax

sillyAdd:                    # FUNCTION LABEL: Entry point for our custom function
                            # This is our first user-defined function in assembly
                            # WHY "sillyAdd"? Because there's a simpler way to add,
                            # but this demonstrates function mechanics clearly

        mov $0,%rax          # INITIALIZE RESULT: Set %rax to 0
                            # %rax is the standard register for return values
                            # WHY clear first? To ensure we start with a clean slate
                            # and avoid leftover values from previous operations

        add %rdi,%rax        # ADD FIRST PARAMETER: %rax = %rax + %rdi
                            # %rdi contains the first parameter (10 when called from _start)
                            # x86-64 calling convention: first integer arg goes in %rdi
                            # Now %rax contains 0 + 10 = 10

        add %rsi,%rax        # ADD SECOND PARAMETER: %rax = %rax + %rsi
                            # %rsi contains the second parameter (20 when called from _start)
                            # x86-64 calling convention: second integer arg goes in %rsi
                            # Now %rax contains 10 + 20 = 30
                            # The result is ready to be returned

        ret                  # RETURN: Pop return address from stack and jump to it
                            # This transfers control back to the instruction after "call sillyAdd"
                            # The return value (30) is in %rax, which the caller can use
                            # WHY ret works: "call" pushed the return address onto the stack
                            # "ret" pops that address and jumps to it

_start:                      # PROGRAM ENTRY POINT: Execution begins here
                            # The linker knows to start execution at _start

        mov $10,%rdi         # FIRST ARGUMENT: Load 10 into %rdi
                            # Preparing to call sillyAdd(10, 20)
                            # %rdi = first parameter

        mov $20,%rsi         # SECOND ARGUMENT: Load 20 into %rsi
                            # %rsi = second parameter
                            # Both arguments are now set up according to calling convention

        call sillyAdd        # CALL FUNCTION: Jump to sillyAdd, saving return address
                            # What happens:
                            # 1. Push address of next instruction (line 18) onto stack
                            # 2. Jump to sillyAdd label
                            # 3. sillyAdd executes, computes 10 + 20 = 30
                            # 4. ret pops return address and jumps back here
                            # 5. %rax now contains 30 (the return value)

        mov %rax,%rdi        # USE RETURN VALUE AS EXIT CODE: %rdi = %rax (30)
                            # We're using the function's return value as our exit status
                            # WHY? To demonstrate that the function worked correctly
                            # You can verify with: ./firstFunc; echo $?
                            # This will show 30 (the sum of 10 + 20)

        mov $60,%rax         # SYSCALL NUMBER: 60 = exit
                            # Preparing to terminate the program

        syscall              # EXIT: Call kernel to terminate
                            # The exit status is 30 (from %rdi)
                            # The program ends here

# CALLING CONVENTION SUMMARY:
#
# When calling a function in x86-64:
# - First 6 integer arguments: %rdi, %rsi, %rdx, %rcx, %r8, %r9
# - Return value: %rax
# - call instruction: pushes return address, jumps to function
# - ret instruction: pops return address, returns to caller
#
# For sillyAdd(10, 20):
#   %rdi = 10 (first parameter)
#   %rsi = 20 (second parameter)
#   %rax = 30 (return value after function executes)

# STACK BEHAVIOR:
#
# Before "call sillyAdd":
#   Stack: [...]
#
# After "call sillyAdd" (inside sillyAdd):
#   Stack: [..., return_address]
#   (return_address points to "mov %rax,%rdi")
#
# After "ret" (back in _start):
#   Stack: [...]
#   (return_address popped, execution continues at "mov %rax,%rdi")

# TO ASSEMBLE, LINK, AND RUN:
# as firstFunc.s -o firstFunc.o    # Assemble to object file
# ld firstFunc.o -o firstFunc      # Link to executable
# ./firstFunc                       # Run program
# echo $?                          # Check exit status (should be 30)

# DESIGN NOTES:
# WHY is this "silly"?
# - We could just do: add %rsi,%rdi; mov %rdi,%rax; ret
# - Or even simpler: lea (%rdi,%rsi),%rax; ret
# - But this version clearly shows each step for pedagogical purposes
#
# WHY use a function for simple addition?
# - To demonstrate function call mechanics in their simplest form
# - Before adding complexity like stack frames or local variables
# - To clearly show parameter passing and return values
