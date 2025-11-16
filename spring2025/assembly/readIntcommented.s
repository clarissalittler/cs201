// PEDAGOGICAL PURPOSE:
// This program demonstrates how to read an integer from standard input in assembly.
// It shows the complete process of reading bytes from stdin, parsing ASCII digit
// characters, and converting them to a binary integer using Horner's method.
//
// Key learning objectives:
// 1. Understanding how to use the read syscall to get input from stdin
// 2. Learning the ASCII-to-integer conversion algorithm (Horner's method)
// 3. Seeing how to process strings character-by-character in assembly
// 4. Understanding the .bss section for uninitialized data storage
// 5. Learning how to compose functions (readInt calls parseInt)
// 6. Understanding the difference between ASCII representation and binary values
// 7. Using registers for loop counters, accumulators, and temporary storage
// 8. Learning indexed addressing mode for array/buffer access
// 9. Understanding why Horner's method is efficient for base conversion
// 10. Seeing practical application of arithmetic operations in parsing

    .section .bss                # Begin uninitialized data section (.bss)
                                 # .bss = Block Started by Symbol
                                 # This section is for uninitialized global/static variables
                                 # WHY .bss? Saves space in the executable file
                                 # The OS automatically zeros this memory when program loads
                                 # More efficient than .data for large uninitialized arrays

