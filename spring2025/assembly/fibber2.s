      ### Program: Compute Fibonacci number recursively in assembly
### This program reads an integer n from the user,
### computes the nth Fibonacci number using recursion,
### and exits with the Fibonacci number as the exit code.
### The result is not printed to the screen, but returned as the exit code.

    .section .text            # Code section where the instructions are stored
    .global _start            # Define _start as global symbol; entry point of the program
    .extern readInt           # Declare that readInt function is defined externally (provided elsewhere)
    .extern writeInt
        
# Define the Fibonacci function
fib:
    # Function prologue - set up the stack frame
    push %rbp                 # Save the base pointer of the previous frame (caller)
    mov %rsp, %rbp            # Set up the base pointer for this function's stack frame
    sub $16, %rsp             # Allocate 16 bytes of space on the stack for local variables (ensure 16-byte alignment)

    # Check if n <= 1 (base case)
    cmp $1, %rdi              # Compare input n (%rdi) with 1
    jle .base                 # If n <= 1, jump to .base label (base case)

    # Recursive case: compute fib(n - 1)
    push %rdi                 # Save current n on the stack before modifying it
    dec %rdi                  # Decrement n by 1; compute n - 1
    call fib                  # Recursive call to fib(n - 1); result returned in %rax
    mov %rax, -8(%rbp)        # Store fib(n - 1) result at offset -8(%rbp) (local variable)
    pop %rdi                  # Restore n from the stack

    # Compute fib(n - 2)
    dec %rdi                  # Decrement n by 1; now n = n - 1
    dec %rdi                  # Decrement n by 1 again; now n = n - 2
    call fib                  # Recursive call to fib(n - 2); result in %rax

    # Add fib(n - 1) and fib(n - 2)
    add -8(%rbp), %rax        # Add fib(n - 1) (from memory) to fib(n - 2) (in %rax); result is fib(n)

    # Function epilogue - restore the stack frame
    mov %rbp, %rsp            # Restore the stack pointer to the base pointer
    pop %rbp                  # Restore the base pointer of the caller
    ret                       # Return to the caller; result is in %rax

    # Base case: when n <= 1, fib(n) = n
.base:
    mov %rdi, %rax            # Set return value to n (%rdi)
    mov %rbp, %rsp            # Restore the stack pointer to the base pointer
    pop %rbp                  # Restore the base pointer of the caller
    ret                       # Return to the caller; result is in %rax

# Entry point of the program
_start:
        call readInt              # Call external function to read an integer from the user; result in %rax
        mov %rax, %rdi            # Move input value from %rax to %rdi (first argument for fib)

        call fib                  # Call fib(n); result will be in %rax

        mov %rax,%rdi
        call writeInt
                              # Prepare to exit the program with fib(n) as the exit code
        xor %rdi, %rdi            # Move fib(n) result to %rdi (exit code expected in %rdi)
        mov $60, %rax             # System call number for exit (60)
        syscall                   # Make the system call to exit the program


        ## fib(0) = 0
        ## fib(1) = 1
        ## fib(2) = 1
        ## fib(3) = 2
        ## fib(4) = 3
        ## fib(5) = 5
        ## fib(6) = 8
