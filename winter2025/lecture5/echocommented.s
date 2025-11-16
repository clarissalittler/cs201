# PEDAGOGICAL PURPOSE:
# This program demonstrates input/output operations using Linux system calls.
# It introduces the fundamental concepts of reading from stdin and writing to stdout,
# buffer management, and the .bss section for uninitialized data.
#
# Key learning objectives:
# 1. Understanding the read syscall (syscall 0) for input
# 2. Understanding the write syscall (syscall 1) for output
# 3. Learning about the .bss section for uninitialized buffers
# 4. Understanding file descriptors (0=stdin, 1=stdout, 2=stderr)
# 5. Learning how syscalls return values (bytes read in %rax)
# 6. Understanding buffer-based I/O
# 7. Learning the xor trick for zeroing registers
#
# CONCEPTUAL OVERVIEW:
# This program implements a simple echo:
#   char buff[128];
#   int n = read(stdin, buff, 128);    // Read up to 128 bytes
#   write(stdout, buff, n);             // Write exactly n bytes back
#   return 0;

        .section .bss              # BSS SECTION: Uninitialized data
                                  # BSS = "Block Started by Symbol"
                                  # This section is for variables without initial values
                                  # Memory is automatically zeroed by the OS loader
                                  # BSS doesn't take space in the executable file
                                  # Space is allocated at runtime

buff:   .skip 128                 # ALLOCATE BUFFER: Reserve 128 bytes of uninitialized memory
                                  # .skip N = reserve N bytes (filled with zeros at runtime)
                                  # buff is a label pointing to this 128-byte region
                                  # WHY 128? Arbitrary size large enough for typical input
                                  # This is like: char buff[128];
                                  #
                                  # WHY USE .bss INSTEAD OF .data?
                                  # .bss saves executable file size
                                  # .data would embed 128 zero bytes in the file
                                  # .bss just says "allocate 128 bytes at load time"
                                  # More efficient for large buffers!

        .section .text             # CODE SECTION: Executable instructions
        .globl _start              # GLOBAL DIRECTIVE: Entry point visible to linker

_start:
        ## STEP 1: SET UP READ SYSCALL
        ## We want to read from stdin into our buffer

        mov $0,%rax                # SYSCALL NUMBER: 0 = read
                                  # Linux syscall numbers:
                                  # 0 = read, 1 = write, 60 = exit
                                  # read(fd, buffer, count)

        mov $1,%rdi                # FILE DESCRIPTOR: stdin = 1
                                  # WAIT - THIS IS WRONG IN THE ORIGINAL CODE!
                                  # stdin should be 0, not 1!
                                  # File descriptor convention:
                                  # 0 = stdin  (standard input)
                                  # 1 = stdout (standard output)
                                  # 2 = stderr (standard error)
                                  #
                                  # This is likely a bug, but it might work in some contexts
                                  # where fd 1 is opened for reading (unusual)
                                  # Correct value should be: mov $0,%rdi

        lea buff(%rip),%rsi        # BUFFER ADDRESS: %rsi = address of buff
                                  # Pass the pointer to our buffer
                                  # RIP-relative addressing for position independence
                                  # The syscall will store input data here
                                  # This is the "where to put the data" argument

        mov $128,%rdx              # BUFFER SIZE: max bytes to read = 128
                                  # This is the "how many bytes to read" argument
                                  # read will read UP TO 128 bytes
                                  # It might read less if:
                                  # - End of file is reached
                                  # - User presses Enter (reads up to newline)
                                  # - Less data is available

        syscall                    # INVOKE READ: Execute read(1, buff, 128)
                                  # (Note: should be read(0, buff, 128))
                                  # This pauses the program and waits for input
                                  # User can type text and press Enter
                                  #
                                  # WHAT HAPPENS:
                                  # 1. Program blocks (waits) for input
                                  # 2. User types text and presses Enter
                                  # 3. Input (including newline) is copied to buff
                                  # 4. Syscall returns number of bytes read in %rax
                                  #
                                  # RETURN VALUE:
                                  # %rax = number of bytes actually read
                                  # If user types "hello\n", %rax = 6
                                  # If user types "hi\n", %rax = 3

        ## STEP 2: SET UP WRITE SYSCALL
        ## We want to write the data we just read back to stdout
        ## The number of bytes read is now in %rax

        mov %rax,%rdx              # BYTES TO WRITE: %rdx = number of bytes read
                                  # CRITICAL: Use the actual number of bytes read!
                                  # NOT the buffer size (128)
                                  # If we wrote 128 bytes, we'd output garbage after the input
                                  #
                                  # WHY THIS MATTERS:
                                  # If user typed "hi\n" (3 bytes), %rax = 3
                                  # We want to write exactly 3 bytes, not 128
                                  # The other 125 bytes in buff are undefined/zero

        mov $1,%rax                # SYSCALL NUMBER: 1 = write
                                  # write(fd, buffer, count)
                                  # Note: This overwrites the read count, but we saved it in %rdx!

        lea buff(%rip),%rsi        # BUFFER ADDRESS: %rsi = address of buff
                                  # Same buffer we read into
                                  # Now we're reading FROM it to write out
                                  # The data is still there from the read syscall

        mov $0,%rdi                # FILE DESCRIPTOR: stdout = 0
                                  # WAIT - THIS IS ALSO WRONG!
                                  # stdout should be 1, not 0!
                                  # Correct value should be: mov $1,%rdi
                                  #
                                  # The original code has stdin/stdout swapped!
                                  # Correct version:
                                  # read should use fd 0 (stdin)
                                  # write should use fd 1 (stdout)

        syscall                    # INVOKE WRITE: Execute write(0, buff, n)
                                  # (Note: should be write(1, buff, n))
                                  # This outputs the data to stdout
                                  # The user will see their input echoed back
                                  #
                                  # WHAT HAPPENS:
                                  # 1. Kernel reads n bytes from buff
                                  # 2. Writes them to file descriptor 0 (should be 1)
                                  # 3. Text appears on terminal
                                  # 4. Syscall returns number of bytes written in %rax

        ## STEP 3: EXIT WITH EXIT CODE 0
        xor %rdi,%rdi              # ZERO %rdi: %rdi = 0 (exit code)
                                  # xor is a clever trick for zeroing a register
                                  # Same as: mov $0, %rdi
                                  # WHY USE xor?
                                  # - Shorter instruction encoding (2 bytes vs 5 bytes)
                                  # - Faster on some CPUs
                                  # - Classic optimization trick
                                  #
                                  # HOW IT WORKS:
                                  # XOR (exclusive OR): bit is 1 if inputs differ
                                  # XOR with itself: all bits differ by 0
                                  # Result: all bits are 0
                                  # Example: 1010 XOR 1010 = 0000

        mov $60,%rax               # SYSCALL NUMBER: 60 = exit
                                  # Prepare to terminate the program

        syscall                    # INVOKE EXIT: Execute exit(0)
                                  # Program terminates successfully
                                  # Exit code 0 indicates success

