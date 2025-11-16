// PEDAGOGICAL PURPOSE:
// This program demonstrates how to convert a binary integer to ASCII and write it
// to standard output. It's the inverse of readInt - converting from binary representation
// to human-readable decimal digits. This is a complex but essential function that shows
// string building, division algorithm, stack buffer management, and edge case handling.
//
// Key learning objectives:
// 1. Understanding binary-to-ASCII conversion algorithm
// 2. Learning how to build strings backward (right-to-left)
// 3. Seeing how to handle special cases (zero, negative numbers, INT_MIN)
// 4. Understanding the division algorithm for extracting digits
// 5. Learning proper stack frame management with local buffers
// 6. Understanding two's complement edge cases (INT_MIN cannot be negated)
// 7. Using the stack for temporary string storage
// 8. Learning about unsigned division and how it helps with INT_MIN
// 9. Understanding ASCII encoding of digits (adding '0' to convert)
// 10. Seeing complete function with prologue, body, epilogue pattern

        .text                       # CODE SECTION
        .global writeInt            # Make this function available to other files
                                   # Other assembly files can call writeInt


    # FUNCTION: writeInt
    # ==================
    # Writes the integer value in %rdi to the standard output.
    #
    # ALGORITHM OVERVIEW:
    # 1. Handle special case: zero (just print "0")
    # 2. Handle negative numbers (save sign, work with absolute value)
    # 3. Extract digits by repeatedly dividing by 10 (RIGHT-TO-LEFT)
    # 4. Convert each digit to ASCII by adding '0'
    # 5. Store digits in buffer from right to left
    # 6. Add minus sign if needed (to the left of digits)
    # 7. Write the complete string to stdout
    #
    # WHY RIGHT-TO-LEFT?
    # Division gives us digits in reverse order (least significant first)
    # Example: 123 / 10 = 12 remainder 3 (we get '3' first)
    # By building the string backward, we get the correct order naturally
    # Alternative would be: extract forward, then reverse - more complex!
    #
    # This function converts the integer to its ASCII string representation
    # and outputs it using the sys_write system call.
    #
    # Inputs:
    #   %rdi - The integer to be printed (64-bit signed long)
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
    #   - null terminator (if needed, not used here)
    #   - alignment padding
    #
    # Notes:
    #   - The function handles negative integers, including INT_MIN (-2^63),
    #     which cannot be negated in two's complement representation.
    #   - The integer is converted to a string by repeatedly dividing by 10
    #     and storing the digits in reverse order in the buffer.
    #   - Special handling for zero (single digit '0')
    #   - Special handling for INT_MIN (overflow on negation)

