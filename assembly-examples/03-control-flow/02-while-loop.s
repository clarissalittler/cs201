        .section .text
        .global _start

_start:
        ## Here's how we write a loop as a while loop that will count
        ## the same thing as the previous program but backwards
        mov $5,%rcx
        mov $0,%rax

        .loopStart:
        mov %rcx,%rbx
        imul %rbx,%rbx
        add %rbx,%rax
        dec %rcx
        cmp $0,%rcx
        jg .loopStart
        
        ## end of the program
        mov %rax,%rdi
        mov $60,%rax
        syscall
