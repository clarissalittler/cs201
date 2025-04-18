        .section .text
        .global _start

_start: 
        mov $11,%rdi            # puts the number 11 in %rdi
        add %rdi,%rdi           # adds %rdi to itself and stores the result in %rdi
        ## add r1,r2 ----> r2 = r2+r1 -----> r2 += r1
        mov $60,%rax
        syscall
