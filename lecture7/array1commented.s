        .section .data
myArray:         .quad 1, 2, 3, 4, 5  # Define a data section called ".data" and create a 
                                      #  quadword (8-byte) array called "myArray" 
                                      #  with initial values 1, 2, 3, 4, 5.

        .section .text
        .global _start   # Define a text section called ".text" and declare the 
                          #  "_start" label as a global symbol (entry point).

_start:               # Label for the starting point of the program.
        mov $0, %rcx    # Initialize register %rcx to 0. This register will act as a
                        #  loop counter for iterating over the array.

        .dubLoop:      # Label for the loop that doubles the elements of the array.
        mov myArray(,%rcx,8), %rax  # Load the current element of myArray (pointed to by 
                                      # %rcx) into register %rax. The offset 8 is used 
                                      #  to access each element in the quadword array.

        imul $2, %rax   # Multiply the value in %rax by 2 (double the value).
                        #  The result is stored in %rax.

        mov %rax, myArray(,%rcx,8)  # Store the doubled value from %rax back into the 
                                     #  current element of myArray.

        inc %rcx        # Increment the loop counter %rcx by 1 to move to the next
                        #  element in the array.

        cmp $5, %rcx     # Compare the value of %rcx with 5. This checks if we've 
                        #  iterated through all 5 elements of the array.

        jl .dubLoop      # If %rcx is less than 5, jump back to the beginning of the 
                        #  .dubLoop to process the next element. This continues the 
                        #  loop until all elements have been doubled.

        mov $0,%rcx      # Reset %rcx to 0 for the next loop.

        mov $0,%rax      # Initialize %rax to 0. This register will store the sum of 
                        #  the array elements.

        .sumLoop:      # Label for the loop to calculate the sum of the array.
        add myArray(,%rcx,8), %rax # Add the current element of myArray (pointed to by 
                                     # %rcx) to the value in %rax.

        inc %rcx        # Increment %rcx by 1 to move to the next element in the array.

        cmp $5, %rcx     # Compare %rcx with 5 to check if we've reached the end of the 
                        #  array.

        jl .sumLoop      # If %rcx is less than 5, jump back to the beginning of 
                        #  the .sumLoop to add the next element. This loop continues 
                        #  until all elements have been added to %rax.

        mov %rax,%rdi    # Move the sum of the array from %rax to %rdi.
                        #  %rdi is the register used to pass the argument to the syscall.

        mov $60,%rax    # Set %rax to 60, the system call number for "exit".

        syscall         # Execute the system call with the exit code in %rdi. This 
                        #  terminates the program. 
