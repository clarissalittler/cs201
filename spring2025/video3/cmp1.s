        .section .text
        .global _start

_start:
        mov $0,%rdi
        mov $50,%r8
        mov $2,%r9
        cmp %r8,%r9             # really a subtract under the hood
        ## calculates %r9 - %r8 and stores a result
        jl less 
less:   
        mov $10,%rdi            #whoops! this line will always happen :(
        ##  the above is if (r8 < r9) {rdi=10}
        
        mov $60,%rax
        syscall
