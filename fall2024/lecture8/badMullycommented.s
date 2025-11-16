	.section .data
	# Define a variable named 'num1' and assign the value 6 to it.
	# '.quad' is used to define a 64-bit integer value.
num1:	 .quad 6
	# Define an array named 'numArr' with elements 1, 2, 3, and 4.
	# Each element is a 64-bit integer.
numArr:  .quad 1,2,3,4

	.section .text
	# Declare the '_start' label as a global symbol.
	# The '_start' label is the entry point of the program.
	.global _start

_start:
	# Move the value 10 to the register %rax.
	# This value will be used later in the 'syscall' instruction.
	mov $10,%rax
	# Move the value 2 to the register %rcx.
	# This value represents the index of the element in the array 'numArr' that we want to access.
	mov $2,%rcx
	# Calculate the address of the array 'numArr' relative to the current instruction pointer (%rip).
	# The result is stored in the register %rdi.
	lea numArr(%rip),%rdi
	# Multiply the value at the address pointed to by %rdi + (%rcx * 8) with the value in %rax.
	# The result is stored in %rax.
	# This instruction is equivalent to accessing the third element of the array 'numArr' (index 2) and multiplying it by 10.
	imul (%rdi,%rcx,8),%rax

	# Move the result from %rax to %rdi.
	# This is done because the 'syscall' instruction expects the result in %rdi.
	mov %rax,%rdi

	# Move the value 60 to %rax.
	# This value represents the system call number for 'exit'.
	mov $60,%rax
	# Perform the system call.
	# In this case, it will terminate the program with the exit code specified in %rdi.
	syscall
