	.section .text  	# Start of the text segment, where executable code resides
	.global _start	# Declare the _start symbol as global (entry point for the program)

	#test does logical and
	#so if we do test $1,%register then
	#we effectively check if the number in the register is even by doing a jz, a jump if the result is 0
	#we want to have a program that will put -1 in the register
	#%rdi if the number is even and put 10 in the register if
	#the number is odd
	#this program will have a function that follows the calling
	#convention of putting the result of the function in %rax


_start:
	# assume we ask for input and put the result in %rax
	mov $11,%rax #pretend this is a function call  --  Load the immediate value 11 into register %rax (pretending it's the result of a function call)

	mov %rax,%rdi  	# Move the value in %rax to register %rdi
	# push %rip
	# jmp iseven
	call iseven 	# Call the 'iseven' function, passing the value in %rdi as the argument. The result will be in %rax

	mov $10,%rax #pretend we got $10 from user input  --  Load the immediate value 10 into %rax (pretending it's a user input)

	mov %rax,%rdi	# Move the value in %rax to register %rdi
	call iseven 	# Call the 'iseven' function again, passing the new value in %rdi as the argument. The result will be in %rax
	
	mov %rax,%rdi  	# Move the result from the 'iseven' function (stored in %rax) to %rdi 
	# the last two lines are our return from main
	# (returning the value in %rdi)
	mov $60,%rax 	# Load the system call number for exit into %rax (60)
	syscall 	# Trigger the system call (exiting the program)

iseven:	# this is the function for whether a thing is even
	test $1,%rdi	# Perform a bitwise AND between the value in %rdi and 1. This checks if the least significant bit is set (odd) or not (even). 
	jz .even 		# Jump to the '.even' label if the result of the 'test' instruction was 0 (the number was even).
	jmp .odd		# Jump to the '.odd' label if the result of the 'test' instruction was not 0 (the number was odd).

	.even:		# This label is reached if the number was even
	mov $10,%rax 	# Load the value 10 into register %rax
	ret 		# Return from the function, leaving the value in %rax for the caller

	.odd: 		# This label is reached if the number was odd
	mov $-1,%rax 	# Load the value -1 into register %rax
	ret 		# Return from the function, leaving the value in %rax for the caller
	
	
