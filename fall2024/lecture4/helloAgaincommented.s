	.file	"helloAgain.c"  // This line indicates the name of the source file (helloAgain.c) from which this assembly code was generated.

	.text  // This directive signifies that the following instructions are part of the code segment, which contains executable instructions.

	.section	.rodata  // This directive declares a new section named ".rodata". This section holds read-only data, such as string literals.

.LC0:
	.string	"Hello, World"  // This line defines a string literal "Hello, World" and labels it as .LC0. This label is used to refer to the string later.

	.text  // Switch back to the code segment.

	.globl	main  // This directive declares the symbol "main" as global, meaning it can be referenced from other files. "main" is the entry point of the program.

	.type	main, @function  // This directive specifies that "main" is a function.

main:
.LFB0:
	.cfi_startproc  // This directive indicates the start of a function, used for debugging information.

	pushq	%rbp  // Push the current value of the base pointer (rbp) onto the stack.
	.cfi_def_cfa_offset 16  // Adjust the current frame pointer offset for debugging purposes.
	.cfi_offset 6, -16  // Specify the offset of the old base pointer (rbp) relative to the current frame pointer.

	movq	%rsp, %rbp  // Move the stack pointer (rsp) to the base pointer (rbp), setting up the function's stack frame.
	.cfi_def_cfa_register 6  // Update the frame pointer register for debugging.

	movl	$.LC0, %edi  // Move the address of the string literal .LC0 into the register %edi. This will be used as the first argument to the printf function.

	movl	$0, %eax  // Move the value 0 into the register %eax. This will be used as the return value for the printf function.

	call	printf  // Call the printf function. printf will print the string at the address stored in %edi.

	movl	$0, %eax  // Set the return value of the main function to 0, indicating successful execution.

	popq	%rbp  // Restore the base pointer (rbp) by popping the saved value from the stack.

	.cfi_def_cfa 7, 8  // Restore the frame pointer for debugging purposes.

	ret  // Return from the main function.

	.cfi_endproc  // End of the function definition for debugging information.

.LFE0:
	.size	main, .-main  // Specify the size of the main function in bytes.

	.ident	"GCC: (GNU) 8.5.0 20210514 (Red Hat 8.5.0-22.0.1)"  // This line identifies the compiler and version used to generate this assembly code.

	.section	.note.GNU-stack,"",@progbits  // This directive specifies that the program uses the GNU stack format.
