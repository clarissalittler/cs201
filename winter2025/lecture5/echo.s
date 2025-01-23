        .section .bss
buff:   .skip 128

        .section .text
        .globl _start

_start:
        ## setting up read 
        mov $0,%rax
        mov $1,%rdi             # stdin
        lea buff(%rip),%rsi     # pass the pointer
        mov $128,%rdx
        syscall

        ## set up the right
        ## the number of bytes is in %rax here
        mov %rax,%rdx           #move the number of bytes to %rdx
        mov $1,%rax
        lea buff(%rip),%rsi
        mov $0,%rdi
        syscall
        
        ## exit with exit code 0
        xor %rdi,%rdi
        mov $60,%rax
        syscall
