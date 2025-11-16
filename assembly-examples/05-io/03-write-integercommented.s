// PEDAGOGICAL PURPOSE:
// This program demonstrates how to convert a binary integer to ASCII and write it
// to standard output. It's the reverse of readInt - instead of parsing ASCII to binary,
// we're converting binary to ASCII for display.
//
// Key learning objectives:
// 1. Understanding binary-to-ASCII conversion algorithm
// 2. Learning how to build strings backward (from right to left)
// 3. Seeing how to handle special cases (zero, negative numbers, INT_MIN)
// 4. Understanding the division algorithm for extracting digits
// 5. Learning proper stack frame management with local buffers
// 6. Understanding two's complement edge cases (INT_MIN cannot be negated)
// 7. Using the stack for temporary string storage
// 8. Proper function prologue and epilogue conventions

// CONCEPTUAL BACKGROUND - NUMBER REPRESENTATION:
//
// BINARY (COMPUTER INTERNAL):
// - Computers store integers in binary
// - Example: 123 is stored as 0x7B (01111011)
// - Compact and efficient for computation
// - Cannot be directly displayed on screen
//
// ASCII (HUMAN READABLE):
// - Humans read numbers as sequences of digit characters
// - Example: 123 is displayed as '1', '2', '3'
// - Each digit is an ASCII character ('0' = 48, '1' = 49, etc.)
// - This is what appears on the terminal screen
//
// CONVERSION CHALLENGE:
// - Need to convert binary integer to ASCII string
// - Then write that string to stdout
// - This program solves that conversion problem

// CONCEPTUAL BACKGROUND - DIVISION ALGORITHM:
//
// PROBLEM: Convert integer 123 to string "123"
//
// KEY INSIGHT: Division by 10 extracts one digit at a time
//   123 ÷ 10 = 12 remainder 3    (least significant digit: 3)
//    12 ÷ 10 = 1 remainder 2     (next digit: 2)
//     1 ÷ 10 = 0 remainder 1     (most significant digit: 1)
//
// OBSERVATION: Digits come out in REVERSE order (3, 2, 1)
// SOLUTION: Build the string backward (right to left)
//
// ALGORITHM:
//   1. Start at the END of a buffer
//   2. Repeatedly divide by 10
//   3. Convert remainder to ASCII digit ('0' + remainder)
//   4. Store digit and move LEFT
//   5. Continue until quotient is 0
//   6. Result: string is built right-to-left in buffer
//
// TRACE FOR 123:
//   Buffer: [????????????????]
//                             ^ start here (right end)
//   123 ÷ 10 = 12 R 3
//   Buffer: [???????????????3]
//                            ^ move left
//   12 ÷ 10 = 1 R 2
//   Buffer: [??????????????23]
//                           ^ move left
//   1 ÷ 10 = 0 R 1
//   Buffer: [?????????????123]
//                          ^ move left
//   Quotient is 0, done!
//   Result: "123" (correct order!)
//
// WHY RIGHT-TO-LEFT?
// - Division gives us digits backward (LSD first)
// - Building backward avoids need to reverse string later
// - Pointer arithmetic: decrement pointer = move left in buffer

// CONCEPTUAL BACKGROUND - SPECIAL CASES:
//
// 1. ZERO:
//    - Division algorithm doesn't work (0 ÷ 10 = 0, infinite loop!)
//    - Must handle specially: output single '0' character
//
// 2. NEGATIVE NUMBERS:
//    - Can't divide negative numbers correctly
//    - Must work with absolute value
//    - Remember to add '-' sign at the end
//
// 3. INT_MIN (-2^63):
//    - Special edge case in two's complement
//    - INT_MIN = -9,223,372,036,854,775,808
//    - Its absolute value (2^63) doesn't fit in signed 64-bit!
//    - Maximum positive value is 2^63 - 1
//    - Negating INT_MIN causes overflow
//    - Must handle with unsigned arithmetic

