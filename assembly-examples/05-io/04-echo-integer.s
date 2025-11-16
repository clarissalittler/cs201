        .section .rodata
msg1:   .ascii "Enter a number: "
msg1len = . - msg1

###  The difference between . (which signifies our current location in the program, as an address) - msg (the address of the start of the data) is the length of the message in bytes
        
msg2:   .ascii "Here's what you entered: "
msg2len = . - msg2
        
msg3:   .ascii "\nGoodbye!\n"
msg3len = . - msg3
        

        .section .text
        .global _start

        .extern readInt
        .extern writeInt
        
_start:
        mov $1,%rax
        mov $1,%rdi             # file descriptor 1 = stdout
        lea msg1(%rip),%rsi     # load message address
        mov $msg1len,%rdx
        syscall
        
        ## first we call readInt
        call readInt
        ## now %rax holds the integer we read in
        ## why %rax? Because that's generally what
        ## things are returned into (calling conv)
        
        push %rax               # whoops better save rax quick

        mov $1,%rax
        mov $1,%rdi             # file descriptor 1 = stdout
        lea msg2(%rip),%rsi     # load message address
        mov $msg2len,%rdx
        syscall

        pop %rax                # we got it back!!!
        mov %rax,%rdi           # setup the writeInt call
        call writeInt

        mov $1,%rax
        mov $1,%rdi             # file descriptor 1 = stdout
        lea msg3(%rip),%rsi     # load message address
        mov $msg3len,%rdx
        syscall
        
        
        mov $60,%rax
        xor %rdi,%rdi           # fast way of making 0 be in rdi
        syscall
