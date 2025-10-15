.section .data
num:    .quad 10                # 64 bit integer (long)

        .section .text
        .global _start

_start:
        lea num(%rip),%r9
        # %r9 = &num
        # what's the equivalent of *
        addq $10,(%r9)
        # *(%r9) = *(%r9) + 10
        movq (%r9), %rdi
        movq $60, %rax
        syscall
