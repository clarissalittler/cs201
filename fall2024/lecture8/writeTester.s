    .extern write_int

    .text
    .global _start

_start:
    movq    $-123456789, %rdi   # The integer to print
    call    write_int

    # Exit the program
    movq    $60, %rax           # sys_exit system call
    xorq    %rdi, %rdi          # Exit code 0
    syscall
