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

          .text
          .global writeInt      # Make this function available to other files
                                # Other assembly files can call writeInt

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
      # Inputs:
      #   %rdi - The integer to be printed.
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
          # FUNCTION PROLOGUE: set up the stack frame
          # Standard function entry sequence
          pushq   %rbp                # Save the caller's base pointer on the stack
                                      # %rbp is a callee-saved register (our responsibility)

          movq    %rsp, %rbp          # Set base pointer (%rbp) to current stack pointer (%rsp)
                                      # Establishes our stack frame
                                      # %rbp provides stable reference for local variables

          subq    $40, %rsp           # Allocate 40 bytes on the stack for local variables
                                      # WHY 40? 32 bytes for buffer + 8 bytes for alignment
                                      # Keeps stack 16-byte aligned per ABI requirement
                                      # STACK LAYOUT:
                                      # %rbp - 1  to %rbp - 32 : buffer (32 bytes)
                                      # %rbp - 40 to %rbp - 33 : padding (8 bytes)

          # Variables:
          # We will use a buffer of 32 bytes on the stack to store the string representation
          # of the integer. The buffer extends from address (%rbp - 32) to (%rbp - 1).
          # We will use %rsi as a pointer to the current position in the buffer.

          # INITIALIZATION:
          # We build the string from RIGHT to LEFT
          # Start at the END of the buffer and work backward

          # Initialize buffer pointer to the end of the buffer (start filling from the end)
          leaq    -1(%rbp), %rsi      # Load effective address (%rbp - 1) into %rsi
                                      # %rsi now points to the LAST byte of the buffer
                                      # We'll move LEFT as we add digits
                                      # This is the rightmost position for our string

          movq    $0, %rcx            # Initialize digit count to 0
                                      # %rcx will track how many characters we've written
                                      # Used later to tell sys_write how many bytes to output

          movq    %rdi, %rax          # Move the input integer from %rdi to %rax for processing
                                      # %rdi holds the parameter (the number to print)
                                      # %rax is used for division operations
                                      # We need to preserve %rdi's role and use %rax for division

          movq    $0, %r8             # Initialize negative flag in %r8 to 0 (0 means positive)
                                      # %r8 = 0: number is positive or zero
                                      # %r8 = 1: number is negative
                                      # We need this flag because we work with absolute value

          # SPECIAL CASE: Handle zero
          # Zero is special: it's neither positive nor negative
          # and we need exactly one digit ('0')
          cmpq    $0, %rax            # Compare %rax with zero
                                      # Sets flags based on value
          jne     check_negative      # If %rax is not zero, skip to check_negative
                                      # Zero is the ONLY number that needs special handling

          # If %rax is zero:
          movb    $'0', (%rsi)        # Store ASCII character '0' into the buffer at (%rsi)
                                      # movb = move byte (8 bits)
                                      # $'0' = ASCII code 48
          movq    $1, %rcx            # Set digit count to 1
                                      # We wrote one character: '0'
          jmp     write_output        # Jump to write_output to print the character
                                      # Skip all the negative/division logic

  check_negative:
          # Check if the number is negative
          # In two's complement, negative numbers have high bit set
          testq   %rax, %rax          # Test %rax; sets flags based on the value
                                      # testq does a bitwise AND and sets flags
                                      # If high bit is set, sign flag (SF) is set
          jge     convert_number      # If %rax >= 0 (signed), skip to convert_number
                                      # jge = jump if greater or equal (checks sign flag)

          # If the number is negative:
          movq    $1, %r8             # Set negative flag (%r8) to 1
                                      # Remember: we need to add '-' later

          negq    %rax                # Negate %rax to get the absolute value
                                      # negq = two's complement negation
                                      # For most negative numbers, this works fine
                                      # Example: -5 becomes 5

          jno     convert_number      # If no overflow, proceed to convert_number
                                      # jno = jump if no overflow flag
                                      # WHY CHECK OVERFLOW?

          # SPECIAL CASE: Handle INT_MIN (-2^63)
          # INT_MIN = -9,223,372,036,854,775,808 = 0x8000000000000000
          # In two's complement, this number CANNOT be negated!
          # Why? Its absolute value (2^63) doesn't fit in a signed 64-bit integer
          # The maximum positive value is 2^63 - 1
          #
          # WHAT HAPPENS:
          # negq on INT_MIN produces INT_MIN again (overflow)
          # We detect this with the overflow flag (OF)
          # Solution: Use the unsigned value 2^63 directly

          # Handle INT_MIN (-2^63), which cannot be negated
          movq    $9223372036854775808, %rax  # Set %rax to 2^63 (absolute value of INT_MIN)
                                              # This is the UNSIGNED value
                                              # In hex: 0x8000000000000000
                                              # We'll process it as unsigned in division

  convert_number:
          # Convert the integer to its string representation
          # ALGORITHM: Repeated division by 10
          # At each step:
          #   quotient = number / 10
          #   remainder = number % 10
          #   digit = remainder (0-9)
          #   Store ASCII digit, move left in buffer
          #   Continue with quotient
          #
          # EXAMPLE: 123
          #   123 / 10 = 12 remainder 3 -> store '3'
          #   12 / 10 = 1 remainder 2   -> store '2' (to left of '3')
          #   1 / 10 = 0 remainder 1    -> store '1' (to left of '2')
          #   Result: "123"

  convert_loop:
          xorq    %rdx, %rdx          # Clear %rdx (set to zero) before division
                                      # WHY? divq divides %rdx:%rax by operand
                                      # We want to divide just %rax, so %rdx must be 0
                                      # xorq is faster than movq $0

          movq    $10, %rbx           # Load divisor 10 into %rbx
                                      # We're extracting digits in base 10

          divq    %rbx                # Unsigned divide %rdx:%rax by %rbx (10)
                                      # divq = unsigned 64-bit division
                                      # Dividend: 128-bit value in %rdx:%rax
                                      # Divisor: %rbx (10)
                                      # After division:
                                      # - %rax: quotient (number / 10)
                                      # - %rdx: remainder (number % 10)
                                      # Example: 123 / 10 -> %rax=12, %rdx=3

          addb    $'0', %dl           # Convert remainder to ASCII character
                                      # %dl = low 8 bits of %rdx (the digit 0-9)
                                      # Adding '0' (48) converts to ASCII
                                      # Example: 3 + 48 = 51 = ASCII '3'

          subq    $1, %rsi            # Move back one byte in the buffer
                                      # We're building the string RIGHT-TO-LEFT
                                      # %rsi now points to the next position (to the left)

          movb    %dl, (%rsi)         # Store character in buffer at (%rsi)
                                      # Write the ASCII digit to the buffer
                                      # movb = move byte (8 bits)

          incq    %rcx                # Increment digit count
                                      # Track total number of characters written
                                      # We need this for sys_write

          cmpq    $0, %rax            # Compare quotient %rax with zero
                                      # Are we done processing all digits?

          jne     convert_loop        # If quotient is not zero, repeat loop
                                      # Keep extracting digits until quotient is 0
                                      # When quotient = 0, we've processed all digits

          # At this point:
          # - All digits are in the buffer (right-to-left)
          # - %rsi points to the FIRST digit
          # - %rcx contains the number of digits
          # - %r8 tells us if the number was negative

          # Add negative sign if necessary
          cmpq    $0, %r8             # Check negative flag (%r8)
                                      # Was the original number negative?

          je      write_output        # If number is positive (flag=0), skip to write_output
                                      # No minus sign needed

          # If number was negative, add minus sign:
          subq    $1, %rsi            # Move back one byte for '-'
                                      # Make room for the minus sign (to the left of digits)

          movb    $'-', (%rsi)        # Store '-' in buffer at (%rsi)
                                      # ASCII code for '-' is 45
                                      # Now %rsi points to the complete string (with minus sign)

          incq    %rcx                # Increment digit count
                                      # Account for the minus sign character
                                      # %rcx now has total length of string

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

          movq    $1, %rdi            # File descriptor for stdout
                                      # File descriptors:
                                      #   0 = stdin, 1 = stdout, 2 = stderr

          # %rsi already points to the start of the string
          # We set %rsi to point to the first character earlier
          # Either the first digit, or the minus sign if negative

          movq    %rcx, %rdx          # Set %rdx to digit count (number of bytes)
                                      # Tell sys_write how many bytes to output

          syscall                     # Make the system call
                                      # Transfers control to kernel
                                      # Kernel writes %rdx bytes from %rsi to stdout
                                      # Returns number of bytes written in %rax (we ignore it)

          # FUNCTION EPILOGUE: restore stack frame and return
          leave                       # Restore stack frame
                                      # 'leave' is shorthand for:
                                      #   movq %rbp, %rsp    (deallocate locals)
                                      #   popq %rbp          (restore caller's %rbp)
                                      # Dismantles our stack frame

          ret                         # Return from the function
                                      # Pops return address and jumps to it
                                      # Caller continues execution

