.section .data
hiThere: .ascii "Hey there, goofuses and goofuxen\n"

        .section .text
        .global _start

        ## writing is a system call of 1
        ## calling convention:
        ## First argument goes into rdi
        ## Put the FD into %rdi
        ## next argument goes into rsi
        ## address of the buffer goes in %rsi
        ## next argument after that goes into rdx
        ## length goes in %rdx
_start:
        mov $1,%rdi
        lea hiThere(%rip),%rsi
        mov $33,%rdx
        mov $1,%rax
        syscall

        mov $60,%rax
        mov $0,%rdi
        syscall
