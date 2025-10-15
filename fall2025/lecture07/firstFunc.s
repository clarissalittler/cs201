.section .text
        .global _start

        ## silly add takes the argument in %rdi
        ## and the argument in %rsi
        ## and adds them together in %rax
sillyAdd:       
        mov $0,%rax
        add %rdi,%rax
        add %rsi,%rax
        ret

_start:
        mov $10,%rdi
        mov $20,%rsi
        call sillyAdd

        mov %rax,%rdi
        mov $60,%rax
        syscall
