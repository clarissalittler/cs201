	.section .bss
buffer:	 .zero 128  	# Declare a buffer of 128 bytes in the BSS section. This section is for uninitialized data. 
			# It will be filled with zeroes by the kernel at runtime.

	.section .text
	.global _start 	# Declare the _start label as global, making it the entry point of the program.

_start:
	## we get the data into the buffer
	mov $0,%rax		# Set rax to 0, the system call number for read.
	mov $0,%rdi		# Set rdi to 0, the file descriptor for standard input (stdin).
	lea buffer(%rip),%rsi	# Calculate the address of the buffer and store it in rsi. 
			# This will be the destination of the read data.
	mov $128,%rdx	# Set rdx to 128, the maximum number of bytes to read.
	syscall		# Make the system call to read data from stdin.

	mov %rax,%rbx 		# Store the number of bytes read in rax into rbx.

	## now we need to process it and accumulate the data
	## into a register

	mov $0,%rcx 		# Initialize rcx to 0, this will be our loop counter (i).
	mov $0,%rax		# Initialize rax to 0, this will hold the sum of the numbers.
	lea buffer(%rip),%rdi   	# Calculate the address of the buffer and store it in rdi.
	## the logic of the loop is
	## read the next byte, put it in rax
	## we need to dec %rbx so we don't
	## grab the newline
	dec %rbx		# Decrement rbx by 1, as the newline character is also read, 
			# and we want to avoid processing it.
	.intRead:
	movb (%rdi,%rcx,1),%r8b # Load the byte at the address pointed to by rdi + rcx into r8b.
				# This essentially reads buffer[i] into r8b.
	sub $'0',%r8 		# Subtract the ASCII value of '0' from r8b. This converts 
			# the ASCII digit to its numeric value (e.g., '1' - '0' = 1).
	imul $10,%rax		# Multiply rax (the current sum) by 10, shifting it to the left by one decimal place.
	add %r8,%rax		# Add the numeric value of the read digit to rax.
	inc %rcx 		# Increment rcx (i) by 1 to move to the next byte.
	cmp %rbx,%rcx 		# Compare rcx (i) with rbx (number of bytes read - 1).
	jl .intRead		# If i < number of bytes - 1 (i.e., there are more bytes to process), jump back to .intRead.

	mov %rax,%rdi		# Move the accumulated sum (in rax) to rdi.
	mov $60,%rax		# Set rax to 60, the system call number for exit.
	syscall		# Exit the program.
