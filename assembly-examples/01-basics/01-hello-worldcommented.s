# PEDAGOGICAL PURPOSE:
# This program demonstrates a complete "Hello, World!" program in x86-64 assembly,
# introducing fundamental concepts of assembly programming including data sections,
# system calls, and different ways to define string data.
#
# Key learning objectives:
# 1. Understanding the .data section vs .text section
# 2. Defining string data in two different ways (.asciz and .byte)
# 3. Computing string length using the location counter (.)
# 4. Using the write system call to output text
# 5. Properly exiting a program with the exit system call
# 6. Position-independent code with RIP-relative addressing
# 7. Understanding file descriptors (stdout = 1)
# 8. The complete structure of a real-world assembly program

.section .data                  # DATA SECTION: Contains initialized data
                                # .data holds variables and constants
                                # Unlike .text (code), .data is readable and writable
                                # Data here persists for program lifetime
                                # Loaded into memory when program starts

hello:                          # LABEL: Marks the location of our first string
                                # Labels give symbolic names to memory addresses
                                # hello points to the start of the string
                                # We can reference this later with hello(%rip)

    .asciz "Hello, World!\n"    # STRING DIRECTIVE: Null-terminated ASCII string
                                # .asciz = ASCII Zero-terminated
                                # Stores: 'H' 'e' 'l' 'l' 'o' ',' ' ' 'W' 'o' 'r' 'l' 'd' '!' '\n' '\0'
                                # Total: 15 bytes (14 visible + 1 null terminator)
                                # \n = newline character (0x0A)
                                # The 'z' in .asciz means zero-terminated (adds \0)
                                #
                                # WHY null-terminated?
                                # C-style strings end with \0 so functions know where string ends
                                # This is the standard for C library functions

hello2: .byte 'H','e','l','l','o', ',' ,' ','W','o','r','l','d','!','\n'
                                # ALTERNATIVE STRING DEFINITION: Using .byte directive
                                # .byte defines individual bytes of data
                                # Each character is explicitly listed
                                # Notice: NO null terminator (we don't need one for syscall write)
                                # Stores 14 bytes: same visible characters as hello
                                #
                                # WHY use .byte instead of .asciz?
                                # - More control over exact bytes
                                # - No automatic null terminator (when you don't want it)
                                # - Can mix characters with raw byte values
                                # - Educational: shows what .asciz does behind the scenes

hellolen = . - hello2           # COMPUTE STRING LENGTH: Calculate bytes between labels
                                # . = location counter (current address during assembly)
                                # hello2 = address where hello2 starts
                                # . - hello2 = current address - start address = length
                                # hellolen = 14 (the length of our string)
                                # = assigns a constant value (not a label)
                                #
                                # WHY compute length this way?
                                # - Assembler calculates it automatically
                                # - No manual counting (error-prone)
                                # - Updates automatically if we change the string
                                # - This is a constant, not a memory location

.section .text                  # CODE SECTION: Contains executable instructions
                                # .text is read-only and executable
                                # CPU executes instructions from here
                                # Cannot be modified at runtime (security)

.global _start                  # GLOBAL DIRECTIVE: Export _start symbol
                                # Makes _start visible to the linker
                                # _start is the entry point for programs linked with ld
                                # (Programs linked with gcc use main as entry point)
                                # Without this, linker won't find where to start

