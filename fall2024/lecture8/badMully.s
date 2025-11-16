	.section .data
num1:	 .quad 6
numArr:  .quad 1,2,3,4

	.section .text
	.global _start

_start:
	mov $10,%rax
	mov $2,%rcx
	lea numArr(%rip),%rdi
	imul (%rdi,%rcx,8),%rax

	mov %rax,%rdi

	mov $60,%rax
	syscall
