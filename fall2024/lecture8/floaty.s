	.section .data
float1:	 .float 1.5
float2:	 .float 2.5

	.section .text
	.global _start
	
_start:
	movss float1(%rip),%xmm0
	addss float2(%rip),%xmm0

	cvttss2si %xmm0,%eax

	mov %eax,%edi
	mov $60,%rax
	syscall
