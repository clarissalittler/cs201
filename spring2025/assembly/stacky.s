        .section .rodata
msg1:   .ascii "\nEnter a number: "
msg1len = . - msg1
        
msg2:   .ascii "\nYour function calculated: "
msg2len = . - msg2

msg3:   .ascii "\nGoodbye!\n"
msg3len = . - msg3
                
        .section .text                   # Start of the code section
        .global _start          # Make _start symbol globally visible for the linker

        .extern readInt
        .extern writeInt

        ## function that takes the address
        ## of a string in %rdi
        ## prints it
printStr:
        mov %rsi,%rdx
        mov %rdi,%rsi     # load message address
        mov $1,%rax
        mov $0,%rdi
        syscall
        ret 

        
## This is a function that demonstrates:
## 1. Using local variables on the stack
## 2. Function call conventions
## 3. Basic arithmetic operations
## long fun(long a, long b){
##   long c = a + b;
##   long d = 2+b;
##   return (c + d);
## }
fun:    
        # Standard function prologue
        # Save the old base pointer (frame pointer)
        push %rbp
      
        # Set up new base pointer for this stack frame
        mov %rsp,%rbp
      
        # Allocate 16 bytes of local stack space
        # This creates space for two 8-byte (64-bit) local variables
        sub $16,%rsp

        # First parameter (a) is in %rdi
        # First local variable will be at -8(%rbp)
        # Compute a + b and store at -8(%rbp)
        mov %rdi,-8(%rbp)
        add %rsi,-8(%rbp)

        # Second local variable at -16(%rbp)
        # Compute b + 2 and store at -16(%rbp)
        mov %rsi,-16(%rbp)
        addq $2,-16(%rbp)

        # Prepare return value by:
        # 1. Load d into %rax
        # 2. Add c to %rax
        mov -16(%rbp),%rax
        add -8(%rbp),%rax

        # Function epilogue: 
        # 'leave' is a single instruction that does:
        # mov %rbp,%rsp
        # pop %rbp
        leave
        ret
      
_start:
        lea msg1(%rip),%rdi
        mov $msg1len,%rsi
        call printStr

        call readInt
        push %rax

        lea msg1(%rip),%rdi
        mov $msg1len,%rsi
        call printStr

        call readInt

        
        # Set up arguments for fun()
        # Second arg (4) goes in %rsi
        mov %rax,%rsi
        pop %rax
        # First arg (3) goes in %rdi
        mov %rax,%rdi
        
        # Call the fun() function
        call fun
        push %rax
        ## print the next message
        lea msg2(%rip),%rdi
        mov $msg2len,%rsi
        call printStr
        
        # Return value from fun() is already in %rax
        pop %rax
        mov %rax,%rdi
        call writeInt

        lea msg3(%rip),%rdi
        mov $msg3len,%rsi
        call printStr
        
        # Syscall number for exit
        mov $60,%rax
        xor %rdi,%rdi
        # Invoke system call to exit
        syscall
