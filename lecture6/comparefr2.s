	.section .text
	.global _start

	#test does logical and
	#so if we do test $1,%register then
	#we effectively check if the number in the
	#register is even by doing a jz, a jump if the result is 0
	#we want to have a program that will put -1 in the register
	#%rdi if the number is even and put 10 in the register if
	#the number is odd
	#this program will have a function that follows the calling
	#convention of putting the result of the function in %rax


_start:
	# assume we ask for input and put the result in %rax
	mov $11,%rax #pretend this is a function call

	mov %rax,%rdi
	# push %rip
	# jmp iseven
	call iseven

	mov $10,%rax #pretend we got $10 from user input

	mov %rax,%rdi
	call iseven
	
	mov %rax,%rdi
	# the last two lines are our return from main
	# (returning the value in %rdi)
	mov $60,%rax
	syscall

iseven:	# this is the function for whether a thing is even
	test $1,%rdi
	jz .even
	jmp .odd

	.even:
	mov $10,%rax
	ret

	.odd:
	mov $-1,%rax
	ret
	
	
