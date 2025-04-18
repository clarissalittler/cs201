        .section .text
        .global _start

_start:
        mov $0,%rdi
        mov $0,%r8
        mov $0,%r9
        cmp %r8,%r9             # really a subtract under the hood
        ## calculates %r9 - %r8 and stores a result
        jne notequal
        jmp end
notequal:   
        mov $10,%rdi            #whoops! this line will always happen :(
        ##  the above is if (r9 < r8) {rdi=10}
end:    
        mov $60,%rax
        syscall

