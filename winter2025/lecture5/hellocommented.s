# PEDAGOGICAL PURPOSE:
# This is the classic "Hello World" program in x86-64 assembly, demonstrating
# the most basic output operation. It introduces system calls, string data,
# and the minimal structure of an assembly program.
#
# Key learning objectives:
# 1. Understanding the basic structure of an assembly program
# 2. Learning the write syscall for output
# 3. Understanding .rodata section for string literals
# 4. Learning .asciz directive for null-terminated strings
# 5. Using '.' (location counter) to calculate string length
# 6. Understanding file descriptors (stdout vs stderr)
# 7. Learning the xor trick for efficient zeroing
# 8. Seeing why stderr (fd 2) might be used instead of stdout (fd 1)
#
# CONCEPTUAL OVERVIEW:
# This program simply outputs "Hello world!\n" and exits:
#   write(stderr, "Hello world!\n", 13);
#   return 0;

        .section .rodata           # RODATA SECTION: Read-only data
                                  # This section holds constants
                                  # Memory is write-protected (segfault if modified)
                                  # Ideal for string literals and other constants

msg:    .asciz "Hello world!\n"   # DEFINE MESSAGE: Null-terminated string
                                  # .asciz = ASCII string with Zero terminator
                                  # Characters: 'H','e','l','l','o',' ','w','o','r','l','d','!','\n','\0'
                                  # Total length: 14 bytes (13 visible + 1 null)
                                  #
                                  # BREAKDOWN:
                                  # 'H' 'e' 'l' 'l' 'o' ' ' 'w' 'o' 'r' 'l' 'd' '!' '\n' '\0'
                                  #  0   1   2   3   4   5   6   7   8   9  10  11  12   13
                                  #
                                  # WHY .asciz?
                                  # - Standard for C-compatible strings
                                  # - Null terminator allows string functions to find end
                                  # - .ascii would NOT include the null terminator

hellolen = . - msg                # CALCULATE STRING LENGTH: hellolen = 14
                                  # '.' = current location counter (address after the string)
                                  # msg = address where string starts
                                  # . - msg = length including null terminator
                                  #
                                  # ASSEMBLY TIME CALCULATION:
                                  # This is computed by the assembler, not at runtime
                                  # hellolen becomes a constant in the code
                                  #
                                  # NOTE: For write syscall, we could use 13 (without null)
                                  # but 14 works fine (write doesn't care about null terminator)
                                  # The null is for C string functions, not for write

        .section .text             # CODE SECTION: Executable instructions
        .globl _start              # GLOBAL DIRECTIVE: Make _start visible to linker
                                  # Linker uses _start as program entry point

_start:
        ## TASK: Call the write syscall and exit appropriately

        mov $1,%rax                # SYSCALL NUMBER: 1 = write
                                  # This says "I want to call the write syscall"
                                  # Linux syscall convention: number goes in %rax
                                  #
                                  # COMMON SYSCALLS:
                                  # 0 = read
                                  # 1 = write
                                  # 60 = exit

        mov $2,%rdi                # FILE DESCRIPTOR: 2 = stderr
                                  # This is interesting - usually we use stdout!
                                  # FILE DESCRIPTOR CONVENTION:
                                  # 0 = stdin  (standard input)
                                  # 1 = stdout (standard output)
                                  # 2 = stderr (standard error)
                                  #
                                  # WHY STDERR INSTEAD OF STDOUT?
                                  # Possible reasons:
                                  # 1. Pedagogical: Show that stderr exists
                                  # 2. Stderr is unbuffered (output appears immediately)
                                  # 3. Stdout can be redirected, stderr often stays on terminal
                                  # 4. Could be intentional to distinguish from normal output
                                  #
                                  # PRACTICAL DIFFERENCE:
                                  # ./hello > output.txt
                                  # If using stdout (1): message goes to file
                                  # If using stderr (2): message appears on terminal

        lea msg(%rip),%rsi         # LOAD MESSAGE ADDRESS: %rsi = &msg
                                  # lea = Load Effective Address
                                  # Loads the ADDRESS of the message, not its contents
                                  # RIP-relative addressing: msg(%rip)
                                  # This is position-independent code (PIC)
                                  #
                                  # WHY RIP-RELATIVE?
                                  # - Enables ASLR (Address Space Layout Randomization)
                                  # - Code works regardless of load address
                                  # - Required for modern security and shared libraries
                                  #
                                  # WHAT'S IN %rsi?
                                  # The memory address where "Hello world!\n" is stored

        mov $hellolen,%rdx         # LOAD MESSAGE LENGTH: %rdx = 14
                                  # Number of bytes to write
                                  # This is the constant calculated earlier
                                  # Using the full length including null terminator
                                  #
                                  # ALTERNATIVE:
                                  # Could use: mov $13,%rdx (without null terminator)
                                  # Both work; null doesn't hurt when writing

        syscall                    # INVOKE WRITE: Execute write(2, msg, 14)
                                  # We finally print!
                                  # This calls the kernel with our parameters
                                  #
                                  # WHAT HAPPENS:
                                  # 1. CPU switches to kernel mode
                                  # 2. Kernel looks at %rax (syscall number 1 = write)
                                  # 3. Kernel reads parameters:
                                  #    - %rdi = 2 (file descriptor)
                                  #    - %rsi = address of data
                                  #    - %rdx = 14 (bytes to write)
                                  # 4. Kernel writes 14 bytes to file descriptor 2 (stderr)
                                  # 5. "Hello world!\n" appears on terminal
                                  # 6. Kernel returns to user mode
                                  # 7. %rax contains return value (bytes written, or -1 on error)

        xor %rdi,%rdi              # ZERO %rdi: %rdi = 0
                                  # This is the exit code
                                  # Same as: mov $0,%rdi
                                  # But more efficient!
                                  #
                                  # XOR TRICK:
                                  # XOR of any value with itself = 0
                                  # Binary: 1010 XOR 1010 = 0000
                                  # Advantages:
                                  # - Shorter instruction (2 bytes vs 5 for mov $0)
                                  # - May be faster on some CPUs
                                  # - Classic optimization technique
                                  # - Breaks CPU dependency chains
                                  #
                                  # COMMON USAGE:
                                  # xor %eax,%eax   -> 32-bit zero (also zeros upper 32 bits)
                                  # xor %rax,%rax   -> 64-bit zero

        mov $60,%rax               # SYSCALL NUMBER: 60 = exit
                                  # Prepare to terminate the program
                                  # Note: This overwrites the write return value in %rax
                                  # But we don't need it anymore

        syscall                    # INVOKE EXIT: Execute exit(0)
                                  # We exit cleanly
                                  # Program terminates with exit code 0
                                  # Exit code 0 conventionally means "success"
                                  #
                                  # WHAT HAPPENS:
                                  # 1. CPU switches to kernel mode
                                  # 2. Kernel sees syscall 60 (exit)
                                  # 3. Kernel terminates the process
                                  # 4. Exit code (0) is returned to parent (shell)
                                  # 5. Shell can check with: echo $?