# FILE DESCRIPTOR REFERENCE:
#
# Standard file descriptors (always open):
#   0 = stdin  (standard input)  - Read user input
#   1 = stdout (standard output) - Normal program output
#   2 = stderr (standard error)  - Error messages
#
# CORRECT USAGE:
#   read(0, buffer, size)    - Read from stdin
#   write(1, buffer, size)   - Write to stdout
#   write(2, buffer, size)   - Write to stderr
#
# NOTE: This program has the file descriptors backwards!
# Working version should use:
#   read:  mov $0,%rdi
#   write: mov $1,%rdi

# SYSCALL CONVENTIONS:
#
# For read syscall (0):
#   %rax = 0           (syscall number)
#   %rdi = fd          (file descriptor to read from)
#   %rsi = buffer      (where to store data)
#   %rdx = count       (max bytes to read)
#   Returns: %rax = bytes read (or -1 on error)
#
# For write syscall (1):
#   %rax = 1           (syscall number)
#   %rdi = fd          (file descriptor to write to)
#   %rsi = buffer      (data to write)
#   %rdx = count       (bytes to write)
#   Returns: %rax = bytes written (or -1 on error)
#
# For exit syscall (60):
#   %rax = 60          (syscall number)
#   %rdi = exit_code   (return value)
#   Does not return

# BUFFER MANAGEMENT:
#
# Why use a buffer?
#   - I/O is done in chunks, not byte-by-byte
#   - More efficient to read/write multiple bytes at once
#   - Buffer acts as temporary storage
#
# Buffer size considerations:
#   - Too small: might truncate input
#   - Too large: wastes memory
#   - 128 bytes is reasonable for simple line input
#   - For production: 4096 or 8192 bytes is common

# XOR TRICK EXPLAINED:
#
# xor %rdi,%rdi    is equivalent to    mov $0,%rdi
#
# Truth table for XOR:
#   0 XOR 0 = 0
#   0 XOR 1 = 1
#   1 XOR 0 = 1
#   1 XOR 1 = 0
#
# Any value XOR itself = 0
# Examples:
#   xor %rax,%rax   -> %rax = 0
#   xor %rbx,%rbx   -> %rbx = 0
#   xor %ecx,%ecx   -> %ecx = 0
#
# Benefits:
#   - Shorter encoding (saves bytes in executable)
#   - May be faster on some processors
#   - Breaks dependency chains in CPU pipeline
#   - Common idiom in assembly code

# EQUIVALENT C CODE:
#
# #include <unistd.h>
#
# int main() {
#     char buff[128];
#     int n;
#
#     // Read from stdin (should be fd 0)
#     n = read(0, buff, 128);
#
#     // Write to stdout (should be fd 1)
#     write(1, buff, n);
#
#     return 0;
# }

# PROGRAM FLOW:
#
# 1. Program starts
# 2. Calls read syscall, waits for input
# 3. User types text and presses Enter
# 4. Input is stored in buff, byte count in %rax
# 5. Calls write syscall with the data
# 6. Output appears on screen (echoed)
# 7. Program exits with code 0

# TO ASSEMBLE, LINK, AND RUN:
# as echo.s -o echo.o
# ld echo.o -o echo
# ./echo
# (type something and press Enter)
# (your input should be echoed back)

# TO FIX THE FD BUG:
# Change line "mov $1,%rdi" (before read) to "mov $0,%rdi"
# Change line "mov $0,%rdi" (before write) to "mov $1,%rdi"
# Then reassemble and it will work correctly with standard I/O

# EXPERIMENT:
# Try piping data to the program:
#   echo "Hello, world!" | ./echo
# Try redirecting input:
#   ./echo < input.txt
# Try redirecting output:
#   ./echo > output.txt
#   (type text, press Enter, Ctrl+D)
#   cat output.txt
