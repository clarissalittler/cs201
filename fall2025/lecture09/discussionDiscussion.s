	.file	"discussionDiscussion.c"
	.text
	.section	.rodata
.LC0:
	.string	"Enter a number: "
.LC1:
	.string	"%d"
.LC2:
	.string	"Enter another: "
.LC3:
	.string	"Ta-da! %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp # pushes the base pointer to save
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16 
	movq	%rsp, %rbp # copies the old stack to the new base pointer
	.cfi_def_cfa_register 6
	subq	$16, %rsp # moves the stack pointer down 16 bytes
	leaq	.LC0(%rip), %rax # this is moving the address for the string "enter a number" into a register so we can print
	movq	%rax, %rdi # why rdi? first argument to a function
	movl	$0, %eax
	call	printf@PLT
	leaq	-8(%rbp), %rax # -8(%rbp) is num1
	# this leaq is &num1 to pass into the scanf
	movq	%rax, %rsi # why rsi? &num1 is the *second* argument
	leaq	.LC1(%rip), %rax # moves the label for "%d" into the first argument of the scanf call
	movq	%rax, %rdi
	movl	$0, %eax
	call	__isoc99_scanf@PLT
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	-12(%rbp), %rax # -12(%rbp) is num2
	movq	%rax, %rsi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	__isoc99_scanf@PLT
	movl	-8(%rbp), %edx # setting up the addition by moving num1 into a register
	movl	-12(%rbp), %eax # moving num2 into a register
	imul	%edx, %eax #performing the addition
	movl	%eax, -4(%rbp) # put the result in num3
	movl	-4(%rbp), %eax # now we load num3 into a register for the final printf
	# eax is a way of saying "only use a 32 bit version of rax for 32 bit operations"
	movl	%eax, %esi
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
	leave # leave does two things
	# mov %rbp,%rsp
	# pop %rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Debian 12.2.0-14+deb12u1) 12.2.0"
	.section	.note.GNU-stack,"",@progbits
