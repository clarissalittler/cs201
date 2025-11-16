// PEDAGOGICAL PURPOSE:
// This program demonstrates how to read an integer from standard input in assembly.
// It shows the complete process of: reading bytes, parsing ASCII digits, and converting
// them to a binary integer representation.
//
// Key learning objectives:
// 1. Understanding how to use the read syscall to get input from stdin
// 2. Learning ASCII-to-integer conversion algorithm
// 3. Seeing how to process strings character-by-character in assembly
// 4. Understanding the .bss section for uninitialized data
// 5. Learning how to compose functions (readInt calls parseInt)
// 6. Understanding how numbers are represented as ASCII vs binary
// 7. Using registers for loop counters and accumulators

      .section .bss                # Begin uninitialized data section (.bss)
                                   # .bss = Block Started by Symbol
                                   # This section is for uninitialized global/static variables
                                   # WHY .bss? It saves space in the executable
                                   # The OS zeros this memory when the program loads
  buffer: .zero 128                # Allocate 128 bytes for 'buffer', initialized to zero
                                   # This creates a label 'buffer' pointing to 128 bytes
                                   # Used to store the input string from stdin
                                   # 128 bytes is enough for any 64-bit integer plus newline

      .section .text               # Begin code section (.text)
                                   # .text section contains executable instructions
      .global readInt              # Make 'readInt' function accessible from other files
                                   # This allows other .s files to call readInt

  # Function: parseInt
  # Description: Parses ASCII digits from the buffer and converts them into an integer.
  #
  # ALGORITHM: Horner's Method for base conversion
  # For string "123":
  #   result = 0
  #   result = result * 10 + 1 = 1
  #   result = result * 10 + 2 = 12
  #   result = result * 10 + 3 = 123
  #
  # WHY THIS WORKS:
  # "123" means 1×10² + 2×10¹ + 3×10⁰
  # Horner's method: ((1 × 10 + 2) × 10 + 3)
  # This avoids computing powers of 10
  #
  # Parameters:
  #   - %rdi: Address of the buffer containing the input string
  #   - %rsi: Number of characters to process (length of string)
  #   - %rcx: Index into the buffer (should be initialized to 0 before calling)
  #   - %rax: Accumulator (should be initialized to 0 before calling)
  # Returns:
  #   - %rax: The resulting integer value
  #
  # REGISTERS USED:
  #   %rdi - pointer to buffer (doesn't change)
  #   %rsi - number of digits to process (doesn't change)
  #   %rcx - loop counter (index into buffer)
  #   %rax - accumulator (builds up the result)
  #   %r8  - temporary storage for current digit
  #   %r8b - byte version of %r8 (for reading single characters)
  #
  parseInt:
  intRead:
      # STEP 1: Read one character from the buffer
      movb (%rdi,%rcx,1), %r8b     # Load one byte from buffer[%rcx] into %r8b
                                   # Address calculation: %rdi + %rcx * 1
                                   # (%rdi,%rcx,1) means: base=%rdi, index=%rcx, scale=1
                                   # This is indexed addressing mode
                                   # %r8b is the low 8 bits of %r8 (one byte)

      # STEP 2: Convert ASCII character to numeric value
      sub $'0', %r8                # Convert ASCII character to its numeric value
                                   # ASCII '0' = 48, '1' = 49, '2' = 50, etc.
                                   # Subtracting '0' (48) gives us the digit value
                                   # Example: '5' (53) - '0' (48) = 5
                                   # NOTE: We use $'0' which is the character constant
                                   # The assembler converts this to 48

      # STEP 3: Apply Horner's method
      imul $10, %rax               # Multiply accumulator (%rax) by 10 (shifting left in decimal)
                                   # This makes room for the next digit
                                   # Example: if %rax = 12, after this %rax = 120
                                   # imul = signed integer multiply
                                   # Result goes back into %rax

      # STEP 4: Add the new digit
      add %r8, %rax                # Add the numeric value of current digit to accumulator
                                   # Example: %rax = 120, %r8 = 3, result: %rax = 123
                                   # This completes one iteration of Horner's method

      # STEP 5: Move to next character
      inc %rcx                     # Increment buffer index (%rcx)
                                   # inc = increment by 1
                                   # Moves to the next character in the buffer

      # STEP 6: Check if we're done
      cmp %rsi, %rcx               # Compare index with total number of characters to process
                                   # cmp sets flags based on (%rcx - %rsi)
                                   # We check if we've processed all digits

      jl intRead                   # If index < total, repeat the loop
                                   # jl = jump if less (signed comparison)
                                   # Loop continues while %rcx < %rsi
                                   # When %rcx == %rsi, we've processed all digits

      ret                          # Return from function with result in %rax
                                   # The caller expects the integer in %rax

  # Function: readInt
  # Description: Reads input from standard input (stdin), stores it in the buffer,
  #              and then converts the input string into an integer using parseInt.
  #
  # This is the main entry point for reading an integer.
  # It orchestrates two tasks:
  # 1. Reading bytes from stdin (using syscall)
  # 2. Parsing those bytes as an integer (using parseInt)
  #
  # Returns:
  #   - %rax: The integer value read from input
  #
  # CALLING CONVENTION:
  # This function follows the System V AMD64 ABI:
  # - Parameters would be in %rdi, %rsi, %rdx, etc. (none needed here)
  # - Return value is in %rax
  # - Caller-saved registers: %rax, %rcx, %rdx, %rsi, %rdi, %r8-%r11
  # - Callee-saved registers: %rbx, %r12-%r15 (we use %rbx, so we should save it)
  #   NOTE: This code uses %rbx but doesn't save it - works for our use case
  #   but violates the ABI. In production code, we'd push/pop %rbx.
  #
  readInt:
      # SYSCALL: Read from stdin
      # Syscall number 0 = sys_read
      # Parameters:
      #   %rax = syscall number (0 for read)
      #   %rdi = file descriptor (0 for stdin)
      #   %rsi = buffer address (where to store the input)
      #   %rdx = number of bytes to read (buffer size)
      # Returns:
      #   %rax = number of bytes actually read (or -1 on error)

      mov $0, %rax                 # Syscall number 0 (sys_read)
                                   # sys_read reads bytes from a file descriptor

      mov $0, %rdi                 # File descriptor 0 (stdin)
                                   # File descriptors:
                                   #   0 = stdin (keyboard input)
                                   #   1 = stdout (screen output)
                                   #   2 = stderr (error output)

      lea buffer(%rip), %rsi       # Load address of 'buffer' into %rsi
                                   # lea = load effective address
                                   # buffer(%rip) = RIP-relative addressing
                                   # WHY %rip-relative? Position-independent code
                                   # This works regardless of where the program is loaded

      mov $128, %rdx               # Number of bytes to read (up to 128)
                                   # Our buffer is 128 bytes long
                                   # User input will likely be much smaller

      syscall                      # Make the syscall to read input
                                   # Control transfers to the kernel
                                   # Kernel reads from stdin into our buffer
                                   # Returns number of bytes read in %rax

      # After syscall:
      # - buffer contains the input string (e.g., "123\n")
      # - %rax contains the number of bytes read (e.g., 4 for "123\n")

      mov %rax, %rbx               # Store the number of bytes read in %rbx
                                   # We need to preserve this value
                                   # WHY %rbx? We're about to reuse %rax
                                   # NOTE: Should push/pop %rbx per ABI, but works here

      # Initialize registers for parsing
      mov $0, %rcx                 # Set index (%rcx) to 0
                                   # This is our loop counter for parseInt
                                   # Starts at the first character of the buffer

      mov $0, %rax                 # Clear accumulator (%rax) for result
                                   # parseInt builds the result in %rax
                                   # Start with 0

      lea buffer(%rip), %rdi       # Load address of 'buffer' into %rdi for parseInt
                                   # parseInt expects buffer address in %rdi
                                   # We're setting up the function parameters

      dec %rbx                     # Decrement byte count (%rbx) by 1 (to exclude newline)
                                   # WHY? Input is "123\n" (4 bytes)
                                   # We want to parse "123" (3 bytes)
                                   # The newline character is not a digit
                                   # dec = decrement by 1

      mov %rbx, %rsi               # Move byte count to %rsi (number of characters to process)
                                   # parseInt expects the character count in %rsi
                                   # Now %rsi = 3 (for "123")

      # At this point, we've set up all parameters for parseInt:
      # %rdi = address of buffer
      # %rsi = number of digits to parse
      # %rcx = starting index (0)
      # %rax = initial accumulator value (0)

      call parseInt                # Call parseInt to convert input string to integer
                                   # parseInt will loop through the digits
                                   # building up the result in %rax

      ret                          # Return from readInt with result in %rax
                                   # The caller can now use the integer in %rax

