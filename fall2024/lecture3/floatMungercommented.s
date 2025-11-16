	.file	"floatMunger.c"
	.text
	// This line declares the start of the text segment, which contains the executable code of the program.
	.globl	toggle
	// This line declares the function 'toggle' as global, so it can be called from other files.
	.type	toggle, @function
	// This line specifies the type of 'toggle' as a function.
toggle:
.LFB6:
	// This label marks the beginning of the function 'toggle'.
	.cfi_startproc
	// This directive indicates the beginning of a procedure for CFI (Call Frame Information), used for stack unwinding in case of exceptions.
	pushq	%rbp
	// Push the value of the register %rbp onto the stack, saving the old stack frame's base pointer.
	.cfi_def_cfa_offset 16
	// Define the offset of the canonical frame address (CFA) to be 16 bytes. CFA points to the beginning of the function's stack frame.
	.cfi_offset 6, -16
	// Specify that register %rbp is located 16 bytes below the CFA.
	movq	%rsp, %rbp
	// Move the value of the register %rsp (stack pointer) into %rbp, setting up the new stack frame's base pointer.
	.cfi_def_cfa_register 6
	// Define the CFA register to be %rbp.
	movq	%rdi, -24(%rbp)
	// Move the value of the register %rdi (first function argument) into the memory location at -24 bytes relative to the frame pointer.
	movl	%esi, -28(%rbp)
	// Move the value of the register %esi (second function argument) into the memory location at -28 bytes relative to the frame pointer.
	movl	-28(%rbp), %eax
	// Move the value stored at -28(%rbp) into the register %eax.
	movl	$1, %edx
	// Move the immediate value 1 into the register %edx.
	movl	%eax, %ecx
	// Move the value in %eax into %ecx.
	sall	%cl, %edx
	// Left shift the value in %edx by the number of bits specified by the lower byte of %ecx. This effectively multiplies %edx by 2 raised to the power of the value in %cl.
	movl	%edx, %eax
	// Move the value in %edx into %eax.
	movl	%eax, -4(%rbp)
	// Move the value in %eax into the memory location at -4 bytes relative to the frame pointer.
	movq	-24(%rbp), %rax
	// Move the value stored at -24(%rbp) into the register %rax.
	movl	(%rax), %eax
	// Move the value at the memory location pointed to by %rax into the register %eax.
	xorl	-4(%rbp), %eax
	// Bitwise XOR the value in %eax with the value at -4(%rbp). This effectively flips the bit at the specified position.
	movl	%eax, %edx
	// Move the value in %eax into %edx.
	movq	-24(%rbp), %rax
	// Move the value stored at -24(%rbp) into the register %rax.
	movl	%edx, (%rax)
	// Move the value in %edx into the memory location pointed to by %rax, updating the original value.
	nop
	// This instruction does nothing, often used as a placeholder.
	popq	%rbp
	// Pop the value at the top of the stack into %rbp, restoring the old stack frame's base pointer.
	.cfi_def_cfa 7, 8
	// Define the CFA to be %rbp + 8.
	ret
	// Return from the function.
	.cfi_endproc
.LFE6:
	// This label marks the end of the function 'toggle'.
	.size	toggle, .-toggle
	// This directive specifies the size of the function 'toggle'.
	.globl	isSet
	// This line declares the function 'isSet' as global, so it can be called from other files.
	.type	isSet, @function
	// This line specifies the type of 'isSet' as a function.
