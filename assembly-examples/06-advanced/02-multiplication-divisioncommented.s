// PEDAGOGICAL PURPOSE:
// This program demonstrates 64-bit multiplication and division operations in x86-64 assembly.
// It shows how multiplication can produce 128-bit results (using two registers), and how
// division works with 128-bit dividends. These are fundamental operations for understanding
// arbitrary-precision arithmetic and the limitations of fixed-width integer arithmetic.
//
// Key learning objectives:
// 1. Understanding how multiplication can overflow a single register
// 2. Learning the RDX:RAX register pair for 128-bit values
// 3. Understanding signed multiplication (imul) vs unsigned (mul)
// 4. Learning how division works with 128-bit dividends
// 5. Understanding quotient and remainder results
// 6. Seeing practical use of external functions (writeInt)
// 7. Understanding the implications of large number arithmetic
// 8. Learning about multi-register results

// UNDERSTANDING 64-BIT MULTIPLICATION:
//
// When you multiply two 64-bit numbers, the result can be up to 128 bits!
//
// Example:
//   Maximum 64-bit value: 2^64 - 1 = 18,446,744,073,709,551,615
//   Multiply by itself:
//     (2^64 - 1) × (2^64 - 1) = approximately 2^128
//     This is 340,282,366,920,938,463,463,374,607,431,768,211,455
//     This CANNOT fit in a 64-bit register!
//
// Solution: x86-64 uses TWO registers for the result:
//   RDX:RAX = 128-bit result
//     RDX = upper 64 bits (high-order bits)
//     RAX = lower 64 bits (low-order bits)
//
// Think of it like this:
//   If RDX = 5 and RAX = 100, the actual value is:
//   5 × 2^64 + 100 = 92,233,720,368,547,758,180

// MULTIPLICATION INSTRUCTIONS:
//
// x86-64 has two multiplication instructions:
//
// 1. imul (signed multiply):
//    - Treats operands as signed two's complement integers
//    - Range: -2^63 to 2^63-1
//    - Used for positive and negative numbers
//
// 2. mul (unsigned multiply):
//    - Treats operands as unsigned integers
//    - Range: 0 to 2^64-1
//    - Used only for non-negative numbers
//
// This program uses imul, so numbers are treated as signed.

// UNDERSTANDING 128-BIT DIVISION:
//
// Division is the reverse of multiplication:
//   Dividend (128-bit, in RDX:RAX) ÷ Divisor (64-bit) = Quotient + Remainder
//
// Results:
//   RAX = quotient (result of division)
//   RDX = remainder (what's left over)
//
// Example:
//   127 ÷ 10 = 12 remainder 7
//   RAX = 12, RDX = 7

	.section .data
# Define the data section for initialized global variables

var1:	.quad 0
# Define a 64-bit variable 'var1' initialized to 0
# .quad = quadword (64 bits = 8 bytes)
# We'll store the remainder (RDX) here

var2:	.quad 0
# Define a 64-bit variable 'var2' initialized to 0
# We'll store the quotient (RAX) here
#
# WHY initialize to 0?
#   Not strictly necessary (we'll overwrite them)
#   But good practice - ensures deterministic behavior

	.section .text
# Begin the code section

	.global _start
# Declare _start as the global entry point

	.extern writeInt
# Declare that writeInt is defined in another file
# writeInt is a function that prints a 64-bit integer to stdout
# Calling convention: integer to print is passed in %rdi
# This function is typically from a helper library

