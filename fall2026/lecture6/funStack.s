	.section .text
	.global _start

	# f(x) = a + 2*x
	# a is going to be a local variable with value 20
	# we're going to store the computation 2*x in
	# another local variable
	
fun:
	# the idiom for creating a stack frame
	# save the old base pointer with a push
	push %rbp
	# now the bottom of our new frame should be the TOP
	# of the current stack i.e. %rsp
	mov %rsp,%rbp
	# now we make a new frame with room for two variables
	# let's assume these are two longs
	# so that's 16 bytes
	sub $16,%rsp
	mov %rdi,-8(%rbp)
	add %rdi,-8(%rbp)
	movq $20,-16(%rbp)
	movq -16(%rbp),%rax
	add -8(%rbp),%rax
	mov %rbp,%rsp
	pop %rbp
	ret

	
_start:
	mov $10,%rdi #set up argument to function
	call fun

	mov %rax,%rdi
	mov $60,%rax
	syscall
	
