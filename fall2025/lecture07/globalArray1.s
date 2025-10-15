.section .data
arr:    .quad 1,2,3,4

        .section .text
        .global _start

_start:
        lea arr(%rip),%r9
        mov (%r9), %rdi         # *arr -> 1
        mov $60, %rax
        syscall
