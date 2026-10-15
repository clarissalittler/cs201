	.section .data
arr:	.quad 1,2,3,4

	.section .text
	.global _start
# the fact that this assembles means the gods have abandonded us
	
_start:
	jmp arr
	
	mov $60,%rax
	mov $0,%rdi
	syscall
