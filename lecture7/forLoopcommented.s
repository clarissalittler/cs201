        .section .text  #  Declares the following code as part of the ".text" section, where executable instructions reside.
        .global _start  #  Declares the label "_start" as a global symbol, making it accessible from outside the current file.
                        #  This is essential because the operating system uses "_start" as the entry point for the program.

_start:                 #  The entry point of the program. Execution starts from here.
        mov $0,%rax     #  Moves the immediate value 0 into the register %rax.  %rax will eventually hold the final result. 
                        #  This initializes %rax to 0, which acts as the starting point for our calculation.
        mov $0,%rcx     #  Moves the immediate value 0 into the register %rcx.  %rcx acts as our loop counter.
                        #  This initializes %rcx to 0, indicating that we haven't performed any iterations yet.

        .loopStart:    #  This label marks the beginning of the loop.  The loop will continue to execute until the 
                        #  "jl .loopStart" instruction determines the loop should end.
        mov %rcx,%rbx  #  Moves the value stored in %rcx into the register %rbx. %rbx is a temporary register used 
                        #  to store the current value of the counter (in %rcx) for the next step.
        imul %rcx,%rbx #  Multiplies the value in %rcx with the value in %rbx (which is also the current loop counter). 
                        #  The result of this multiplication is then stored in %rbx.  This effectively squares the loop counter.
        add %rbx,%rax #  Adds the value in %rbx (the squared loop counter) to the value in %rax. This updates the 
                        #  running sum (stored in %rax).
        inc %rcx      #  Increments the value in %rcx by 1.  This acts as the loop counter, advancing the loop to the next iteration.
        cmp $6,%rcx    #  Compares the value in %rcx with the immediate value 6. This checks if the loop has reached its limit. 
                        #  The "jl" instruction below will branch if the loop counter is less than 6.
        jl .loopStart  #  This instruction checks the result of the previous "cmp" instruction. If the value in %rcx is less
                        #  than 6 (meaning the loop counter hasn't reached its limit), it "jumps" back to the ".loopStart" 
                        #  label, starting the next iteration of the loop. 

        mov %rax,%rdi #  Moves the value in %rax (the final result of the calculation) to the register %rdi. %rdi is a 
                        #  special register used for passing arguments to the "syscall" instruction. 
        mov $60,%rax #  Moves the value 60 (representing the "exit" system call) into the register %rax.  %rax is also 
                        #  used to indicate which system call is requested.
        syscall     #  This is the system call instruction. It instructs the operating system to execute the system call 
                        #  indicated by the value in %rax (which is 60 for "exit"). The argument to "exit" is provided in 
                        #  %rdi, which contains the final result of the calculation. 