isSet:
.LFB7:
	// This label marks the beginning of the function 'isSet'.
	.cfi_startproc
	// This directive indicates the beginning of a procedure for CFI (Call Frame Information), used for stack unwinding in case of exceptions.
	pushq	%rbp
	// Push the value of the register %rbp onto the stack, saving the old stack frame's base pointer.
	.cfi_def_cfa_offset 16
	// Define the offset of the canonical frame address (CFA) to be 16 bytes. CFA points to the beginning of the function's stack frame.
	.cfi_offset 6, -16
	// Specify that register %rbp is located 16 bytes below the CFA.
	movq	%rsp, %rbp
	// Move the value of the register %rsp (stack pointer) into %rbp, setting up the new stack frame's base pointer.
	.cfi_def_cfa_register 6
	// Define the CFA register to be %rbp.
	movq	%rdi, -8(%rbp)
	// Move the value of the register %rdi (first function argument) into the memory location at -8 bytes relative to the frame pointer.
	movl	%esi, -12(%rbp)
	// Move the value of the register %esi (second function argument) into the memory location at -12 bytes relative to the frame pointer.
	movq	-8(%rbp), %rax
	// Move the value stored at -8(%rbp) into the register %rax.
	movl	(%rax), %edx
	// Move the value at the memory location pointed to by %rax into the register %edx.
	movl	-12(%rbp), %eax
	// Move the value stored at -12(%rbp) into the register %eax.
	movl	%eax, %ecx
	// Move the value in %eax into %ecx.
	sarl	%cl, %edx
	// Right shift the value in %edx by the number of bits specified by the lower byte of %ecx. This effectively divides %edx by 2 raised to the power of the value in %cl.
	movl	%edx, %eax
	// Move the value in %edx into %eax.
	andl	$1, %eax
	// Bitwise AND the value in %eax with the immediate value 1. This extracts the least significant bit, indicating whether the specified bit was set.
	popq	%rbp
	// Pop the value at the top of the stack into %rbp, restoring the old stack frame's base pointer.
	.cfi_def_cfa 7, 8
	// Define the CFA to be %rbp + 8.
	ret
	// Return from the function.
	.cfi_endproc
.LFE7:
	// This label marks the end of the function 'isSet'.
	.size	isSet, .-isSet
	// This directive specifies the size of the function 'isSet'.
	.section	.rodata
	// This directive declares the start of the read-only data section.
.LC0:
	// This label marks the beginning of a string literal in the read-only data section.
	.string	"%d"
	// The string literal "%d", used for formatted output.
	.text
	// This line declares the start of the text segment, which contains the executable code of the program.
	.globl	printBits
	// This line declares the function 'printBits' as global, so it can be called from other files.
	.type	printBits, @function
	// This line specifies the type of 'printBits' as a function.
printBits:
.LFB8:
	// This label marks the beginning of the function 'printBits'.
	.cfi_startproc
	// This directive indicates the beginning of a procedure for CFI (Call Frame Information), used for stack unwinding in case of exceptions.
	pushq	%rbp
	// Push the value of the register %rbp onto the stack, saving the old stack frame's base pointer.
	.cfi_def_cfa_offset 16
	// Define the offset of the canonical frame address (CFA) to be 16 bytes. CFA points to the beginning of the function's stack frame.
	.cfi_offset 6, -16
	// Specify that register %rbp is located 16 bytes below the CFA.
	movq	%rsp, %rbp
	// Move the value of the register %rsp (stack pointer) into %rbp, setting up the new stack frame's base pointer.
	.cfi_def_cfa_register 6
	// Define the CFA register to be %rbp.
	subq	$32, %rsp
	// Subtract 32 bytes from the stack pointer to allocate space for local variables.
	movq	%rdi, -24(%rbp)
	// Move the value of the register %rdi (first function argument) into the memory location at -24 bytes relative to the frame pointer.
	movl	$31, -4(%rbp)
	// Move the immediate value 31 into the memory location at -4 bytes relative to the frame pointer. This initializes a counter variable for iterating through bits.
	jmp	.L5
	// Jump to the label .L5, starting the bit-checking loop.
.L6:
	// This label marks the beginning of the loop body.
	movl	-4(%rbp), %edx
	// Move the value stored at -4(%rbp) (the bit counter) into the register %edx.
	movq	-24(%rbp), %rax
	// Move the value stored at -24(%rbp) (the input number) into the register %rax.
	movl	%edx, %esi
	// Move the value in %edx into the register %esi.
	movq	%rax, %rdi
	// Move the value in %rax into the register %rdi.
	call	isSet
	// Call the 'isSet' function, passing the input number and bit position as arguments.
	movl	%eax, %esi
	// Move the return value of 'isSet' into the register %esi.
	movl	$.LC0, %edi
	// Move the address of the string literal "%d" into the register %edi.
	movl	$0, %eax
	// Move the immediate value 0 into the register %eax.
	call	printf
	// Call the 'printf' function to print the value in %esi using the format specified by the string literal "%d".
	subl	$1, -4(%rbp)
	// Decrement the value stored at -4(%rbp) (the bit counter) by 1.
.L5:
	// This label marks the beginning of the loop condition check.
	cmpl	$0, -4(%rbp)
	// Compare the value stored at -4(%rbp) (the bit counter) with the immediate value 0.
	jns	.L6
	// Jump to the label .L6 (loop body) if the bit counter is not less than zero.
	nop
	// This instruction does nothing, often used as a placeholder.
	leave
	// Restore the old stack frame by popping the saved value of %rbp into %rbp and then adjusting the stack pointer.
	.cfi_def_cfa 7, 8
	// Define the CFA to be %rbp + 8.
	ret
	// Return from the function.
	.cfi_endproc