buffer: .zero 128                # Allocate 128 bytes for 'buffer', initialized to zero
                                 # .zero N = reserve N bytes, filled with zeros at load time
                                 # Creates a label 'buffer' pointing to this 128-byte region
                                 # Used to store the input string from stdin
                                 # WHY 128? Large enough for any 64-bit integer:
                                 # - Max 64-bit int: -9,223,372,036,854,775,808 (20 digits + sign)
                                 # - Plus newline character
                                 # - 128 provides comfortable margin

    .section .text               # Begin code section (.text)
                                 # .text section contains executable instructions
    .global readInt              # Make 'readInt' function accessible from other files
                                 # Other assembly files can call this function
                                 # Linker will resolve external references to readInt


  # HELPER FUNCTION: parseInt
  # =========================
  # Parses ASCII digits from a buffer and converts them into a binary integer.
  # Uses Horner's method for efficient base conversion.
  #
  # ALGORITHM: Horner's Method for base-10 conversion
  # For string "123":
  #   result = 0
  #   result = result * 10 + 1 = 1
  #   result = result * 10 + 2 = 12
  #   result = result * 10 + 3 = 123
  #
  # WHY HORNER'S METHOD WORKS:
  # "123" mathematically means: 1×10² + 2×10¹ + 3×10⁰ = 100 + 20 + 3
  # Horner's reformulation: ((1 × 10 + 2) × 10 + 3)
  #   = ((10 + 2) × 10 + 3)
  #   = (12 × 10 + 3)
  #   = (120 + 3)
  #   = 123
  #
  # ADVANTAGES:
  # - Avoids computing powers of 10 (10⁰, 10¹, 10², etc.)
  # - Only one multiply and one add per digit
  # - Works left-to-right through the string (natural order)
  # - Generalizes to any base (binary, hex, etc.)
  # - Numerically stable (no accumulation of rounding errors)
  #
  # Parameters:
  #   - %rdi: Address of the buffer containing the input string
  #   - %rsi: Number of characters to process (string length)
  #   - %rcx: Index into the buffer (should be initialized to 0 before calling)
  #   - %rax: Accumulator (should be initialized to 0 before calling)
  #
  # Returns:
  #   - %rax: The resulting integer value
  #
  # REGISTERS USED:
  #   %rdi - pointer to buffer (read-only, doesn't change)
  #   %rsi - number of digits to process (read-only, doesn't change)
  #   %rcx - loop counter/index into buffer (incremented each iteration)
  #   %rax - accumulator that builds up the result
  #   %r8  - temporary storage for current digit value
  #   %r8b - byte version of %r8 (low 8 bits) for reading characters
  #
  parseInt:
  intRead:
      # STEP 1: READ ONE CHARACTER FROM THE BUFFER
      # ==========================================
      movb (%rdi,%rcx,1), %r8b     # Load one byte from buffer[%rcx] into %r8b
                                   # INDEXED ADDRESSING: (%rdi,%rcx,1)
                                   # Syntax: (base, index, scale)
                                   # Address = base + (index × scale)
                                   # Here: address = %rdi + %rcx × 1 = buffer + index
                                   # %r8b is the low 8 bits of %r8 (one byte)
                                   # This reads one ASCII character
                                   # Example: buffer[0] might be '1' (ASCII 49 = 0x31)

      # STEP 2: CONVERT ASCII CHARACTER TO NUMERIC VALUE
      # ================================================
      sub $'0', %r8                # Convert ASCII character to its numeric value
                                   # ASCII encoding: '0'=48, '1'=49, '2'=50, ..., '9'=57
                                   # Subtracting '0' (48) gives digit value
                                   # Example: '5' (53) - '0' (48) = 5
                                   # Example: '9' (57) - '0' (48) = 9
                                   # NOTE: $'0' is a character constant
                                   # The assembler converts it to the value 48
                                   # After this, %r8 contains 0-9 (the digit value)

      # STEP 3: APPLY HORNER'S METHOD - MULTIPLY ACCUMULATOR BY 10
      # ===========================================================
      imul $10, %rax               # Multiply accumulator (%rax) by 10
                                   # This "shifts left" in decimal
                                   # Makes room for the next digit
                                   # Example: if %rax = 12, after this %rax = 120
                                   # imul = signed integer multiply (immediate operand)
                                   # Result goes back into %rax
                                   # This is the key step of Horner's method

      # STEP 4: ADD THE NEW DIGIT
      # =========================
      add %r8, %rax                # Add the numeric value of current digit to accumulator
                                   # Example: %rax = 120, %r8 = 3, result: %rax = 123
                                   # This completes one iteration of Horner's method
                                   # We've incorporated one more digit into the result

      # STEP 5: MOVE TO NEXT CHARACTER
      # ===============================
      inc %rcx                     # Increment buffer index (%rcx)
                                   # inc = increment by 1
                                   # Moves to the next character in the buffer
                                   # Next iteration will read buffer[%rcx+1]

      # STEP 6: CHECK IF WE'RE DONE
      # ===========================
      cmp %rsi, %rcx               # Compare index with total number of characters to process
                                   # cmp sets flags based on (%rcx - %rsi)
                                   # Are we done processing all digits?

      jl intRead                   # If index < total, repeat the loop
                                   # jl = jump if less (signed comparison)
                                   # Loop continues while %rcx < %rsi
                                   # When %rcx == %rsi, we've processed all digits
                                   # Falls through to ret

      ret                          # Return from function with result in %rax
                                   # The caller expects the integer in %rax
                                   # Pops return address and jumps back


  # MAIN FUNCTION: readInt
  # ======================
  # Reads input from standard input (stdin), stores it in the buffer,
  # and then converts the input string into an integer using parseInt.
  #
  # This is the public interface for reading integers.
  # It orchestrates two tasks:
  # 1. Reading bytes from stdin (using read syscall)
  # 2. Parsing those bytes as an integer (using parseInt helper)
  #
  # Returns:
  #   - %rax: The integer value read from input
  #
  # CALLING CONVENTION:
  # This function follows the System V AMD64 ABI:
  # - Parameters would be in %rdi, %rsi, %rdx, etc. (none needed here)
  # - Return value is in %rax
  # - Caller-saved registers: %rax, %rcx, %rdx, %rsi, %rdi, %r8-%r11
  #   (These can be modified without preserving)
  # - Callee-saved registers: %rbx, %r12-%r15
  #   (Must preserve if used)
  #
  # NOTE: This function uses %rbx but doesn't save/restore it
  # This violates the ABI but works for our specific use case
  # In production code, we should: push %rbx ... pop %rbx
  #
  readInt:
      # SYSCALL: Read from stdin
      # ========================
      # Linux read syscall (number 0)
      # Reads bytes from a file descriptor into a buffer
      #
      # Syscall interface:
      #   %rax = 0           (syscall number for read)
      #   %rdi = fd          (file descriptor to read from)
      #   %rsi = buffer      (where to store the data)
      #   %rdx = count       (maximum bytes to read)
      #
      # Returns:
      #   %rax = number of bytes actually read (or -1 on error)
      #          Can be less than count if EOF or less data available

      mov $0, %rax                 # Syscall number 0 (sys_read)
                                   # read(fd, buffer, count)
                                   # Tells kernel we want to read data

      mov $0, %rdi                 # File descriptor 0 (stdin)
                                   # Standard file descriptors:
                                   #   0 = stdin  (keyboard input, can be redirected)
                                   #   1 = stdout (screen output, can be redirected)
                                   #   2 = stderr (error output)
                                   # Reading from stdin blocks until user presses Enter

      lea buffer(%rip), %rsi       # Load address of 'buffer' into %rsi
                                   # lea = load effective address (gets address, not contents)
                                   # buffer(%rip) = RIP-relative addressing
                                   # WHY %rip-relative?
                                   # - Position-independent code (PIC)
                                   # - Works regardless of where program is loaded
                                   # - Required for shared libraries
                                   # - Better for security (ASLR - Address Space Layout Randomization)

      mov $128, %rdx               # Number of bytes to read (up to 128)
                                   # Our buffer is 128 bytes long
                                   # User input will likely be much smaller
                                   # read will return when user presses Enter

      syscall                      # Make the syscall to read input
                                   # Control transfers to the kernel
                                   # Program blocks (waits) here
                                   # User types text and presses Enter
                                   # Kernel copies input to our buffer
                                   # Returns number of bytes read in %rax
                                   # Example: user types "123\n", %rax = 4

      # After syscall returns:
      # - buffer contains the input string (e.g., "123\n")
      # - %rax contains the number of bytes read (e.g., 4 for "123\n")

      mov %rax, %rbx               # Store the number of bytes read in %rbx
                                   # We need to preserve this value
                                   # WHY %rbx? We're about to reuse %rax for parseInt
                                   # %rbx is callee-saved (should push/pop, but works here)

      # PREPARE TO PARSE THE INPUT
      # ==========================
      # Initialize registers for calling parseInt

      mov $0, %rcx                 # Set index (%rcx) to 0
                                   # This is our loop counter for parseInt
                                   # Starts at the first character of the buffer
                                   # parseInt will increment this

      mov $0, %rax                 # Clear accumulator (%rax) for result
                                   # parseInt builds the result in %rax
                                   # Start with 0 (Horner's method initial value)

      lea buffer(%rip), %rdi       # Load address of 'buffer' into %rdi for parseInt
                                   # parseInt expects buffer address in %rdi
                                   # Setting up the first parameter

      dec %rbx                     # Decrement byte count (%rbx) by 1 (to exclude newline)
                                   # WHY? Input is "123\n" (4 bytes)
                                   # We want to parse "123" (3 bytes)
                                   # The newline character '\n' is not a digit
                                   # dec = decrement by 1
                                   # %rbx now contains count - 1

      mov %rbx, %rsi               # Move byte count to %rsi (number of characters to process)
                                   # parseInt expects the character count in %rsi
                                   # This is the second parameter
                                   # Now %rsi = number of digits to parse (excluding newline)

      # At this point, we've set up all parameters for parseInt:
      # %rdi = address of buffer (pointer to "123")
      # %rsi = number of digits to parse (3 for "123")
      # %rcx = starting index (0)
      # %rax = initial accumulator value (0)

      call parseInt                # Call parseInt to convert input string to integer
                                   # parseInt will loop through the digits
                                   # building up the result in %rax using Horner's method
                                   # Example: "123" -> %rax = 123

      ret                          # Return from readInt with result in %rax
                                   # The caller can now use the integer in %rax
                                   # Pops return address and jumps back to caller

