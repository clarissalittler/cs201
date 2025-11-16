// PEDAGOGICAL PURPOSE:
// This program demonstrates how to read an integer from standard input in x86-64 assembly.
// It shows the complete process of reading ASCII text, parsing it character by character,
// and converting it to a binary integer representation using Horner's method.
//
// Key learning objectives:
// 1. Understanding how to use the read syscall to get input from stdin
// 2. Learning the ASCII-to-integer conversion algorithm (Horner's method)
// 3. Seeing how to process strings character-by-character in assembly
// 4. Understanding the .bss section for uninitialized data
// 5. Learning how functions are composed (readInt as a reusable module)
// 6. Understanding how numbers are represented as ASCII vs binary
// 7. Using registers for loop counters and accumulators
// 8. Proper handling of newline characters in input

// CONCEPTUAL BACKGROUND - NUMBER REPRESENTATION:
//
// BINARY (COMPUTER INTERNAL):
// - Computers store integers in binary (base 2)
// - Example: The number 123 is stored as 0x7B (01111011 in binary)
// - This is compact and efficient for computation
// - One 64-bit register can hold values from -2^63 to 2^63-1
//
// ASCII (HUMAN READABLE):
// - Humans type numbers as sequences of digit characters
// - Example: The number 123 is typed as '1', '2', '3'
// - Each digit is a separate character with its own ASCII code:
//   '0' = 48 (0x30)
//   '1' = 49 (0x31)
//   '2' = 50 (0x32)
//   ...
//   '9' = 57 (0x39)
// - To convert ASCII digit to numeric value: subtract 48 (or '0')
//
// CONVERSION CHALLENGE:
// - Input from keyboard comes as ASCII characters
// - We need to convert to binary integer for computation
// - This program solves that conversion problem

// CONCEPTUAL BACKGROUND - HORNER'S METHOD:
//
// PROBLEM: Convert string "123" to integer 123
//
// MATHEMATICAL UNDERSTANDING:
// "123" means: 1×10² + 2×10¹ + 3×10⁰ = 100 + 20 + 3 = 123
//
// NAIVE APPROACH (inefficient):
//   result = (1 * 100) + (2 * 10) + (3 * 1)
//   Requires computing powers of 10: 10⁰, 10¹, 10²
//   Need to know string length in advance
//
// HORNER'S METHOD (efficient):
//   result = ((1 × 10 + 2) × 10 + 3)
//   Rewrite the polynomial to eliminate powers:
//   1×10² + 2×10¹ + 3×10⁰ = ((1 × 10 + 2) × 10 + 3)
//
// ALGORITHM:
//   result = 0
//   for each digit d from left to right:
//       result = result * 10 + d
//
// TRACE FOR "123":
//   Start: result = 0
//   Process '1': result = 0 * 10 + 1 = 1
//   Process '2': result = 1 * 10 + 2 = 12
//   Process '3': result = 12 * 10 + 3 = 123
//   Done!
//
// WHY THIS WORKS:
// - Processes digits left-to-right (natural reading order)
// - Each iteration shifts previous result left by one decimal place
// - Only needs one multiply and one add per digit
// - No need to compute or store powers of 10
// - Works for any base (not just decimal)

// SYSCALL INTERFACE - READ:
//
// Syscall number: 0 (sys_read)
// Purpose: Read bytes from a file descriptor
//
// Parameters:
//   %rax = 0 (syscall number)
//   %rdi = file descriptor (0 for stdin)
//   %rsi = pointer to buffer (where to store input)
//   %rdx = number of bytes to read (buffer size)
//
// Return value:
//   %rax = number of bytes actually read
//          or 0 if EOF (end of file)
//          or -1 if error
//
// Side effects:
//   - Fills buffer with input data
//   - Blocks (waits) until data is available or EOF
//   - May clobber %rcx, %r11 and caller-saved registers

	.section .bss                  # Begin uninitialized data section
                                   # .bss = "Block Started by Symbol"
                                   # This section is for uninitialized global/static variables
                                   # WHY .bss? It saves space in the executable
                                   # The OS zeros this memory when the program loads

buffer:	 .zero 128                 # Allocate 128 bytes for 'buffer', initialized to zero
                                   # .zero N reserves N bytes, all set to 0 by OS
                                   # This creates a label 'buffer' pointing to 128 bytes
                                   # Used to store the input string from stdin
                                   # 128 bytes is enough for any 64-bit integer:
                                   #   - Max 20 digits for signed 64-bit
                                   #   - Plus newline character
                                   #   - Plenty of extra room

	.section .text                 # Begin code section (.text)
                                   # .text section contains executable instructions
                                   # This section is typically read-only and executable

	.global readInt                # Make 'readInt' function accessible from other files
                                   # .global exports the symbol to the linker
                                   # This allows other .s files to call readInt
                                   # Makes this a reusable library function

