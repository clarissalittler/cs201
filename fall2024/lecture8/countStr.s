	.section .text
	.global countStr

	## takes an address in %rdi
	## will count until it hits a null
countStr:
	mov $0,%rcx
	.loopStart:
	movb (%rdi,%rcx,1),%r8b
	cmp $0,%r8b
	je .end
	inc %rcx
	jmp .loopStart
	.end:
	mov %rcx,%rax
	ret
