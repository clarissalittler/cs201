        .section .data
num1:   .long 200               #exists for the lifetime of the program
        ## .long means 32 bit

        .section .text
        .global _start

_start:
        ## rip-relative addressing
        ## lea is used for the & operation
        lea num1(%rip),%rbx          # &num1 -> %rbx
        addl $10,(%rbx)         # *(%rbx) = 10 + *(%rbx)
        movl num1,%edi          # 32-bit load to match .long! writing %edi
                                # zero-extends into all of %rdi for free
                                # (a 64-bit mov here would read 8 bytes
                                # from a 4-byte variable -- garbage)
        mov $60,%rax
        syscall 