# Function: readInt
# ------------------
# Reads an integer from standard input and returns it.
#
# This function reads a line of text from stdin, converts the ASCII
# string to a binary integer using Horner's method (multiply by 10
# and add each digit).
#
# CALLING CONVENTION (System V AMD64 ABI):
# - This function takes NO parameters
# - Return value is in %rax (the integer that was read)
# - Clobbers (modifies) caller-saved registers: %rax, %rbx, %rcx, %rdx, %rsi, %rdi, %r8
# - Preserves callee-saved registers (except %rbx, which should be saved but isn't in this code)
#
# Inputs: None (reads from stdin)
#
# Outputs:
#   %rax - The integer value read from stdin
#
# Registers used (clobbered):
#   %rax, %rbx, %rcx, %rdx, %rsi, %rdi, %r8
#   (All caller-saved registers may be modified)
#
# Notes:
#   - Reads up to 128 characters from stdin
#   - Expects decimal integer (no error checking for non-digits)
#   - Strips the trailing newline
#   - Does NOT handle negative numbers (only positive integers)
#   - Does NOT check for overflow or invalid input
#   - For production code, should save/restore %rbx (callee-saved register)

readInt:
	# Function prologue: save caller's registers we'll clobber
	push %rbx		               # Save %rbx (callee-saved register)
                                   # WHY? %rbx must be preserved across function calls (ABI)
                                   # We use %rbx to store the byte count
                                   # Must restore it before returning

	# ================================================================
	# STEP 1: READ INPUT FROM STDIN
	# ================================================================
	# We're calling: sys_read(fd=0, buf=buffer, count=128)

	## Read data into the buffer from stdin
	mov $0,%rax		               # syscall 0 = read
                                   # sys_read reads bytes from a file descriptor
                                   # This is the same read() function from C

	mov $0,%rdi		               # file descriptor 0 = stdin
                                   # stdin is standard input (keyboard)
                                   # File descriptors:
                                   #   0 = stdin, 1 = stdout, 2 = stderr

	lea buffer(%rip),%rsi	       # buffer address for input
                                   # lea = load effective address (gets address, not contents)
                                   # buffer(%rip) = RIP-relative addressing
                                   # %rsi will point to where input should be stored
                                   # This is the second argument to sys_read

	mov $128,%rdx		           # read up to 128 bytes
                                   # %rdx is the third argument (max bytes to read)
                                   # Limits how much data can be written to buffer
                                   # Prevents buffer overflow

	syscall                        # Execute the system call
                                   # WHAT HAPPENS:
                                   # 1. Transfer control to kernel
                                   # 2. Kernel blocks until user presses Enter
                                   # 3. Input is copied from keyboard to our buffer
                                   # 4. Control returns to our program
                                   # 5. %rax contains number of bytes read

	mov %rax,%rbx 		           # %rbx = how many bytes were read
                                   # Save the byte count from %rax to %rbx
                                   # WHY? We need to preserve this while setting up the loop
                                   # %rax will be reused as our accumulator

	# After read syscall:
	# - buffer contains input, e.g., "123\n" (4 bytes)
	# - %rbx contains number of bytes read, e.g., 4
	# - We're about to parse these bytes as an integer

	# ================================================================
	# STEP 2: INITIALIZE FOR PARSING
	# ================================================================

	## Now process the buffer and convert ASCII to integer
	## using Horner's method: result = result * 10 + digit

	mov $0,%rcx 		           # int i = 0 (loop counter)
                                   # %rcx will be our index into the buffer
                                   # Starts at 0 (first character)

	mov $0,%rax		               # total = 0 (accumulator for result)
                                   # %rax will build up the final integer
                                   # Starts at 0 before processing any digits

	lea buffer(%rip),%rdi	       # %rdi = pointer to buffer
                                   # %rdi will be the base address for indexed reads
                                   # We'll read buffer[i] using (%rdi,%rcx,1)

	## We need to dec %rbx so we don't grab the newline
	## (the last character read is '\n')
	dec %rbx                       # Decrement byte count to exclude newline
                                   # WHY? Input is "123\n" (4 bytes)
                                   # We want to parse "123" (3 bytes)
                                   # The newline ('\n') is not a digit
                                   # Processing it would give garbage
                                   # After dec: %rbx = 3 (number of digits to process)

	# ================================================================
	# STEP 3: PARSE LOOP (HORNER'S METHOD)
	# ================================================================
	# Loop invariant:
	#   %rax contains the integer value of digits processed so far
	#   %rcx is the index of the next digit to process
	#   %rdi points to the buffer
	#   %rbx is the total number of digits (doesn't change in loop)

