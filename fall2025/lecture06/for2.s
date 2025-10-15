        .section .data
num:    .quad 0                 # num is a global variable, 64 bits in size, initial value of 0
        ## i.e. long num = 0;
num2:   .quad 0
        
        .section .text
        .global _start

        ## let's have a loop that adds all the numbers between 1 and 10 and puts that as the exit code
_start:
        ## let's store our computations in %r8
        ## where do we store the index of our for-loop?
        ## By convention! Use %rcx for that
        ## for(%rcx = 1; %rcx<11; %rcx++){
        ##   num = num + %rcx
        ## }
        mov $1,%rcx

loop:                           #while loop style, tests before the loop body
        cmp $11,%rcx
        jge finish
        add %rcx,num(%rip)      #???? is the (%rip)
        addq $1,num2(%rip)
        add $1,%rcx
        jmp loop
        
finish:
        mov num(%rip),%rdi
        mov $60,%rax
        syscall