// STACK FRAME MANAGEMENT:
//
// Functions often need temporary local variables.
// The STACK provides temporary storage.
//
// STACK FRAME STRUCTURE:
//   Higher addresses
//   +------------------+
//   | caller's data    |
//   +------------------+
//   | return address   | <- pushed by 'call' instruction
//   +------------------+
//   | saved %rbp       | <- pushed by 'pushq %rbp'
//   +------------------+ <- %rbp points here (base pointer)
//   | local variables  |
//   |   (buffer)       | <- %rbp - 32 to %rbp - 1
//   +------------------+
//   | padding          | <- for alignment
//   +------------------+ <- %rsp points here (stack pointer)
//   Lower addresses
//
// PROLOGUE (function entry):
//   pushq %rbp          # Save caller's base pointer
//   movq %rsp, %rbp     # Set our base pointer
//   subq $40, %rsp      # Allocate local space
//
// EPILOGUE (function exit):
//   leave               # Restore stack (movq %rbp,%rsp; popq %rbp)
//   ret                 # Return to caller
//
// WHY %rbp?
// - Provides stable reference for local variables
// - %rsp changes as we push/pop, but %rbp stays constant
// - Local variables accessed as offsets from %rbp: -1(%rbp), -2(%rbp), etc.

// SYSCALL INTERFACE - WRITE:
//
// Syscall number: 1 (sys_write)
// Purpose: Write bytes to a file descriptor
//
// Parameters:
//   %rax = 1 (syscall number)
//   %rdi = file descriptor (1 for stdout)
//   %rsi = pointer to buffer (data to write)
//   %rdx = number of bytes to write
//
// Return value:
//   %rax = number of bytes actually written
//          or -1 if error
//
// Side effects:
//   - Writes data from buffer to file descriptor
//   - May clobber %rcx, %r11 and caller-saved registers

        .text                      # Begin code section
                                   # .text contains executable instructions

        .global writeInt           # Make this function available to other files
                                   # .global exports the symbol to the linker
                                   # Other assembly files can call writeInt
                                   # Makes this a reusable library function

    # Function: writeInt
    # -------------------
    # Writes the integer value in %rdi to the standard output.
    #
    # ALGORITHM OVERVIEW:
    # 1. Handle special case: zero
    # 2. Handle negative numbers (save sign, work with absolute value)
    # 3. Extract digits by repeatedly dividing by 10
    # 4. Convert each digit to ASCII by adding '0'
    # 5. Store digits RIGHT-TO-LEFT in buffer
    # 6. Add minus sign if needed
    # 7. Write the complete string to stdout
    #
    # WHY RIGHT-TO-LEFT?
    # Division gives us digits in reverse order (least significant first)
    # Example: 123 / 10 = 12 remainder 3 (we get '3' first)
    # By building the string backward, we get the correct order
    #
    # This function converts the integer to its ASCII string representation
    # and outputs it using the sys_write system call.
    #
    # CALLING CONVENTION (System V AMD64 ABI):
    # - First parameter is in %rdi (the integer to print)
    # - Return value would be in %rax (we don't return a value)
    # - Must preserve callee-saved registers: %rbx, %rbp, %r12-%r15
    # - May clobber caller-saved registers: %rax, %rcx, %rdx, %rsi, %rdi, %r8-%r11
    #
    # Inputs:
    #   %rdi - The integer to be printed.
    #
    # Outputs:
    #   None (prints to stdout as a side effect)
    #
    # Registers used (clobbered):
    #   %rax, %rbx, %rcx, %rdx, %rsi, %r8
    #   All caller-saved registers may be modified
    #
    # Stack layout:
    #   Uses a local buffer of 32 bytes on the stack to store the string.
    #   32 bytes is enough for:
    #   - 20 digits (max for 64-bit signed: -9,223,372,036,854,775,808)
    #   - 1 minus sign
    #   - null terminator (if needed)
    #   - alignment padding
    #
    # Notes:
    #   - The function handles negative integers, including INT_MIN (-2^63),
    #     which cannot be negated in two's complement representation.
    #   - The integer is converted to a string by repeatedly dividing by 10
    #     and storing the digits in reverse order in the buffer.