_start:
	# STEP 1: Load first operand
	mov $10000000000, %rax
	# Move 10,000,000,000 (10 billion) into RAX
	# This is our first operand for multiplication
	# In decimal: 10,000,000,000
	# In hex: 0x2540BE400
	#
	# Note: This fits comfortably in a 64-bit register
	# Maximum 64-bit value is ~18.4 quintillion

	# STEP 2: Load second operand
	mov $10000000000, %rbx
	# Move 10,000,000,000 into RBX
	# This is our second operand
	# We're going to compute: 10 billion × 10 billion

	# STEP 3: Perform signed multiplication
	imul %rbx
	# imul (signed multiply) with one operand uses implicit form:
	#   RDX:RAX ← RAX × source
	#
	# Detailed behavior:
	#   Source: %rbx = 10,000,000,000
	#   Multiplicand: %rax = 10,000,000,000
	#   Product: 10,000,000,000 × 10,000,000,000 = 100,000,000,000,000,000,000
	#            (100 quintillion)
	#
	# Result (128-bit):
	#   RDX:RAX = 100,000,000,000,000,000,000
	#
	# Let's break down the 128-bit result:
	#   In hex: 0x0000000000000005 6BC75E2D 63100000
	#   RDX = 0x0000000000000005 (upper 64 bits)
	#   RAX = 0x6BC75E2D63100000 (lower 64 bits)
	#
	# To verify: 5 × 2^64 + 7,766,279,631,452,241,920 = 100,000,000,000,000,000,000
	#
	# WHY does multiplication modify RDX?
	#   Because the result might not fit in RAX alone
	#   CPU automatically stores overflow in RDX
	#   If RDX = 0 after multiplication, result fit in 64 bits
	#   If RDX ≠ 0, we had overflow beyond 64 bits

	# AFTER IMUL:
	#   RDX = 5 (high-order 64 bits of product)
	#   RAX = 7,766,279,631,452,241,920 (low-order 64 bits)
	#   Together they represent: 100,000,000,000,000,000,000

	# STEP 4: Perform division
	div %rbx
	# div (unsigned division) with one operand uses implicit form:
	#   RAX ← RDX:RAX ÷ source (quotient)
	#   RDX ← RDX:RAX mod source (remainder)
	#
	# IMPORTANT: div treats the values as UNSIGNED
	#   This is a mix: we used signed multiply (imul) but unsigned divide (div)
	#   Works here because our values are positive
	#   For general code, use idiv for signed division
	#
	# Detailed behavior:
	#   Dividend: RDX:RAX = 100,000,000,000,000,000,000 (128-bit)
	#   Divisor: %rbx = 10,000,000,000 (64-bit)
	#   Quotient: 100,000,000,000,000,000,000 ÷ 10,000,000,000
	#           = 10,000,000,000
	#   Remainder: 100,000,000,000,000,000,000 mod 10,000,000,000
	#            = 0 (divides evenly!)
	#
	# After division:
	#   RAX = 10,000,000,000 (quotient)
	#   RDX = 0 (remainder)
	#
	# WHY is the quotient the same as our original value?
	#   Because (A × B) ÷ B = A (basic algebra!)
	#   We multiplied: 10,000,000,000 × 10,000,000,000
	#   Then divided by: 10,000,000,000
	#   Result: 10,000,000,000 (back to original!)

	# AFTER DIV:
	#   RDX = 0 (remainder)
	#   RAX = 10,000,000,000 (quotient)

	# STEP 5: Store remainder
	mov %rdx,var1(%rip)
	# Move the remainder (RDX = 0) to var1
	# var1(%rip) = RIP-relative addressing
	#   Calculates address of var1 relative to current instruction
	#   Required for position-independent code
	# After this: var1 = 0

	# STEP 6: Store quotient
	mov %rax,var2(%rip)
	# Move the quotient (RAX = 10,000,000,000) to var2
	# After this: var2 = 10,000,000,000

	# STEP 7: Print the remainder
	mov var1(%rip),%rdi
	# Load var1 (remainder = 0) into RDI
	# RDI is the first parameter register for function calls
	# Linux x86-64 calling convention:
	#   1st param: %rdi
	#   2nd param: %rsi
	#   3rd param: %rdx
	#   etc.
	# After this: %rdi = 0

	call writeInt
	# Call the writeInt function
	# writeInt expects the integer to print in %rdi
	# This will print "0" to stdout
	#
	# What happens during call:
	#   1. Push return address onto stack
	#   2. Jump to writeInt function
	#   3. writeInt executes (prints 0)
	#   4. writeInt returns (pops return address, jumps back)

	# STEP 8: Print the quotient
	mov var2(%rip),%rdi
	# Load var2 (quotient = 10,000,000,000) into RDI
	# After this: %rdi = 10,000,000,000

	call writeInt
	# Call writeInt again to print the quotient
	# This will print "10000000000" to stdout

	# STEP 9: Exit the program
	mov $60,%rax
	# Move syscall number 60 into RAX
	# Syscall 60 = sys_exit (terminate the program)

	mov $0,%rdi
	# Move exit code 0 into RDI
	# Exit code 0 means success (no errors)
	# Convention: 0 = success, non-zero = error

	syscall
	# Invoke the syscall
	# Kernel executes sys_exit(0)
	# Program terminates successfully

