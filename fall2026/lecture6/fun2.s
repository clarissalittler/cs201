	.section .text
	.global _start


	# now we've upgraded rdadder to a very very simple function
rdadder:
	mov %rdi,%r10
	add %r10,%r10
	mov %r10,%rax
	ret # jumps back to the saved instruction pointer

_start:
	mov $20,%rdi # 20 is in rdi
	call rdadder # saves the instruction pointer for where to return # now 20 is in %rdi but 40 is in %rax
	mov %rax,%rdi
	call rdadder
	mov %rax,%rdi 
	# setup our exit
	mov $60,%rax
	syscall