writeInt:
        # ================================================================
        # FUNCTION PROLOGUE: Set up stack frame
        # ================================================================
        # Standard function entry sequence
        pushq   %rbp                # Save the caller's base pointer on the stack
                                    # %rbp is a callee-saved register (our responsibility)
                                    # We must preserve its value for the caller

        movq    %rsp, %rbp          # Set base pointer (%rbp) to current stack pointer (%rsp)
                                    # Establishes our stack frame
                                    # %rbp provides stable reference for local variables
                                    # Now: %rbp points to saved caller's %rbp

        subq    $40, %rsp           # Allocate 40 bytes on the stack for local variables
                                    # WHY 40? 32 bytes for buffer + 8 bytes for alignment
                                    # Keeps stack 16-byte aligned per ABI requirement
                                    # Stack grows DOWN (toward lower addresses)
                                    # STACK LAYOUT:
                                    # %rbp - 1  to %rbp - 32 : buffer (32 bytes)
                                    # %rbp - 40 to %rbp - 33 : padding (8 bytes)

        # Variables:
        # We will use a buffer of 32 bytes on the stack to store the string representation
        # of the integer. The buffer extends from address (%rbp - 32) to (%rbp - 1).
        # We will use %rsi as a pointer to the current position in the buffer.

        # ================================================================
        # INITIALIZATION: Prepare for conversion
        # ================================================================
        # We build the string from RIGHT to LEFT
        # Start at the END of the buffer and work backward

        # Initialize buffer pointer to the end of the buffer (start filling from the end)
        leaq    -1(%rbp), %rsi      # Load effective address (%rbp - 1) into %rsi
                                    # %rsi now points to the LAST byte of the buffer
                                    # We'll move LEFT (decrement) as we add digits
                                    # This is the rightmost position for our string

        movq    $0, %rcx            # Initialize digit count to 0
                                    # %rcx will track how many characters we've written
                                    # Used later to tell sys_write how many bytes to output

        movq    %rdi, %rax          # Move the input integer from %rdi to %rax for processing
                                    # %rdi holds the parameter (the number to print)
                                    # %rax is used for division operations
                                    # We need to preserve %rdi's parameter role temporarily
                                    # (though we'll overwrite it later for syscall)

        movq    $0, %r8             # Initialize negative flag in %r8 to 0 (0 means positive)
                                    # %r8 = 0: number is positive or zero
                                    # %r8 = 1: number is negative
                                    # We need this flag because we work with absolute value

        # ================================================================
        # SPECIAL CASE: Handle zero
        # ================================================================
        # Zero is special: it's neither positive nor negative
        # Division algorithm would fail (0/10 = 0, infinite loop)
        # We need exactly one digit ('0')

        cmpq    $0, %rax            # Compare %rax with zero
                                    # Sets flags based on value
                                    # ZF (zero flag) set if equal

        jne     check_negative      # If %rax is not zero, skip to check_negative
                                    # Zero is the ONLY number that needs special handling here
                                    # All other numbers go through the division algorithm

        # If %rax is zero:
        movb    $'0', (%rsi)        # Store ASCII character '0' into the buffer at (%rsi)
                                    # movb = move byte (8 bits)
                                    # $'0' = ASCII code 48 (0x30)
                                    # Buffer now has '0' at the rightmost position

        movq    $1, %rcx            # Set digit count to 1
                                    # We wrote one character: '0'

        jmp     write_output        # Jump to write_output to print the character
                                    # Skip all the negative/division logic

        # ================================================================
        # SPECIAL CASE: Handle negative numbers
        # ================================================================
