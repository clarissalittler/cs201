        .section .text       ; This line defines the current section as '.text', indicating that the following code will contain instructions. 
        .global _start      ; This line declares the label '_start' as a global symbol, making it visible to the linker. 
                            ; The '_start' label is the entry point of the program.

_start:
        ## Here's how we write a loop as a while loop that will count 
        ## the same thing as the previous program but backwards
        mov $5,%rcx          ; Move the immediate value 5 into the register %rcx. This initializes the loop counter.
        mov $0,%rax          ; Move the immediate value 0 into the register %rax. This initializes the accumulator, which will store the result of the calculation.

        .loopStart:         ; This label marks the beginning of the loop.
        mov %rcx,%rbx        ; Move the value of %rcx into register %rbx. This is done to preserve the value of %rcx for the calculation.
        imul %rbx,%rbx      ; Multiply the value in %rbx by itself (square it). The result is stored in %rbx.
        add %rbx,%rax       ; Add the value in %rbx to the value in %rax. This accumulates the squared values.
        dec %rcx           ; Decrement the value in %rcx by 1. This is the loop counter.
        cmp $0,%rcx          ; Compare the value in %rcx with the immediate value 0. 
                            ; This checks if the counter has reached zero.
        jg .loopStart       ; Jump to the label '.loopStart' if the value in %rcx is greater than 0.
                            ; This continues the loop until the counter reaches zero.

        ## end of the program
        mov %rax,%rdi       ; Move the value in %rax (the accumulated sum) into %rdi, which is the first argument to the syscall.
        mov $60,%rax       ; Move the system call number 60 (exit) into %rax. 
        syscall             ; Execute the system call (exit), passing the accumulated sum as the exit code. 

