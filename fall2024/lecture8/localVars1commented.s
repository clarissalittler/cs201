	.section .data
message1:	.asciz "Enter an integer, will ya: " # 27  
	# This line defines a string literal named "message1" in the data section. 
	# The ".asciz" directive specifies that it's a null-terminated ASCII string. 
	# The comment "27" indicates the length of the string.

message2:	.asciz "Enter another integer: "     # 23
	# Similar to the previous line, this defines another string literal 
	# named "message2" with its length specified as a comment.

	.section .text
	.global _start  
	# These lines mark the beginning of the text section where the code will reside. 
	# The ".global _start" directive makes the "_start" label visible to the linker.
	# This is the entry point of the program.

	.extern readInt  
	# This line declares the "readInt" function as an external symbol. 
	# This means the code is using a function defined elsewhere (likely in another file).

	## int sillyFun(){
	##   int num1;
	##   int num2;
	##   printf(message1)
	##   num1 = readInt()
	##   printf(message2)
	##   num2 = readInt()
	##   return (num1 + num2)
	## }
	
sillyFun: 
	push %rbp 		# we save the OLD base pointer
	# This instruction pushes the current value of the base pointer (%rbp) 
	# onto the stack. This is done to preserve the caller's stack frame.

	movq %rsp,%rbp
	# This instruction moves the value of the stack pointer (%rsp) into the base pointer (%rbp).
	# This establishes a new stack frame for the "sillyFun" function.

	sub $16,%rsp
	# This instruction subtracts 16 bytes from the stack pointer (%rsp).
	# This allocates space on the stack for local variables in the "sillyFun" function.
	# In this case, it allocates space for two integer variables (each 8 bytes).

	## write the first message
	mov $1, %rax
	# This instruction moves the value 1 into the %rax register.
	# This is the system call number for writing to standard output (stdout).

	mov $1, %rdi
	# This instruction moves the value 1 into the %rdi register.
	# This is the file descriptor for standard output (stdout).

	lea message1(%rip),%rsi
	# This instruction calculates the address of the "message1" string literal 
	# relative to the current instruction pointer (%rip) and stores it in the %rsi register.
	# This sets up the address of the string to be printed.

	mov $27,%rdx
	# This instruction moves the value 27 into the %rdx register. 
	# This specifies the length of the string "message1" to be printed.

	syscall
	# This instruction makes a system call to print the string "message1" to standard output.

	## read the first int
	call readInt
	# This instruction calls the "readInt" function, which is expected to read an integer from 
	# standard input and return the value in the %rax register.

	mov %rax,-8(%rbp)
	# This instruction moves the value returned by "readInt" (stored in %rax) into the 
	# memory location pointed to by %rbp - 8. This effectively assigns the read integer 
	# to the local variable "num1".

	## write the second message
	mov $1, %rax
	# Same as before, this sets the system call number for writing to stdout.

	mov $1, %rdi
	# Same as before, this sets the file descriptor for stdout.

	lea message2(%rip),%rsi
	# Same as before, this sets the address of the "message2" string to be printed.

	mov $23,%rdx
	# Same as before, this sets the length of the "message2" string.

	syscall
	# This instruction makes the system call to print the string "message2" to stdout.

	## read the first int
	call readInt
	# This instruction calls the "readInt" function again to read another integer 
	# from standard input and store it in %rax.

	mov %rax,-16(%rbp)
	# This instruction moves the value returned by "readInt" into the memory location
	# pointed to by %rbp - 16, effectively assigning the read integer to the local variable "num2".

	mov -8(%rbp),%rcx
	# This instruction moves the value of "num1" (stored at %rbp - 8) into the %rcx register.

	mov -16(%rbp),%rbx
	# This instruction moves the value of "num2" (stored at %rbp - 16) into the %rbx register.

	mov $0,%rax
	# This instruction initializes the %rax register to 0. This will be used to accumulate the sum.

	add %rcx,%rax
	# This instruction adds the value of "num1" (in %rcx) to the sum (in %rax).

	add %rbx,%rax
	# This instruction adds the value of "num2" (in %rbx) to the sum (in %rax).

	movq %rbp,%rsp
	# This instruction moves the value of the base pointer (%rbp) back into the stack pointer (%rsp).
	# This restores the stack pointer to its original position before entering the function, effectively releasing
	# the stack space allocated for local variables.

	pop %rbp
	# This instruction pops the value of the old base pointer from the stack and restores it to %rbp.
	# This restores the caller's stack frame.

	ret
	# This instruction returns control back to the caller. The value in %rax is the return value of the "sillyFun" function.

	
_start:
	call sillyFun
	# This instruction calls the "sillyFun" function. The return value will be stored in %rax.

	mov %rax,%rdi
	# This instruction moves the return value of "sillyFun" (in %rax) into the %rdi register.
	# This is preparing for the next system call.

	mov $60,%rax
	# This instruction moves the value 60 into the %rax register. This is the system call number for exiting the program.

	syscall
	# This instruction makes the system call to exit the program. The value in %rdi is the exit code.