// COMPLETE EXECUTION TRACE:
//
// Initial state:
//   var1 = 0
//   var2 = 0
//   RAX = undefined
//   RBX = undefined
//   RDX = undefined
//
// After mov $10000000000, %rax:
//   RAX = 10,000,000,000
//
// After mov $10000000000, %rbx:
//   RBX = 10,000,000,000
//
// After imul %rbx:
//   RAX = 7,766,279,631,452,241,920 (lower 64 bits of 100 quintillion)
//   RDX = 5 (upper 64 bits)
//   Combined (128-bit): 100,000,000,000,000,000,000
//
// After div %rbx:
//   RAX = 10,000,000,000 (quotient: 100 quintillion ÷ 10 billion)
//   RDX = 0 (remainder: perfectly divisible)
//
// After mov %rdx,var1(%rip):
//   var1 = 0
//
// After mov %rax,var2(%rip):
//   var2 = 10,000,000,000
//
// After mov var1(%rip),%rdi; call writeInt:
//   Output: "0"
//
// After mov var2(%rip),%rdi; call writeInt:
//   Output: "10000000000"
//
// After mov $60,%rax; mov $0,%rdi; syscall:
//   Program exits with code 0

// WHY THIS EXAMPLE MATTERS:
//
// 1. DEMONSTRATES OVERFLOW:
//    When multiplying large 64-bit numbers, results often exceed 64 bits
//    Must use RDX:RAX pair to capture full 128-bit result
//
// 2. SHOWS REGISTER PAIRING:
//    RDX:RAX is a special register pair for double-width operations
//    Other architectures have similar concepts (ARM: R0:R1)
//
// 3. ILLUSTRATES DIVISION SETUP:
//    Division requires dividend in RDX:RAX before executing
//    If you only have a 64-bit dividend, must zero RDX first!
//
// 4. PRACTICAL APPLICATIONS:
//    - Cryptography (large number operations)
//    - Arbitrary precision arithmetic
//    - Financial calculations (avoiding floating-point errors)
//    - Timekeeping (nanoseconds can overflow 32-bit)

// COMMON PITFALL: UNINITIALIZED RDX
//
// BAD CODE:
//   mov $100, %rax
//   mov $10, %rbx
//   div %rbx         # WRONG! RDX might contain garbage
//
// What happens?
//   Division uses RDX:RAX as dividend
//   If RDX = random garbage, you'll get wrong quotient or division error
//
// CORRECT CODE:
//   mov $100, %rax
//   xor %rdx, %rdx   # Clear RDX (set to 0)
//   mov $10, %rbx
//   div %rbx         # Correct: divides 100 ÷ 10 = 10
//
// After correct code:
//   RAX = 10 (quotient)
//   RDX = 0 (remainder)

// SIGNED VS UNSIGNED:
//
// This program mixes signed and unsigned operations (imul + div)
// For correctness with negative numbers, use matching pairs:
//
// Signed operations:
//   imul - signed multiply
//   idiv - signed divide
//
// Unsigned operations:
//   mul  - unsigned multiply
//   div  - unsigned divide
//
// Example with negative numbers:
//   mov $-10, %rax
//   mov $3, %rbx
//   cqo              # Sign-extend RAX into RDX:RAX
//   idiv %rbx        # Signed division
//   Result: RAX = -3, RDX = -1 (since -10 = -3×3 + (-1))

// THE CQO INSTRUCTION:
//
// cqo = Convert Quadword to Octword
// Extends RAX (64-bit) to RDX:RAX (128-bit) with sign extension
//
// How it works:
//   If RAX is positive: RDX = 0x0000000000000000
//   If RAX is negative: RDX = 0xFFFFFFFFFFFFFFFF
//
// Example:
//   RAX = 100 (positive):
//     cqo → RDX = 0, RAX = 100
//     RDX:RAX represents 100
//
//   RAX = -100 (negative, stored as two's complement):
//     cqo → RDX = -1 (all bits set), RAX = -100
//     RDX:RAX represents -100 as a 128-bit value
//
// When to use cqo:
//   Before idiv if you have a 64-bit dividend
//   Ensures correct signed division

