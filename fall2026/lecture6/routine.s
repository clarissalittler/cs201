	.section .text
	.global _start


	
	# we're going to jump to a label
	# that's a subroutine that
	# doubles the value in %rdi
	# and passes it back in %rax

rdadder:
	add %rdi,%rdi
	mov %rdi,%rax
	jmp rdret

_start:
	mov $20,%rdi
	jmp rdadder
rdret:
	mov %rax,%rdi
	# setup our exit
	mov $60,%rax
	syscall
