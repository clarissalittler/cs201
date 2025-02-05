        .section .data
arr:    .long 1,2,3,4 

        .section .text
        .global _start

fun1:
        push %rbp
        mov %rsp,%rbp
        sub $16,%rsp
        ## this is setting up the stack to hold
        ## local variables
        movl $3,-16(%rbp)
        ## start of the loop
        ## let's null out the array with 0s
        mov -16(%rbp),%rcx
1:
        cmp $0,%rcx
        jl 2f
        lea arr(%rip),%rbx
        movl $0,(%rbx,%rcx,4)
        dec %rcx
        jmp 1b
2:            
        leave
        ret

_start:
        call fun1
        mov $60,%rax
        mov arr(%rip),%rdi
        syscall
