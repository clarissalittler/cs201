        .text
        .globl _start

        ## this function adds two numbers
        ## then multiplies by 3
        ## then returns that
fun:    
        mov $0,%rax
        add %rdi,%rax
        add %rsi,%rax
        imul $3,%rax            #imul is multiplication of signed numbers
                                #mul is the multiplication of unsigned numbers 
        ret

        ## int fun(int a,int b)
        ## ...
        ## return 3*(a + b);
        
_start:
        mov $2,%rdi
        mov $3,%rsi

        call fun

        mov %rax,%rdi
        mov $60,%rax
        syscall
