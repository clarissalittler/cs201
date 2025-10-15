.section .data
arr:    .quad 1,2,3,4

        .section .text
        .global _start

_start:
        lea arr(%rip),%r9
        mov $1,%rcx
        mov (%r9,%rcx,8), %rdi
        # *(%r9 + 1*8)
        # In C you'd just say *(%r9 + 1)
        mov $60, %rax
        syscall
