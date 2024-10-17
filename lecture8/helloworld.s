        .section .data
hello:
    .asciz "Hello, World!\n"
hello2: .byte 'H','e','l','l','o', ',' ,' ','W','o','r','l','d','!','\n'
hellolen = . - hello2
	
.section .text
.global _start

_start:
    mov $1, %rax        
    mov $1, %rdi        
    lea hello2(%rip), %rsi 
    mov $hellolen, %rdx       # length of the string
    syscall

    # Exit the program
    mov $60, %rax       
    xor %rdi, %rdi      
    syscall
