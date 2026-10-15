	.section .text
	.global _start
	# cmp
	# jmp and friends
_start:
	mov $10,%rbx
	mov $20,%rcx
	mov $-1,%rdi
	cmpq %rbx,%rcx # cmp S,D --> D - S
	# cmp $lit,%reg but NOT cmp %reg,$lit
	jge CHICKEN # jump if greater than or equal
	# jg, jl, jle, je, jne, jz
	# jmp 
	mov $1,%rdi
CHICKEN:	
	mov $60,%rax
	syscall
	
	# if(!(rcx>=rbx)){
	#   rdi = 1
	# }
	# ...
