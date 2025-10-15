        .section .text
        .global _start

        ## let's have a loop that adds all the numbers between 1 and 10 and puts that as the exit code
_start:
        ## let's store our computations in %r8
        ## where do we store the index of our for-loop?
        ## By convention! Use %rcx for that
        ## for(%rcx = 1; %rcx<11; %rcx++){
        ##   %r8 = %r8 + %rcx
        ## }
        mov $1,%rcx
        mov $0,%r8

loop:                           #while loop style, tests before the loop body
        cmp $11,%rcx
        jge finish
        add %rcx,%r8
        add $1,%rcx
        jmp loop
        
finish:
        mov %r8,%rdi
        mov $60,%rax
        syscall
