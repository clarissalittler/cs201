	.section .data
num:	.quad 200,300,400,500
	
	.section .text
	.global _start

	# rip-relative addressing
	# this is a way of calculating
	# the position of the memory you're accessing by where it will be with respect to
	# the instruction pointer
	
_start:
	lea num(%rip),%rbx # lea is the equivalent of the & operator
	# mov num(%rip),%rbx <--- %rbx = num[0]
	# lea num(%rip),%rbx <--- %rbx = num
	# assume var: .quad 1234
	# lea var(%rip),%rbx <---- %rbx = &var
	# this means I've just loaded the pointer
	# into %rbx instead of just the value
	addq $10,(%rbx) # parentheses dereference
	mov num(%rip),%rdi
	mov $60,%rax
	syscall

