	.section .text
	.global _start

_start: 
  mov $0, %rdi
  add $10,%rdi # but I can't do add %rdi,$10 because 10 = 10 + rdi is gibberish
  mov $60,%rax
  syscall
