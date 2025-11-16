        .section .text  
        # This line indicates that the following code belongs to the ".text" section.
        # The ".text" section is where executable instructions are stored.
        
        .global _start  
        # This line declares the "_start" symbol as global. 
        # This means that the label "_start" can be referenced from other files or modules.
        # The "_start" symbol is the entry point of the program, meaning execution begins at this label.

_start:
        mov $60, %rax  
        # This line moves the value 60 into the register %rax. 
        # The $ sign indicates an immediate value, which is a value that is directly encoded in the instruction.
        # The value 60 is the system call number for "exit" in the x86-64 Linux system call interface.
        # The %rax register is the register that is used to pass the system call number to the kernel.
