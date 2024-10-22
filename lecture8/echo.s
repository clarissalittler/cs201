.section .bss
buffer: .skip 128 

.section .text
.global _start

_start:
    mov $0, %rax 		#0 in rax for read
    mov $0, %rdi             	#0 in rdi for stdin
    lea buffer(%rip), %rsi   	#put the pointer in rsi
    mov $128, %rdx           	#the *max* length of the buffer
    syscall 			#tons of code runs RIGHT HERE

	## now I'm back
	## and I have no idea what has happened to allll my registers
	## except that I know the return value is in rax
    mov %rax, %rbx           	#unlike write, we need the return value of read
				#rax contains the number of bytes read in


	## rdi is always argument 1 to a function
	## rsi is always argument 2 to a function
	## rdx is always argument 3 to a function
    mov $1, %rax             
    mov $1, %rdi             
    lea buffer(%rip), %rsi   
    mov %rbx, %rdx           
    syscall

    # Exit the program
    mov $60, %rax            
    xor %rdi, %rdi           
    syscall
