        .section .data
arr1:   .long 1,2,3,4

        .section .text
        .global _start

        ## write a for loop that adds up all the
        ## numbers in the array
_start:
        lea arr1(%rip),%rbx     # load the address of the array into %rbx (relative to %rip)
        mov $0,%rcx             # int i = 0
        mov $0,%rax             # int sum = 0

loopBody:
        cmp $4,%rcx             # i < 4
        jge done
        add (%rbx,%rcx,4),%rax
        ## *(%rbx + 4 * %rcx)
        ## arr1[rcx]
        ## arr1[rcx] -> *(arr1 + rcx)
        add $1,%rcx             #i++
        jmp loopBody
        
done:
        mov %rax,%rdi
        mov $60,%rax
        syscall
