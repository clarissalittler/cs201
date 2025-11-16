# PEDAGOGICAL PURPOSE:
# This program demonstrates the write() system call in pure assembly.
# It shows how to make system calls directly without the C library.
# Key learning objectives:
# 1. Understanding the write() system call at the lowest level
# 2. System call numbers (1 = write, 60 = exit)
# 3. The x86-64 system call calling convention
# 4. Using .ascii directive for string data
# 5. Difference between .data and .bss sections
# 6. How C's write() function translates to assembly

.section .data               # DATA SECTION: Initialized global variables
                            # This section contains data with initial values
                            # It's loaded into memory when the program starts

hiThere: .ascii "Hey there, goofuses and goofuxen\n"
                            # STRING DATA: 33 bytes of text
                            # .ascii stores the string WITHOUT null terminator
                            # (unlike .asciz which adds \0)
                            #
                            # MEMORY LAYOUT:
                            # hiThere+0:  'H'
                            # hiThere+1:  'e'
                            # hiThere+2:  'y'
                            # ...
                            # hiThere+32: 'n'
                            # hiThere+33: '\n' (newline)
                            #
                            # Total: 33 bytes
                            # WHY .ascii? Because write() uses explicit byte count
                            # We don't need the null terminator

        .section .text      # CODE SECTION: Executable instructions
        .global _start      # Make _start visible to linker

        ## writing is a system call of 1
        ## calling convention:
        ## First argument goes into rdi
        ## Put the FD into %rdi
        ## next argument goes into rsi
        ## address of the buffer goes in %rsi
        ## next argument after that goes into rdx
        ## length goes in %rdx

        # SYSTEM CALL CONVENTION FOR x86-64 LINUX:
        # %rax = system call number
        # %rdi = 1st argument
        # %rsi = 2nd argument
        # %rdx = 3rd argument
        # %r10 = 4th argument (not used here)
        # %r8  = 5th argument (not used here)
        # %r9  = 6th argument (not used here)
        #
        # WRITE SYSCALL SPECIFICALLY:
        # syscall number: 1
        # ssize_t write(int fd, const void *buf, size_t count)
        # %rdi = fd (file descriptor)
        # %rsi = buf (buffer address)
        # %rdx = count (number of bytes)
        # Returns: number of bytes written in %rax

_start:                     # PROGRAM ENTRY POINT

        mov $1,%rdi         # ARGUMENT 1: File descriptor = 1
                            # File descriptor 1 = stdout (standard output)
                            # 0 = stdin, 1 = stdout, 2 = stderr
                            # We're writing to the terminal

        lea hiThere(%rip),%rsi  # ARGUMENT 2: Buffer address
                                # lea = Load Effective Address
                                # Computes address of hiThere string
                                # hiThere(%rip) = RIP-relative addressing
                                # %rsi now points to first character 'H'
                                #
                                # WHY lea instead of mov?
                                # - lea gets the ADDRESS (pointer)
                                # - mov would get the VALUE (first 8 bytes of string)
                                # - write() needs the address to know where to read from

        mov $33,%rdx        # ARGUMENT 3: Byte count = 33
                            # We want to write 33 bytes
                            # This is the length of our string including \n
                            # WHY 33? Count the characters in the string!

        mov $1,%rax         # SYSCALL NUMBER: 1 = write
                            # This tells the kernel which system call to execute
                            # Each syscall has a unique number
                            # 1 = write, 0 = read, 60 = exit, etc.

        syscall             # INVOKE KERNEL: Execute the system call
                            # WHAT HAPPENS:
                            # 1. CPU switches from user mode to kernel mode
                            # 2. Kernel looks at %rax to determine syscall (write)
                            # 3. Kernel reads arguments from %rdi, %rsi, %rdx
                            # 4. Kernel performs the write operation
                            # 5. Kernel writes 33 bytes from hiThere to stdout
                            # 6. Kernel returns number of bytes written in %rax
                            # 7. CPU switches back to user mode
                            # 8. Execution continues at next instruction
                            #
                            # RESULT: "Hey there, goofuses and goofuxen\n" appears on terminal

        # EXIT SEQUENCE:
        # Now we need to exit cleanly, otherwise program will crash
        # (would continue executing whatever bytes come after our code!)

        mov $60,%rax        # SYSCALL NUMBER: 60 = exit
                            # The exit syscall terminates the program
                            # void exit(int status)
                            # Takes one argument: exit status code

        mov $0,%rdi         # ARGUMENT 1: Exit status = 0
                            # 0 indicates success
                            # Non-zero values indicate errors
                            # Shell can check this with: echo $?

        syscall             # EXIT: Terminate the program
                            # Kernel ends the process
                            # Control returns to shell
                            # Program is unloaded from memory

