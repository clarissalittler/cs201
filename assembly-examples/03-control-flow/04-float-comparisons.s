	.section .text
	.global _start

	#write a program that puts 10 in %rdi
	#if a 10 < 5
	#else it puts -1 in %rdi
_start:
	mov $10,%rcx
	cmp $5,%rcx # cmp x,y => compares (y-x) and sets flags for if they are equal or if the result is positive
	jl .good
	mov $-1,%rdi
	jmp .finish

.good:
	mov $10,%rdi
.finish:
	mov $60,%rax
	syscall
	
