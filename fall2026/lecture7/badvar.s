	.section .text
	.global _start

badfun:
	push %rbp
	mov %rsp,%rbp
	sub $16,%rsp
	mov $10,-16(%rbp)
	lea -16(%rbp),%rax
	mov %rbp,%rsp
	pop %rbp
	ret
	
_start:
	call badfun
	# ...
