        .text
        .global _start

        ## q - quad sized (64 bits)
        ## l - long (32 bits)
        ## w - word sized (16 bits)
        ## b - byte (8 bits)
        ##  add/sub source arg,destination arg
        ## source can be a value or a register (or a memory location)
        ## destination has to be a register (or memory location)
_start:
        mov $1,%rax
        mov $2,%rdx
        sub %rax,%rdx           # sub s,d -> d = d - s 
        mov %rdx,%rdi           # we stored our computation in rdx BUT the syscall to exit needs the exit code value passed in via rdi (by The Calling Convention)

        mov $60,%rax
        syscall
