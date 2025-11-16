	.text  	  	#  Declare the following code as part of the text segment. 
	.global _start	#  Declare _start as a global symbol. This is the entry point for the program.

	# Okay so we want a program that
	# will let us test out how the compare operator works
	# in assembly 
_start:
	mov $0,%rdi	#  Move the immediate value 0 into the register %rdi.  
					#  %rdi is used to store the first argument passed to a function.
					#  We'll use %rdi to hold the value we want to compare.

	cmp $5,%rdi	#  Compare the value in %rdi (which is 0) with the immediate value 5.
					#  This instruction sets flags based on the comparison.
					#  The flags are used by conditional jump instructions like jle.

	jle .less	#  Jump to the label .less if the value in %rdi is less than or equal to 5.
					#  The 'jle' instruction stands for "jump if less than or equal to".
					#  If the comparison is true, the program will jump to the .less section.

	mov $-1,%rax	#  Move the immediate value -1 into the register %rax.
					#  %rax is used to store the return value of a function.
					#  Here, we're setting the return value to -1 if the value in %rdi is greater than 5.

	jmp .finish	#  Jump to the label .finish. 
					#  This is an unconditional jump, always executed after the mov instruction above.

.less:			#  This is the target label for the jle instruction.
	mov $10,%rax	#  Move the immediate value 10 into the register %rax.
					#  This sets the return value to 10 if the value in %rdi is less than or equal to 5.

.finish:		#  This label marks the end of the program's logic.
	mov %rax,%rdi	#  Move the value in %rax (our return value) into %rdi.
					#  %rdi is used as the first argument for the syscall instruction.

	mov $60,%rax	#  Move the immediate value 60 into the register %rax.
					#  This sets the syscall number to 60, which is the system call for exit.

	syscall		#  Execute the system call.
					#  The program terminates with the value in %rdi as its exit code.
