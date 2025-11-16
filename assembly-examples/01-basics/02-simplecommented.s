# PEDAGOGICAL PURPOSE:
# This is a very simple x86-64 assembly program that exits with status code -1.
# It demonstrates the absolute minimum viable program structure while introducing
# signed vs unsigned integer representation.
#
# Key learning objectives:
# 1. Minimal assembly program structure (.text, .global, _start)
# 2. The exit system call (syscall number 60)
# 3. How to set exit status codes
# 4. Signed vs unsigned byte representation (two's complement)
# 5. Understanding exit codes and shell variable $?
# 6. The syscall instruction and kernel interface
# 7. Why -1 becomes 255 when viewed as exit status

        .section .text          # CODE SECTION: Contains executable instructions
                                # .section .text (or just .text) starts the code section
                                # This is where CPU instructions live
                                # The .text section has these properties:
                                #   - Read-only (cannot modify code at runtime)
                                #   - Executable (CPU can run instructions from here)
                                #   - Loaded into memory when program starts
                                #
                                # WHY separate sections?
                                # - Security: Code can't be modified (prevents code injection)
                                # - Organization: Separates code from data
                                # - Memory protection: OS can enforce different permissions

        .global _start          # GLOBAL SYMBOL: Export _start for the linker
                                # .global (or .globl) makes a symbol visible outside this file
                                # The linker needs to find _start to know where program begins
                                # Without this directive:
                                #   - _start would be local to this file only
                                #   - Linker would error: "undefined reference to _start"
                                #   - Program wouldn't link
                                #
                                # WHY _start and not main?
                                # - _start is for programs linked with ld (low-level)
                                # - main is for programs linked with gcc (includes C runtime)
                                # - C runtime calls _start, which eventually calls main
                                # - We're writing pure assembly, so we use _start

_start:                         # ENTRY POINT LABEL: Where execution begins
                                # This is a label - marks a location in code
                                # Labels end with : and can be jumped to or referenced
                                # When OS loads this program, it jumps to _start
                                # This is like main() in C programs
                                # Everything after this label is code that executes

        # PREPARE EXIT SYSTEM CALL
        # We're going to call exit(-1) via syscall
        # Linux x86-64 System Call Convention:
        #   %rax = syscall number
        #   %rdi = 1st argument
        #   %rsi = 2nd argument
        #   %rdx = 3rd argument
        #   %r10 = 4th argument (not %rcx!)
        #   %r8  = 5th argument
        #   %r9  = 6th argument
        #
        # For exit:
        #   syscall number: 60
        #   arguments: just one - the exit status

        mov $60, %rax           # Load immediate value 60 into register %rax
                                # AT&T SYNTAX: mov source, destination
                                # $ indicates immediate value (literal number)
                                # %rax is the 64-bit accumulator register
                                # 60 is the syscall number for exit on Linux x86-64
                                #
                                # SYSTEM CALL NUMBERS:
                                # Each OS operation has a unique number
                                # On Linux x86-64:
                                #   1 = write (output)
                                #   60 = exit (terminate program)
                                #   More in /usr/include/x86_64-linux-gnu/asm/unistd_64.h
                                #
                                # The kernel looks at %rax to determine which syscall to execute

        mov $-1, %rdi           # Load immediate value -1 into register %rdi
                                # %rdi holds the first argument to the syscall
                                # For exit(), the argument is the exit status code
                                # -1 in decimal = 0xFFFFFFFFFFFFFFFF in 64-bit two's complement
                                #
                                # SIGNED VS UNSIGNED:
                                # We're passing -1 (signed integer)
                                # But exit codes are unsigned bytes (0-255)
                                # The kernel only looks at the lowest 8 bits
                                #
                                # -1 in binary (64-bit): 1111111111111111...11111111
                                # Lower 8 bits:          11111111
                                # As unsigned byte:      255
                                #
                                # So exit(-1) results in exit status 255

        syscall                 # SYSCALL INSTRUCTION: Invoke system call
                                # This is a special instruction that:
                                # 1. Saves current state (registers, flags)
                                # 2. Switches CPU to kernel mode (privileged)
                                # 3. Jumps to kernel syscall handler
                                # 4. Kernel reads %rax to determine which syscall (60 = exit)
                                # 5. Kernel reads %rdi to get the argument (status = -1)
                                # 6. Kernel executes exit(-1)
                                # 7. Process terminates (never returns from syscall)
                                #
                                # WHAT HAPPENS IN THE KERNEL:
                                # - Kernel validates the syscall number
                                # - Kernel terminates this process
                                # - Kernel closes all open files
                                # - Kernel frees all memory
                                # - Kernel returns exit status (255) to parent process (shell)
                                # - Shell stores 255 in special variable $?

