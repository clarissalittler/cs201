        .section .bss
buff:   .skip 128

        .section .rodata
msg1:   .asciz "Was zero\n"
msg1len = . - msg1
        
msg2:   .asciz "Not zero\n"
msg2len = . - msg2
        

        .section .text
        .globl _start

_start:

        lea buff(%rip),%rbx
        cmpb $0,(%rbx)
        je isZero
        jmp notZero
        
        ## set up the right
        ## the number of bytes is in %rax here
        ## mov $128,%rdx           #move the number of bytes to %rdx
        ## mov $1,%rax
        ## lea buff(%rip),%rsi
        ## mov $0,%rdi
        ## syscall

isZero:
        ## set %rsi and %rdx
        lea msg1(%rip),%rsi
        mov $msg1len,%rdx
        jmp writeLabel
        
notZero:        
        lea msg2(%rip),%rsi
        mov $msg2len,%rdx
        
writeLabel:     
        mov $1,%rax
        mov $0,%rdi
        syscall
        
        ## exit with exit code 0
        xor %rdi,%rdi
        mov $60,%rax
        syscall
