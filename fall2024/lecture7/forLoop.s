        .section .text
        .global _start

_start:
        mov $0,%rax
        mov $0,%rcx             # setting up the loop

        .loopStart:
        mov %rcx,%rbx
        imul %rcx,%rbx
        add %rbx,%rax
        inc %rcx
        cmp $6,%rcx
        jl .loopStart

        mov %rax,%rdi # ending the program
        mov $60, %rax
        syscall
