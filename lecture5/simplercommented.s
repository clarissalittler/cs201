        .section .text  
        # This line declares a section called ".text" where the code will be stored. 
        # The ".text" section is a standard section for code in assembly.

        .global _start 
        # This line makes the "_start" symbol globally visible.  
        # This means that the linker can find and use this label.  
        # The "_start" label is the entry point for the program, 
        # where the program execution begins.

_start:
        mov $60, %rax  
        # This instruction moves the value 60 into the register %rax.
        # The %rax register is used to store the system call number.
        # In this case, 60 is the system call number for "exit".

        syscall     
        # This instruction performs a system call.
        # It uses the value in %rax (which is 60) to determine what system call to execute.
        # The "exit" system call terminates the program.
