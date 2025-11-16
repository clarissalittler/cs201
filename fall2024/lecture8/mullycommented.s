	.section .data
	# Declare a variable named 'num1' and initialize it with the value 6 (as a quad word - 64 bits).
	num1:	 .quad 6
	# Declare an array named 'numArr' and initialize it with the values 1, 2, 3, and 4 (as quad words - 64 bits).
	numArr:  .quad 1,2,3,4

	.section .text
	# Declare the '_start' label as a global symbol, making it accessible from outside the program.
	.global _start

_start:
	# Move the value 10 into the rax register.
	mov $10,%rax
	# Multiply the value in the rax register (10) by the value at the address pointed to by 'num1' (which is 6).
	# Store the result in the rax register.
	# The 'num1(%rip)' syntax gets the address of 'num1' relative to the instruction pointer (rip).
	imul num1(%rip),%rax

	# Move the value in the rax register (10 * 6 = 60) to the rdi register.
	mov %rax,%rdi

	# Move the value 60 (which is the system call number for exit) into the rax register.
	mov $60,%rax
	# Initiate the system call with the value in rax (60) as the syscall number, and the value in rdi (60) as the exit code.
	syscall 
