        .text
        .global _start

        ## here's a thing that will
        ## use local variables on the stack
        ## long fun(long a, long b){
        ##   long c = a + b;
        ##   long d = 2+b;
        ##   return (c + d);
fun:    
        
        ## first we need to save the old stack pointer into the base pointer
        ## the stack pointer is %rsp
        ## the base pointer is %rbp
        push %rbp
        mov %rsp,%rbp
        sub $16,%rsp
        ## set aside space for 16 bytes on the stack
        ## relative addressing
        ## -8(%rbp) to be our first variable
        ## -16(%rbp) to be our second variable
        mov %rdi,-8(%rbp)
        add %rsi,-8(%rbp)

        mov %rsi,-16(%rbp)
        addq $2,-16(%rbp)
        mov -16(%rbp),%rax
        add -8(%rbp),%rax
        ## mov %rbp,%rsp
        ## pop %rbp
        leave
        ret
        
_start:
        mov $3,%rdi
        mov $4,%rsi

        call fun

        mov %rax,%rdi
        mov $60,%rax
        syscall