_start:                         # ENTRY POINT: Where program execution begins
                                # OS jumps here when program starts
                                # Like main() in C, but lower-level

    # SYSTEM CALL #1: write(1, hello2, hellolen)
    # Print "Hello, World!\n" to standard output
    #
    # The write syscall has this signature:
    #   ssize_t write(int fd, const void *buf, size_t count)
    # Syscall number: 1
    # Arguments:
    #   %rdi (arg1): file descriptor (1 = stdout)
    #   %rsi (arg2): pointer to buffer (address of string)
    #   %rdx (arg3): number of bytes to write

    mov $1, %rax        	# Load syscall number 1 into %rax
                                # AT&T syntax: mov source, destination
                                # $ means immediate value (literal number 1)
                                # %rax holds the system call number
                                # 1 = write syscall (output to file/terminal)
                                #
                                # REGISTER PURPOSE IN SYSCALLS:
                                # %rax: syscall number
                                # %rdi: 1st argument
                                # %rsi: 2nd argument
                                # %rdx: 3rd argument
                                # %r10: 4th argument
                                # %r8:  5th argument
                                # %r9:  6th argument

    mov $1, %rdi        	# First argument: file descriptor 1
                                # %rdi = first argument to syscall
                                # File descriptors:
                                #   0 = stdin  (standard input - keyboard)
                                #   1 = stdout (standard output - terminal)
                                #   2 = stderr (standard error - terminal)
                                # We want to write to stdout, so use 1
                                #
                                # WHY file descriptors?
                                # Unix philosophy: "everything is a file"
                                # Files, terminals, pipes, sockets all use same interface

    lea hello2(%rip), %rsi      # Second argument: address of string to write
                                # lea = Load Effective Address
                                # Loads the ADDRESS of hello2, not its contents
                                # hello2(%rip) = RIP-relative addressing
                                # %rip = instruction pointer (current code location)
                                # hello2(%rip) = offset from current instruction to hello2
                                #
                                # POSITION-INDEPENDENT CODE (PIC):
                                # RIP-relative addressing makes code relocatable
                                # Works regardless of where OS loads program in memory
                                # Enables ASLR (Address Space Layout Randomization)
                                # Security feature that randomizes memory addresses
                                #
                                # After this: %rsi points to 'H' of "Hello, World!\n"

    mov $hellolen, %rdx         # Third argument: number of bytes to write
                                # %rdx = third argument to syscall
                                # hellolen = 14 (computed earlier as . - hello2)
                                # We'll write exactly 14 bytes
                                #
                                # WHY specify length?
                                # - write() needs to know how many bytes to output
                                # - Unlike C strings, no null terminator to mark end
                                # - Allows writing binary data (which may contain \0)

    syscall                     # INVOKE SYSTEM CALL: Transfer control to kernel
                                # Triggers a context switch to kernel mode
                                # Kernel reads %rax (1) and knows this is write()
                                # Kernel executes: write(1, hello2, 14)
                                # Writes "Hello, World!\n" to stdout (terminal)
                                # Returns number of bytes written in %rax
                                # Control returns to user mode at next instruction
                                #
                                # WHAT HAPPENS DURING syscall:
                                # 1. Save current state
                                # 2. Switch to kernel mode (privileged)
                                # 3. Kernel validates arguments
                                # 4. Kernel performs the write operation
                                # 5. Kernel returns result in %rax
                                # 6. Switch back to user mode
                                # 7. Continue at next instruction

    # SYSTEM CALL #2: exit(0)
    # Exit the program cleanly with status code 0
    #
    # The exit syscall has this signature:
    #   void exit(int status)
    # Syscall number: 60
    # Arguments:
    #   %rdi (arg1): exit status code (0 = success)

    mov $60, %rax               # Load syscall number 60 into %rax
                                # 60 = exit syscall (terminate program)
                                # This is how we tell the OS we're done

    xor %rdi, %rdi              # Set first argument to 0 (exit status)
                                # xor %rdi, %rdi is a clever way to set %rdi = 0
                                # XOR of any value with itself = 0
                                # %rdi %rdi = 0
                                # Exit status 0 means "success, no errors"
                                #
                                # WHY use xor instead of mov $0, %rdi?
                                # - Shorter instruction (saves bytes)
                                # - Faster execution (xor is very fast)
                                # - Common idiom in assembly programming
                                # Both do the same thing: set register to 0

    syscall                     # INVOKE EXIT SYSCALL: Terminate program
                                # Kernel reads %rax (60) and knows this is exit()
                                # Kernel terminates the process
                                # Returns 0 to parent process (usually the shell)
                                # Program ends here - never returns
                                # OS reclaims all memory and resources
                                #
                                # WHY explicit exit?
                                # - Clean program termination
                                # - Return status code to shell
                                # - Close all open files
                                # - Release system resources

