        .section .data
myArray:         .quad 1, 2, 3, 4, 5
        
        .section .text
        .global _start
        

_start:
        mov $0, %rcx

        .dubLoop:
        mov myArray(,%rcx,8), %rax
        imul $2, %rax

        mov %rax, myArray(,%rcx,8)

        inc %rcx
        cmp $5, %rcx
        jl .dubLoop

        mov $0,%rcx

        mov $0,%rax
        .sumLoop:
        add myArray(,%rcx,8), %rax
        inc %rcx
        cmp $5, %rcx
        jl .sumLoop

        mov %rax,%rdi
        mov $60,%rax
        syscall
        
        

        
        
