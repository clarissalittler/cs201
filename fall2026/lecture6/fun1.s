	.section .text
	.global _start


	# now we've upgraded rdadder to a very very simple function
	# note: rdadder *destroys* its argument register %rdi as it works
	# (compare with fun2.s, which stashes %rdi in %r9 to preserve it)
rdadder:
	add %rdi,%rdi
	mov %rdi,%rax
	ret # jumps back to the saved instruction pointer

_start:
	mov $20,%rdi # 20 is in rdi
	call rdadder # saves the instruction pointer for where to return # rdadder clobbered %rdi: both %rdi and %rax are 40 now
	mov %rax,%rdi # copy the result into %rdi for the next call
	call rdadder
	mov %rax,%rdi # both are 80 now; %rdi becomes our exit status (echo $? shows 80)
	# setup our exit
	mov $60,%rax
	syscall