# COMPREHENSIVE SUMMARY:
#
# WHAT THIS PROGRAM DOES:
# 1. Defines "Hello, World!\n" in two different ways (pedagogical)
# 2. Computes the string length automatically
# 3. Uses write syscall to print the string to terminal
# 4. Uses exit syscall to terminate with status 0 (success)
#
# DATA STRUCTURES:
# - hello: null-terminated string (15 bytes including \0)
# - hello2: raw byte string (14 bytes, no \0)
# - hellolen: constant = 14 (length of hello2)
#
# SYSTEM CALLS USED:
# 1. write(1, hello2, 14) - outputs "Hello, World!\n"
# 2. exit(0) - terminates program successfully
#
# REGISTER USAGE:
# %rax: syscall number (1 for write, 60 for exit)
# %rdi: 1st syscall argument (fd=1, then status=0)
# %rsi: 2nd syscall argument (buffer address)
# %rdx: 3rd syscall argument (byte count)
# %rip: instruction pointer (for position-independent addressing)

# MEMORY LAYOUT:
#
# .data section (at some address, e.g., 0x404000):
#   hello:  'H' 'e' 'l' 'l' 'o' ',' ' ' 'W' 'o' 'r' 'l' 'd' '!' '\n' '\0'
#   hello2: 'H' 'e' 'l' 'l' 'o' ',' ' ' 'W' 'o' 'r' 'l' 'd' '!' '\n'
#
# .text section (at some address, e.g., 0x401000):
#   [executable instructions]

# COMPARISON: .asciz vs .byte
#
# .asciz "Hello, World!\n"
#   - Convenience directive
#   - Automatically adds null terminator
#   - Uses escape sequences (\n)
#   - 15 bytes total
#   - Perfect for C-style strings
#
# .byte 'H','e','l','l','o',...
#   - Manual byte specification
#   - No automatic null terminator
#   - Explicit control over each byte
#   - 14 bytes total (no \0)
#   - Good for raw data or when you don't need \0

# WHY TWO STRING DEFINITIONS?
# Educational purposes! The program uses hello2 for write(),
# but includes hello to show both methods.
# In real code, you'd only define the string once.

# TO ASSEMBLE, LINK, AND RUN:
as 01-hello-world.s -o 01-hello-world.o    # Assemble source to object file
ld 01-hello-world.o -o 01-hello-world      # Link object to executable
./01-hello-world                            # Run the program
echo $?                                     # Check exit status (should be 0)

# EXPECTED OUTPUT:
# Hello, World!
#
# Exit status: 0 (success)

# WHAT IF WE USED hello INSTEAD OF hello2?
# Change: lea hello(%rip), %rsi
# And:    mov $15, %rdx          # 15 bytes including \n and \0
# Would print: Hello, World!\n and then a null byte
# The null wouldn't be visible, but it would be sent to terminal
# Usually we use the .byte version for write() to avoid sending \0

# DEBUGGING TIPS:
# 1. Use strace to see system calls:
#    strace ./01-hello-world
#    Shows: write(1, "Hello, World!\n", 14) = 14
#           exit(0)
#
# 2. Use gdb to step through:
#    gdb ./01-hello-world
#    (gdb) break _start
#    (gdb) run
#    (gdb) stepi          # Step one instruction
#    (gdb) info registers # See register values
#
# 3. Check return code:
#    ./01-hello-world
#    echo $?              # Should print 0

# COMMON MISTAKES TO AVOID:
# 1. Forgetting .global _start (linker won't find entry point)
# 2. Wrong syscall numbers (check /usr/include/x86_64-linux-gnu/asm/unistd_64.h)
# 3. Wrong file descriptor (2 instead of 1 writes to stderr)
# 4. Wrong string length (might truncate or include garbage)
# 5. Forgetting exit syscall (program crashes instead of clean exit)

# RELATIONSHIP TO C:
# This assembly program is roughly equivalent to:
#
# #include <unistd.h>
# int main() {
#     const char msg[] = {'H','e','l','l','o',',',' ','W','o','r','l','d','!','\n'};
#     write(1, msg, 14);
#     return 0;
# }
#
# Or more simply:
# #include <stdio.h>
# int main() {
#     printf("Hello, World!\n");
#     return 0;
# }

# NEXT STEPS:
# After understanding this program, explore:
# 1. Reading input (read syscall)
# 2. Working with multiple strings
# 3. Conditional logic (cmp, jmp instructions)
# 4. Loops and iteration
# 5. Functions and the call stack