check_negative:
        # Check if the number is negative
        # In two's complement, negative numbers have high bit set
        testq   %rax, %rax          # Test %rax; sets flags based on the value
                                    # testq does a bitwise AND (%rax & %rax)
                                    # Result is always %rax, but flags are set
                                    # If high bit is set, sign flag (SF) is set
                                    # SF indicates negative in two's complement

        jge     convert_number      # If %rax >= 0 (signed), skip to convert_number
                                    # jge = jump if greater or equal (checks sign flag)
                                    # Positive numbers go straight to conversion

        # If the number is negative:
        movq    $1, %r8             # Set negative flag (%r8) to 1
                                    # Remember: we need to add '-' later
                                    # This flag survives through the conversion loop

        negq    %rax                # Negate %rax to get the absolute value
                                    # negq = two's complement negation
                                    # For most negative numbers, this works fine
                                    # Example: -5 (0xFFFFFFFFFFFFFFFB) -> 5 (0x0000000000000005)
                                    # Formula: -x = ~x + 1 (invert bits, add 1)

        jno     convert_number      # If no overflow, proceed to convert_number
                                    # jno = jump if no overflow flag
                                    # WHY CHECK OVERFLOW?
                                    # There's ONE number that can't be negated...

        # ================================================================
        # SPECIAL CASE: Handle INT_MIN (-2^63)
        # ================================================================
        # INT_MIN = -9,223,372,036,854,775,808 = 0x8000000000000000
        # In two's complement, this number CANNOT be negated!
        # Why? Its absolute value (2^63) doesn't fit in a signed 64-bit integer
        # The maximum positive value is 2^63 - 1
        #
        # WHAT HAPPENS:
        # negq on INT_MIN produces INT_MIN again (overflow)
        # Binary: 1000...000 (INT_MIN)
        # Negate: ~1000...000 + 1 = 0111...111 + 1 = 1000...000 (back to INT_MIN!)
        # We detect this with the overflow flag (OF)
        #
        # SOLUTION: Use the unsigned value 2^63 directly
        # Division with unsigned value works correctly

        # Handle INT_MIN (-2^63), which cannot be negated
        movq    $9223372036854775808, %rax  # Set %rax to 2^63 (absolute value of INT_MIN)
                                            # This is the UNSIGNED interpretation
                                            # In hex: 0x8000000000000000
                                            # Same bit pattern as INT_MIN, but unsigned
                                            # Division will work correctly with this value

        # ================================================================
        # MAIN ALGORITHM: Convert integer to string
        # ================================================================
convert_number:
        # Convert the integer to its string representation
        # ALGORITHM: Repeated division by 10
        # At each step:
        #   quotient = number / 10
        #   remainder = number % 10
        #   digit = remainder (0-9)
        #   Convert digit to ASCII ('0' + digit)
        #   Store ASCII digit, move left in buffer
        #   Continue with quotient
        #
        # EXAMPLE: 123
        #   123 / 10 = 12 remainder 3 -> store '3'
        #   12 / 10 = 1 remainder 2   -> store '2' (to left of '3')
        #   1 / 10 = 0 remainder 1    -> store '1' (to left of '2')
        #   Quotient is 0, done!
        #   Result: "123"

