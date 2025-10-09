        .text
        .global _start

_start:
        mov $1, %r8              # put 1 in r8
        mov $2, %r9              # put 2 in r9
        add %r8, %r9             # add r8 and r9 and put the result back in r9
        mov %r9, %rdi
        mov $60, %rax            # set up the exit syscall
        syscall
