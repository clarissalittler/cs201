        .section .text
        .global _start

_start: 
        mov $11,%rdi
        sub $20,%rdi            # %rdi - $20 -> %rdi
        
        mov $60,%rax
        syscall