// MULTIPLICATION VARIATIONS:
//
// 1. Two-operand imul (most common):
//    imul %rbx, %rax    # RAX = RAX × RBX (only 64-bit result)
//    - Convenient for cases where overflow is impossible
//    - Discards upper 64 bits
//
// 2. Three-operand imul:
//    imul $10, %rax, %rbx   # RBX = RAX × 10
//    - Multiply with immediate value
//    - Stores result in different register
//
// 3. One-operand imul (this program):
//    imul %rbx          # RDX:RAX = RAX × RBX
//    - Full 128-bit result
//    - Use when overflow is possible

// DETECTING OVERFLOW:
//
// After multiplication, check RDX:
//   If RDX = 0: result fits in 64 bits (no overflow)
//   If RDX ≠ 0: result exceeded 64 bits (overflow occurred)
//
// Code example:
//   imul %rbx
//   test %rdx, %rdx    # Check if RDX is zero
//   jnz overflow       # Jump if overflow occurred
//   # No overflow, RAX contains full result
//   jmp continue
//   overflow:
//   # Handle overflow (error message, use 128-bit arithmetic, etc.)
//   continue:

// DIVISION ERRORS:
//
// Division can cause #DE (Divide Error) exception:
//
// 1. Division by zero:
//    mov $100, %rax
//    xor %rdx, %rdx
//    mov $0, %rbx
//    div %rbx         # CRASH! Division by zero
//
// 2. Quotient overflow:
//    mov $100, %rax
//    xor %rdx, %rdx
//    mov $0, %rbx
//    div %bl          # Dividing 64-bit by 8-bit
//                     # Quotient might not fit in AL!
//
// Best practice: Check divisor before dividing:
//   test %rbx, %rbx
//   jz divide_by_zero_handler
//   div %rbx

// REAL-WORLD APPLICATIONS:
//
// 1. Arbitrary Precision Arithmetic:
//    Libraries like GMP use these techniques
//    Implement 128-bit, 256-bit, or unlimited precision math
//
// 2. Cryptography:
//    RSA encryption uses large prime number multiplication
//    Often 2048-bit or 4096-bit numbers
//
// 3. Time Calculations:
//    Converting between nanoseconds and seconds
//    1 second = 1,000,000,000 nanoseconds (needs 64-bit)
//
// 4. Financial Software:
//    Fixed-point arithmetic to avoid floating-point rounding
//    Example: $1000.50 stored as 100050 cents
//
// 5. Hash Functions:
//    Many hash algorithms use 64-bit × 64-bit → 128-bit multiplication
//    Example: MurmurHash, XXHash

// COMPILING AND RUNNING:
//
// Assemble:
//   as 02-multiplication-division.s -o 02-multiplication-division.o
//
// You'll need to provide the writeInt function:
//   Link with a library that provides writeInt, or write your own:
//
//   ld 02-multiplication-division.o writeInt.o -o 02-multiplication-division
//
// Run:
//   ./02-multiplication-division
//
// Expected output:
//   0
//   10000000000
//
// First line is the remainder (0)
// Second line is the quotient (10,000,000,000)

// EXTENDING THIS EXAMPLE:
//
// 1. Try different numbers:
//    Change 10,000,000,000 to other values
//    Try values that don't divide evenly (non-zero remainder)
//
// 2. Use signed division:
//    Replace div with idiv
//    Try negative numbers
//    Use cqo before idiv
//
// 3. Check for overflow:
//    Add code to detect when RDX ≠ 0 after multiplication
//    Print a message if overflow occurs
//
// 4. Implement 128-bit addition:
//    Add two 128-bit numbers (stored in RDX:RAX and RCX:RBX)
//    Use add for lower 64 bits, adc for upper 64 bits
//
// 5. Calculate modular exponentiation:
//    Useful for cryptography
//    Compute (base^exp) mod modulus efficiently

// REGISTER CONVENTIONS SUMMARY:
//
// Multiplication (imul/mul):
//   Input:  RAX × source
//   Output: RDX:RAX (128-bit product)
//
// Division (idiv/div):
//   Input:  RDX:RAX (128-bit dividend) ÷ source
//   Output: RAX (quotient), RDX (remainder)
//
// Remember: imul/idiv are SIGNED, mul/div are UNSIGNED
