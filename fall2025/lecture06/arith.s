        .section .text
        .global _start

_start:
        mov $5,%r8
        add $10,%r8             # r8 = r8 + 10
        add %r8,%r8             # r8 = r8 + r8
        mov %r8,%rdi

        mov $60,%rax
        syscall