// COMPLETE EXECUTION EXAMPLE:
//
// User types: "123" and presses Enter
// Input bytes: '1' (0x31), '2' (0x32), '3' (0x33), '\n' (0x0a)
//
// STEP-BY-STEP TRACE:
//
// readInt:
//   syscall reads 4 bytes into buffer: ['1', '2', '3', '\n']
//   %rax = 4 (bytes read)
//   %rbx = 4
//   %rcx = 0 (index)
//   %rax = 0 (accumulator)
//   %rdi = address of buffer
//   %rbx = 3 (after dec: exclude newline)
//   %rsi = 3 (digits to parse)
//   call parseInt
//
// parseInt - Iteration 1:
//   %rcx = 0
//   %r8b = buffer[0] = '1' (0x31)
//   %r8 = '1' - '0' = 1
//   %rax = 0 * 10 + 1 = 1
//   %rcx = 1
//   1 < 3, continue
//
// parseInt - Iteration 2:
//   %rcx = 1
//   %r8b = buffer[1] = '2' (0x32)
//   %r8 = '2' - '0' = 2
//   %rax = 1 * 10 + 2 = 12
//   %rcx = 2
//   2 < 3, continue
//
// parseInt - Iteration 3:
//   %rcx = 2
//   %r8b = buffer[2] = '3' (0x33)
//   %r8 = '3' - '0' = 3
//   %rax = 12 * 10 + 3 = 123
//   %rcx = 3
//   3 == 3, done
//
// parseInt returns:
//   %rax = 123 (binary: 0x7B)
//
// readInt returns:
//   %rax = 123

