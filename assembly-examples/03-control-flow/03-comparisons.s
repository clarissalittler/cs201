	.text
	.global _start

	# Okay so we want a program that
	# will let us test out how the compare operator works
	# in assembly 
_start:
	mov $0,%rdi
	
	cmp $5,%rdi
	jle .less
	mov $-1,%rax
	jmp .finish
.less:
	mov $10,%rax

.finish:
	mov %rax,%rdi
	mov $60,%rax
	syscall