// COMPLETE EXECUTION EXAMPLE:
//
// Call: writeInt with %rdi = -123
//
// STEP-BY-STEP TRACE:
//
// Prologue:
//   push %rbp, mov %rsp,%rbp, sub $40,%rsp
//   Stack frame established
//
// Initialization:
//   %rsi = %rbp - 1 (points to end of buffer)
//   %rcx = 0 (digit count)
//   %rax = -123 (number to convert)
//   %r8 = 0 (negative flag)
//
// check_negative:
//   %rax = -123 (negative)
//   %r8 = 1 (set negative flag)
//   negq %rax -> %rax = 123 (absolute value)
//
// convert_loop - Iteration 1:
//   %rdx = 0
//   123 / 10 -> %rax = 12, %rdx = 3
//   %dl + '0' = '3' (ASCII 51)
//   %rsi-- (move left)
//   Store '3' at %rsi
//   %rcx = 1
//
// convert_loop - Iteration 2:
//   %rdx = 0
//   12 / 10 -> %rax = 1, %rdx = 2
//   %dl + '0' = '2' (ASCII 50)
//   %rsi-- (move left)
//   Store '2' at %rsi
//   %rcx = 2
//
// convert_loop - Iteration 3:
//   %rdx = 0
//   1 / 10 -> %rax = 0, %rdx = 1
//   %dl + '0' = '1' (ASCII 49)
//   %rsi-- (move left)
//   Store '1' at %rsi
//   %rcx = 3
//   %rax = 0, exit loop
//
// Buffer now contains: "123" (at positions %rsi, %rsi+1, %rsi+2)
//
// Add negative sign:
//   %r8 = 1 (was negative)
//   %rsi-- (move left)
//   Store '-' at %rsi
//   %rcx = 4
//
// Buffer now contains: "-123" (at positions %rsi through %rsi+3)
//
// write_output:
//   sys_write(1, %rsi, 4)
//   Writes "-123" to stdout
//
// Epilogue:
//   leave, ret
//   Stack frame destroyed, return to caller

