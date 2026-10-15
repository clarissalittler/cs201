	.section .text
	.global _start

	# f(x) = a + 2*x
	# a is going to be a local variable with value 20
	# we're going to store the computation 2*x in
	# another local variable

	# %rbp points to the base of the current stack frame
	# so if the current frame starts at address 100
	# then that's the value that should be in %rbp
	# %rsp points to the *top* of the current stack frame
	# so if the current stack frame is 64 bytes in size
	# then %rsp is going to have the value 36 bytes
	
	
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
	mov %rdi,-8(%rbp) # store %rdi into the local variable x
	add %rdi,-8(%rbp) # double the local variable x
	movq $20,-16(%rbp) # store 20 in the local variable a
	movq -16(%rbp),%rax # put a in rax
	add -8(%rbp),%rax # add 2x to it
	# here's our leaving idiom
	mov %rbp,%rsp
	pop %rbp
	ret

	
_start:
	mov $10,%rdi #set up argument to function
	call fun

	mov %rax,%rdi
	mov $60,%rax
	syscall
	
