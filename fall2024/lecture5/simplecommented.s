        .section .text  
        # This directive tells the assembler to place the following code in the ".text" section. 
        # The ".text" section typically contains executable instructions. 
        
        .global _start  
        # This directive declares the symbol "_start" as a global symbol. This means that the linker can find and use this symbol from other files. 
        # "_start" is a special symbol that indicates the entry point of the program. 
        
_start:  
        # This is the entry point of the program. Execution begins here. 
        mov $60, %rax  
        # This instruction moves the value 60 into the register %rax. 
        # The value 60 is the system call number for "exit". 
        
        mov $-1, %rdi  
        # This instruction moves the value -1 into the register %rdi. 
        # This is the exit code that will be returned to the operating system. 
        # In this case, -1 indicates an error. 
        
        syscall  
        # This instruction executes the system call specified in %rax. 
        # In this case, it calls the "exit" system call, which terminates the program. 