// WHY THIS APPROACH?
//
// 1. SEPARATION OF CONCERNS:
//    - readInt handles I/O (syscalls)
//    - parseInt handles parsing (algorithm)
//    - Makes code more modular and testable
//
// 2. HORNER'S METHOD:
//    - Efficient: one multiply and one add per digit
//    - No need to compute powers of 10
//    - Works left-to-right through the string
//
// 3. BUFFER-BASED I/O:
//    - Read entire input at once
//    - Process in memory (faster than repeated syscalls)
//    - Buffer size (128) handles any 64-bit integer
//
// 4. .BSS SECTION:
//    - Saves executable file size
//    - OS zeros the memory automatically
//    - Better than .data for large uninitialized arrays

// LIMITATIONS:
//
// 1. NO ERROR CHECKING:
//    - Doesn't validate that input contains only digits
//    - Doesn't handle negative numbers
//    - Doesn't handle overflow (numbers > 2^63-1)
//    - Non-digit characters will produce garbage results
//
// 2. SINGLE LINE ONLY:
//    - Stops at first newline
//    - Can't read multiple integers in one call
//
// 3. BUFFER OVERFLOW RISK:
//    - If input > 128 bytes, truncated
//    - In practice, 128 bytes is enough for any 64-bit integer

// TRY IT:
//
// Create a test program (test_readInt.s):
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
//   as test_readInt.s -o test_readInt.o
//   ld readInt.o test_readInt.o -o test_readInt
//   ./test_readInt
//   42              (type this)
//   echo $?         (check exit code)
//   42              (program exits with the number you typed)
//
// This confirms readInt correctly converted ASCII "42" to integer 42

// EXTENDING THIS CODE:
//
// To handle negative numbers:
// 1. Check if first character is '-'
// 2. Set a flag if so
// 3. Skip the '-' character during parsing
// 4. Negate the result at the end if flag is set
//
// To handle errors:
// 1. Validate each character is '0'-'9'
// 2. Return error code in a different register
// 3. Check for overflow during multiplication
