    .text
    .global write_int

    # Function: write_int
    # -------------------
    # Writes the integer value in %rdi to the standard output.
    #
    # Inputs:
    #   %rdi - The integer to be printed.
    #
    # Clobbers:
    #   %rax, %rdx, %rcx, %rsi, %rdi, %rbx, %r8, %r9, %r10, %r11
    #
    # Preserves:
    #   %rbp, %rsp, and other callee-saved registers.

write_int:
        # Prologue
        pushq   %rbp                # Save base pointer
        movq    %rsp, %rbp          # Establish new base pointer
        subq    $40, %rsp           # Allocate stack space (40 bytes)

        # Variables:
        # Buffer is from -32(%rbp) to -1(%rbp), 32 bytes
        # Note: We avoid using 0(%rbp) or any positive offsets from %rbp

        # Initialize buffer pointer to the end of the buffer
        leaq    -1(%rbp), %rsi      # %rsi points just below %rbp
        movq    $0, %rcx            # Digit count
        movq    %rdi, %rax          # Move the input integer to %rax
        movq    $0, %r8             # Negative flag (0 means positive)

        # Handle zero separately
        cmpq    $0, %rax
        jne     check_negative
        # If zero, store '0' character
        movb    $'0', (%rsi)
        movq    %rsi, %rsi          # %rsi already points to the character
        movq    $1, %rcx
        jmp     write_output

check_negative:
        # Check if the number is negative
        testq   %rax, %rax
        jge     convert_number
        # If negative, set the negative flag and negate the number
        movq    $1, %r8             # Set negative flag
        negq    %rax                # Negate %rax
        jno     convert_number      # Jump if no overflow (i.e., not INT_MIN)
        # Handle INT_MIN (-2^63) which cannot be negated
        movq    $9223372036854775808, %rax

convert_number:
        # Convert the integer to a string
convert_loop:
        xorq    %rdx, %rdx          # Clear %rdx before division
        movq    $10, %rbx
        divq    %rbx                # Unsigned divide %rdx:%rax by 10
        # %rax = quotient, %rdx = remainder
        addb    $'0', %dl           # Convert remainder to ASCII character
        movq    %rsi, %rdi          # Copy %rsi to %rdi for decrement
        subq    $1, %rdi            # Move back in the buffer
        movb    %dl, (%rdi)         # Store the character
        movq    %rdi, %rsi          # Update %rsi to new position
        incq    %rcx                # Increment digit count
        cmpq    $0, %rax
        jne     convert_loop        # Continue if quotient is not zero

        # Add negative sign if necessary
        cmpq    $0, %r8
        je      write_output
        subq    $1, %rsi
        movb    $'-', (%rsi)
        incq    %rcx

write_output:
        # Prepare for sys_write call
        movq    $1, %rax            # sys_write syscall number
        movq    $1, %rdi            # File descriptor (1 for stdout)
        # %rsi already points to the start of the string
        movq    %rcx, %rdx          # Number of bytes to write
        syscall                     # Make the system call

        # Epilogue
        leave                       # Equivalent to movq %rbp, %rsp; popq %rbp
        ret                         # Return from function
