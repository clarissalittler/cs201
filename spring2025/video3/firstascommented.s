# PEDAGOGICAL PURPOSE:
# This is the simplest possible x86-64 assembly program that demonstrates
# the minimum structure needed for a standalone executable.
# Key learning objectives:
# 1. Minimum structure of an assembly program
# 2. The .text section for code
# 3. The _start entry point for non-C programs
# 4. The exit system call (syscall 60)
# 5. Exit status codes and how to check them
# 6. Signed vs unsigned byte representation for exit codes

        .text                   # CODE SECTION: Marks beginning of executable code
                                # .text is a directive (not an instruction)
                                # Everything after this is in the code section
                                # The code section is read-only and executable

        .global _start          # GLOBAL SYMBOL: Export _start for the linker
                                # Without this, the linker wouldn't find the entry point
                                # _start is the default entry point for programs
                                # that don't use the C runtime library

_start:                         # ENTRY POINT LABEL: Where program execution begins
                                # When the OS loads this program, it jumps here
                                # This is like main() in C programs

        mov $60,%rax            # SETUP EXIT SYSCALL: Load 60 into %rax
                                # $60 is immediate value (the number 60)
                                # %rax holds the system call number
                                # 60 = exit syscall on Linux x86-64
                                # put 60 IN %rax
                                #
                                # System call numbers are documented in:
                                # /usr/include/x86_64-linux-gnu/asm/unistd_64.h
                                # Or: man 2 syscall

        mov $-1,%rdi            # SET EXIT CODE: Load -1 into %rdi
                                # $-1 is the immediate value -1
                                # %rdi holds the first argument to the syscall
                                # For exit, %rdi is the exit status code
                                # return -1
                                #
                                # IMPORTANT: Exit codes are unsigned bytes!
                                # -1 in two's complement = 0xFF = 255 unsigned
                                # So this program will exit with status 255

        syscall                 # INVOKE SYSTEM CALL: Call the kernel
                                # Executes the system call specified in %rax (60 = exit)
                                # Uses %rdi as the argument (exit status)
                                # Control transfers to the operating system
                                # The program terminates and never returns
                                # with 60 in %rax, this syscall will be to exit the program
                                ## will return 255 with echo $?
                                ## return codes are an unsigned byte
                                ## so they can take values 0 - 255
                                ## -1 -> 111111...[11111111]
                                #
                                # WHY 255?
                                # Exit codes are 8-bit unsigned integers (0-255)
                                # -1 as a signed byte: 11111111 (binary)
                                # When interpreted as unsigned: 255

# WHAT THIS PROGRAM DOES:
# 1. Sets up the exit system call (60)
# 2. Sets the exit status to -1 (which becomes 255)
# 3. Calls the kernel to exit
# Result: Program exits with status code 255

# TO ASSEMBLE, LINK, AND RUN:
# as firstas.s -o firstas.o    # Assemble source to object file
# ld firstas.o -o firstas      # Link object file to executable
# ./firstas                    # Run the program
# echo $?                      # Print the exit status (will show 255)

# EXPECTED OUTPUT:
# Running ./firstas produces no output
# But echo $? shows: 255

# WHY -1 BECOMES 255:
#
# In two's complement representation (8-bit):
#   Decimal  Binary     Unsigned
#   -1    =  11111111 = 255
#   -2    =  11111110 = 254
#   -128  =  10000000 = 128
#   0     =  00000000 = 0
#   1     =  00000001 = 1
#   127   =  01111111 = 127
#
# The kernel reads the lower 8 bits of %rdi as an unsigned value.
# So -1 (all bits set) maps to 255 (maximum unsigned byte).

# SYSTEM CALL DETAILS:
#
# Linux x86-64 System Call Convention:
#   %rax: System call number
#   %rdi: 1st argument
#   %rsi: 2nd argument
#   %rdx: 3rd argument
#   %r10: 4th argument (not %rcx!)
#   %r8:  5th argument
#   %r9:  6th argument
#
# The exit syscall:
#   Number: 60 (in %rax)
#   Prototype: void exit(int status)
#   Arguments: %rdi = exit status (0-255)
#   Return: Never returns (program terminates)

# COMPARISON TO C:
# This assembly program is equivalent to:
#   int main() {
#       return -1;  // Shell will see exit code 255
#   }
#
# Or using the exit function:
#   int main() {
#       exit(-1);   // Shell will see exit code 255
#   }

# THE THREE ESSENTIAL PARTS OF A MINIMAL ASSEMBLY PROGRAM:
#
# 1. .text section - where code lives
# 2. .global _start - export the entry point
# 3. _start label - the entry point itself
#
# Without these, the program won't link or run correctly.

# WHAT HAPPENS WHEN YOU RUN THIS:
#
# 1. Shell executes ./firstas
# 2. OS loads the executable into memory
# 3. OS sets up the process (stack, registers, etc.)
# 4. OS jumps to _start
# 5. CPU executes: mov $60,%rax (set syscall number)
# 6. CPU executes: mov $-1,%rdi (set exit status)
# 7. CPU executes: syscall (trap to kernel)
# 8. Kernel reads %rax (60) - ah, this is an exit syscall
# 9. Kernel reads %rdi (-1 = 255 unsigned) - the exit status
# 10. Kernel terminates the process
# 11. Kernel returns 255 to the shell
# 12. Shell stores 255 in $? variable

# REGISTER STATE AT syscall:
#   %rax = 60 (exit syscall number)
#   %rdi = -1 = 0xFFFFFFFFFFFFFFFF (64-bit two's complement)
#   Lower 8 bits: 0xFF = 255 (what the shell sees)

# WHY START WITH THIS PROGRAM?
# - Shows the absolute minimum needed for assembly
# - Introduces system calls (the bridge to the OS)
# - Demonstrates signed vs unsigned interpretation
# - Builds foundation for more complex programs
# - No dependencies on C library or other code
