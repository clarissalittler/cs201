.section .text
        .global _start

_start:
        push $10
        pop %rdi

        mov $60,%rax
        syscall
