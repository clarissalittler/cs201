# This is a simple program that reads input from stdin and then writes it back out

        .section .bss           # Uninitialized data section
buff:   .skip 128       # Allocate 128 bytes of buffer space for reading input

        .section .text  # Code section
        .global _start   # Makes the entry point visible to the linker

_start:
        ## Reading input from stdin
        mov $0,%rax             # System call number for read (0)
        mov $0,%rdi             # File descriptor for stdin (standard input)
        lea buff(%rip),%rsi     # Load effective address of buffer into source index
        mov $128,%rdx           # Maximum number of bytes to read (buffer size)
        syscall                 # Invoke the system call to read input

        ## Writing the input back out
        ## Note: %rax now contains the number of bytes actually read
        mov %rax,%rdx           # Move the byte count to the length parameter
        mov $1,%rax             # System call number for write (1)
        lea buff(%rip),%rsi     # Point to the same buffer we read into
        mov $1,%rdi             # File descriptor for stdout (standard output)
        syscall                 # Invoke the system call to write output

        ## Exit the program cleanly
        xor %rdi,%rdi           # Zero out exit code (0 means successful exit)
        mov $60,%rax            # System call number for exit
        syscall                 # Terminate the program
