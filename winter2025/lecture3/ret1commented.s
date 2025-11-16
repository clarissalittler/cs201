# PEDAGOGICAL PURPOSE:
# This is a minimal x86-64 assembly program that demonstrates direct system calls
# and the absolute basics of assembly programming without the C standard library.
# Key learning objectives:
# 1. Understanding assembly language from scratch (no compiler-generated code)
# 2. Direct system calls using the syscall instruction
# 3. The exit system call (syscall number 60 on x86-64 Linux)
# 4. How to pass arguments to system calls
# 5. Exit codes and their meaning
# 6. The _start entry point (alternative to main)
# 7. Creating programs without the C runtime or standard library

        .text                 # CODE SECTION: Executable code goes here
                              # .text is read-only and executable
                              # No data section needed - this program has no data

        .global _start        # GLOBAL SYMBOL: Make _start visible to linker
                              # _start is the entry point when NOT using C runtime
                              # The OS directly calls _start (not main)
                              # This is the lowest-level entry point
                              # When using C runtime, it sets up and calls main
                              # Here we bypass C runtime entirely

_start:                       # ENTRY POINT: Program begins execution here
                              # The operating system jumps to this label when program starts
                              # Initial state:
                              #   - Stack set up by OS
                              #   - argc, argv, envp on stack (we ignore them)
                              #   - No initialization from C runtime
                              #   - No atexit handlers
                              #   - No stdio buffering

        # PREPARE EXIT SYSTEM CALL:
        # We're going to call the exit() system call directly
        # System calls are the interface between user programs and the kernel
        # On x86-64 Linux, system calls use the syscall instruction

        mov $60,%rax          # SYSTEM CALL NUMBER: Load 60 into rax
                              # AT&T syntax: mov source, destination
                              # $ prefix means immediate value (literal constant)
                              # % prefix means register
                              # $60 = syscall number for exit on x86-64 Linux
                              #
                              # System call numbers are OS-specific:
                              # x86-64 Linux syscalls defined in <asm/unistd_64.h>
                              # 60 = __NR_exit
                              #
                              # rax holds the system call number
                              # After syscall instruction, kernel checks rax
                              # to determine which system call to execute

        mov $-1,%rdi          # EXIT CODE: Load -1 into rdi
                              # rdi holds the first argument to the system call
                              # For exit(), the argument is the exit status code
                              # -1 conventionally indicates an error
                              #
                              # Typical exit codes:
                              #   0   = success
                              #   1   = generic error
                              #   -1  = error (treated as 255 by shell)
                              #
                              # WHY -1?
                              # This program demonstrates error exit
                              # You can check this in shell: ./ret1; echo $?
                              # Shell will show: 255 (since exit codes are 0-255)
                              #
                              # In two's complement (32-bit):
                              # -1 = 0xFFFFFFFF (all bits set)
                              # But exit code is only 8 bits, so: 0xFF = 255

        syscall               # SYSTEM CALL: Execute the system call
                              # syscall is the x86-64 instruction for system calls
                              # (Older x86 used int $0x80, but that's 32-bit)
                              #
                              # WHAT HAPPENS:
                              # 1. CPU switches from user mode to kernel mode
                              # 2. Kernel reads rax (syscall number: 60 = exit)
                              # 3. Kernel reads rdi (first argument: -1 = exit code)
                              # 4. Kernel executes sys_exit(-1)
                              # 5. Process terminates immediately
                              #
                              # THE PROGRAM NEVER RETURNS FROM syscall
                              # No epilogue, no return instruction
                              # The process is terminated by the kernel

# COMPREHENSIVE EXPLANATION:
#
# SYSTEM CALL INTERFACE (x86-64 Linux):
#
# System calls are how user programs request kernel services
# On x86-64 Linux, the interface is:
#
# Registers for arguments:
#   rax: system call number
#   rdi: first argument
#   rsi: second argument
#   rdx: third argument
#   r10: fourth argument
#   r8:  fifth argument
#   r9:  sixth argument
#
# Return value:
#   rax: return value (or error code if negative)
#
# Instruction:
#   syscall - invoke system call
#
# Example system calls:
#   0:   read(fd, buf, count)
#   1:   write(fd, buf, count)
#   60:  exit(status)
#   231: exit_group(status)  - exit all threads

# THE exit() SYSTEM CALL:
#
# System call number: 60 (x86-64 Linux)
# Prototype: void exit(int status)
# Arguments:
#   rdi: exit status (0-255, though any int is accepted)
#
# What it does:
# 1. Closes all file descriptors
# 2. Releases memory
# 3. Terminates the process
# 4. Returns exit status to parent process
#
# This is the KERNEL's exit(), not libc's exit()
# libc's exit() does additional cleanup:
# - Calls atexit() handlers
# - Flushes stdio buffers
# - Calls destructors
# Then calls this system call

# EXIT CODES:
#
# Exit codes are 8-bit values: 0-255
# Negative values wrap around (two's complement)
#   -1 becomes 255
#   -2 becomes 254
#   etc.
#
# Common conventions:
#   0:     success
#   1:     general error
#   2:     misuse of shell command
#   126:   command cannot execute
#   127:   command not found
#   128+N: terminated by signal N
#   255:   exit status out of range
#
# In this program:
#   exit(-1) → exit status 255 (visible in shell)

