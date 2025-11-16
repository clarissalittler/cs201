# PEDAGOGICAL PURPOSE:
# This is the SMALLEST possible x86-64 assembly program that can run on Linux.
# Key learning objectives:
# 1. Understanding the minimal structure of an assembly program
# 2. Introduction to sections (.text for code)
# 3. Understanding the _start entry point
# 4. System calls - the interface between user programs and the OS kernel
# 5. How programs exit and return status codes to the shell
# 6. Introduction to registers and their purposes
# 7. Understanding AT&T syntax (source, destination operand order)

        .text                   # SECTION DIRECTIVE: All code must be in the .text section
                               # This tells the assembler where executable instructions live
                               # (as opposed to .data for initialized data or .bss for uninitialized data)

        .global _start         # GLOBAL DIRECTIVE: Makes _start visible to the linker
                               # _start is the entry point for programs linked with ld
                               # (When using gcc, the entry point would be main)

_start:
        # SYSTEM CALL #60: exit
        # On Linux x86-64, syscalls are made by:
        # 1. Put syscall number in %rax
        # 2. Put arguments in %rdi, %rsi, %rdx, %r10, %r8, %r9 (in that order)
        # 3. Execute the syscall instruction

        mov $60, %rax           # Load immediate value 60 into register %rax
                               # AT&T SYNTAX: source comes first, destination second
                               # The $ indicates an immediate value (literal number)
                               # Syscall 60 is exit() - terminates the program
                               # The kernel looks in %rax to determine which syscall to execute

        mov $-1, %rdi          # Load -1 into register %rdi
                               # %rdi holds the first argument to the syscall
                               # For exit(), this is the exit code/status
                               # -1 as signed byte = 255 as unsigned byte
                               # The shell will see exit status 255 (255 = 0xFF = error)
                               # Typically 0 means success, non-zero means error

        ## the exit code is 1 unsigned byte
        ## This means only values 0-255 are meaningful
        ## The kernel truncates to 8 bits

        syscall                # SYSCALL INSTRUCTION: Transfer control to kernel
                               # This triggers a context switch to kernel mode
                               # The kernel executes the syscall specified in %rax (exit)
                               # With the argument from %rdi (status = -1 = 255)
                               # The program terminates and never returns from this instruction

# AFTER RUNNING:
# You can check the exit status in bash with: echo $?
# It will show 255

# TO ASSEMBLE AND LINK:
# as smallest.s -o smallest.o
# ld smallest.o -o smallest
# ./smallest
# echo $?  # Shows 255