# COMPREHENSIVE SUMMARY:
#
# WHAT THIS PROGRAM DOES:
# 1. Sets syscall number to 60 (exit)
# 2. Sets exit status to -1 (which becomes 255 unsigned)
# 3. Calls kernel to terminate program
# 4. Shell receives exit status 255
#
# CONTROL FLOW:
# 1. OS loads program into memory
# 2. OS jumps to _start
# 3. mov $60, %rax executes
# 4. mov $-1, %rdi executes
# 5. syscall executes
# 6. Program terminates (doesn't continue past syscall)
#
# REGISTERS AFTER EACH INSTRUCTION:
# Initial state: (registers have undefined values)
# After mov $60, %rax:  %rax = 60, %rdi = undefined
# After mov $-1, %rdi:  %rax = 60, %rdi = -1 (0xFFFFFFFFFFFFFFFF)
# After syscall:        (program terminated, no more state)

# TWO'S COMPLEMENT REPRESENTATION:
#
# How -1 is represented in binary (two's complement):
# 1. Start with positive 1:     00000001 (8-bit)
# 2. Flip all bits (one's comp): 11111110
# 3. Add 1 (two's complement):   11111111
# Result: -1 = 11111111
#
# When interpreted as unsigned: 11111111 = 255
#
# This is why exit(-1) shows up as 255 in the shell!
#
# 8-bit two's complement range:
#   Signed:   -128 to 127
#   Unsigned: 0 to 255
#   Same bit patterns, different interpretations

# EXIT STATUS CODES:
#
# Exit codes are 8-bit unsigned integers (0-255)
# Convention:
#   0     = success, no errors
#   1-255 = failure, error occurred
#
# Common exit codes:
#   0   = success
#   1   = general error
#   2   = misuse of shell command
#   126 = command cannot execute
#   127 = command not found
#   128+N = fatal error signal N
#   255 = exit status out of range (or -1)
#
# The kernel truncates exit status to 8 bits:
#   exit(0)    -> 0
#   exit(1)    -> 1
#   exit(256)  -> 0   (256 % 256 = 0)
#   exit(-1)   -> 255 (lower 8 bits of two's complement)

# TO ASSEMBLE, LINK, AND RUN:
as 02-simple.s -o 02-simple.o    # Assemble source to object file
ld 02-simple.o -o 02-simple      # Link object file to executable
./02-simple                      # Run the program (no output)
echo $?                          # Print exit status (shows 255)

# EXPECTED OUTPUT:
# (no terminal output)
#
# Exit status: 255
#
# The program produces no visible output because it doesn't call write().
# It only exits with status 255.

# CHECKING EXIT STATUS IN SHELL:
#
# In bash/zsh:
# $ ./02-simple
# $ echo $?
# 255
#
# The special variable $? holds the exit status of the last command.
# After running our program, $? contains 255.
#
# You can also use this in scripts:
# if ./02-simple; then
#     echo "Success (exit 0)"
# else
#     echo "Failed with status $?"
# fi
# This would print: Failed with status 255

# COMPARISON TO OTHER SIMPLE PROGRAMS:
#
# Exit with 0 (success):
#   mov $60, %rax
#   mov $0, %rdi    # or: xor %rdi, %rdi
#   syscall
#
# Exit with 1 (generic error):
#   mov $60, %rax
#   mov $1, %rdi
#   syscall
#
# Exit with 42 (custom code):
#   mov $60, %rax
#   mov $42, %rdi
#   syscall
#
# This program (exit with -1 -> 255):
#   mov $60, %rax
#   mov $-1, %rdi
#   syscall

