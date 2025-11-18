# ============================================================================
# Echo Program - Guided Version
# ============================================================================
# This program demonstrates basic input/output using system calls.
# It reads a line from stdin and writes it back to stdout.
#
# WHAT IT DOES: Reads user input, echoes it back, exits cleanly
#
# NEW CONCEPTS:
# - .bss section (uninitialized data)
# - read syscall
# - Using syscall return values
# ============================================================================

# BSS SECTION: Uninitialized data (zero-filled at program start)
.section .bss

buffer:
    .skip 128               # Allocate 128 bytes for input buffer
                            # .skip reserves space without initializing it
                            # (More efficient than .zero for large buffers)

# CODE SECTION
.section .text
.global _start

_start:
    # ========================================================================
    # SYSTEM CALL 1: read(fd=0, buf=buffer, count=128)
    # Reads up to 128 bytes from stdin into buffer
    # ========================================================================

    mov $0, %rax            # syscall number: 0 = read
    mov $0, %rdi            # arg1: file descriptor (0 = stdin)
    lea buffer(%rip), %rsi  # arg2: address of buffer
    mov $128, %rdx          # arg3: max bytes to read
    syscall                 # Blocks until user presses Enter

    # IMPORTANT: read() returns number of bytes actually read in %rax
    # Save it because we need it for write()!

    mov %rax, %rbx          # Save byte count in %rbx
                            # (We'll use %rax again for next syscall)

    # ========================================================================
    # SYSTEM CALL 2: write(fd=1, buf=buffer, count=bytes_read)
    # Writes exactly what we read back to stdout
    # ========================================================================

    mov $1, %rax            # syscall number: 1 = write
    mov $1, %rdi            # arg1: file descriptor (1 = stdout)
    lea buffer(%rip), %rsi  # arg2: address of buffer (same as before)
    mov %rbx, %rdx          # arg3: bytes to write (from read's return)
    syscall                 # Outputs the text

    # ========================================================================
    # SYSTEM CALL 3: exit(0)
    # ========================================================================

    mov $60, %rax           # syscall number: 60 = exit
    xor %rdi, %rdi          # arg1: exit status = 0
    syscall

# ============================================================================
# KEY CONCEPTS:
#
# 1. .bss vs .data:
#    - .data: Initialized data (takes space in executable file)
#    - .bss: Uninitialized data (just reserves memory at runtime)
#    - Use .bss for large buffers to keep executable small
#
# 2. SYSCALL RETURN VALUES:
#    - read() returns number of bytes actually read
#    - This might be less than buffer size!
#    - Return value is in %rax
#    - Must save it if you need it later (we use %rbx)
#
# 3. BUFFER REUSE:
#    - Same buffer used for both read and write
#    - We read into it, then write from it
#    - No need to copy data
#
# 4. WHY SAVE BYTE COUNT?
#    - We must write exactly what we read
#    - If user types 10 chars, write 10 chars (not 128!)
#    - That's why we save %rax (byte count) to %rbx
#
# ============================================================================
# SYSCALLS USED:
#
# read(int fd, void *buf, size_t count)
#   - Syscall 0
#   - Reads up to count bytes from file descriptor
#   - Returns number of bytes actually read
#   - Blocks until data available or EOF
#
# write(int fd, const void *buf, size_t count)
#   - Syscall 1
#   - Writes count bytes from buffer to file descriptor
#   - Returns number of bytes actually written
#
# ============================================================================
# TO BUILD AND RUN:
#
# as 01-echoGuided.s -o echo.o
# ld echo.o -o echo
# ./echo
# [Type something and press Enter]
# [Your text appears again]
#
# EXAMPLE SESSION:
# $ ./echo
# Hello, World!
# Hello, World!
# $
#
# The first line is what you type, the second is the echo.
# ============================================================================
