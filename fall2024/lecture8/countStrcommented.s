	.section .text  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  # Declares a section named ".text" which is where executable code resides.
	.global countStr  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  # Declares the label "countStr" as a global symbol, making it accessible from other code files.

	## takes an address in %rdi
	## will count until it hits a null
countStr:  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  # This is the definition of the function "countStr". It takes a single argument in the %rdi register.
	mov $0,%rcx  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  # Initializes the %rcx register (which will be used as a counter) to 0.
	.loopStart:  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  # Defines a label called ".loopStart" which marks the beginning of the loop.
	movb (%rdi,%rcx,1),%r8b  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  # Loads the byte at the address pointed to by %rdi + %rcx (which is the current position) into the %r8b register.
	cmp $0,%r8b  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  # Compares the value in %r8b (the current byte) to 0.
	je .end  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  # If the value in %r8b is equal to 0 (null character), jump to the .end label.
	inc %rcx  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  # If the value is not null, increment the counter in %rcx by 1.
	jmp .loopStart  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  # Jump back to the .loopStart label to process the next byte.
	.end:  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  # This label marks the end of the loop.
	mov %rcx,%rax  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  # Moves the final value of the counter in %rcx into the %rax register, which is the return value of the function.
	ret  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  # Returns from the function.