intRead:
	# STEP 3a: Read one character from buffer
	movb (%rdi,%rcx,1),%r8b        # temp = buffer[i] (load one byte)
                                   # movb = move byte (8-bit operation)
                                   # (%rdi,%rcx,1) = indexed addressing
                                   # Address = %rdi + %rcx * 1 = buffer + i
                                   # %r8b is the low 8 bits of %r8 (one byte)
                                   # This loads the ASCII character at position i
                                   # Example: if buffer[i] = '2', then %r8b = 0x32

	# STEP 3b: Convert ASCII character to numeric value
	sub $'0',%r8 		           # temp = temp - '0' (convert ASCII to digit)
                                   # ASCII '0' = 48, '1' = 49, '2' = 50, etc.
                                   # Subtracting '0' (48) gives us the digit value
                                   # Example: '2' (0x32 = 50) - '0' (48) = 2
                                   # After this: %r8 contains the numeric digit (0-9)
                                   # NOTE: No error checking! Non-digits give garbage

	# STEP 3c: Apply Horner's method
	imul $10,%rax		           # result = result * 10
                                   # Multiply accumulator by 10 (shift left in decimal)
                                   # This makes room for the next digit
                                   # Example: if %rax = 12, after this %rax = 120
                                   # imul = signed integer multiply
                                   # Result goes back into %rax (2-operand form)

	# STEP 3d: Add the new digit
	add %r8,%rax		           # result = result + digit
                                   # Add the numeric value of current digit
                                   # Example: %rax = 120, %r8 = 3, result: %rax = 123
                                   # This completes one iteration of Horner's method

	# STEP 3e: Advance to next character
	inc %rcx 		               # i++
                                   # Increment loop counter
                                   # Moves to the next character in the buffer
                                   # inc = increment by 1 (faster than add $1)

	# STEP 3f: Check if we're done
	cmp %rbx,%rcx 		           # compare i with numBytes-1
                                   # Sets flags based on (%rcx - %rbx)
                                   # Are we done processing all digits?

	jl intRead		               # if i < numBytes-1, continue loop
                                   # jl = jump if less (signed comparison)
                                   # Loop continues while %rcx < %rbx
                                   # When %rcx == %rbx, we've processed all digits

	# ================================================================
	# STEP 4: RETURN
	# ================================================================
	# At this point:
	# - %rax contains the final integer value
	# - All digits have been processed
	# - Ready to return to caller

	# Function epilogue: restore saved registers and return
	pop %rbx		               # Restore %rbx
                                   # Undo the push from the prologue
                                   # %rbx now has its original value from before our function

	ret			                   # Return with result in %rax
                                   # Pops return address from stack and jumps to it
                                   # Caller can now use the integer in %rax
                                   # CALLING CONVENTION: Return values go in %rax

// COMPLETE EXECUTION EXAMPLE:
//
// User types: "123" and presses Enter
// Input bytes: '1' (0x31), '2' (0x32), '3' (0x33), '\n' (0x0a)
//
// STEP-BY-STEP TRACE:
//
// readInt starts:
//   push %rbx (save caller's %rbx)
//
// SYSCALL (read):
//   Setup: %rax=0, %rdi=0, %rsi=buffer, %rdx=128
//   syscall reads 4 bytes into buffer: ['1', '2', '3', '\n']
//   %rax = 4 (bytes read)
//
// Save count and initialize:
//   %rbx = 4 (from %rax)
//   %rcx = 0 (loop counter)
//   %rax = 0 (accumulator)
//   %rdi = address of buffer
//   %rbx = 3 (after dec: exclude newline)
//
// intRead - Iteration 1 (process '1'):
//   %rcx = 0 (index)
//   %r8b = buffer[0] = '1' (0x31 = 49)
//   %r8 = 49 - 48 = 1 (digit value)
//   %rax = 0 * 10 = 0
//   %rax = 0 + 1 = 1
//   %rcx = 1
//   1 < 3, so continue loop
//
// intRead - Iteration 2 (process '2'):
//   %rcx = 1 (index)
//   %r8b = buffer[1] = '2' (0x32 = 50)
//   %r8 = 50 - 48 = 2 (digit value)
//   %rax = 1 * 10 = 10
//   %rax = 10 + 2 = 12
//   %rcx = 2
//   2 < 3, so continue loop
//
// intRead - Iteration 3 (process '3'):
//   %rcx = 2 (index)
//   %r8b = buffer[2] = '3' (0x33 = 51)
//   %r8 = 51 - 48 = 3 (digit value)
//   %rax = 12 * 10 = 120
//   %rax = 120 + 3 = 123
//   %rcx = 3
//   3 == 3, so exit loop
//
// Return:
//   pop %rbx (restore caller's %rbx)
//   ret
//   %rax = 123 (binary: 0x7B)

