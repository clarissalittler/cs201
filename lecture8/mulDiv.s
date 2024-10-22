	.section .data
var1:	.quad 0
var2:	.quad 0
	
	.section .text
	.global _start
	.extern write_int

_start:
	mov $10000000000, %rax
	mov $10000000000, %rbx
	imul %rbx

	div %rbx
	
	mov %rdx,var1(%rip)
	mov %rax,var2(%rip)
	mov var1(%rip),%rdi
	call write_int
	mov var2(%rip),%rdi
	call write_int
	
	mov $60,%rax
	mov $0,%rdi
	syscall
