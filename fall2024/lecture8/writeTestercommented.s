    .extern write_int  # Declare that the `write_int` function is defined elsewhere (in a library)

    .text  # This section contains executable code

    .global _start  # Declare that the `_start` label is a global symbol, which is the entry point for the program

_start: 
    movq    $-123456789, %rdi  # Load the integer -123456789 into the %rdi register
                               # %rdi is used to pass the first argument to a function call
    call    write_int       # Call the `write_int` function, which will print the integer in %rdi 

    # Exit the program
    movq    $60, %rax           # Load the system call number for `sys_exit` into %rax
                               # %rax is used to specify the system call to be executed
    xorq    %rdi, %rdi          # Set %rdi to 0 (exit code 0)
                               # %rdi is used to pass the exit code to the `sys_exit` call
    syscall                     # Execute the system call `sys_exit`
                               # This will terminate the program
