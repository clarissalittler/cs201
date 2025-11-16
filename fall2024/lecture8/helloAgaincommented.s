	.section .data
	# Declare a data section, this is where the program will store constant data.

hello:
	# Define a label named "hello" which marks the start of the string.
	.asciz "Hello, World!\n"
	# Use the .asciz directive to define a null-terminated string, 
	# this string is the message we want to print.

	.section .text
	# Declare a text section, this is where the program's instructions are stored.

	.global _start
	# Make the "_start" label globally visible, 
	# this is the entry point of the program.

	.extern .countStr
	# Declare that the function "countStr" is defined in another file 
	# (external) and can be called by this program.

_start:
	# This is the entry point of the program, execution begins here.

	## calculate length
	lea hello(%rip), %rdi
	# Calculate the address of the "hello" string and store it in the %rdi register.
	# This uses the "lea" instruction which calculates the effective address of the label "hello" relative to the instruction pointer (rip).

	call countStr
	# Call the external function "countStr" which likely calculates the length of the string pointed to by %rdi (the "hello" string). 
	# The result of this calculation is stored in the %rax register.

	mov %rax,%rdx
	# Move the length of the string (stored in %rax) to the %rdx register.
	# This is likely setting up for the syscall that will write to the console.

	lea hello(%rip), %rsi
	# Calculate the address of the "hello" string again and store it in the %rsi register.
	# This will be used as the pointer to the string we want to write.

	mov $1, %rax
	# Set the %rax register to 1, this is the system call number for writing to the console.

	mov $1, %rdi
	# Set the %rdi register to 1, this is the file descriptor for the standard output.

	syscall
	# Execute the system call. This will write the string pointed to by %rsi to the console, with the length specified by %rdx.

	mov $60, %rax
	# Set the %rax register to 60, this is the system call number for exiting the program.

	mov $0, %rdi
	# Set the %rdi register to 0, this is the exit code, 0 indicates successful termination.

	syscall
	# Execute the system call, this will exit the program.
