# ============================================================================
# Hello World - Guided Version
# ============================================================================
# This program demonstrates the fundamental structure of an assembly program:
# - Data section for constants
# - Text section for code
# - System calls for I/O and program termination
#
# WHAT IT DOES: Prints "Hello, World!\n" to stdout and exits with status 0
# ============================================================================

# DATA SECTION: Where we store constants and variables
.section .data

hello:
    # .asciz creates a null-terminated string (automatically adds \0)
    # Useful for C-style strings
    .asciz "Hello, World!\n"

hello2:
    # .byte creates raw bytes without null terminator
    # We'll use this one for the write syscall
    .byte 'H','e','l','l','o', ',' ,' ','W','o','r','l','d','!','\n'

hellolen = . - hello2
    # Calculate string length: current location (.) minus start (hello2)
    # This gives us 14 bytes (the length of "Hello, World!\n")
    # The assembler computes this at assembly time

# CODE SECTION: Where executable instructions live
.section .text

# Make _start visible to the linker (required for ld)
.global _start

_start:
    # ========================================================================
    # SYSTEM CALL 1: write(fd=1, buf=hello2, count=14)
    # Prints our message to standard output
    # ========================================================================

    mov $1, %rax            # syscall number: 1 = write
    mov $1, %rdi            # arg1: file descriptor (1 = stdout)
    lea hello2(%rip), %rsi  # arg2: address of string (RIP-relative)
    mov $hellolen, %rdx     # arg3: number of bytes to write
    syscall                 # invoke the kernel

    # ========================================================================
    # SYSTEM CALL 2: exit(status=0)
    # Cleanly terminates the program with success code
    # ========================================================================

    mov $60, %rax           # syscall number: 60 = exit
    xor %rdi, %rdi          # arg1: exit status (0 = success)
                            # xor reg,reg is a fast way to set to 0
    syscall                 # invoke the kernel (program ends here)

# ============================================================================
# KEY CONCEPTS:
#
# 1. SECTIONS:
#    - .data = initialized data (read/write)
#    - .text = code (read/execute only)
#
# 2. SYSCALLS: Requests to the operating system
#    - Number goes in %rax
#    - Arguments go in %rdi, %rsi, %rdx, %rcx, %r8, %r9 (in that order)
#    - Use 'syscall' instruction to invoke
#
# 3. REGISTERS USED:
#    - %rax: syscall number
#    - %rdi: first argument
#    - %rsi: second argument
#    - %rdx: third argument
#    - %rip: instruction pointer (for position-independent code)
#
# 4. AT&T SYNTAX:
#    - $ prefix = immediate value (literal number)
#    - % prefix = register
#    - Source comes before destination: mov source, dest
#
# ============================================================================
# TO BUILD AND RUN:
#
# as 01-hello-worldGuided.s -o hello.o    # Assemble to object file
# ld hello.o -o hello                      # Link to executable
# ./hello                                  # Run it!
#
# Expected output:
# Hello, World!
#
# Check exit status:
# echo $?                                  # Should print: 0
# ============================================================================
