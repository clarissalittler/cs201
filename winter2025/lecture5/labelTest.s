        .section .rodata
msg1:   .asciz "In the first sublabel\n"
msg1len = . - msg1

msg2:   .asciz "In the second sublabel\n"
msg2len = . - msg2

        .section .text
        .globl _start

_start:
        call biglabel2
        call biglabel1
        
        ## exit
1:      
        mov $60,%rax
        xor %rdi,%rdi
        syscall
        
biglabel1:
        jmp 1f
1:
        lea msg1(%rip),%rsi
        mov $msg1len,%rdx
        mov $1,%rax
        mov $0,%rdi
        syscall
        ret

biglabel2:
        jmp 1f
1:
        lea msg2(%rip),%rsi
        mov $msg2len,%rdx
        mov $1,%rax
        mov $0,%rdi
        syscall
        ret
