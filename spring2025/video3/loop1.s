        .section .text
        .global _start
        ## let's have a function
        ## that will put a number in %rcx
        ## and then use labels to make a loop
        ## that adds all the numbers from
        ## 0 to the initial value of %rcx

loopstart:
        cmp $0,%rcx
        jle end
        add %rcx,%rax           #adds current value of rcx into rax, rax is the accumulator
        sub $1,%rcx             # set rcx = rcx-1
        jmp loopstart
        
_start:
        mov $0,%rax
        mov $10,%rcx            # the sum should be 55
        jmp loopstart

end:
        mov %rax,%rdi           #moves accumulated value into the return code
        mov $60,%rax            #sets up exit system call (with %rdi as exit code)
        syscall
