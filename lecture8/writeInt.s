	.section .bss
writeBuff:	.zero 64
	
	.section .text
	.global writeInt
	.extern countStr

	## takes an integer in %rdi
writeInt:
	
