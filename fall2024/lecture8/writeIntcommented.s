	.section .bss
	# Allocate 64 bytes of uninitialized memory for the write buffer.
	writeBuff:	.zero 64
	
	.section .text
	# Declare the function as global so it can be called from other files.
	.global writeInt
	# Declare the function `countStr` as external, meaning it is defined elsewhere.
	.extern countStr

	## takes an integer in %rdi
	# This comment describes the function's purpose: it takes an integer as input.
writeInt:
	# Push the value in %rbp onto the stack. This is a standard prologue for a function.
	pushq %rbp
	# Set %rbp to the current value of %rsp. This establishes the function's stack frame.
	movq %rsp, %rbp
	# Save the value of %rdi (the input integer) onto the stack. 
	pushq %rdi
	# Calculate the address of `writeBuff` by adding its offset to the base address of the .bss section.
	movq $.writeBuff, %rsi 
	# Call the `countStr` function, passing the address of `writeBuff` as the first argument (in %rsi)
	# and the integer stored on the stack as the second argument (on the stack).
	call countStr
	# Pop the integer value from the stack. This is the reverse of the `pushq %rdi` instruction.
	popq %rdi
	# Move the address of `writeBuff` into the %rdi register.
	movq $.writeBuff, %rdi
	# Call the system call `write` to print the contents of the write buffer. 
	# `1` specifies the standard output stream (stdout).
	# `%rax` is the return value of the `countStr` function, which is the length of the string in `writeBuff`.
	# `%rdi` holds the address of `writeBuff`.
	movq %rax, %rdx 
	movq $1, %rax
	syscall
	# Restore the original value of %rsp.
	movq %rbp, %rsp
	# Pop the saved value of %rbp from the stack. This is the standard epilogue for a function.
	popq %rbp
	# Return from the function.
	ret
