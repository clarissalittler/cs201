        .section .data
num1:   .long 200

        .section .text
        .global _start

_start:
        mov $num1,%rbx          # &num1 -> %rbx
        addl $10,(%rbx)         # *(%rbx) = 10 + *(%rbx)
        mov num1,%rdi
        mov $60,%rax
        syscall 