// COMPLETE EXECUTION EXAMPLE:
//
// User types: "42" and presses Enter
// Input bytes: '4' (0x34), '2' (0x32), '\n' (0x0a)
//
// STEP-BY-STEP TRACE:
//
// readInt:
//   mov $0,%rax        # %rax = 0 (read syscall)
//   mov $0,%rdi        # %rdi = 0 (stdin)
//   lea buffer(%rip),%rsi  # %rsi = address of buffer
//   mov $128,%rdx      # %rdx = 128 (max bytes)
//   syscall            # Blocks, waits for input
//     User types "42" and presses Enter
//     Kernel writes ['4', '2', '\n'] to buffer
//     Returns %rax = 3 (bytes read)
//
//   mov %rax,%rbx      # %rbx = 3
//   mov $0,%rcx        # %rcx = 0 (index)
//   mov $0,%rax        # %rax = 0 (accumulator)
//   lea buffer(%rip),%rdi  # %rdi = buffer address
//   dec %rbx           # %rbx = 2 (exclude newline)
//   mov %rbx,%rsi      # %rsi = 2 (digits to parse)
//
//   call parseInt
//
// parseInt - Iteration 1:
//   %rcx = 0
//   movb (%rdi,%rcx,1),%r8b  # %r8b = buffer[0] = '4' (0x34)
//   sub $'0',%r8             # %r8 = '4' - '0' = 4
//   imul $10,%rax            # %rax = 0 * 10 = 0
//   add %r8,%rax             # %rax = 0 + 4 = 4
//   inc %rcx                 # %rcx = 1
//   cmp %rsi,%rcx            # 1 < 2?
//   jl intRead               # Yes, continue
//
// parseInt - Iteration 2:
//   %rcx = 1
//   movb (%rdi,%rcx,1),%r8b  # %r8b = buffer[1] = '2' (0x32)
//   sub $'0',%r8             # %r8 = '2' - '0' = 2
//   imul $10,%rax            # %rax = 4 * 10 = 40
//   add %r8,%rax             # %rax = 40 + 2 = 42
//   inc %rcx                 # %rcx = 2
//   cmp %rsi,%rcx            # 2 < 2?
//   jl intRead               # No, fall through
//
//   ret                      # Return with %rax = 42
//
// Back in readInt:
//   ret                      # Return with %rax = 42
//
// Result: %rax = 42 (binary: 0x2A)

