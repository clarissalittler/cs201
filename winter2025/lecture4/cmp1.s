        .text
        .global _start

_start:
        mov $1,%rax
        mov $2,%rbx

        cmp %rbx,%rax           # cmp arg2,arg1
        jne less                #Q: is this jump if arg2 <= arg1 or arg1 <= arg2?
        mov $-1,%rdi
        jmp finish
less:
        mov $10,%rdi
finish:
        mov $60,%rax
        syscall
        
        
