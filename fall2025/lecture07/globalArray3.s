.section .data
arr:    .quad 1,2,3,4,5

        .section .text
        .global _start

_start:
        lea arr(%rip),%r8
        ## choose %r9 as our accumulator
        mov $0,%r9
        mov $0,%rcx             # %rcx holds the loop index (e.g. it is `i`)
        ## time to start the loop
loopStart:
        cmp $5,%rcx
        jge loopEnd

        add (%r8,%rcx,8),%r9
        add $1,%rcx
        
        jmp loopStart
loopEnd:
        mov %r9,%rdi
        mov $60,%rax
        syscall
