	.section .bss
buffer:	 .zero 128

	.section .text
	.global _start

parseInt:
	.intRead:
	movb (%rdi,%rcx,1),%r8b
	sub $'0',%r8
	imul $10,%rax
	add %r8,%rax
	inc %rcx
	cmp %rsi,%rcx
	jl .intRead
	ret

	
_start:
	## we get the data into the buffer
	mov $0,%rax
	mov $0,%rdi
	lea buffer(%rip),%rsi
	mov $128,%rdx
	syscall
	
	mov %rax,%rbx 		#how many bytes were read

	## now we need to process it and accumulate the data
	## into a register

	mov $0,%rcx
	mov $0,%rax
	lea buffer(%rip),%rdi
	dec %rbx
	mov %rbx,%rsi
	call parseInt
	
	mov %rax,%rdi
	mov $60,%rax
	syscall

	
	
	
