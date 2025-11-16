	.section .data
hello:
	.asciz "Hello, World!\n"

	.section .text
	.global _start
	.extern .countStr

_start:
	## calculate length
	lea hello(%rip), %rdi
	call countStr
	mov %rax,%rdx
	lea hello(%rip), %rsi
	mov $1, %rax
	mov $1, %rdi
	syscall

	mov $60, %rax
	mov $0, %rdi
	syscall
