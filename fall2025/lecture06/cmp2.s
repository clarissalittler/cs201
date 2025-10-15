        .section .text
        .global _start

        ## the equivalent of <,==,!=, &c.
        ## is cmp
        ## cmp takes two arguments
        ## cmp %r1,%r2
        ## this calculates r2 - r1
        ## from here you have to choose
        ## one of the jmp instructions
        ## cmp sets the flags
        ## ZF (is the result = 0)
        ## SF (is the result negative)
        ## OF (was there overflow)
        ## CF (did an arithmetic carry happen)
        
_start:
        mov $1,%r8
        mov $2,%r9
        cmp %r8,%r9
        jl less
        ## this code happens if the jump doesn't
        mov $0,%rdi             # 0 if they're not equal
        ## jl means r9 < r8
        jmp finish
        
less:  
        mov $1,%rdi             # return 1 if they're equal
        ## jl means r8 < r9

finish: 
        mov $60,%rax
        syscall