# SYSCALL NUMBERS REFERENCE:
#
# Common Linux x86-64 system calls:
# 0  = read      - read from file descriptor
# 1  = write     - write to file descriptor
# 2  = open      - open file
# 3  = close     - close file descriptor
# 60 = exit      - terminate process
# 39 = getpid    - get process ID
# 57 = fork      - create child process
# 59 = execve    - execute program
#
# Full list: man 2 syscalls
# Or see: /usr/include/asm/unistd_64.h

# CALLING CONVENTION SUMMARY:
#
# Function calls (using 'call' instruction):
# Arguments in: %rdi, %rsi, %rdx, %rcx, %r8, %r9
# Return value: %rax
#
# System calls (using 'syscall' instruction):
# Syscall number: %rax
# Arguments in: %rdi, %rsi, %rdx, %r10, %r8, %r9
# Return value: %rax
#
# Note: 4th arg differs - %rcx for functions, %r10 for syscalls
# WHY? Because syscall instruction uses %rcx internally

# COMPARISON WITH C VERSION:
#
# C code (readWrite.c):
#   char hiThere[] = "Hey there, goofuses and goofuxen\n";
#   write(1, hiThere, 33);
#   return 0;
#
# Assembly (this file):
#   hiThere: .ascii "Hey there, goofuses and goofuxen\n"
#   mov $1,%rdi
#   lea hiThere(%rip),%rsi
#   mov $33,%rdx
#   mov $1,%rax
#   syscall
#   mov $60,%rax
#   mov $0,%rdi
#   syscall
#
# The C library's write() function is just a thin wrapper:
# - It sets up the registers
# - Makes the syscall
# - Returns the result
# We're doing the same thing directly!

# WHY USE ASSEMBLY FOR SYSCALLS?
#
# Normally you'd use C library functions (write, printf, etc.)
# Direct syscalls are useful for:
# 1. Learning - understanding how it really works
# 2. Minimal programs - smallest possible executables
# 3. Special situations - avoiding library dependencies
# 4. Security - reducing attack surface
# 5. Embedded systems - no C library available

# DATA SECTION DIRECTIVES:
#
# .ascii "text"    - String without null terminator
# .asciz "text"    - String WITH null terminator (\0)
# .string "text"   - Same as .asciz
# .byte 1,2,3      - Individual bytes
# .word 1,2,3      - 16-bit values (2 bytes each)
# .long 1,2,3      - 32-bit values (4 bytes each)
# .quad 1,2,3      - 64-bit values (8 bytes each)

# REGISTER STATE AFTER EACH INSTRUCTION:
#
# After mov $1,%rdi:
#   %rdi = 1, %rsi = ?, %rdx = ?, %rax = ?
#
# After lea hiThere(%rip),%rsi:
#   %rdi = 1, %rsi = address_of_hiThere, %rdx = ?, %rax = ?
#
# After mov $33,%rdx:
#   %rdi = 1, %rsi = address_of_hiThere, %rdx = 33, %rax = ?
#
# After mov $1,%rax:
#   %rdi = 1, %rsi = address_of_hiThere, %rdx = 33, %rax = 1
#
# After syscall:
#   %rax = 33 (number of bytes written), others may be clobbered
#
# After mov $60,%rax:
#   %rax = 60, %rdi = 1 (still), others unchanged
#
# After mov $0,%rdi:
#   %rax = 60, %rdi = 0, others unchanged
#
# After syscall:
#   (program terminates, no return)

# TO ASSEMBLE, LINK, AND RUN:
# as readWriteASM1.s -o readWriteASM1.o
# ld readWriteASM1.o -o readWriteASM1
# ./readWriteASM1
# Output: Hey there, goofuses and goofuxen

# FILE SIZE COMPARISON:
# This assembly program is MUCH smaller than C equivalent:
# - readWriteASM1: ~1-2 KB (just code + data)
# - readWrite (C): ~16 KB (includes C runtime, dynamic linking)
# - With static linking, C version would be 100s of KB!