convert_loop:
        xorq    %rdx, %rdx          # Clear %rdx (set to zero) before division
                                    # WHY? divq divides %rdx:%rax by operand
                                    # %rdx:%rax forms a 128-bit dividend
                                    # We want to divide just %rax (64 bits), so %rdx must be 0
                                    # xorq %rdx,%rdx is faster than movq $0,%rdx
                                    # Same instruction size, but xorq has better throughput

        movq    $10, %rbx           # Load divisor 10 into %rbx
                                    # We're extracting digits in base 10 (decimal)
                                    # For hex, would use 16; for octal, 8; etc.

        divq    %rbx                # Unsigned divide %rdx:%rax by %rbx (10)
                                    # divq = unsigned 64-bit division
                                    # Dividend: 128-bit value in %rdx:%rax (upper:lower)
                                    # Divisor: %rbx (10)
                                    # After division:
                                    # - %rax: quotient (number / 10)
                                    # - %rdx: remainder (number % 10)
                                    # Example: 123 / 10 -> %rax=12, %rdx=3
                                    # The remainder is the digit we want!

        addb    $'0', %dl           # Convert remainder to ASCII character
                                    # %dl = low 8 bits of %rdx (the digit 0-9)
                                    # Adding '0' (48) converts to ASCII
                                    # Example: digit 3 + 48 = 51 = ASCII '3'
                                    # After this: %dl contains ASCII character '0'-'9'

        subq    $1, %rsi            # Move back one byte in the buffer
                                    # We're building the string RIGHT-TO-LEFT
                                    # %rsi now points to the next position (to the left)
                                    # This is like ptr-- in C

        movb    %dl, (%rsi)         # Store character in buffer at (%rsi)
                                    # Write the ASCII digit to the buffer
                                    # movb = move byte (8 bits)
                                    # Only writing one character at a time

        incq    %rcx                # Increment digit count
                                    # Track total number of characters written
                                    # We need this for sys_write (how many bytes to output)

        cmpq    $0, %rax            # Compare quotient %rax with zero
                                    # Are we done processing all digits?
                                    # When quotient reaches 0, we've extracted all digits

        jne     convert_loop        # If quotient is not zero, repeat loop
                                    # Keep extracting digits until quotient is 0
                                    # When quotient = 0, we've processed all digits

        # At this point:
        # - All digits are in the buffer (right-to-left)
        # - %rsi points to the FIRST digit (leftmost)
        # - %rcx contains the number of digits
        # - %r8 tells us if the number was negative

        # ================================================================
        # ADD MINUS SIGN (if negative)
        # ================================================================

        # Add negative sign if necessary
        cmpq    $0, %r8             # Check negative flag (%r8)
                                    # Was the original number negative?
                                    # %r8 = 0: positive, %r8 = 1: negative

        je      write_output        # If number is positive (flag=0), skip to write_output
                                    # No minus sign needed

        # If number was negative, add minus sign:
        subq    $1, %rsi            # Move back one byte for '-'
                                    # Make room for the minus sign (to the left of digits)

        movb    $'-', (%rsi)        # Store '-' in buffer at (%rsi)
                                    # ASCII code for '-' is 45 (0x2D)
                                    # Now %rsi points to the complete string (with minus sign)

        incq    %rcx                # Increment digit count
                                    # Account for the minus sign character
                                    # %rcx now has total length of string (digits + sign)

        # ================================================================
        # SYSCALL: Write string to stdout
        # ================================================================
write_output:
        # Prepare for sys_write system call
        # SYSCALL: write to file descriptor
        # Syscall number 1 = sys_write
        # sys_write arguments:
        #   %rax - syscall number (1)
        #   %rdi - file descriptor (1 for stdout)
        #   %rsi - pointer to buffer (already set)
        #   %rdx - number of bytes to write

        movq    $1, %rax            # Syscall number for sys_write
                                    # 1 = write to file descriptor
                                    # Common syscalls: 0=read, 1=write, 60=exit

        movq    $1, %rdi            # File descriptor for stdout
                                    # File descriptors:
                                    #   0 = stdin, 1 = stdout, 2 = stderr
                                    # Writing to 1 makes output appear on screen

        # %rsi already points to the start of the string
        # We set %rsi to point to the first character earlier
        # Either the first digit, or the minus sign if negative
        # No need to modify %rsi here

        movq    %rcx, %rdx          # Set %rdx to digit count (number of bytes)
                                    # Tell sys_write how many bytes to output
                                    # Must be exact - too few truncates, too many shows garbage

        syscall                     # Make the system call
                                    # Transfers control to kernel
                                    # Kernel writes %rdx bytes from %rsi to stdout
                                    # Returns number of bytes written in %rax (we ignore it)
                                    # Output appears on the terminal

        # ================================================================
        # FUNCTION EPILOGUE: Restore stack frame and return
        # ================================================================
        leave                       # Restore stack frame
                                    # 'leave' is shorthand for:
                                    #   movq %rbp, %rsp    (deallocate locals)
                                    #   popq %rbp          (restore caller's %rbp)
                                    # Dismantles our stack frame
                                    # Stack pointer back to where it was before our function

        ret                         # Return from the function
                                    # Pops return address from stack and jumps to it
                                    # Control returns to caller
                                    # Caller continues execution after the 'call' instruction