writeInt:
        # FUNCTION PROLOGUE - Setup stack frame
        # ======================================
        # Standard function entry sequence
        pushq   %rbp                # Save the caller's base pointer on the stack
                                   # %rbp is a callee-saved register (our responsibility)
                                   # Must preserve it for the caller
                                   # Stack: [..., old_rbp]

        movq    %rsp, %rbp          # Set base pointer (%rbp) to current stack pointer (%rsp)
                                   # Establishes our stack frame
                                   # %rbp now points to saved old_rbp
                                   # Provides stable reference for local variables
                                   # %rbp doesn't change, %rsp might

        subq    $40, %rsp           # Allocate 40 bytes on the stack for local variables
                                   # WHY 40? 32 bytes for buffer + 8 bytes for alignment
                                   # Keeps stack 16-byte aligned per System V ABI
                                   # STACK LAYOUT:
                                   # %rbp + 0  : saved old_rbp (from push)
                                   # %rbp - 1  to %rbp - 32 : buffer (32 bytes)
                                   # %rbp - 40 to %rbp - 33 : padding (8 bytes)
                                   # %rsp points to %rbp - 40

        # INITIALIZE VARIABLES
        # ====================
        # We build the string from RIGHT to LEFT (backward)
        # Start at the END of the buffer and work toward the beginning

        # Variables we'll use:
        # %rsi - Pointer to current position in buffer (moves left)
        # %rcx - Digit count (number of characters written)
        # %rax - Working value (the number being converted)
        # %r8  - Negative flag (0 = positive, 1 = negative)

        # Initialize buffer pointer to the end of the buffer (start filling from the end)
        leaq    -1(%rbp), %rsi      # Load effective address (%rbp - 1) into %rsi
                                   # %rsi now points to the LAST byte of the buffer
                                   # This is the rightmost position for our string
                                   # We'll move LEFT as we add digits
                                   # Think of it as writing right-to-left

        movq    $0, %rcx            # Initialize digit count to 0
                                   # %rcx will track how many characters we've written
                                   # Used later to tell sys_write how many bytes to output

        movq    %rdi, %rax          # Move the input integer from %rdi to %rax for processing
                                   # %rdi holds the parameter (the number to print)
                                   # %rax is used for division operations
                                   # We need to preserve %rdi's original role

        movq    $0, %r8             # Initialize negative flag in %r8 to 0 (0 means positive)
                                   # %r8 = 0: number is positive or zero
                                   # %r8 = 1: number is negative
                                   # We need this flag because we work with absolute value

        # SPECIAL CASE: HANDLE ZERO
        # =========================
        # Zero is special: it's neither positive nor negative
        # and we need exactly one digit ('0')
        # Without this check, the division loop wouldn't run (0/10 = 0)
        cmpq    $0, %rax            # Compare %rax with zero
                                   # Sets flags based on value

        jne     check_negative      # If %rax is not zero, skip to check_negative
                                   # Zero is the ONLY number that needs special handling here

        # ZERO CASE: Print single '0'
        movb    $'0', (%rsi)        # Store ASCII character '0' into the buffer at (%rsi)
                                   # movb = move byte (8 bits)
                                   # $'0' = ASCII code 48 (0x30)
                                   # Buffer now contains: [garbage... '0']
                                   # %rsi points to '0'

        movq    $1, %rcx            # Set digit count to 1
                                   # We wrote one character: '0'

        jmp     write_output        # Jump to write_output to print the character
                                   # Skip all the negative/division logic

check_negative:
        # CHECK IF NUMBER IS NEGATIVE
        # ============================
        # In two's complement, negative numbers have high bit (sign bit) set
        testq   %rax, %rax          # Test %rax; sets flags based on the value
                                   # testq does a bitwise AND and sets flags
                                   # If high bit is set, sign flag (SF) is set
                                   # Effectively checks if %rax < 0

        jge     convert_number      # If %rax >= 0 (signed), skip to convert_number
                                   # jge = jump if greater or equal (checks sign flag)
                                   # For positive numbers, no special handling needed

        # NEGATIVE NUMBER HANDLING
        # ========================
        # For negative numbers:
        # 1. Set negative flag (to remember to add '-' later)
        # 2. Convert to absolute value (to extract digits)
        # 3. Handle INT_MIN edge case (can't negate it!)

        movq    $1, %r8             # Set negative flag (%r8) to 1
                                   # Remember: we need to add '-' later

        negq    %rax                # Negate %rax to get the absolute value
                                   # negq = two's complement negation
                                   # For most negative numbers, this works fine
                                   # Example: -5 (0xFFFFFFFFFFFFFFFB) -> 5 (0x0000000000000005)
                                   # BUT: For INT_MIN, this OVERFLOWS!

        jno     convert_number      # If no overflow, proceed to convert_number
                                   # jno = jump if no overflow flag
                                   # WHY CHECK OVERFLOW?
                                   # If overflow occurred, we're dealing with INT_MIN

        # SPECIAL CASE: HANDLE INT_MIN (-2^63)
        # ====================================
        # INT_MIN = -9,223,372,036,854,775,808 = 0x8000000000000000
        # In two's complement, this number CANNOT be negated!
        #
        # WHY?
        # - Positive range: 0 to 2^63 - 1 (9,223,372,036,854,775,807)
        # - Negative range: -2^63 to -1 (-9,223,372,036,854,775,808 to -1)
        # - Notice: |INT_MIN| = 2^63 > INT_MAX = 2^63 - 1
        # - The absolute value doesn't fit in signed 64-bit!
        #
        # WHAT HAPPENS WITH negq ON INT_MIN?
        # - Binary: 1000...000 (INT_MIN)
        # - Two's complement negation: invert bits -> 0111...111, add 1 -> 1000...000
        # - Result: INT_MIN again! (overflow detected by OF flag)
        #
        # SOLUTION:
        # Use the UNSIGNED value 2^63 directly
        # Division will work correctly with unsigned arithmetic

        # Handle INT_MIN (-2^63), which cannot be negated
        movq    $9223372036854775808, %rax  # Set %rax to 2^63
                                           # This is the UNSIGNED representation
                                           # In hex: 0x8000000000000000
                                           # Same bit pattern as INT_MIN, but interpreted unsigned
                                           # Division will treat it as unsigned, giving correct digits

