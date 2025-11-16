# PEDAGOGICAL PURPOSE:
# This program demonstrates both read() and write() system calls in assembly,
# creating a simple echo program that reads input and writes it back.
# Key learning objectives:
# 1. Understanding the read() system call (syscall 0)
# 2. Using the .bss section for uninitialized data
# 3. Chaining multiple system calls in sequence
# 4. Using return values from one syscall as input to another
# 5. The difference between .data (initialized) and .bss (uninitialized)
# 6. Building interactive programs in assembly

.section .bss                # BSS SECTION: Uninitialized data
                            # BSS = "Block Started by Symbol"
                            # This section reserves space but doesn't initialize it
                            # Why use .bss instead of .data?
                            # - Saves space in the executable file
                            # - .data would store 100 zeros in the file
                            # - .bss just records "reserve 100 bytes" (metadata)
                            # - OS zeros the memory when program loads

buff:   .skip 100           # RESERVE BUFFER: Allocate 100 uninitialized bytes
                            # .skip N reserves N bytes without setting values
                            # Also called .space in some assemblers
                            # 'buff' is a label pointing to first byte
                            # Memory contains garbage until we write to it
                            # WHY 100? Arbitrary choice - enough for typical input
                            #
                            # EQUIVALENT DIRECTIVES:
                            # .skip 100  (AT&T syntax)
                            # .space 100 (alternative)
                            # .bss buff,100  (some assemblers)

        .section .text      # CODE SECTION: Executable instructions
        .global _start      # Make _start visible to linker

_start:                     # PROGRAM ENTRY POINT

        # SYSTEM CALL 1: read() - Get input from user
        # ssize_t read(int fd, void *buf, size_t count)
        # Returns: number of bytes read, or -1 on error, or 0 on EOF

        mov $0,%rdi         # ARGUMENT 1: File descriptor = 0 (stdin)
                            # fd 0 = standard input (keyboard)
                            # This is where user input comes from
                            # Could be redirected: ./program < input.txt

        lea buff(%rip),%rsi # ARGUMENT 2: Buffer address
                            # lea loads the ADDRESS of buff
                            # RIP-relative addressing: buff(%rip)
                            # %rsi now points to our 100-byte buffer
                            # read() will store input here

        mov $100,%rdx       # ARGUMENT 3: Maximum bytes to read
                            # Read up to 100 bytes
                            # WHY 100? That's our buffer size
                            # IMPORTANT: Never read more than buffer size!
                            # That would cause buffer overflow
                            #
                            # If user types less, read() returns early
                            # If user types more, only first 100 bytes read

        mov $0,%rax         # SYSCALL NUMBER: 0 = read
                            # System call 0 is the read syscall

        syscall             # INVOKE READ: Read from stdin into buff
                            # WHAT HAPPENS:
                            # 1. Program blocks (waits) for input
                            # 2. User types text and presses Enter
                            # 3. Kernel copies input to buff
                            # 4. Kernel returns byte count in %rax
                            # 5. Program continues
                            #
                            # AFTER SYSCALL:
                            # %rax = number of bytes read
                            # buff contains the input data
                            # Other registers may be clobbered
                            #
                            # EXAMPLE:
                            # User types: "Hello\n"
                            # buff contains: 'H','e','l','l','o','\n',<garbage>
                            # %rax = 6

        ## we've read in bytes
        ## now %rax has inside it the number of bytes read

        # IMPORTANT: Save the return value!
        # %rax contains the byte count from read()
        # We need this for write(), but we're about to overwrite %rax
        # SOLUTION: Move it to %rdx (write's count parameter)
        # This is elegant - we'll use %rdx directly in write() call

        # SYSTEM CALL 2: write() - Echo input back to user
        # ssize_t write(int fd, const void *buf, size_t count)
        # Returns: number of bytes written, or -1 on error

        mov $1,%rdi         # ARGUMENT 1: File descriptor = 1 (stdout)
                            # fd 1 = standard output (terminal)
                            # We're writing back to the user

        lea buff(%rip),%rsi # ARGUMENT 2: Buffer address
                            # Same buffer we just read into
                            # %rsi points to our data
                            # WHY reload? The syscall might have clobbered %rsi
                            # (Though in practice, Linux preserves it)

        mov %rax,%rdx       # ARGUMENT 3: Byte count from read()
                            # THIS IS KEY: Use read's return value!
                            # %rax contains number of bytes read
                            # We write exactly that many bytes
                            # WHY? To avoid writing garbage beyond input
                            #
                            # EXAMPLE FLOW:
                            # User types 6 bytes → read() returns 6 in %rax
                            # We copy 6 to %rdx → write() outputs 6 bytes
                            # Result: user sees exactly their input, nothing more

        mov $1,%rax         # SYSCALL NUMBER: 1 = write
                            # System call 1 is the write syscall

        syscall             # INVOKE WRITE: Write buff contents to stdout
                            # WHAT HAPPENS:
                            # 1. Kernel reads %rdx bytes from buff
                            # 2. Kernel writes them to stdout
                            # 3. Terminal displays the data
                            # 4. Kernel returns bytes written in %rax
                            #
                            # RESULT: User sees their input echoed back

        # SYSTEM CALL 3: exit() - Terminate cleanly
        # void exit(int status)

        mov $60,%rax        # SYSCALL NUMBER: 60 = exit
                            # Terminate the program

        mov $0,%rdi         # ARGUMENT 1: Exit status = 0 (success)
                            # 0 means no errors
                            # Shell can check with: echo $?

        syscall             # EXIT: End the program
                            # Kernel terminates the process
                            # Memory is freed, file descriptors closed
                            # Control returns to shell

