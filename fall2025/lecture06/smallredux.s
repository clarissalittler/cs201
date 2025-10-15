        .text
        .global _start

_start:
        ## this program will put -1 in %rdi
        ## it will put the number 60 in %rax
        ## then execute syscall to end
        mov $-1,%rdi
        mov $60,%rax
        syscall
