.section .data
hello:
    .asciz "Hello, World!\n"
hello2: .byte 'H','e','l','l','o', ',' ,' ','W','o','r','l','d','!','\n'
hellolen = . - hello2 
	
.section .text
.global _start

_start:
    mov $1, %rax        	# 1 is the syscall number of write
    mov $1, %rdi        	# the file descripter
    lea hello2(%rip), %rsi 
    mov $hellolen, %rdx       # length of the string
    syscall

    # Exit the program
    mov $60, %rax       
    xor %rdi, %rdi      
    syscall