convert_number:
        # CONVERT INTEGER TO STRING
        # =========================
        # Algorithm: Repeated division by 10
        # At each step:
        #   quotient = number / 10
        #   remainder = number % 10
        #   digit = remainder (0-9)
        #   Store ASCII digit ('0' + digit)
        #   Move left in buffer
        #   Continue with quotient
        #
        # EXAMPLE: 123
        #   123 / 10 = 12 remainder 3 -> store '3' at rightmost position
        #   12 / 10 = 1 remainder 2   -> store '2' to left of '3'
        #   1 / 10 = 0 remainder 1    -> store '1' to left of '2'
        #   Result: "123" (correct order!)
        #
        # Why this works:
        # - Division by 10 extracts one decimal digit (the remainder)
        # - Digits come out right-to-left (least significant first)
        # - By storing right-to-left, we build the string in correct order

convert_loop:
        # STEP 1: PREPARE FOR DIVISION
        # ============================
        xorq    %rdx, %rdx          # Clear %rdx (set to zero) before division
                                   # WHY? divq divides %rdx:%rax by operand
                                   # We want to divide just %rax, so %rdx must be 0
                                   # xorq %rdx,%rdx is faster/shorter than movq $0,%rdx
                                   # Any value XOR itself = 0

        movq    $10, %rbx           # Load divisor 10 into %rbx
                                   # We're extracting digits in base 10 (decimal)

        # STEP 2: DIVIDE BY 10
        # ====================
        divq    %rbx                # Unsigned divide %rdx:%rax by %rbx (10)
                                   # divq = unsigned 64-bit division
                                   # Dividend: 128-bit value in %rdx:%rax (we cleared %rdx, so it's just %rax)
                                   # Divisor: %rbx (10)
                                   # After division:
                                   # - %rax: quotient (number / 10)
                                   # - %rdx: remainder (number % 10)
                                   # Example: 123 / 10 -> %rax = 12, %rdx = 3
                                   #
                                   # WHY UNSIGNED (divq) instead of signed (idivq)?
                                   # For INT_MIN case, we're using unsigned 2^63
                                   # Unsigned division gives correct result

        # STEP 3: CONVERT REMAINDER TO ASCII
        # ==================================
        addb    $'0', %dl           # Convert remainder to ASCII character
                                   # %dl = low 8 bits of %rdx (the digit 0-9)
                                   # Adding '0' (48) converts to ASCII
                                   # Example: 3 + 48 = 51 = ASCII '3'
                                   # Example: 9 + 48 = 57 = ASCII '9'
                                   # addb = add byte (8-bit operation)

        # STEP 4: STORE IN BUFFER
        # =======================
        subq    $1, %rsi            # Move back one byte in the buffer
                                   # We're building the string RIGHT-TO-LEFT
                                   # %rsi now points to the next position (to the left)
                                   # Think: writing backward

        movb    %dl, (%rsi)         # Store character in buffer at (%rsi)
                                   # Write the ASCII digit to the buffer
                                   # movb = move byte (8 bits)
                                   # Buffer now has one more digit

        # STEP 5: UPDATE COUNT
        # ====================
        incq    %rcx                # Increment digit count
                                   # Track total number of characters written
                                   # We need this for sys_write (tells it how many bytes)

        # STEP 6: CHECK IF DONE
        # =====================
        cmpq    $0, %rax            # Compare quotient %rax with zero
                                   # Are we done processing all digits?
                                   # When quotient becomes 0, we've extracted all digits

        jne     convert_loop        # If quotient is not zero, repeat loop
                                   # Keep extracting digits until quotient = 0
                                   # When quotient = 0, we've processed all digits

        # At this point:
        # - All digits are in the buffer (right-to-left, so correct order)
        # - %rsi points to the FIRST digit (leftmost)
        # - %rcx contains the number of digits
        # - %r8 tells us if the number was negative

        # ADD NEGATIVE SIGN IF NECESSARY
        # ==============================
        cmpq    $0, %r8             # Check negative flag (%r8)
                                   # Was the original number negative?

        je      write_output        # If number is positive (flag=0), skip to write_output
                                   # No minus sign needed

        # NEGATIVE: Add minus sign
        subq    $1, %rsi            # Move back one byte for '-'
                                   # Make room for the minus sign (to the left of digits)

        movb    $'-', (%rsi)        # Store '-' in buffer at (%rsi)
                                   # ASCII code for '-' is 45 (0x2D)
                                   # Now %rsi points to the complete string (with minus sign)
                                   # Buffer: [... '-' '1' '2' '3' ...]

        incq    %rcx                # Increment digit count
                                   # Account for the minus sign character
                                   # %rcx now has total length of string (digits + sign)

