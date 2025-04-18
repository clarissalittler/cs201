        .text
        .global _start

_start:
        mov $60,%rax            # put 60 IN %rax
        mov $-1,%rdi             # return -1
        syscall                 # with 60 in %rax, this syscall will be to exit the program
        ## will return 255 with echo $?
        ## return codes are an unsigned byte
        ## so they can take values 0 - 255
        ## -1 -> 111111...[11111111]
