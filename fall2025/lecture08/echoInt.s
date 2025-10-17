.section .text
        .extern readInt,writeInt
        .global _start
        
_start:
        call readInt
        ## the integer read from stdin
        ## is in %rax
        ## we put that in %rdi
        ## and call writeInt to print
        mov %rax,%rdi
        call writeInt

        mov $0,%rdi
        mov $60,%rax
        syscall
