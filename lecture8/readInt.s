	.section .bss
buffer:	 .zero 128

	.section .text
	.global _start

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

	mov $0,%rcx 		#int i = 0
	mov $0,%rax		#total = 0
	lea buffer(%rip),%rdi   
	## the logic of the loop is
	## read the next byte, put it in rax
	## we need to dec %rbx so we don't
	## grab the newline
	dec %rbx
	.intRead:
	movb (%rdi,%rcx,1),%r8b # temp = buffer[i]
	sub $'0',%r8 		# temp - '0'
	imul $10,%rax
	add %r8,%rax
	inc %rcx 		#i++
	cmp %rbx,%rcx 		#i < numBytes-1
	jl .intRead

	mov %rax,%rdi
	mov $60,%rax
	syscall

	
	
	
