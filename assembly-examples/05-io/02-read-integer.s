	.section .bss
buffer:	 .zero 128

	.section .text
	.global readInt

# Function: readInt
# ------------------
# Reads an integer from standard input and returns it.
#
# This function reads a line of text from stdin, converts the ASCII
# string to a binary integer using Horner's method (multiply by 10
# and add each digit).
#
# Inputs: None
# Outputs:
#   %rax - The integer value read from stdin
#
# Registers used (clobbered):
#   %rax, %rbx, %rcx, %rdx, %rsi, %rdi, %r8
#
# Notes:
#   - Reads up to 128 characters from stdin
#   - Expects decimal integer (no error checking for non-digits)
#   - Strips the trailing newline

readInt:
	# Function prologue: save caller's registers we'll clobber
	push %rbx		# Save %rbx (callee-saved register)

	## Read data into the buffer from stdin
	mov $0,%rax		# syscall 0 = read
	mov $0,%rdi		# file descriptor 0 = stdin
	lea buffer(%rip),%rsi	# buffer address for input
	mov $128,%rdx		# read up to 128 bytes
	syscall

	mov %rax,%rbx 		# %rbx = how many bytes were read

	## Now process the buffer and convert ASCII to integer
	## using Horner's method: result = result * 10 + digit

	mov $0,%rcx 		# int i = 0 (loop counter)
	mov $0,%rax		# total = 0 (accumulator for result)
	lea buffer(%rip),%rdi	# %rdi = pointer to buffer

	## We need to dec %rbx so we don't grab the newline
	## (the last character read is '\n')
	dec %rbx

intRead:
	movb (%rdi,%rcx,1),%r8b # temp = buffer[i] (load one byte)
	sub $'0',%r8 		# temp = temp - '0' (convert ASCII to digit)
	imul $10,%rax		# result = result * 10
	add %r8,%rax		# result = result + digit
	inc %rcx 		# i++
	cmp %rbx,%rcx 		# compare i with numBytes-1
	jl intRead		# if i < numBytes-1, continue loop

	# Function epilogue: restore saved registers and return
	pop %rbx		# Restore %rbx
	ret			# Return with result in %rax