// HORNER'S METHOD DETAILED EXAMPLE:
//
// Converting "5973" to integer:
//
// Traditional method (inefficient):
//   result = 5×10³ + 9×10² + 7×10¹ + 3×10⁰
//   result = 5×1000 + 9×100 + 7×10 + 3×1
//   result = 5000 + 900 + 70 + 3
//   result = 5973
//   Problems: Must compute 10³, 10², 10¹ (3 exponentiations)
//
// Horner's method (efficient):
//   result = 0
//   result = 0×10 + 5 = 5
//   result = 5×10 + 9 = 59
//   result = 59×10 + 7 = 597
//   result = 597×10 + 3 = 5973
//   Benefits: Only multiplications by 10, no exponentiation needed!
//
// Assembly trace:
//   Initial: %rax = 0
//   Digit '5': %rax = 0×10 + 5 = 5
//   Digit '9': %rax = 5×10 + 9 = 59
//   Digit '7': %rax = 59×10 + 7 = 597
//   Digit '3': %rax = 597×10 + 3 = 5973

// WHY THIS APPROACH?
//
// 1. SEPARATION OF CONCERNS:
//    - readInt handles I/O (syscalls)
//    - parseInt handles parsing (algorithm)
//    - Makes code more modular and testable
//    - Each function has single responsibility
//
// 2. HORNER'S METHOD:
//    - Efficient: one multiply and one add per digit (O(n))
//    - No need to compute powers of 10
//    - Works left-to-right through the string (natural order)
//    - Generalizes to any base (binary, octal, hex, etc.)
//    - Numerically stable
//
// 3. BUFFER-BASED I/O:
//    - Read entire input at once (one syscall)
//    - Process in memory (faster than repeated syscalls)
//    - Buffer size (128) handles any 64-bit integer
//    - Syscalls are expensive, minimize them
//
// 4. .BSS SECTION:
//    - Saves executable file size
//    - OS zeros the memory automatically
//    - Better than .data for large uninitialized arrays
//    - Doesn't take space on disk

// LIMITATIONS AND EDGE CASES:
//
// 1. NO ERROR CHECKING:
//    - Doesn't validate that input contains only digits
//    - Doesn't handle negative numbers (no '-' sign support)
//    - Doesn't handle overflow (numbers > 2⁶³-1)
//    - Non-digit characters will produce garbage results
//    - Example: "12a3" would parse as "12" then fail on 'a'
//
// 2. SINGLE LINE ONLY:
//    - Stops at first newline
//    - Can't read multiple integers in one call
//    - Would need loop for multiple inputs
//
// 3. BUFFER OVERFLOW RISK:
//    - If input > 128 bytes, truncated
//    - In practice, 128 bytes is enough for any 64-bit integer
//    - Max: -9,223,372,036,854,775,808 (20 digits + sign + newline = 22 bytes)
//
// 4. ABI VIOLATION:
//    - Uses %rbx without saving/restoring
//    - Violates System V AMD64 ABI
//    - Should: push %rbx at start, pop %rbx before ret
//    - Works for our use case but not robust

