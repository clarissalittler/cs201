.section .data
# Define a section for data.
vec1:
    .float 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0
    # Define a float array named 'vec1' with values from 1.0 to 16.0
vec2:
    .float 16.0, 15.0, 14.0, 13.0, 12.0, 11.0, 10.0, 9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0
    # Define a float array named 'vec2' with values from 16.0 to 1.0 (reversed)
result:
    .space 64  # Reserve space for 16 floats (16 * 4 bytes)
    # Define a space for the result of the vector addition, large enough to hold 16 floats.

.section .text
# Define a section for executable code.
.global _start
# Declare the _start symbol as global, making it the entry point for the program.
_start:
	vmovups vec1(%rip), %zmm0        # Load vec1 into zmm0
	# Load the entire 'vec1' array into the ZMM0 register, which can hold 16 floats (using the vmovups instruction).
	# %rip is the instruction pointer, used to calculate the address of 'vec1' relative to the current instruction.
	vmovups vec2(%rip), %zmm1        # Load vec2 into zmm1
	# Similar to the previous line, load the entire 'vec2' array into the ZMM1 register.
	vaddps  %zmm1, %zmm0, %zmm2      # Add zmm0 and zmm1, store in zmm2
	# Perform a vector addition operation between the contents of ZMM0 and ZMM1, storing the result in ZMM2.
	# 'vaddps' stands for "vector add packed single precision".
	vmovups %zmm2, result(%rip)      # Store the result
	# Store the contents of ZMM2 (the result of the vector addition) into the 'result' array.
	
	lea result(%rip),%rbx
	# Load the address of 'result' into the RBX register using the 'lea' (load effective address) instruction.
	mov $4,%rcx
	# Move the value 4 into the RCX register. 
	movss (%rbx,%rcx,4),%xmm0
	# Load the fourth element of the 'result' array into XMM0. This is done by calculating the address of the element using RBX + (RCX * 4) (because each float is 4 bytes).
	# 'movss' moves a single precision floating point value.
	cvttss2si %xmm0,%eax
	# Convert the floating point value in XMM0 to an integer and store it in the EAX register.
	mov %eax,%edi
	# Move the integer value from EAX into EDI.
	mov $60,%rax
	# Move the value 60 into the RAX register. This value represents the system call number for 'exit'.
	syscall
	# Execute the 'exit' system call, passing the value in EDI (the integer result) as the exit code.