// COMPLETE EXECUTION EXAMPLE:
//
// Call: writeInt with %rdi = -123
//
// STEP-BY-STEP TRACE:
//
// Prologue:
//   push %rbp (save caller's base pointer)
//   mov %rsp,%rbp (establish our base pointer)
//   sub $40,%rsp (allocate 40 bytes for locals)
//   Stack frame established
//   Buffer space: %rbp-1 to %rbp-32
//
// Initialization:
//   %rsi = %rbp - 1 (points to end of buffer)
//   %rcx = 0 (digit count)
//   %rax = -123 (number to convert)
//   %r8 = 0 (negative flag)
//
// check_negative:
//   %rax = -123 (negative, SF is set)
//   %r8 = 1 (set negative flag)
//   negq %rax -> %rax = 123 (absolute value, no overflow)
//   jno -> proceed to convert_number
//
// convert_loop - Iteration 1:
//   %rdx = 0 (clear for division)
//   %rbx = 10 (divisor)
//   123 / 10 -> %rax = 12 (quotient), %rdx = 3 (remainder)
//   %dl = 3 + '0' = 51 = '3' (ASCII)
//   %rsi-- (move left in buffer)
//   Store '3' at %rsi
//   %rcx = 1 (one character written)
//   %rax = 12 (not zero, continue)
//
// convert_loop - Iteration 2:
//   %rdx = 0
//   12 / 10 -> %rax = 1, %rdx = 2
//   %dl = 2 + '0' = '2' (ASCII 50)
//   %rsi-- (move left)
//   Store '2' at %rsi
//   %rcx = 2
//   %rax = 1 (not zero, continue)
//
// convert_loop - Iteration 3:
//   %rdx = 0
//   1 / 10 -> %rax = 0, %rdx = 1
//   %dl = 1 + '0' = '1' (ASCII 49)
//   %rsi-- (move left)
//   Store '1' at %rsi
//   %rcx = 3
//   %rax = 0 (zero, exit loop)
//
// Buffer now contains: "123" at positions (%rsi, %rsi+1, %rsi+2)
// Memory layout: [???????...???????123]
//                                 ^%rsi
//
// Add negative sign:
//   %r8 = 1 (was negative)
//   %rsi-- (move left)
//   Store '-' at %rsi
//   %rcx = 4 (total characters: '-', '1', '2', '3')
//
// Buffer now contains: "-123" at positions (%rsi through %rsi+3)
// Memory layout: [???????...??????-123]
//                                ^%rsi
//
// write_output:
//   Setup syscall: %rax=1, %rdi=1, %rsi=ptr, %rdx=4
//   syscall -> sys_write(1, %rsi, 4)
//   Kernel writes 4 bytes from %rsi to stdout
//   Terminal displays: -123
//
// Epilogue:
//   leave (restore %rsp and %rbp)
//   ret (return to caller)

// WHY THIS APPROACH?
//
// 1. DIVISION ALGORITHM:
//    - Natural way to extract digits in any base
//    - Each division gives us one digit (the remainder)
//    - Works for any base (binary, octal, hex, decimal)
//    - Same algorithm used in computer science for centuries
//
// 2. RIGHT-TO-LEFT CONSTRUCTION:
//    - Division gives digits in reverse order (LSD first)
//    - Building backward avoids reversing the string later
//    - More efficient than extracting digits forward
//    - Pointer arithmetic is simple: just decrement
//
// 3. STACK BUFFER:
//    - Temporary storage, automatically deallocated on return
//    - No need for static/global buffer
//    - Thread-safe (each call has own buffer)
//    - Fast allocation (just adjust stack pointer)
//
// 4. SPECIAL CASE HANDLING:
//    - Zero: needs exactly one digit (division wouldn't work)
//    - Negative: needs sign flag and minus character
//    - INT_MIN: cannot be negated, requires unsigned arithmetic

