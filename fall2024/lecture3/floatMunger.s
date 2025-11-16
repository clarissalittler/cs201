	.file	"floatMunger.c"
	.text
	.globl	toggle
	.type	toggle, @function
toggle:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movl	-28(%rbp), %eax
	movl	$1, %edx
	movl	%eax, %ecx
	sall	%cl, %edx
	movl	%edx, %eax
	movl	%eax, -4(%rbp)
	movq	-24(%rbp), %rax
	movl	(%rax), %eax
	xorl	-4(%rbp), %eax
	movl	%eax, %edx
	movq	-24(%rbp), %rax
	movl	%edx, (%rax)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	toggle, .-toggle
	.globl	isSet
	.type	isSet, @function
isSet:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %edx
	movl	-12(%rbp), %eax
	movl	%eax, %ecx
	sarl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	isSet, .-isSet
	.section	.rodata
.LC0:
	.string	"%d"
	.text
	.globl	printBits
	.type	printBits, @function
printBits:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	$31, -4(%rbp)
	jmp	.L5
.L6:
	movl	-4(%rbp), %edx
	movq	-24(%rbp), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	isSet
	movl	%eax, %esi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	subl	$1, -4(%rbp)
.L5:
	cmpl	$0, -4(%rbp)
	jns	.L6
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	printBits, .-printBits
	.section	.rodata
	.align 8
.LC2:
	.string	"The bits set in the number %f are:\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB9:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movss	.LC1(%rip), %xmm0
	movss	%xmm0, -4(%rbp)
	movss	-4(%rbp), %xmm0
	cvtss2sd	%xmm0, %xmm0
	movl	$.LC2, %edi
	movl	$1, %eax
	call	printf
	leaq	-4(%rbp), %rax
	movq	%rax, %rdi
	call	printBits
	movl	$10, %edi
	call	putchar
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	main, .-main
	.section	.rodata
	.align 4
.LC1:
	.long	1094713344
	.ident	"GCC: (GNU) 8.5.0 20210514 (Red Hat 8.5.0-22.0.1)"
	.section	.note.GNU-stack,"",@progbits
