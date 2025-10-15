.section .data
num:    .quad 10                # 64 bit integer (long)

        .section .text
        .global _start

_start: 
        addq $10,num(%rip)
        movq num(%rip), %rdi
        movq $60, %rax
        syscall
