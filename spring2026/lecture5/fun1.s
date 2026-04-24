	.section .text
	.global _start


	# now we've upgraded rdadder to a very very simple function
rdadder:
	add %rdi,%rdi
	mov %rdi,%rax
	ret # jumps back to the saved instruction pointer

_start:
	mov $20,%rdi # 20 is in rdi
	call rdadder # saves the instruction pointer for where to return # now 20 is in %rdi but 40 is in %rax
	mov %rax,%rdi # now 40 is in %rdi
	call rdadder
	mov %rax,%rdi # 40 is in %rdi but now 80 is in %rax
	# setup our exit
	mov $60,%rax
	syscall
