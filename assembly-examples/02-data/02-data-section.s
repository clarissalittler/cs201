        .section .data
num1:   .long 200               #exists for the lifetime of the program
        ## .long means 32 bit

        .section .text
        .global _start

_start:
        addl $10,num1           #need the l so that it understands that it's adding into a 32bit number
        ## l means long
        ## usually we can leave these off
        ## because the assembler can figure out
        ## if it's using the q (64 bit), l (32bit), w (16 bit), b (8 bit)
        ## q - 8 bytes, l - 4 bytes, w - 2 bytes, b - 1 byte 
        mov num1,%rdi
        mov $60,%rax
        syscall 
