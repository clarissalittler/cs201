.section .bss
buffer: .skip 128 

.section .text
.global _start

_start:
    mov $0, %rax
    mov $0, %rdi             
    lea buffer(%rip), %rsi   
    mov $128, %rdx           
    syscall

    mov %rax, %rbx           

    mov $1, %rax             
    mov $1, %rdi             
    lea buffer(%rip), %rsi   
    mov %rbx, %rdx           
    syscall

    # Exit the program
    mov $60, %rax            
    xor %rdi, %rdi           
    syscall