# PROGRAM EXECUTION FLOW:
#
# 1. Program starts at _start
# 2. Set up write syscall parameters:
#    - %rax = 1 (write)
#    - %rdi = 2 (stderr)
#    - %rsi = address of "Hello world!\n"
#    - %rdx = 14 (length)
# 3. Call write syscall
# 4. "Hello world!" appears on screen
# 5. Set up exit syscall parameters:
#    - %rax = 60 (exit)
#    - %rdi = 0 (success)
# 6. Call exit syscall
# 7. Program terminates

# SYSCALL PARAMETER PASSING:
#
# x86-64 syscall convention uses registers:
#   %rax - Syscall number (input) / Return value (output)
#   %rdi - Argument 1
#   %rsi - Argument 2
#   %rdx - Argument 3
#   %r10 - Argument 4 (note: NOT %rcx!)
#   %r8  - Argument 5
#   %r9  - Argument 6
#
# For write(fd, buf, count):
#   %rax = 1 (syscall number)
#   %rdi = fd (file descriptor)
#   %rsi = buf (pointer to data)
#   %rdx = count (bytes to write)

# FILE DESCRIPTOR DETAILS:
#
# Every process starts with three open file descriptors:
#   0 (stdin)  - Reads keyboard input (or redirected input)
#   1 (stdout) - Normal program output (can be redirected)
#   2 (stderr) - Error messages (usually stays on terminal)
#
# BUFFERING DIFFERENCES:
#   stdout (1): Line-buffered (waits for \n or buffer full)
#   stderr (2): Unbuffered (appears immediately)
#
# REDIRECTION BEHAVIOR:
#   ./hello > file.txt
#   - With fd=1: output goes to file
#   - With fd=2: output appears on terminal (stderr not redirected)
#
#   ./hello 2> file.txt
#   - With fd=1: output appears on terminal
#   - With fd=2: output goes to file

# STRING DIRECTIVES:
#
# .ascii "text"   - String without null terminator
# .asciz "text"   - String WITH null terminator ('\0')
# .string "text"  - Same as .asciz
#
# Example:
#   .ascii "Hi"   -> Bytes: 'H' 'i' (2 bytes)
#   .asciz "Hi"   -> Bytes: 'H' 'i' '\0' (3 bytes)
#
# For syscalls like write: null terminator doesn't matter
# For C string functions: null terminator is required

# LOCATION COUNTER ('.') EXAMPLES:
#
# start:
#   .byte 1,2,3       # '.' is now at start+3
#   len1 = . - start  # len1 = 3
#
# str:
#   .asciz "ABC"      # '.' is now at str+4 ('A','B','C','\0')
#   len2 = . - str    # len2 = 4
#
# The assembler evaluates these at assembly time!

# EQUIVALENT C CODE:
#
# #include <unistd.h>
#
# int main() {
#     const char *msg = "Hello world!\n";
#     int hellolen = 14;
#
#     write(2, msg, hellolen);  // Write to stderr
#     return 0;
# }

# TO ASSEMBLE, LINK, AND RUN:
# as hello.s -o hello.o
# ld hello.o -o hello
# ./hello
# (outputs: Hello world!)
# echo $?
# (outputs: 0)

# TO TEST REDIRECTION:
# ./hello > output.txt
# (message appears on terminal because we use stderr)
# ./hello 2> output.txt
# (message goes to file because we redirect stderr)
# cat output.txt

# EXPERIMENT:
# Change fd from 2 to 1:
#   mov $1,%rdi
# Then test redirection:
#   ./hello > output.txt
# Now the message goes to the file instead of terminal!

# MINIMAL ASSEMBLY PROGRAM TEMPLATE:
#
# .section .rodata
#   (constant data here)
#
# .section .text
# .globl _start
# _start:
#   (your code here)
#   mov $60,%rax
#   xor %rdi,%rdi
#   syscall