# PROGRAM FLOW EXAMPLE:
#
# User runs: ./readWriteASM2
# Program executes:
#
# 1. mov $0,%rdi          → %rdi = 0
# 2. lea buff(%rip),%rsi  → %rsi = address of buff
# 3. mov $100,%rdx        → %rdx = 100
# 4. mov $0,%rax          → %rax = 0 (read syscall)
# 5. syscall              → [BLOCKS, waiting for input]
#
# User types: "Testing\n" and presses Enter
#
# 6. syscall returns      → %rax = 8 (bytes read)
#                          → buff = "Testing\n" + garbage
# 7. mov $1,%rdi          → %rdi = 1
# 8. lea buff(%rip),%rsi  → %rsi = address of buff
# 9. mov %rax,%rdx        → %rdx = 8 (from read)
# 10. mov $1,%rax         → %rax = 1 (write syscall)
# 11. syscall             → Outputs "Testing\n" to terminal
#                          → %rax = 8 (bytes written)
# 12. mov $60,%rax        → %rax = 60 (exit syscall)
# 13. mov $0,%rdi         → %rdi = 0 (success)
# 14. syscall             → Program terminates

# SECTIONS EXPLAINED:
#
# .text (code):
# - Contains executable instructions
# - Loaded as read-only and executable
# - Shared between processes running same program
# - Lives at low addresses (typically 0x400000+)
#
# .data (initialized data):
# - Contains variables with initial values
# - Loaded as read-write, not executable
# - Each process gets its own copy
# - Example: msg: .ascii "Hello\n"
#
# .bss (uninitialized data):
# - Contains variables without initial values
# - NOT stored in executable file (saves space)
# - Kernel zeros this memory when loading program
# - Each process gets its own copy
# - Example: buff: .skip 100
#
# .rodata (read-only data):
# - Constants that shouldn't be modified
# - Loaded as read-only, not executable
# - Can be shared between processes
# - Example: in C, string literals go here

# BSS VS DATA SIZE COMPARISON:
#
# Using .data (what we DON'T do):
#   .section .data
#   buff: .byte 0,0,0,...,0  # 100 zeros
#   → Adds 100 bytes to executable file
#
# Using .bss (what we DO):
#   .section .bss
#   buff: .skip 100
#   → Adds ~0 bytes to executable (just metadata)
#   → OS allocates and zeros the space at runtime
#
# For large buffers, .bss is much more efficient!

# READ SYSCALL RETURN VALUES:
#
# Successful read:
#   Returns: number of bytes read (> 0)
#   Example: User types "Hi\n" → returns 3
#
# End of file (EOF):
#   Returns: 0
#   Example: User presses Ctrl+D → returns 0
#   Example: ./program < file.txt, at end → returns 0
#
# Error:
#   Returns: -1 (0xFFFFFFFFFFFFFFFF in 64-bit)
#   Sets errno to error code
#   Example: Invalid file descriptor → returns -1
#
# This program doesn't check for errors!
# Robust version would check if %rax < 0 after read

# COMPARISON WITH C VERSION:
#
# C code (readWrite2.c):
#   char buff[30];
#   int charsRead = read(0, buff, 30);
#   write(1, buff, charsRead);
#   return 0;
#
# Assembly (this file):
#   buff: .skip 100
#   mov $0,%rdi
#   lea buff(%rip),%rsi
#   mov $100,%rdx
#   mov $0,%rax
#   syscall
#   mov %rax,%rdx
#   mov $1,%rdi
#   lea buff(%rip),%rsi
#   mov $1,%rax
#   syscall
#   mov $60,%rax
#   mov $0,%rdi
#   syscall
#
# Main differences:
# - Assembly uses .bss section explicitly
# - Assembly makes syscalls directly
# - Assembly uses larger buffer (100 vs 30)
# - C handles exit automatically

# INTERACTIVE USAGE:
#
# $ ./readWriteASM2
# Hello World         ← user types this
# Hello World         ← program echoes
# $
#
# With redirection:
# $ echo "Test" | ./readWriteASM2
# Test
# $
# $ ./readWriteASM2 < input.txt > output.txt
# (reads from input.txt, writes to output.txt)

# EDGE CASES:
#
# Empty input (just Enter):
#   User presses Enter → read() gets "\n" (1 byte)
#   write() outputs "\n" → blank line appears
#
# EOF (Ctrl+D):
#   read() returns 0 → %rax = 0
#   write() tries to write 0 bytes → does nothing
#   Program exits normally
#
# Input > 100 bytes:
#   read() returns 100 → buffer is full
#   Remaining input stays in kernel buffer
#   Next read() would get it
#   (But this program only reads once)

# TO ASSEMBLE, LINK, AND RUN:
# as readWriteASM2.s -o readWriteASM2.o
# ld readWriteASM2.o -o readWriteASM2
# ./readWriteASM2
#
# Try it:
# $ ./readWriteASM2
# Testing 123         ← type this
# Testing 123         ← see echo
# ^D                  ← Ctrl+D to exit

# SYSCALL CONVENTION REFERENCE:
#
# x86-64 Linux system calls:
# - Syscall number in %rax
# - Arguments in %rdi, %rsi, %rdx, %r10, %r8, %r9
# - Return value in %rax
# - Syscalls may clobber: %rcx, %r11
# - Syscalls preserve: %rbx, %rbp, %r12-%r15
#
# Our syscalls:
# read(0, buff, 100):
#   %rax=0, %rdi=0, %rsi=buff, %rdx=100
#   Returns byte count in %rax
#
# write(1, buff, count):
#   %rax=1, %rdi=1, %rsi=buff, %rdx=count
#   Returns byte count in %rax
#
# exit(0):
#   %rax=60, %rdi=0
#   Does not return
