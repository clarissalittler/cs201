        .section .data
num1:   .long 200               #exists for the lifetime of the program
        ## .long means 32 bit

        .section .text
        .global _start

_start:
        ## rip-relative addressing
        ## lea is used for the & operation
        lea num1(%rip),%rbx          # &num1 -> %rbx
        addl $10,(%rbx)         # *(%rbx) = 10 + *(%rbx)
        mov num1,%rdi
        mov $60,%rax
        syscall 
