	.section .data
arr:	.byte 97,10

	.section .text
	.global _start

	# argument order is
	# rdi, rsi, rdx, rsx, then numbered
	
_start:
	# the same way the syscall for
	# exit was 60
	# the syscall for write is 1
	mov $1, %rax
	mov $1, %rdi
	# we put the first argument for
	# our write syscall which is the file
	# handle, in this case 1 for stdout
	# in our next argument (which has to be passed via %rsi)
	# this argument is the *pointer* to
	# the buffer to write out
	# pointer as data means
	# you should think about lea
	lea arr(%rip),%rsi
	# there's one more argument
	# to write and that's the length
	mov $2,%rdx
	syscall

	mov $0,%rdi
	mov $60,%rax
	syscall
	
