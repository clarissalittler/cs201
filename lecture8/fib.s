	.section .text
	.global _start
	.extern readInt

fib:
	push %rbp
	mov %rsp,%rbp
	sub $8,%rsp

	cmp $1,%rdi
	jle .base

	push %rdi
	dec %rdi
	call fib
	mov %rax,-8(%rbp)
	pop %rdi

	push %rdi
	dec %rdi
	dec %rdi
	call fib
	pop %rdi
	
	add -8(%rbp),%rax
	mov %rbp,%rsp
	pop %rbp
	ret
	
	.base:
	mov %rdi,%rax
	mov %rbp,%rsp
	pop %rbp
	ret
	
	
_start:
	call readInt
	mov %rax,%rdi

	call fib

	mov %rax,%rdi
	mov $60,%rax
	syscall
