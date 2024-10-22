	.section .data
message1:	.asciz "Enter an integer, will ya: " # 27
message2:	.asciz "Enter another integer: "     # 23
	
	.section .text
	.global _start

	.extern readInt

	## int sillyFun(){
	##   int num1;
	##   int num2;
	##   printf(message1)
	##   num1 = readInt()
	##   printf(message2)
	##   num2 = readInt()
	##   return (num1 + num2)
	## }
	
sillyFun: 
	push %rbp 		# we save the OLD base pointer
	movq %rsp,%rbp
	sub $16,%rsp

	## write the first message
	mov $1, %rax
	mov $1, %rdi
	lea message1(%rip),%rsi
	mov $27,%rdx
	syscall

	## read the first int
	call readInt
	mov %rax,-8(%rbp)

	## write the second message
	mov $1, %rax
	mov $1, %rdi
	lea message2(%rip),%rsi
	mov $23,%rdx
	syscall

	## read the first int
	call readInt
	mov %rax,-16(%rbp)

	mov -8(%rbp),%rcx
	mov -16(%rbp),%rbx
	mov $0,%rax

	add %rcx,%rax
	add %rbx,%rax

	movq %rbp,%rsp
	pop %rbp
	ret
	
	
_start:
	call sillyFun

	mov %rax,%rdi
	mov $60,%rax
	syscall
	