// EQUIVALENT C CODE:
//
// int parseInt(const char* buffer, int length) {
//     int result = 0;
//     for (int i = 0; i < length; i++) {
//         result = result * 10 + (buffer[i] - '0');
//     }
//     return result;
// }
//
// int readInt() {
//     char buffer[128];
//     int n = read(0, buffer, 128);  // Read from stdin
//     return parseInt(buffer, n - 1);  // Exclude newline
// }

// TRY IT:
//
// Create a test program:
//   .section .text
//   .extern readInt
//   .global _start
//   _start:
//       call readInt      # Read integer into %rax
//       mov %rax, %rdi    # Move to exit code
//       mov $60, %rax     # Exit syscall
//       syscall
//
// Compile and run:
//   as readInt.s -o readInt.o
//   as test.s -o test.o
//   ld readInt.o test.o -o test
//   ./test
//   123              (type this)
//   echo $?          (check exit code)
//   123              (program exits with the number you typed)
//
// This confirms readInt correctly converted ASCII "123" to integer 123

// EXTENDING THIS CODE:
//
// To handle negative numbers:
// 1. Check if first character is '-'
// 2. Set a flag if so
// 3. Skip the '-' character during parsing (start at index 1)
// 4. Negate the result at the end if flag is set
// Example:
//   if (buffer[0] == '-') {
//       negative = 1;
//       start = 1;
//   }
//   result = parseInt(buffer + start, length - start);
//   if (negative) result = -result;
//
// To handle errors:
// 1. Validate each character is '0'-'9' (or '-' at start)
// 2. Return error code in a different register
// 3. Check for overflow during multiplication
// Example:
//   if (buffer[i] < '0' || buffer[i] > '9') {
//       return ERROR_INVALID_DIGIT;
//   }
//
// To handle overflow:
// 1. Check if result would overflow before multiplying
// 2. Compare against MAX_INT/10
// Example:
//   if (result > LONG_MAX / 10) {
//       return ERROR_OVERFLOW;
//   }

// PERFORMANCE CONSIDERATIONS:
//
// 1. SYSCALL OVERHEAD:
//    - read syscall is expensive (context switch to kernel)
//    - But we only do it once per integer
//    - Alternative: read byte-by-byte (many syscalls) is much slower
//
// 2. BUFFER SIZE:
//    - 128 bytes is comfortable for 64-bit integers
//    - Could use smaller (32 bytes sufficient)
//    - Larger buffer doesn't hurt (we're in .bss)
//
// 3. HORNER'S METHOD:
//    - O(n) time where n is number of digits
//    - Very efficient (one loop, simple operations)
//    - Better than exponentiation-based methods
//
// 4. REGISTER USAGE:
//    - Keeps everything in registers (fast)
//    - Only memory access is reading buffer
//    - Modern CPUs prefetch buffer into cache
//
// 5. FUNCTION CALL OVERHEAD:
//    - Calling parseInt has small cost
//    - Could inline for performance
//    - But readability benefits outweigh cost

// DEBUGGING TIPS:
//
// 1. Print intermediate values:
//    - After read, print number of bytes
//    - In loop, print each character and accumulated result
//    - Verify Horner's method step-by-step
//
// 2. Hexdump the buffer:
//    - Use gdb: x/32xb buffer
//    - See exact bytes (including newline, etc.)
//    - Verify ASCII codes
//
// 3. Test edge cases:
//    - Single digit: "0", "9"
//    - Multiple digits: "123", "999"
//    - Maximum value: "9223372036854775807"
//    - Zero: "0"
//
// 4. Use debugger:
//    - Set breakpoint at parseInt
//    - Step through loop
//    - Watch %rax accumulate value
//    - Verify %rcx increments correctly

// ASCII TABLE REFERENCE (relevant digits):
//
// '0' = 48 = 0x30 = 0b00110000
// '1' = 49 = 0x31 = 0b00110001
// '2' = 50 = 0x32 = 0b00110010
// '3' = 51 = 0x33 = 0b00110011
// '4' = 52 = 0x34 = 0b00110100
// '5' = 53 = 0x35 = 0b00110101
// '6' = 54 = 0x36 = 0b00110110
// '7' = 55 = 0x37 = 0b00110111
// '8' = 56 = 0x38 = 0b00111000
// '9' = 57 = 0x39 = 0b00111001
// '\n' = 10 = 0x0a = 0b00001010
//
// Notice: Subtracting 48 ('0') from any digit character
// gives its numeric value (0-9)
