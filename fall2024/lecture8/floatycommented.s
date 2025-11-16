	.section .data
	# Declare a floating-point variable named 'float1' and assign the value 1.5 to it.
	float1:	 .float 1.5
	# Declare a floating-point variable named 'float2' and assign the value 2.5 to it.
	float2:	 .float 2.5

	.section .text
	# Declare the '_start' symbol as global, making it the entry point of the program.
	.global _start
	
_start:
	# Load the single-precision floating-point value from the address 'float1' relative to the instruction pointer (%rip) into the XMM0 register.
	movss float1(%rip),%xmm0
	# Add the single-precision floating-point value from the address 'float2' relative to the instruction pointer (%rip) to the value in XMM0, storing the result back in XMM0.
	addss float2(%rip),%xmm0

	# Convert the single-precision floating-point value in XMM0 to a signed integer and store it in the EAX register.
	cvttss2si %xmm0,%eax

	# Move the integer value from EAX to the EDI register.
	mov %eax,%edi
	# Move the value 60 (representing the 'exit' system call) into the RAX register.
	mov $60,%rax
	# Invoke the system call.
	syscall
