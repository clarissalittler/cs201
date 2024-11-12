	.section .bss
buffer:	 .zero 128  # Allocate 128 bytes of uninitialized memory in the BSS section, named 'buffer'. This will be used to store the input string.

	.section .text
	.global readInt  # Declare a global symbol named 'readInt'. This makes the function visible to other parts of the program.

parseInt:
	.intRead:
	movb (%rdi,%rcx,1),%r8b  # Load the byte at address %rdi + %rcx (offset into the buffer) into the %r8b register.
	sub $'0',%r8  # Subtract the ASCII value of '0' from the byte. This converts the character representation of a digit to its numerical value.
	imul $10,%rax  # Multiply the current accumulated value in %rax by 10. This shifts the existing digits to the left.
	add %r8,%rax  # Add the newly converted digit from %r8 to the accumulated value in %rax.
	inc %rcx  # Increment the loop counter %rcx.
	cmp %rsi,%rcx  # Compare the loop counter %rcx with the end pointer %rsi. This checks if we have processed all digits in the input string.
	jl .intRead  # Jump back to the '.intRead' label if %rcx is less than %rsi, continuing the loop.
	ret  # Return from the 'parseInt' function.

readInt:	
	## we get the data into the buffer
	mov $0,%rax  # Set %rax to 0. This is the system call number for 'read'.
	mov $0,%rdi  # Set %rdi to 0. This is the file descriptor for standard input (stdin).
	lea buffer(%rip),%rsi  # Calculate the address of the 'buffer' and store it in %rsi. This is the address where the input string will be stored.
	mov $128,%rdx  # Set %rdx to 128. This is the maximum number of bytes to read from the input.
	syscall  # Execute the system call 'read' to read data from stdin into the 'buffer'.
	
	mov %rax,%rbx 		#how many bytes were read
	# Store the return value of the 'read' system call (number of bytes read) in %rbx.

	## now we need to process it and accumulate the data
	## into a register

	mov $0,%rcx  # Initialize the loop counter %rcx to 0.
	mov $0,%rax  # Initialize the accumulator register %rax to 0. This will hold the final integer value.
	lea buffer(%rip),%rdi  # Load the address of the 'buffer' into %rdi.
	dec %rbx  # Decrement the number of bytes read (%rbx) by 1. This is because we're starting from the first character (which might be a newline).
	mov %rbx,%rsi  # Copy the number of bytes read (%rbx) to %rsi. This is the end pointer for the input string.
	call parseInt  # Call the 'parseInt' function to process the digits in the 'buffer' and accumulate them in %rax.
	ret  # Return from the 'readInt' function.
