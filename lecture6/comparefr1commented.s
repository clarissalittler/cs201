	.section .text  	# This directive tells the assembler that the following code belongs to the text section. 
                        	# The text section typically contains executable instructions.
	.global _start		# This directive declares the symbol '_start' as global. 
                        	# This means that the symbol can be referenced from other files or programs.
                        	# '_start' is the entry point for the program, where execution begins. 

	#write a program that puts 10 in %rdi
	#if a 10 < 5
	#else it puts -1 in %rdi
_start: 		    	# The program begins execution at this label '_start'.
	mov $10,%rcx 		# Move the immediate value 10 into the register %rcx.  This is the value we'll be comparing.
	cmp $5,%rcx 		# Compare the value in %rcx (which is 10) with the immediate value 5. 
                        	# The 'cmp' instruction sets the flags (ZF, SF, PF, CF) based on the comparison. 
                        	#  ZF (Zero Flag): Set if the values are equal (y-x == 0).
                        	#  SF (Sign Flag): Set if the result is negative (y-x < 0).
                        	#  PF (Parity Flag): Set if the result has an even number of 1 bits.
                        	#  CF (Carry Flag): Set if there was a borrow (y-x < 0).
	jl .good 		    # Jump to the label '.good' if the comparison resulted in a value less than (JL: jump if less).
                        	# In this case, the jump is executed because 10 is NOT less than 5.
	mov $-1,%rdi 		# Move the immediate value -1 into the register %rdi.  This is the value to be used if the condition is not met.
	jmp .finish 		# Unconditionally jump to the label '.finish'.

.good: 			    	# This label marks the code block to be executed if the jump from 'cmp' was successful.
	mov $10,%rdi 		# Move the immediate value 10 into the register %rdi. This is the value to be used if the condition is met.
.finish:			    	# This label marks the final step of the program.
	mov $60,%rax 		# Move the system call number for exit (60) into register %rax.
	syscall 			    	# Execute the system call. 