write_output:
        # PREPARE FOR WRITE SYSCALL
        # =========================
        # SYSCALL: write to file descriptor
        # Syscall number 1 = sys_write
        # Writes bytes from a buffer to a file descriptor
        #
        # sys_write arguments:
        #   %rax - syscall number (1)
        #   %rdi - file descriptor (1 for stdout)
        #   %rsi - pointer to buffer (already set)
        #   %rdx - number of bytes to write

        movq    $1, %rax            # Syscall number for sys_write
                                   # 1 = write to file descriptor
                                   # Tells kernel we want to output data

        movq    $1, %rdi            # File descriptor for stdout
                                   # File descriptors:
                                   #   0 = stdin  (standard input)
                                   #   1 = stdout (standard output)
                                   #   2 = stderr (standard error)

        # %rsi already points to the start of the string
        # We set %rsi to point to the first character earlier
        # Either the first digit, or the minus sign if negative
        # No need to modify %rsi here

        movq    %rcx, %rdx          # Set %rdx to digit count (number of bytes)
                                   # Tell sys_write how many bytes to output
                                   # This is the length of our string

        syscall                     # Make the system call
                                   # Transfers control to kernel
                                   # Kernel writes %rdx bytes from %rsi to stdout
                                   # Text appears on terminal
                                   # Returns number of bytes written in %rax (we ignore it)

        # FUNCTION EPILOGUE - Restore stack frame and return
        # ===================================================
        leave                       # Restore stack frame
                                   # 'leave' is shorthand for:
                                   #   movq %rbp, %rsp    (deallocate locals)
                                   #   popq %rbp          (restore caller's %rbp)
                                   # Dismantles our stack frame efficiently

        ret                         # Return from the function
                                   # Pops return address from stack and jumps to it
                                   # Caller continues execution

