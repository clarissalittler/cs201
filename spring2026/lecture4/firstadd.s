	.section .text
	.global _start

_start:
	mov $10,%rbx
	mov $20,%rcx
	add %rbx,%rcx
	mov %rcx,%rdi # we're providing the exit code to the syscall for exiting
	mov $60,%rax # special case! put a number in rax before syscall to select which function to run
	syscall
