.section .text
        .global _start

sillyFunk:
        ## this function is going to declare two local variables
        ## then put 1 and 2 in the local variables
        ## it will add the values in these variables into %rax and then return
        ## to allocate the local variables
        push %rbp               # old base pointer is aved
        mov %rsp,%rbp           # copy old top of the frame to be the new bottom of the frame
        sub $16,%rsp            # move the new top of the frame down by 16 bytes
        movq $1,-8(%rbp)
        movq $2,-16(%rbp)
        mov $0,%rax
        add -8(%rbp),%rax
        add -16(%rbp),%rax

        ## cleanup time
        mov %rbp,%rsp
        pop %rbp
        ret
        
_start:
        call sillyFunk
        mov %rax,%rdi
        mov $60,%rax
        syscall
