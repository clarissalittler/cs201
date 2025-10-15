.section .bss
buff:   .skip 100

        .section .text
        .global _start

_start: 
        mov $0,%rdi
        lea buff(%rip),%rsi
        mov $100,%rdx
        mov $0,%rax
        syscall
        ## we've read in bytes
        ## now %rax has inside it the number of bytes read
        mov $1,%rdi
        lea buff(%rip),%rsi
        mov %rax,%rdx
        mov $1,%rax
        syscall

        mov $60,%rax
        mov $0,%rdi
        syscall
