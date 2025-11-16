# PEDAGOGICAL PURPOSE:
# This is a minimal x86-64 assembly program that exits with code 255 (-1 as signed).
# It serves as a review of the smallest possible program structure and demonstrates
# negative exit codes. "Redux" means "brought back" or "revisited".
#
# Key learning objectives:
# 1. Review of minimal program structure (.text, .global, _start)
# 2. Understanding signed vs unsigned interpretation of exit codes
# 3. Two's complement representation: how -1 becomes 255
# 4. The exit system call (syscall 60) and its parameter
# 5. How the shell interprets exit codes as unsigned 8-bit values
# 6. Register roles: %rax for syscall number, %rdi for first argument
#
# CONCEPTUAL OVERVIEW:
# This program does three things:
#   1. Load -1 into %rdi (the exit code)
#   2. Load 60 into %rax (the syscall number for exit)
#   3. Execute syscall to terminate
#
# The shell will see exit code 255 (unsigned interpretation of -1)

        .text                  # TEXT SECTION: Executable code goes here
                               # This is the CODE SECTION (as opposed to .data or .bss)
                               # .text is loaded into memory as read-only executable pages
                               # The period (.) indicates this is an assembler directive

        .global _start         # GLOBAL DIRECTIVE: Export _start symbol
                               # Makes _start visible to the linker
                               # The linker needs to find _start as the entry point
                               # When using ld (not gcc), _start is the entry point
                               # (gcc uses 'main' and provides its own _start wrapper)

_start:
        # PROGRAM PURPOSE COMMENT:
        ## this program will put -1 in %rdi
        ## it will put the number 60 in %rax
        ## then execute syscall to end

        # STEP 1: Set up exit code
        mov $-1,%rdi           # LOAD EXIT CODE: %rdi = -1 (signed interpretation)
                               # The $ indicates an immediate (literal) value
                               # -1 in signed 64-bit = 0xFFFFFFFFFFFFFFFF
                               # But exit codes are only 8 bits!
                               # The kernel truncates to 8 bits: 0xFF = 255 unsigned
                               #
                               # TWO'S COMPLEMENT REPRESENTATION:
                               # -1 in binary (8-bit): 11111111 = 0xFF = 255 unsigned
                               # The shell interprets exit codes as UNSIGNED bytes
                               # So -1 (signed) becomes 255 (unsigned)
                               #
                               # %rdi is used because:
                               # - System call convention: first argument in %rdi
                               # - exit(int status) takes one argument: the status code
                               # - %rdi will pass this value to the kernel

        # STEP 2: Specify which system call to execute
        mov $60,%rax           # SYSCALL NUMBER: %rax = 60
                               # Syscall 60 is exit() on Linux x86-64
                               # Different syscalls have different numbers:
                               #   60 = exit
                               #   1 = write
                               #   0 = read
                               #   2 = open
                               #   etc. (see /usr/include/asm/unistd_64.h)
                               #
                               # The kernel reads %rax to determine which syscall to invoke
                               # This is the system call dispatch mechanism

        # STEP 3: Invoke the kernel
        syscall                # SYSTEM CALL INSTRUCTION: Transfer to kernel
                               # This is the MECHANISM for invoking the kernel
                               # What happens:
                               #   1. CPU switches from user mode to kernel mode
                               #   2. Kernel reads %rax (60) -> knows to call exit
                               #   3. Kernel reads %rdi (-1 = 255) -> exit status
                               #   4. Kernel terminates the process
                               #   5. Exit code is stored for parent process to read
                               #   6. Program never returns from syscall
                               #
                               # The syscall instruction:
                               #   - Saves return address (nowhere to return to!)
                               #   - Switches to kernel stack
                               #   - Jumps to kernel syscall handler
                               #   - Kernel executes requested operation
                               #   - For exit: process terminates, never returns

# WHY THIS PROGRAM EXISTS:
# This is called "smallredux" = "small revisited" or "small brought back"
# It reviews the concepts from lecture 5's smallest.s
# It's placed in lecture 6 to:
#   1. Refresh memory before learning new concepts
#   2. Provide a simple example to compare with more complex programs
#   3. Demonstrate that lecture 6 builds on lecture 5
#   4. Serve as a minimal test program

