        .section .text
        .global _start

_start:
        mov $0,%rdi
        mov $0,%r8
        mov $0,%r9
        cmp $-1,%r9             # really a subtract under the hood
        ## calculates %r9 - %r8 and stores a result
        jne notequal
        jmp end
notequal:   
        mov $10,%rdi           
end:    
        mov $60,%rax
        syscall

