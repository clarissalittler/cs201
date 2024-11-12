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
        pushq   %rbp                # Save the old base pointer (%rbp) onto the stack.
        movq    %rsp, %rbp          # Set the new base pointer (%rbp) to the current stack pointer (%rsp).
        subq    $40, %rsp           # Allocate 40 bytes of space on the stack for local variables.

        # Variables:
        # Buffer is from -32(%rbp) to -1(%rbp), 32 bytes
        # Note: We avoid using 0(%rbp) or any positive offsets from %rbp
        # This is done to prevent potential conflicts with the function's arguments, 
        # which might be stored at 0(%rbp) or higher addresses.

        # Initialize buffer pointer to the end of the buffer
        leaq    -1(%rbp), %rsi      # %rsi points to the last byte of the allocated stack space (just below %rbp).
        movq    $0, %rcx            # Initialize the digit count to 0.
        movq    %rdi, %rax          # Copy the input integer from %rdi to %rax.
        movq    $0, %r8             # Initialize the negative flag to 0 (meaning the number is positive).

        # Handle zero separately
        cmpq    $0, %rax
        jne     check_negative      # If the number is not zero, jump to the 'check_negative' label.
        # If zero, store '0' character
        movb    $'0', (%rsi)        # Store the ASCII character '0' at the address pointed to by %rsi (which is the end of the buffer).
        movq    %rsi, %rsi          # %rsi already points to the character, so just assign it back to itself.
        movq    $1, %rcx            # Set the digit count to 1 (since we only have the character '0').
        jmp     write_output        # Jump to the 'write_output' label to print the result.

check_negative:
        # Check if the number is negative
        testq   %rax, %rax          # Set the Zero flag (ZF) if the number is zero.
        jge     convert_number      # If the number is not negative (ZF is not set), jump to the 'convert_number' label.
        # If negative, set the negative flag and negate the number
        movq    $1, %r8             # Set the negative flag to 1.
        negq    %rax                # Negate the number in %rax.
        jno     convert_number      # If there was no overflow during negation, jump to the 'convert_number' label.
        # Handle INT_MIN (-2^63) which cannot be negated
        # Since negating INT_MIN results in an overflow, we manually set %rax to INT_MIN again.
        movq    $9223372036854775808, %rax 

convert_number:
        # Convert the integer to a string
convert_loop:
        xorq    %rdx, %rdx          # Clear %rdx. This is necessary because division uses both %rdx:%rax as the dividend.
        movq    $10, %rbx            # Set %rbx to 10 (the base for decimal conversion).
        divq    %rbx                # Perform unsigned division of %rdx:%rax by %rbx (10).
        # %rax = quotient, %rdx = remainder
        addb    $'0', %dl           # Convert the remainder (in %dl) to its ASCII representation by adding '0'.
        movq    %rsi, %rdi          # Copy the value of %rsi (which points to the current character in the buffer) to %rdi.
        subq    $1, %rdi            # Decrement %rdi to move back one position in the buffer.
        movb    %dl, (%rdi)         # Store the ASCII character (in %dl) at the address pointed to by %rdi.
        movq    %rdi, %rsi          # Update %rsi to point to the new position in the buffer (where the current character is stored).
        incq    %rcx                # Increment the digit count.
        cmpq    $0, %rax            # Compare the quotient (in %rax) with 0.
        jne     convert_loop        # If the quotient is not zero, continue the loop.

        # Add negative sign if necessary
        cmpq    $0, %r8
        je      write_output        # If the negative flag is not set, jump to the 'write_output' label.
        subq    $1, %rsi            # Decrement %rsi to move back one position in the buffer.
        movb    $'-', (%rsi)        # Store the ASCII character '-' at the address pointed to by %rsi.
        incq    %rcx                # Increment the digit count to account for the '-' sign.

write_output:
        # Prepare for sys_write call
        movq    $1, %rax            # Set %rax to the system call number for sys_write (1).
        movq    $1, %rdi            # Set %rdi to the file descriptor for standard output (1).
        # %rsi already points to the start of the string
        movq    %rcx, %rdx          # Set %rdx to the number of bytes to write (stored in %rcx).
        syscall                     # Make the system call to write the string to standard output.

        # Epilogue
        leave                       # Restore the old base pointer (%rbp) and stack pointer (%rsp) using the 'leave' instruction, which is equivalent to:
                                    #   movq %rbp, %rsp
                                    #   popq %rbp
        ret                         # Return from the function.