// COMPLETE EXECUTION EXAMPLE:
//
// Call: writeInt with %rdi = -42
//
// STEP-BY-STEP TRACE:
//
// Prologue:
//   push %rbp              # Save caller's %rbp
//   mov %rsp,%rbp          # Establish our frame
//   sub $40,%rsp           # Allocate locals
//
// Initialization:
//   leaq -1(%rbp),%rsi     # %rsi = end of buffer
//   movq $0,%rcx           # %rcx = 0 (count)
//   movq %rdi,%rax         # %rax = -42
//   movq $0,%r8            # %r8 = 0 (negative flag)
//
// Check zero:
//   cmpq $0,%rax           # -42 != 0
//   jne check_negative     # Jump
//
// Check negative:
//   testq %rax,%rax        # -42 < 0 (negative)
//   jge convert_number     # Don't jump
//   movq $1,%r8            # %r8 = 1 (set negative flag)
//   negq %rax              # %rax = 42 (absolute value)
//   jno convert_number     # No overflow, jump
//
// convert_loop - Iteration 1:
//   xorq %rdx,%rdx         # %rdx = 0
//   movq $10,%rbx          # %rbx = 10
//   divq %rbx              # 42 / 10 -> %rax = 4, %rdx = 2
//   addb $'0',%dl          # %dl = '2' (ASCII 50)
//   subq $1,%rsi           # Move buffer pointer left
//   movb %dl,(%rsi)        # Store '2'
//   incq %rcx              # %rcx = 1
//   cmpq $0,%rax           # 4 != 0
//   jne convert_loop       # Continue
//
// convert_loop - Iteration 2:
//   xorq %rdx,%rdx         # %rdx = 0
//   divq %rbx              # 4 / 10 -> %rax = 0, %rdx = 4
//   addb $'0',%dl          # %dl = '4' (ASCII 52)
//   subq $1,%rsi           # Move buffer pointer left
//   movb %dl,(%rsi)        # Store '4'
//   incq %rcx              # %rcx = 2
//   cmpq $0,%rax           # 0 == 0
//   jne convert_loop       # Don't jump, exit loop
//
// Buffer now contains: [... '4' '2' ...]
// %rsi points to '4', %rcx = 2
//
// Add negative sign:
//   cmpq $0,%r8            # %r8 = 1 (was negative)
//   je write_output        # Don't jump
//   subq $1,%rsi           # Move buffer pointer left
//   movb $'-',(%rsi)       # Store '-'
//   incq %rcx              # %rcx = 3
//
// Buffer now contains: [... '-' '4' '2' ...]
// %rsi points to '-', %rcx = 3
//
// write_output:
//   movq $1,%rax           # write syscall
//   movq $1,%rdi           # stdout
//   movq %rcx,%rdx         # %rdx = 3 (bytes to write)
//   syscall                # write(1, "-42", 3)
//   Output: "-42"
//
// Epilogue:
//   leave                  # Restore stack
//   ret                    # Return

// WHY THIS APPROACH?
//
// 1. DIVISION ALGORITHM:
//    - Natural way to extract digits in any base
//    - Each division gives us one digit (the remainder)
//    - Works for binary, octal, decimal, hex
//    - Standard algorithm taught in CS courses
//
// 2. RIGHT-TO-LEFT CONSTRUCTION:
//    - Division gives digits in reverse order (least significant first)
//    - Building backward avoids reversing the string later
//    - More efficient than extracting forward then reversing
//    - Uses less memory (no need for second buffer)
//
// 3. STACK BUFFER:
//    - Temporary storage, automatically deallocated
//    - No need for static/global buffer
//    - Thread-safe (each call has own buffer)
//    - Better than malloc (no allocation overhead)
//
// 4. SPECIAL CASE HANDLING:
//    - Zero: needs exactly one digit ('0')
//    - Negative: needs sign flag and minus character
//    - INT_MIN: cannot be negated, use unsigned arithmetic
//    - Each case handled correctly

// INT_MIN EDGE CASE EXPLAINED IN DETAIL:
//
// Two's complement representation (64-bit signed):
// - Range: -2^63 to 2^63 - 1
// - Minimum: -9,223,372,036,854,775,808 (INT_MIN)
// - Maximum: +9,223,372,036,854,775,807 (INT_MAX)
//
// Notice: |INT_MIN| > |INT_MAX|
// - Absolute value of INT_MIN is 2^63 = 9,223,372,036,854,775,808
// - But maximum positive is 2^63 - 1 = 9,223,372,036,854,775,807
// - Therefore: -INT_MIN doesn't fit in signed 64-bit!
//
// What happens with negq on INT_MIN?
// INT_MIN in binary: 1000 0000 0000 0000 ... 0000 (high bit set, rest zero)
// Two's complement negation:
//   1. Invert all bits: 0111 1111 1111 1111 ... 1111
//   2. Add 1:           1000 0000 0000 0000 ... 0000
// Result: Same as INT_MIN! (overflow detected by OF flag)
//
// Our solution:
// - Detect overflow with 'jno' (jump if no overflow)
// - If overflow, use unsigned value 2^63 directly
// - Division treats it as unsigned, extracts digits correctly
// - We already have the sign flag set, so '-' will be added
// - Output: "-9223372036854775808" (correct!)
//
// Why unsigned division works:
// - Binary representation is same: 0x8000000000000000
// - Signed: interprets as -2^63
// - Unsigned: interprets as 2^63
// - Division extracts same digits either way
// - Example: 2^63 / 10 gives same digits as |-2^63| would

