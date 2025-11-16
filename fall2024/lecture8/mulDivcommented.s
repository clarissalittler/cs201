	.section .data
# Define two variables 'var1' and 'var2' in the data section.
# Both are initialized to 0 and are of type quad (64-bit integers)
var1:	.quad 0
var2:	.quad 0
	
	.section .text
	# Declare the entry point for the program, '_start'
	.global _start
	# Declare an external function 'write_int' that will be used to print integers
	.extern write_int

_start:
	# Move the value 10000000000 (decimal) into register rax
	mov $10000000000, %rax
	# Move the same value into register rbx
	mov $10000000000, %rbx
	# Multiply rax by rbx, storing the result in rax (rax * rbx -> rax)
	imul %rbx

	# Divide rax by rbx, storing the quotient in rax and the remainder in rdx 
	# (rax / rbx -> rax, remainder -> rdx)
	div %rbx
	
	# Move the remainder from the division (rdx) into the memory location of var1
	mov %rdx,var1(%rip)
	# Move the quotient from the division (rax) into the memory location of var2
	mov %rax,var2(%rip)
	# Move the value stored in var1 into register rdi (this is the argument for write_int)
	mov var1(%rip),%rdi
	# Call the external function 'write_int' to print the value in rdi (which is var1)
	call write_int
	# Move the value stored in var2 into register rdi (this is the argument for write_int)
	mov var2(%rip),%rdi
	# Call the external function 'write_int' to print the value in rdi (which is var2)
	call write_int
	
	# Set rax to 60, which is the system call number for 'exit'
	mov $60,%rax
	# Set rdi to 0, which is the exit code
	mov $0,%rdi
	# Invoke the system call to exit the program
	syscall
