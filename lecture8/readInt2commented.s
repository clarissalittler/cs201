	.section .bss
buffer:	 .zero 128  	# Declare a buffer named "buffer" in the BSS section.
			# It is 128 bytes in size and will be initialized to 0.
			# This buffer will store the input data from the user.

	.section .text
	.global _start	# Declare the entry point of the program "_start".
			# This symbol needs to be globally visible for the kernel.

parseInt:		# Define a function named "parseInt".
			# It takes a string as input and converts it to an integer.
	.intRead:		# This label marks the beginning of a loop to read digits.
	movb (%rdi,%rcx,1),%r8b	# Read the next character from the input string.
			# %rdi contains the address of the string.
			# %rcx is a loop counter.
			# %r8b stores the character read.
	sub $'0',%r8	# Convert the character to its numerical value.
			# Subtract the ASCII value of '0' (48) to get the integer value.
	imul $10,%rax	# Multiply the accumulated integer value by 10.
			# %rax stores the result of the integer conversion.
	add %r8,%rax	# Add the new digit to the accumulated integer.
	inc %rcx	# Increment the loop counter.
	cmp %rsi,%rcx	# Compare the loop counter to the length of the input string.
			# %rsi contains the address of the end of the string.
	jl .intRead	# Jump to the beginning of the loop if the loop counter is less than the string length.
	ret		# Return from the function. The integer result is in %rax.

	
_start:		# This is the entry point of the program.
	## we get the data into the buffer
	mov $0,%rax	# Set the system call number to 0 (read).
	mov $0,%rdi	# Set the file descriptor to 0 (standard input).
	lea buffer(%rip),%rsi	# Set the address of the buffer to store the input.
	mov $128,%rdx	# Set the maximum number of bytes to read to 128.
	syscall		# Call the system call to read data from the user.
			# The result (number of bytes read) is stored in %rax.
	
	mov %rax,%rbx 		# Store the number of bytes read in %rbx.
			# This value will be used later.

	## now we need to process it and accumulate the data
	## into a register

	mov $0,%rcx	# Initialize the loop counter to 0.
	mov $0,%rax	# Initialize the accumulated integer value to 0.
	lea buffer(%rip),%rdi	# Set the address of the buffer to the start of the input string.
	dec %rbx	# Decrement the number of bytes read.
			# The loop should iterate only up to the last byte read.
	mov %rbx,%rsi	# Set the address of the end of the input string.
	call parseInt	# Call the "parseInt" function to convert the input string to an integer.
			# The result of the conversion will be stored in %rax.
	
	mov %rax,%rdi	# Move the integer result to %rdi for the exit system call.
	mov $60,%rax	# Set the system call number to 60 (exit).
	syscall		# Call the system call to exit the program.