// EQUIVALENT C CODE:
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
// Create test program:
//   .section .text
//   .extern writeInt
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
//   as test.s -o test.o
//   ld writeInt.o test.o -o test
//   ./test
//   -123                    # Output
//
// Test edge cases:
//   mov $0, %rdi            # Output: 0
//   mov $9223372036854775807, %rdi   # Output: 9223372036854775807 (INT_MAX)
//   mov $-9223372036854775808, %rdi  # Output: -9223372036854775808 (INT_MIN)
//   mov $1, %rdi            # Output: 1
//   mov $-1, %rdi           # Output: -1
//   mov $999, %rdi          # Output: 999

// PERFORMANCE CONSIDERATIONS:
//
// 1. DIVISION IS SLOW:
//    - Division is one of the slowest CPU operations (10-100 cycles)
//    - Alternative: multiplication by reciprocal (compiler optimization)
//    - For pedagogical code, clarity > performance
//    - Modern CPUs have pipelined dividers, not as bad as old CPUs
//
// 2. STACK VS HEAP:
//    - Stack allocation is very fast (just adjust %rsp)
//    - Heap allocation would require malloc/free (much slower)
//    - Stack is appropriate for temporary, fixed-size buffers
//    - Automatic deallocation (no memory leaks possible)
//
// 3. BUFFER SIZE:
//    - 32 bytes is conservative (only need ~21 for 64-bit)
//    - Larger buffer wastes stack space (8 bytes extra)
//    - Smaller buffer risks overflow (though we know max size)
//    - 32 is power of 2, good for alignment
//
// 4. LOOP ITERATIONS:
//    - Number of iterations = number of digits
//    - Max for 64-bit: 20 digits (for INT_MIN)
//    - Linear in digits, not in value
//    - O(log₁₀ n) iterations

// COMMON BUGS AND HOW WE AVOID THEM:
//
// 1. Forgot to handle zero:
//    - Would result in empty string
//    - Our code: explicit check, output "0"
//
// 2. Forgot to handle negative:
//    - Would output garbage or wrong value
//    - Our code: check sign, negate, add '-'
//
// 3. Didn't handle INT_MIN:
//    - negq would overflow, give INT_MIN again
//    - Our code: check overflow, use unsigned value
//
// 4. Built string forward:
//    - Would need to reverse (more complex)
//    - Our code: build backward (natural order)
//
// 5. Used wrong buffer size:
//    - Too small: overflow/corruption
//    - Our code: 32 bytes is plenty for any 64-bit int
//
// 6. Didn't track string length:
//    - Would print garbage after digits
//    - Our code: %rcx tracks exact length

// DEBUGGING TIPS:
//
// 1. Print intermediate values:
//    - After each division, print quotient and remainder
//    - Verify digits are extracted correctly
//    - Check buffer pointer moves correctly
//
// 2. Hexdump the buffer:
//    - Use gdb: x/32xb $rbp-32
//    - See exact bytes including ASCII codes
//    - Verify string is formed correctly
//
// 3. Test systematically:
//    - Start with 0 (special case)
//    - Try single digit: 1, 9
//    - Try two digits: 10, 99
//    - Try negative: -1, -99
//    - Try INT_MAX and INT_MIN
//
// 4. Use debugger:
//    - Set breakpoint at convert_loop
//    - Watch %rax (quotient) decrease
//    - Watch %rcx (count) increase
//    - Examine buffer as it fills

// EXTENSIONS AND EXERCISES:
//
// 1. Add padding:
//    - Pad with zeros: 00123
//    - Pad with spaces:    123
//    - Right align in field width
//
// 2. Different bases:
//    - Binary (base 2)
//    - Octal (base 8)
//    - Hexadecimal (base 16)
//    - Just change divisor and digit chars
//
// 3. Thousands separator:
//    - Output: 1,234,567
//    - Add comma every 3 digits
//    - Count from right, insert commas
//
// 4. Scientific notation:
//    - Output: 1.23e+5
//    - Requires floating point
//    - More complex algorithm
//
// 5. Error handling:
//    - Check syscall return value
//    - Handle write errors
//    - Retry on EINTR
