        .section .rodata
msg:    .asciz "Hello world!\n"
hellolen = . - msg

        .section .text
        .globl _start

_start:
        ## call the write syscall
        ## and exit appropriately
        mov $1,%rax             # this says call write
        mov $2,%rdi             # 0 stdout, 2 stderr
        lea msg(%rip),%rsi
        mov $hellolen,%rdx

        syscall                 # we finally print
        
        xor %rdi,%rdi           # same as mov $0, %rdi
        mov $60,%rax
        syscall                 # we exit