// WHY THIS APPROACH?
//
// 1. HORNER'S METHOD:
//    - Efficient: one multiply and one add per digit
//    - No need to compute powers of 10
//    - Works left-to-right through the string (natural order)
//    - Generalizes to any base (binary, octal, hex, etc.)
//    - Used in polynomial evaluation and number parsing
//
// 2. BUFFER-BASED I/O:
//    - Read entire input at once (one syscall)
//    - Process in memory (fast)
//    - No repeated syscalls for each character
//    - Buffer size (128) handles any 64-bit integer
//
// 3. .BSS SECTION:
//    - Saves executable file size
//    - OS zeros the memory automatically
//    - Better than .data for large uninitialized arrays
//    - Shared by all calls (single buffer for the program)
//
// 4. BYTE OPERATIONS:
//    - movb loads single character (8 bits)
//    - More explicit than loading full register
//    - Clearer intent: we're processing bytes

// IMPORTANT CONCEPTS:
//
// 1. ASCII ENCODING:
//    - Text is encoded as bytes
//    - Digit characters '0'-'9' are ASCII codes 48-57
//    - To convert digit character to value: subtract 48 (or '0')
//    - Example: '7' (ASCII 55) - '0' (ASCII 48) = 7
//
// 2. INDEXED ADDRESSING:
//    - (%rdi,%rcx,1) means: address = %rdi + %rcx * 1
//    - General form: offset(%base,%index,scale)
//    - Efficient for array access
//    - Scale can be 1, 2, 4, or 8 (for different data sizes)
//
// 3. REGISTER USAGE:
//    - %rcx: loop counter (index)
//    - %rax: accumulator (builds result)
//    - %rbx: byte count (preserved across loop)
//    - %rdi: buffer pointer (base address)
//    - %r8: temporary (current digit)
//
// 4. NEWLINE HANDLING:
//    - Terminal input includes '\n' (newline) when user presses Enter
//    - We must exclude it from parsing
//    - dec %rbx reduces count by 1 to skip last character

// LIMITATIONS:
//
// 1. NO ERROR CHECKING:
//    - Doesn't validate that input contains only digits
//    - Non-digit characters produce garbage results
//    - Example: "12a3" would process '1', '2', 'a', '3'
//    - 'a' (ASCII 97) - '0' (48) = 49 (wrong!)
//
// 2. NO NEGATIVE NUMBER SUPPORT:
//    - Doesn't recognize '-' sign
//    - Example: "-123" would fail or give garbage
//    - Would need to check first character for '-'
//
// 3. NO OVERFLOW DETECTION:
//    - Large numbers (> 2^63-1) overflow silently
//    - Example: "99999999999999999999" wraps around
//    - Should check for overflow during multiplication
//
// 4. SINGLE LINE ONLY:
//    - Reads until first newline
//    - Can't read multiple integers in one call
//    - Caller must call readInt multiple times
//
// 5. BUFFER SIZE LIMIT:
//    - Input longer than 128 bytes is truncated
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
//   as 02-read-integer.s -o readInt.o
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
// 2. Set a flag if negative
// 3. Skip the '-' when parsing digits
// 4. Negate the result at the end if flag is set
//
// Example modification:
//   mov $0, %r9              # negative flag
//   movb (%rdi), %r8b        # Check first char
//   cmp $'-', %r8b
//   jne parse_digits
//   mov $1, %r9              # Set negative flag
//   inc %rcx                 # Skip '-' character
//   dec %rbx                 # One less digit to process
//   parse_digits:
//   # ... existing parsing loop ...
//   # After loop:
//   test %r9, %r9
//   jz done
//   neg %rax                 # Negate if negative flag set
//   done:
//
// To add error checking:
// 1. Validate each character is '0'-'9'
// 2. Set error flag if invalid
// 3. Return error code in %rdi (exit status)
//
// Example validation:
//   movb (%rdi,%rcx,1), %r8b
//   cmp $'0', %r8b
//   jl error                  # Less than '0'
//   cmp $'9', %r8b
//   jg error                  # Greater than '9'
//   # Continue with valid digit...
//
// To detect overflow:
// 1. Before multiply, check if result would overflow
// 2. Max safe value before *10: (2^63-1)/10
// 3. If current result > max safe, set overflow flag
//
// Example overflow check:
//   mov $922337203685477580, %r10   # (2^63-1)/10
//   cmp %r10, %rax
//   jg overflow                     # Would overflow on next *10

// DEBUGGING TIPS:
//
// Use GDB to trace execution:
//   gdb ./test_readInt
//   break readInt
//   run
//   123<Enter>
//
// Step through loop:
//   si                          # Step instruction
//   info registers rax rcx r8   # Check values
//   x/s $rdi                    # View buffer as string
//   x/4xb $rdi                  # View first 4 bytes in hex
//
// After each loop iteration:
//   p/d $rax                    # Print accumulator in decimal
//   p/d $rcx                    # Print loop counter
//   p/c $r8                     # Print current character