// INT_MIN EDGE CASE EXPLAINED:
//
// Two's complement representation (64-bit):
// - Positive range: 0 to 2^63 - 1 (9,223,372,036,854,775,807)
// - Negative range: -2^63 to -1 (-9,223,372,036,854,775,808 to -1)
//
// ASYMMETRY: |MIN| > |MAX|
// - Absolute value of INT_MIN is 2^63 = 9,223,372,036,854,775,808
// - But maximum positive is 2^63 - 1 = 9,223,372,036,854,775,807
// - Therefore: -INT_MIN doesn't fit in signed 64-bit!
//
// What happens with negq on INT_MIN?
// - Binary: 1000000000000000000000000000000000000000000000000000000000000000
// - Two's complement negation: invert bits, add 1
// - Invert: 0111111111111111111111111111111111111111111111111111111111111111
// - Add 1:  1000000000000000000000000000000000000000000000000000000000000000
// - Result: Same as original! (overflow)
// - Overflow flag (OF) is set
//
// Our solution:
// - Detect overflow with 'jno' (jump if no overflow)
// - If overflow occurred, use unsigned value 2^63 directly
// - movq $9223372036854775808, %rax
// - Division works correctly with unsigned interpretation
// - Output: "-9223372036854775808" (correct!)

// COMPARISON WITH C CODE:
//
// This assembly is similar to:
//
// void writeInt(long n) {
//     char buffer[32];
//     char *ptr = &buffer[31];  // Start at end
//     int count = 0;
//     int negative = 0;
//
//     if (n == 0) {
//         *ptr = '0';
//         count = 1;
//     } else {
//         if (n < 0) {
//             negative = 1;
//             if (n == LONG_MIN) {
//                 n = (unsigned long)LONG_MIN;  // Special case
//             } else {
//                 n = -n;
//             }
//         }
//         while (n > 0) {
//             *ptr-- = '0' + (n % 10);
//             n /= 10;
//             count++;
//         }
//         if (negative) {
//             *ptr-- = '-';
//             count++;
//         }
//         ptr++;  // ptr points to first character
//     }
//     write(1, ptr, count);
// }

// TRY IT:
//
// Create test program (test_writeInt.s):
//   .section .text
//   .global _start
//   _start:
//       mov $-123, %rdi     # Number to print
//       call writeInt
//       mov $60, %rax       # Exit
//       mov $0, %rdi
//       syscall
//
// Compile and run:
//   as 03-write-integer.s -o writeInt.o
//   as test_writeInt.s -o test_writeInt.o
//   ld writeInt.o test_writeInt.o -o test_writeInt
//   ./test_writeInt
//   -123                    # Output
//
// Test edge cases:
//   mov $0, %rdi                           # Output: 0
//   mov $42, %rdi                          # Output: 42
//   mov $-1, %rdi                          # Output: -1
//   mov $9223372036854775807, %rdi         # Output: 9223372036854775807 (INT_MAX)
//   mov $-9223372036854775808, %rdi        # Output: -9223372036854775808 (INT_MIN)

// DEBUGGING TIPS:
//
// Use GDB to trace execution:
//   gdb ./test_writeInt
//   break writeInt
//   run
//
// After each division:
//   si                          # Step instruction
//   p/d $rax                    # Quotient (decimal)
//   p/d $rdx                    # Remainder (decimal)
//   p/c $dl                     # Remainder as character
//   x/32xb $rsi                 # View buffer contents
//
// View final string:
//   # After conversion loop:
//   x/s $rsi                    # View as string
//   p/d $rcx                    # Character count
//
// Common issues:
//   - Forgot to clear %rdx before divq (division gives wrong results)
//   - Buffer too small (writes beyond buffer, crashes)
//   - Wrong character count (outputs garbage or truncates)
//   - Didn't handle INT_MIN (outputs wrong value)