# _start vs main:
#
# _start:
# - True entry point of program
# - Called directly by OS kernel
# - No setup or cleanup
# - Must call exit() explicitly to terminate
# - Used when not linking with C runtime
#
# main:
# - Entry point for C programs
# - Called by C runtime (_start calls __libc_start_main, which calls main)
# - C runtime provides setup (argc/argv parsing, environment, etc.)
# - C runtime provides cleanup (atexit handlers, buffer flushing)
# - Can return instead of calling exit
# - Requires linking with C standard library

# AT&T SYNTAX vs INTEL SYNTAX:
#
# This file uses AT&T syntax (GNU assembler default)
#
# AT&T syntax:
#   mov $60, %rax       - $ for immediates, % for registers
#   mov %rbx, %rax      - source first, destination second
#
# Intel syntax:
#   mov rax, 60         - no prefixes
#   mov rax, rbx        - destination first, source second
#
# To assemble with Intel syntax:
#   .intel_syntax noprefix
#   mov rax, 60
#   mov rdi, -1
#   syscall

# MINIMAL PROGRAM DEMONSTRATION:
#
# This is about as minimal as a program can be:
# - 3 instructions (mov, mov, syscall)
# - No data segment
# - No function calls
# - No standard library
# - No C runtime
# - Direct system call to kernel
#
# Resulting binary is very small:
#   Text section: ~10 bytes
#   Total file: ~8 KB (mostly ELF headers and metadata)

# WHAT'S MISSING:
#
# Compared to a C program, this lacks:
# - No stdio (printf, puts, etc.)
# - No heap (malloc, free)
# - No atexit handlers
# - No signal handlers
# - No buffered I/O
# - No constructors/destructors
# - No environment variable access (without parsing stack)
# - No command-line argument parsing (without parsing stack)

# EXPANDING THIS PROGRAM:
#
# To make this program do more:
#
# 1. Print "Hello World":
#    .section .data
#    msg: .string "Hello World\n"
#    .text
#    mov $1, %rax        # syscall: write
#    mov $1, %rdi        # fd: stdout
#    lea msg(%rip), %rsi # buf: message
#    mov $12, %rdx       # count: 12 bytes
#    syscall
#    mov $60, %rax       # syscall: exit
#    mov $0, %rdi        # status: 0
#    syscall
#
# 2. Exit with code 0 (success):
#    mov $60, %rax
#    mov $0, %rdi        # Change -1 to 0
#    syscall

# DEBUGGING AND TESTING:
#
# To see system calls made:
# $ strace ./ret1
# execve("./ret1", ["./ret1"], ...) = 0
# exit(-1)                              = ?
# +++ exited with 255 +++
#
# To check exit code:
# $ ./ret1
# $ echo $?
# 255
#
# The shell variable $? holds the exit status of last command

# COMPARISON TO C EQUIVALENT:
#
# C code:
#   int main() {
#     return -1;
#   }
#
# Or:
#   #include <stdlib.h>
#   int main() {
#     exit(-1);
#   }
#
# This assembly version does the same thing but:
# - No C runtime initialization
# - No libc linking
# - Direct system call
# - Uses _start instead of main

# TO ASSEMBLE AND LINK:
# as ret1.s -o ret1.o              # Assemble to object file
# ld ret1.o -o ret1                # Link (no C library needed!)
# ./ret1                           # Run
# echo $?                          # Check exit code: 255
#
# Alternative (using gcc to link, but still no C runtime):
# as ret1.s -o ret1.o
# gcc -nostdlib ret1.o -o ret1     # -nostdlib: don't link C runtime
# ./ret1
# echo $?
#
# If you try to link with C runtime, you'd need to rename _start to main
# and the C runtime would provide _start that calls main

# REGISTERS AT PROGRAM START:
#
# When OS calls _start, registers contain:
#   rsp: points to stack with argc, argv, envp
#   Other registers: undefined (don't rely on initial values)
#
# Stack layout at _start:
#   [argc]           ← rsp (number of command-line arguments)
#   [argv[0]]        ← rsp+8 (program name)
#   [argv[1]]        ← rsp+16 (first argument)
#   ...
#   [NULL]           (end of argv)
#   [envp[0]]        (first environment variable)
#   ...
#   [NULL]           (end of envp)
#   [auxiliary vector entries]
#
# We ignore all of this and just exit immediately

# SYSTEM CALL REFERENCE:
#
# Common x86-64 Linux system calls:
#   0   read(fd, buf, count)
#   1   write(fd, buf, count)
#   2   open(filename, flags, mode)
#   3   close(fd)
#   9   mmap(addr, len, prot, flags, fd, offset)
#   60  exit(status)
#   231 exit_group(status)
#
# Full list: /usr/include/asm/unistd_64.h
# Or: man syscalls

# WHY THIS MATTERS:
#
# Understanding this program teaches:
# 1. Programs don't need C or libc - they can talk directly to the kernel
# 2. Everything else (printf, malloc, etc.) is built on system calls
# 3. The C runtime does a lot of hidden work before calling main
# 4. Assembly gives you complete control at the lowest level
# 5. System calls are the boundary between user space and kernel space