.LFE8:
	// This label marks the end of the function 'printBits'.
	.size	printBits, .-printBits
	// This directive specifies the size of the function 'printBits'.
	.section	.rodata
	// This directive declares the start of the read-only data section.
	.align 8
	// This directive ensures the following data is aligned to an 8-byte boundary.
.LC2:
	// This label marks the beginning of a string literal in the read-only data section.
	.string	"The bits set in the number %f are:\n"
	// The string literal "The bits set in the number %f are:\n", used for formatted output.
	.text
	// This line declares the start of the text segment, which contains the executable code of the program.
	.globl	main
	// This line declares the function 'main' as global, so it can be called from other files.
	.type	main, @function
	// This line specifies the type of 'main' as a function.
main:
.LFB9:
	// This label marks the beginning of the function 'main'.
	.cfi_startproc
	// This directive indicates the beginning of a procedure for CFI (Call Frame Information), used for stack unwinding in case of exceptions.
	pushq	%rbp
	// Push the value of the register %rbp onto the stack, saving the old stack frame's base pointer.
	.cfi_def_cfa_offset 16
	// Define the offset of the canonical frame address (CFA) to be 16 bytes. CFA points to the beginning of the function's stack frame.
	.cfi_offset 6, -16
	// Specify that register %rbp is located 16 bytes below the CFA.
	movq	%rsp, %rbp
	// Move the value of the register %rsp (stack pointer) into %rbp, setting up the new stack frame's base pointer.
	.cfi_def_cfa_register 6
	// Define the CFA register to be %rbp.
	subq	$16, %rsp
	// Subtract 16 bytes from the stack pointer to allocate space for local variables.
	movss	.LC1(%rip), %xmm0
	// Move the single-precision floating-point value stored at the address .LC1(%rip) into the XMM register %xmm0.
	movss	%xmm0, -4(%rbp)
	// Move the value in %xmm0 into the memory location at -4 bytes relative to the frame pointer.
	movss	-4(%rbp), %xmm0
	// Move the value stored at -4(%rbp) into the XMM register %xmm0.
	cvtss2sd	%xmm0, %xmm0
	// Convert the single-precision floating-point value in %xmm0 to a double-precision floating-point value and store the result back in %xmm0.
	movl	$.LC2, %edi
	// Move the address of the string literal "The bits set in the number %f are:\n" into the register %edi.
	movl	$1, %eax
	// Move the immediate value 1 into the register %eax.
	call	printf
	// Call the 'printf' function to print the string literal "The bits set in the number %f are:\n" followed by the double-precision floating-point value in %xmm0.
	leaq	-4(%rbp), %rax
	// Calculate the address of the memory location at -4 bytes relative to the frame pointer and store it in the register %rax.
	movq	%rax, %rdi
	// Move the value in %rax into the register %rdi.
	call	printBits
	// Call the 'printBits' function, passing the address of the input number as an argument.
	movl	$10, %edi
	// Move the immediate value 10 (ASCII code for newline character) into the register %edi.
	call	putchar
	// Call the 'putchar' function to print the newline character.
	movl	$0, %eax
	// Move the immediate value 0 into the register %eax, indicating successful program termination.
	leave
	// Restore the old stack frame by popping the saved value of %rbp into %rbp and then adjusting the stack pointer.
	.cfi_def_cfa 7, 8
	// Define the CFA to be %rbp + 8.
	ret
	// Return from the function.
	.cfi_endproc
.LFE9:
	// This label marks the end of the function 'main'.
	.size	main, .-main
	// This directive specifies the size of the function 'main'.
	.section	.rodata
	// This directive declares the start of the read-only data section.
	.align 4
	// This directive ensures the following data is aligned to a 4-byte boundary.
.LC1:
	// This label marks the beginning of a single-precision floating-point value in the read-only data section.
	.long	1094713344
	// The single-precision floating-point value 10.0, represented in IEEE 754 format.
	.ident	"GCC: (GNU) 8.5.0 20210514 (Red Hat 8.5.0-22.0.1)"
	// This line provides information about the compiler used to generate the code.
	.section	.note.GNU-stack,"",@progbits
	// This section contains information related to the GNU stack, ensuring proper stack behavior.
