.section .data
# This section defines the data used by the program.

# Define a string literal named "hello".
# .asciz directive creates a null-terminated string.
hello:
    .asciz "Hello, World!\n"

# Define another string literal named "hello2" but this time using .byte directive.
# .byte directive creates a sequence of bytes.
hello2: .byte 'H','e','l','l','o', ',' ,' ','W','o','r','l','d','!','\n'

# Calculate the length of the hello2 string and store it in a variable named hellolen.
# . - hello2 calculates the difference between the current address and the address of hello2,
# which gives us the size of the hello2 string.
hellolen = . - hello2 

# End of the .data section.

.section .text
# This section contains the executable instructions of the program.

# Declare the _start symbol as global, so the linker can find it.
# _start is the entry point of the program.
.global _start

# The _start label defines the beginning of the program's execution.
_start:
    # Move the value 1 into register rax. 
    # rax is used to store the system call number.
    # 1 is the system call number for the "write" function.
    mov $1, %rax        	

    # Move the value 1 into register rdi.
    # rdi is used to store the file descriptor.
    # 1 is the file descriptor for standard output (stdout).
    mov $1, %rdi        	

    # Load the address of hello2 into register rsi.
    # lea (load effective address) instruction calculates the address of hello2 
    # relative to the current instruction pointer (%rip).
    # rsi is used to store the address of the string to be printed.
    lea hello2(%rip), %rsi 

    # Move the value of hellolen into register rdx.
    # rdx is used to store the length of the string.
    mov $hellolen, %rdx       

    # Execute the system call.
    # This will call the "write" function with the arguments specified in rax, rdi, rsi, and rdx.
    syscall

    # Exit the program.
    # Move the value 60 into register rax.
    # 60 is the system call number for the "exit" function.
    mov $60, %rax       

    # Move the value 0 into register rdi.
    # rdi is used to store the exit code.
    # 0 indicates a successful exit.
    xor %rdi, %rdi      

    # Execute the system call.
    # This will call the "exit" function with the exit code specified in rdi.
    syscall