// WHY THIS APPROACH?
//
// 1. DIVISION ALGORITHM:
//    - Natural way to extract digits in any base
//    - Each division gives us one digit (the remainder)
//    - Works for any base (binary, octal, hex, decimal)
//
// 2. RIGHT-TO-LEFT CONSTRUCTION:
//    - Division gives digits in reverse order (LSD first)
//    - Building backward avoids reversing the string later
//    - More efficient than extracting digits forward
//
// 3. STACK BUFFER:
//    - Temporary storage, automatically deallocated
//    - No need for static/global buffer
//    - Thread-safe (each call has own buffer)
//
// 4. SPECIAL CASE HANDLING:
//    - Zero: needs exactly one digit
//    - Negative: needs sign flag and minus character
//    - INT_MIN: cannot be negated, special handling

// INT_MIN EDGE CASE EXPLAINED:
//
// Two's complement representation (64-bit):
// - Positive range: 0 to 2^63 - 1 (9,223,372,036,854,775,807)
// - Negative range: -2^63 to -1 (-9,223,372,036,854,775,808 to -1)
//
// Notice: |MIN| > |MAX|
// - Absolute value of INT_MIN is 2^63
// - But maximum positive is 2^63 - 1
// - Therefore: -INT_MIN doesn't fit in signed 64-bit!
//
// What happens with negq on INT_MIN?
// - Binary: 1000...000 (INT_MIN)
// - Negation: invert bits -> 0111...111, add 1 -> 1000...000
// - Result: INT_MIN again! (overflow)
//
// Our solution:
// - Detect overflow with 'jno'
// - Use unsigned value 2^63 directly
// - Division works correctly with unsigned value
// - Output: "-9223372036854775808" (correct!)

// COMPARE WITH C CODE:
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
//         ptr++;
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
//   as writeInt.s -o writeInt.o
//   as test_writeInt.s -o test_writeInt.o
//   ld writeInt.o test_writeInt.o -o test_writeInt
//   ./test_writeInt
//   -123                    # Output
//
// Test edge cases:
//   mov $0, %rdi            # Output: 0
//   mov $9223372036854775807, %rdi   # Output: 9223372036854775807 (INT_MAX)
//   mov $-9223372036854775808, %rdi  # Output: -9223372036854775808 (INT_MIN)

// PERFORMANCE CONSIDERATIONS:
//
// 1. DIVISION IS SLOW:
//    - Division is one of the slowest CPU operations
//    - Alternative: multiplication by reciprocal (compiler optimization)
//    - For this pedagogical code, clarity > performance
//
// 2. STACK VS HEAP:
//    - Stack allocation is very fast (just adjust %rsp)
//    - Heap allocation would require malloc/free
//    - Stack is appropriate for temporary, fixed-size buffers
//
// 3. BUFFER SIZE:
//    - 32 bytes is conservative (only need ~21 for 64-bit)
//    - Larger buffer wastes stack space
//    - Smaller buffer risks overflow (though we know max size)