# EXIT CODES AND TWO'S COMPLEMENT:
# Exit codes are 8-bit unsigned values (0-255)
# But C's exit() takes a signed int
# The kernel truncates to 8 bits
#
# Examples:
#   exit(0)   -> 0b00000000 -> 0
#   exit(1)   -> 0b00000001 -> 1
#   exit(255) -> 0b11111111 -> 255
#   exit(256) -> 0b00000000 -> 0 (wraps around!)
#   exit(-1)  -> 0b11111111 -> 255
#   exit(-2)  -> 0b11111110 -> 254
#   exit(-128)-> 0b10000000 -> 128
#
# Two's complement conversion (8-bit):
#   Negative N becomes 256 + N when interpreted as unsigned
#   -1 becomes 256 + (-1) = 255
#   -2 becomes 256 + (-2) = 254
#   -128 becomes 256 + (-128) = 128

# REGISTER USAGE IN SYSTEM CALLS:
# x86-64 Linux system call convention:
#   %rax = syscall number
#   %rdi = 1st argument
#   %rsi = 2nd argument
#   %rdx = 3rd argument
#   %r10 = 4th argument (note: not %rcx!)
#   %r8  = 5th argument
#   %r9  = 6th argument
#
# Return value comes back in %rax
# (exit never returns, so this doesn't matter here)
#
# Why %r10 instead of %rcx for 4th argument?
#   syscall instruction uses %rcx internally (saves %rip)
#   So %rcx can't be used for argument passing

# COMPARING TO smallest.s FROM LECTURE 5:
# Both programs have identical structure:
#   .text section
#   .global _start
#   _start label
#   Load exit code into %rdi
#   Load syscall number into %rax
#   Execute syscall
#
# The only difference might be:
#   smallest.s might use a different exit code
#   smallredux.s uses -1 (= 255)
#
# This demonstrates that lecture 6 students understand lecture 5 concepts

# SECTIONS IN ASSEMBLY:
# .text   - Executable code (this program only has this)
# .data   - Initialized global variables (not used here)
# .rodata - Read-only data like strings (not used here)
# .bss    - Uninitialized globals (not used here)
#
# This program is minimal: only code, no data

# MINIMAL PROGRAM CHECKLIST:
# ✓ .text section (required for code)
# ✓ .global _start (required for linker to find entry point)
# ✓ _start label (entry point)
# ✓ Exit syscall (required to terminate gracefully)
# ✗ .data section (not needed if no global variables)
# ✗ Functions (not needed for simple programs)
# ✗ Stack manipulation (not needed if no function calls)

# WHAT HAPPENS IN THE SHELL:
# $ ./smallredux
# $ echo $?
# 255
#
# The $? variable in bash holds the exit code of the last command
# 255 is often used to indicate an error
# But here it's just demonstrating two's complement

# SIGNED VS UNSIGNED INTERPRETATION:
# The same bit pattern can mean different things:
#
# 8-bit value: 11111111 (0xFF)
#   Unsigned:  255
#   Signed:    -1
#
# 8-bit value: 10000000 (0x80)
#   Unsigned:  128
#   Signed:    -128
#
# 8-bit value: 01111111 (0x7F)
#   Unsigned:  127
#   Signed:    127 (same!)
#
# The hardware doesn't care!
# It's up to the program to interpret values as signed or unsigned
# Exit codes are interpreted as unsigned by the shell

# EQUIVALENT C CODE:
# int main() {
#     return -1;    // Shell sees 255
# }
#
# Or using exit():
# #include <stdlib.h>
# int main() {
#     exit(-1);     // Shell sees 255
# }

# COMMON EXIT CODE CONVENTIONS:
# 0   = Success
# 1   = General error
# 2   = Misuse of shell command
# 126 = Command can't execute
# 127 = Command not found
# 128+N = Fatal error signal N (e.g., 130 = Ctrl+C)
# 255 = Exit status out of range (or -1)

# WHY ASSEMBLY USES .text NOT .code:
# Historical reasons:
#   .text refers to "text segment" from Unix
#   "Text" means machine code (read-only, shareable)
#   "Data" means variables (read-write, per-process)
#   This terminology dates back to 1970s Unix

# TO ASSEMBLE, LINK, AND RUN:
# as smallredux.s -o smallredux.o    # Assemble to object file
# ld smallredux.o -o smallredux      # Link to executable
# ./smallredux                       # Run program
# echo $?                            # Display exit code: 255
#
# Alternative with one command:
# as smallredux.s -o smallredux.o && ld smallredux.o -o smallredux && ./smallredux && echo $?

# DEBUGGING THIS PROGRAM:
# gdb ./smallredux
# (gdb) break _start      # Set breakpoint at entry
# (gdb) run               # Start program
# (gdb) info registers    # Show all registers
# (gdb) stepi             # Step one instruction
# (gdb) print $rdi        # Show %rdi value
# (gdb) print/u $rdi      # Show as unsigned
# (gdb) print/d $rdi      # Show as signed (-1)
# (gdb) print/x $rdi      # Show as hex (0xffffffffffffffff)