# WHY IS THIS USEFUL?
#
# This minimal program teaches:
# 1. How to structure assembly code
# 2. How system calls work
# 3. How signed/unsigned conversion happens
# 4. How programs communicate with the shell
#
# It's the foundation for:
# - More complex programs
# - Understanding function returns
# - Error handling and status codes
# - Building larger assembly projects

# DEBUGGING WITH strace:
# strace shows all system calls a program makes:
#
# $ strace ./02-simple
# execve("./02-simple", ["./02-simple"], ...) = 0
# exit(-1)                                = ?
# +++ exited with 255 +++
#
# This clearly shows:
# 1. The OS execve's our program
# 2. Our program calls exit(-1)
# 3. The shell sees exit status 255

# DEBUGGING WITH gdb:
# Step through the program instruction by instruction:
#
# $ gdb ./02-simple
# (gdb) break _start       # Set breakpoint at entry
# (gdb) run                # Start program
# (gdb) stepi              # Step one instruction
# (gdb) info registers     # See register values
# (gdb) print/x $rax       # Print %rax in hex (0x3c = 60)
# (gdb) print/x $rdi       # Print %rdi in hex (0xffffffffffffffff = -1)
# (gdb) print/u $rdi       # Print %rdi unsigned (18446744073709551615)
# (gdb) stepi              # Step to next instruction
# (gdb) stepi              # Execute syscall (program exits)

# RELATION TO C:
#
# This assembly program is equivalent to:
#
# int main() {
#     return -1;  // Shell sees 255
# }
#
# Or using the exit function:
#
# #include <stdlib.h>
# int main() {
#     exit(-1);   // Shell sees 255
# }
#
# Or using the _exit system call directly:
#
# #include <unistd.h>
# int main() {
#     _exit(-1);  // Shell sees 255
# }

# THE THREE ESSENTIAL COMPONENTS:
#
# Every minimal assembly program needs:
# 1. .text section     - where code lives
# 2. .global _start    - export the entry point
# 3. _start: label     - the actual entry point
#
# Without these, the program won't assemble/link correctly.

# COMMON MISTAKES TO AVOID:
#
# 1. Forgetting .global _start
#    Error: ld: warning: cannot find entry symbol _start
#
# 2. Misspelling _start (e.g., _Start, start, _begin)
#    Error: ld: warning: cannot find entry symbol _start
#
# 3. Wrong syscall number (e.g., 1 instead of 60)
#    Result: write() syscall attempted with garbage arguments
#
# 4. Forgetting syscall instruction
#    Result: CPU continues executing garbage/next instruction
#
# 5. Wrong register for syscall number (e.g., %rdi instead of %rax)
#    Result: Unpredictable behavior (whatever was in %rax)

# WHAT HAPPENS WITHOUT THE EXIT SYSCALL?
#
# If you remove the syscall instruction:
#   mov $60, %rax
#   mov $-1, %rdi
#   # (no syscall here)
#
# The CPU keeps executing whatever comes next in memory.
# Usually garbage bytes or padding, causing a segmentation fault.
# Always explicitly exit your assembly programs!

# MEMORY STATE AT RUNTIME:
#
# When this program runs, memory layout:
#
# High addresses
#   [kernel space - off limits]
#   [stack - grows downward]
#   [heap - grows upward]
#   [.data and .bss - uninitialized in this program]
#   [.text - our code]
#   0x400000 (typical program start)
# Low addresses
#
# Our three instructions are in the .text section.
# No .data section needed (we don't use any variables).

# NEXT STEPS:
#
# After understanding this program, try:
# 1. Changing exit status to other values (0, 1, 42, etc.)
# 2. Adding a write syscall before exit (print something)
# 3. Using different registers and see what happens
# 4. Exploring other syscalls (read, open, close)
# 5. Building more complex programs with loops and conditions
