        .text                   # code lives in the text section
        .global _start

_start:
        mov $60, %rax           # syscall looks in the register named %rax for the number of the system call to run, 60 means "exit this program"
        mov $-1, %rdi            # the exit system call looks in %rdi for the exit code number
        ## the exit code is 1 unsigned byte
        syscall
