// PEDAGOGICAL PURPOSE:
// This program demonstrates fundamental floating-point arithmetic using SSE instructions.
// It shows how to load, add, and convert floating-point numbers in x86-64 assembly,
// which is essential for scientific computing, graphics, and any application requiring
// fractional values or very large/small numbers.
//
// Key learning objectives:
// 1. Understanding IEEE 754 floating-point representation
// 2. Learning SSE (Streaming SIMD Extensions) scalar floating-point instructions
// 3. Understanding XMM registers for floating-point operations
// 4. Seeing how floating-point differs from integer arithmetic
// 5. Learning conversion between float and integer (cvttss2si)
// 6. Understanding the .float directive for data definition
// 7. Understanding truncation vs rounding in conversions
// 8. Learning RIP-relative addressing with floating-point data

// WHAT IS FLOATING-POINT?
//
// Floating-point is a way to represent real numbers (including fractional values)
// in binary format. It's called "floating-point" because the decimal point can
// "float" to different positions.
//
// Scientific notation analogy:
//   1.23 × 10^4 = 12,300
//   1.23 × 10^-2 = 0.0123
//
// IEEE 754 single-precision (32-bit float):
//   Sign bit (1 bit) | Exponent (8 bits) | Mantissa/Fraction (23 bits)
//
// Example: 1.5 in binary
//   Decimal: 1.5
//   Binary: 1.1 (1 + 0.5 = 1.5)
//   IEEE 754: 0 01111111 10000000000000000000000
//             ^ sign     ^ exponent (127)  ^ mantissa (.5)
//   Hex: 0x3FC00000

// WHY FLOATING-POINT?
//
// Integers can't represent:
//   - Fractions (1.5, 3.14159, 0.001)
//   - Very large numbers (Avogadro's number: 6.022 × 10^23)
//   - Very small numbers (Planck constant: 6.626 × 10^-34)
//
// Floating-point allows:
//   - Huge range: approximately ±10^-38 to ±10^38 (single-precision)
//   - Precision: about 7 decimal digits (single-precision)
//
// Trade-off: Not all values can be exactly represented
//   Example: 0.1 cannot be exactly represented in binary floating-point
//   Just like 1/3 = 0.333... cannot be exactly represented in decimal

// SSE INSTRUCTIONS:
//
// SSE = Streaming SIMD Extensions (introduced with Pentium III in 1999)
// Adds 128-bit XMM registers (xmm0-xmm15) for floating-point operations
//
// Before SSE:
//   x87 FPU with 80-bit registers (st0-st7) and stack-based operations
//   Complex to program, awkward calling conventions
//
// With SSE:
//   Register-based like integer operations
//   Can operate on 4 floats in parallel (SIMD)
//   Or single float (scalar mode - what this program uses)
//
// SSE scalar instructions end in 'ss':
//   movss - move scalar single-precision
//   addss - add scalar single-precision
//   mulss - multiply scalar single-precision
//   etc.

// XMM REGISTERS:
//
// XMM registers are 128 bits wide
// Can hold:
//   - 4 single-precision floats (32-bit each) - packed mode
//   - 2 double-precision floats (64-bit each) - packed mode
//   - 1 single-precision float (in lowest 32 bits) - scalar mode
//   - 1 double-precision float (in lowest 64 bits) - scalar mode
//
// This program uses scalar mode:
//   Only the lowest 32 bits of XMM0 are used
//   Upper 96 bits are ignored (or zeroed by some instructions)
//
// Register layout for scalar mode:
//   XMM0: [unused 96 bits | float value 32 bits]

	.section .data
# Define the data section for initialized global variables

float1:	 .float 1.5
# Define a 32-bit single-precision float named 'float1' with value 1.5
# .float directive creates a 32-bit IEEE 754 single-precision value
#
# 1.5 in binary: 1.1 (binary point notation)
# IEEE 754 representation:
#   Sign: 0 (positive)
#   Exponent: 127 (biased, actual = 0)
#   Mantissa: .5 (the .1 part in binary 1.1)
# Hex representation: 0x3FC00000
# Memory layout (little-endian): 00 00 C0 3F

float2:	 .float 2.5
# Define another float named 'float2' with value 2.5
# 2.5 in binary: 10.1
# IEEE 754 representation:
#   Sign: 0 (positive)
#   Exponent: 128 (biased, actual = 1)
#   Mantissa: .25 (the .01 part in binary 10.1)
# Hex representation: 0x40200000
# Memory layout (little-endian): 00 00 20 40

	.section .text
# Begin the code section for executable instructions

	.global _start
# Declare _start as the program entry point

_start:
	# STEP 1: Load first floating-point value
	movss float1(%rip),%xmm0
	# movss = Move Scalar Single-precision
	# Loads a single 32-bit float from memory into XMM0
	#
	# Breakdown:
	#   movs = move scalar (single value, not packed)
	#   s    = single-precision (32-bit float)
	#
	# Source: float1(%rip)
	#   float1 = label for our data
	#   (%rip) = RIP-relative addressing
	#   Calculates: address of float1 relative to current instruction
	#   WHY? Position-independent code (works at any memory address)
	#
	# Destination: %xmm0
	#   XMM0 is a 128-bit register
	#   Only lowest 32 bits are written with the float value
	#   Upper 96 bits are zeroed (by movss behavior)
	#
	# After this instruction:
	#   XMM0[31:0]   = 1.5 (0x3FC00000)
	#   XMM0[127:32] = 0 (upper bits zeroed)
	#
	# Technical detail: movss zero-extends
	#   Many SSE scalar instructions zero the upper bits
	#   This prevents dependency on previous register contents
	#   Helps CPU performance (no partial register stalls)

	# STEP 2: Add second floating-point value
	addss float2(%rip),%xmm0
	# addss = Add Scalar Single-precision
	# Adds a 32-bit float from memory to the float in XMM0
	#
	# Breakdown:
	#   adds = add scalar
	#   s    = single-precision
	#
	# AT&T syntax: source, destination
	#   Source: float2(%rip) = 2.5
	#   Destination: %xmm0 (currently holds 1.5)
	#
	# Operation:
	#   XMM0 = XMM0 + float2(%rip)
	#   XMM0 = 1.5 + 2.5
	#   XMM0 = 4.0
	#
	# After this instruction:
	#   XMM0[31:0]   = 4.0 (0x40800000)
	#   XMM0[127:32] = 0 (upper bits remain zero)
	#
	# Floating-point addition details:
	#   1. Extract exponents and mantissas from both operands
	#   2. Align mantissas (adjust for different exponents)
	#   3. Add the aligned mantissas
	#   4. Normalize the result
	#   5. Round if necessary
	#   6. Store result
	#
	# This all happens in hardware (floating-point unit)!

	# AFTER addss, XMM0 contains 4.0
	# Let's verify: 1.5 + 2.5 = 4.0
	#
	# IEEE 754 representation of 4.0:
	#   Sign: 0 (positive)
	#   Exponent: 129 (biased, actual exponent = 2, since 4 = 2^2 × 1.0)
	#   Mantissa: 0.0 (4.0 in binary is exactly 100.0, normalized to 1.00 × 2^2)
	# Hex: 0x40800000

	# STEP 3: Convert float to integer
	cvttss2si %xmm0,%eax
	# cvttss2si = Convert with Truncation Scalar Single-precision to Signed Integer
	# Converts a 32-bit float to a 32-bit signed integer
	#
	# Breakdown:
	#   cvtt   = convert with truncation
	#   ss     = scalar single-precision (source)
	#   2si    = to signed integer (destination)
	#
	# Source: %xmm0 (contains 4.0 as float)
	# Destination: %eax (32-bit integer register)
	#
	# Truncation means rounding toward zero:
	#   4.0 → 4
	#   4.9 → 4 (not 5!)
	#   4.1 → 4
	#   -4.9 → -4 (not -5!)
	#
	# After this instruction:
	#   EAX = 4 (integer)
	#
	# Alternative conversion instructions:
	#   cvtss2si  - Convert with rounding (uses current rounding mode)
	#   cvttss2si - Convert with truncation (always rounds toward zero)
	#
	# WHY truncation?
	#   Matches C language type casting behavior: (int)4.9 = 4
	#   Predictable, no dependency on rounding mode flags
	#   Simpler to understand
	#
	# Floating-point to integer conversion process:
	#   1. Extract the fractional value from IEEE 754 format
	#   2. Truncate (discard) the fractional part
	#   3. Store the integer part in destination register
	#
	# Edge cases:
	#   If float > 2^31-1 or float < -2^31:
	#     Result is undefined (typically 0x80000000, the "integer indefinite")
	#   If float is NaN (Not a Number):
	#     Result is integer indefinite
	#   If float is infinity:
	#     Result is integer indefinite

	# STEP 4: Prepare for exit
	mov %eax,%edi
	# Move the integer result from EAX to EDI
	# EDI will be the exit code for the program
	#
	# After this: EDI = 4
	#
	# WHY EDI?
	#   Linux x86-64 syscall calling convention
	#   For sys_exit, the exit code is passed in RDI
	#   Using EDI (32-bit) instead of RDI (64-bit) is fine
	#   When you write to a 32-bit register, the upper 32 bits are automatically zeroed
	#
	# After mov %eax,%edi:
	#   RDI = 0x0000000000000004 (64-bit view)
	#   EDI = 0x00000004 (32-bit view)

	# STEP 5: Exit syscall
	mov $60,%rax
	# Move syscall number 60 into RAX
	# Syscall 60 = sys_exit (terminate the program)
	#
	# Linux syscall numbers (x86-64):
	#   0  = sys_read
	#   1  = sys_write
	#   60 = sys_exit
	#   etc.

	syscall
	# Invoke the syscall
	# Control transfers to the kernel
	# Kernel executes sys_exit(4)
	# Program terminates with exit code 4
	#
	# To verify the exit code:
	#   Run: ./03-floating-point
	#   Check: echo $?
	#   Output: 4

// COMPLETE EXECUTION TRACE:
//
// Initial state:
//   float1 in memory: 1.5 (0x3FC00000)
//   float2 in memory: 2.5 (0x40200000)
//   XMM0: undefined
//   EAX: undefined
//
// After movss float1(%rip),%xmm0:
//   XMM0 = 1.5
//   Binary representation in XMM0[31:0]: 0x3FC00000
//   Upper bits XMM0[127:32]: all zeros
//
// After addss float2(%rip),%xmm0:
//   XMM0 = 1.5 + 2.5 = 4.0
//   Binary representation: 0x40800000
//   This is exactly representable in IEEE 754
//
// After cvttss2si %xmm0,%eax:
//   EAX = 4 (integer)
//   Conversion: 4.0 (float) → 4 (int)
//   No fractional part to truncate
//
// After mov %eax,%edi:
//   EDI = 4
//   RDI = 0x0000000000000004
//
// After mov $60,%rax; syscall:
//   Program exits with code 4

// WHY THIS EXAMPLE MATTERS:
//
// 1. DEMONSTRATES BASIC FLOATING-POINT OPERATIONS:
//    Loading, arithmetic, and conversion
//    These are building blocks for more complex calculations
//
// 2. SHOWS SSE SCALAR MODE:
//    Simplest form of SSE (one value at a time)
//    Foundation for understanding packed (SIMD) operations
//
// 3. ILLUSTRATES FLOAT-TO-INT CONVERSION:
//    Critical for interfacing between floating-point calculations
//    and integer-based operations (like exit codes, array indices)
//
// 4. INTRODUCES XMM REGISTERS:
//    Distinct from general-purpose registers (RAX, RBX, etc.)
//    Separate register file for floating-point operations

// FLOATING-POINT PRECISION LIMITS:
//
// Single-precision (32-bit) floats have limited precision:
//   About 7 decimal digits of precision
//
// Example of precision loss:
//   float1: .float 1000000.0
//   float2: .float 0.1
//
//   After addition:
//     Result: 1000000.0 (the 0.1 is lost!)
//
//   Why? 0.1 is too small relative to 1,000,000
//   The mantissa can't represent both the large and small parts
//
// This is called "catastrophic cancellation" or "precision loss"
//
// Solution: Use double-precision (64-bit) for more precision
//   About 15-16 decimal digits of precision
//   Instructions: movsd, addsd, cvttsd2si, etc.

// ROUNDING MODES:
//
// Floating-point arithmetic can use different rounding modes:
//   1. Round to nearest (default, most common)
//   2. Round toward +∞ (ceiling)
//   3. Round toward -∞ (floor)
//   4. Round toward 0 (truncation)
//
// cvttss2si always uses truncation (mode 4)
//
// To use other rounding modes:
//   Use cvtss2si (respects current rounding mode)
//   Set rounding mode in MXCSR register
//
// Example:
//   stmxcsr mxcsr_save    # Save current MXCSR
//   or mxcsr_save, 0x6000 # Set rounding mode bits
//   ldmxcsr mxcsr_save    # Load modified MXCSR
//   cvtss2si %xmm0, %eax  # Convert with new rounding mode

// SPECIAL FLOATING-POINT VALUES:
//
// IEEE 754 defines special values:
//
// 1. Zero:
//    +0.0 (0x00000000) and -0.0 (0x80000000)
//    Yes, there are two zeros! (differ in sign bit)
//
// 2. Infinity:
//    +∞ (0x7F800000) and -∞ (0xFF800000)
//    Results from overflow or division by zero
//
// 3. NaN (Not a Number):
//    Signaling NaN: raises exception
//    Quiet NaN: propagates through calculations
//    Example operations that produce NaN:
//      0.0 / 0.0
//      ∞ - ∞
//      sqrt(-1.0)
//
// 4. Denormal numbers:
//    Very small numbers (smaller than normal range)
//    Allow gradual underflow
//    May be much slower to compute

// DOUBLE PRECISION EXAMPLE:
//
// To use 64-bit doubles instead of 32-bit floats:
//
// Data section:
//   double1: .double 1.5
//   double2: .double 2.5
//
// Code:
//   movsd double1(%rip), %xmm0  # 'd' for double
//   addsd double2(%rip), %xmm0
//   cvttsd2si %xmm0, %eax       # sd2si (scalar double to int)
//
// Differences:
//   - .double instead of .float (8 bytes vs 4 bytes)
//   - movsd, addsd instead of movss, addss
//   - cvttsd2si instead of cvttss2si
//   - Uses lower 64 bits of XMM0 instead of lower 32 bits

// PACKED (SIMD) MODE PREVIEW:
//
// SSE can also operate on 4 floats in parallel:
//
// Data:
//   vec1: .float 1.0, 2.0, 3.0, 4.0
//   vec2: .float 5.0, 6.0, 7.0, 8.0
//
// Code:
//   movaps vec1(%rip), %xmm0  # Load 4 floats
//   addps vec2(%rip), %xmm0   # Add 4 floats in parallel
//
// Result in XMM0:
//   [6.0, 8.0, 10.0, 12.0]
//   (Each float added independently)
//
// This is SIMD: Single Instruction, Multiple Data
//   One addps instruction performs 4 additions!
//   See 01-avx512.s for advanced SIMD examples

// COMPARISON WITH INTEGER ARITHMETIC:
//
// Integer addition (add):
//   - Exact results (no rounding)
//   - Overflow wraps around (modulo 2^64)
//   - Flags set (carry, overflow, zero, sign)
//
// Floating-point addition (addss):
//   - May round (not all values exactly representable)
//   - Overflow produces infinity (not wrap-around)
//   - Flags in MXCSR register (not FLAGS register)
//   - NaN propagation (NaN + anything = NaN)
//
// Example:
//   Integer: 2^63 + 1 = -2^63 + 1 (wraps to negative!)
//   Float:   2^63 + 1 ≈ 2^63 (rounds, not enough precision)

// REAL-WORLD APPLICATIONS:
//
// 1. Graphics and Game Development:
//    - 3D transformations (rotate, scale, translate)
//    - Physics simulations (gravity, collisions)
//    - Lighting calculations
//
// 2. Scientific Computing:
//    - Numerical analysis
//    - Differential equations
//    - Statistics and data analysis
//
// 3. Machine Learning:
//    - Neural network training (matrix operations)
//    - Gradient descent optimization
//    - Activation functions (sigmoid, tanh, ReLU)
//
// 4. Signal Processing:
//    - Audio filtering
//    - Image processing
//    - Fourier transforms
//
// 5. Financial Calculations:
//    - Interest calculations
//    - Option pricing (Black-Scholes)
//    - Risk analysis
//    Note: Sometimes fixed-point is preferred for exact currency

// COMPILING AND RUNNING:
//
// Assemble:
//   as 03-floating-point.s -o 03-floating-point.o
//
// Link:
//   ld 03-floating-point.o -o 03-floating-point
//
// Run:
//   ./03-floating-point
//
// Check exit code:
//   echo $?
//   (should print 4)
//
// Expected result:
//   Program exits silently with code 4
//   This confirms: 1.5 + 2.5 = 4.0, converted to integer 4

// EXTENDING THIS EXAMPLE:
//
// 1. Try other operations:
//    Replace addss with:
//      subss - subtraction
//      mulss - multiplication
//      divss - division
//      sqrtss - square root
//
// 2. Use different values:
//    Change float1 and float2 to see different results
//    Try values that don't convert evenly (like 4.7)
//
// 3. Handle fractional results:
//    Use cvtss2si instead of cvttss2si
//    Compare rounding vs truncation
//
// 4. Work with multiple values:
//    Load different floats into XMM0, XMM1, XMM2
//    Perform calculations between registers
//
// 5. Add comparison:
//    Use ucomiss to compare floats
//    Jump based on comparison results
//
// 6. Implement a function:
//    float add_floats(float a, float b) {
//      return a + b;
//    }
//    Calling convention: args in XMM0, XMM1; return in XMM0

// COMMON PITFALLS:
//
// 1. Mixing integer and float registers:
//    BAD:  mov %eax, %xmm0    # Can't directly move!
//    GOOD: movd %eax, %xmm0   # Use movd for int→xmm
//    GOOD: movd %xmm0, %eax   # Use movd for xmm→int
//
// 2. Forgetting conversion:
//    If you need an integer result, must use cvt instruction
//    Can't just move XMM to integer register (wrong interpretation)
//
// 3. Precision assumptions:
//    0.1 + 0.2 ≠ 0.3 exactly in floating-point!
//    Use epsilon for comparisons: |a - b| < 0.00001
//
// 4. Using wrong precision:
//    mixing ss (single) and sd (double) instructions
//    mixing movss and movsd can cause errors

// FLOATING-POINT CALLING CONVENTION:
//
// Linux x86-64 ABI for floating-point:
//   - Arguments: XMM0-XMM7 (first 8 float/double arguments)
//   - Return value: XMM0
//   - Caller-saved: XMM0-XMM15 (caller must save if needed)
//
// Example function:
//   float multiply(float a, float b)
//   Arguments: a in XMM0, b in XMM1
//   Return: result in XMM0
//
//   multiply:
//     mulss %xmm1, %xmm0   # XMM0 = XMM0 * XMM1
//     ret                  # Return with result in XMM0

// FURTHER READING:
//
// IEEE 754 Standard:
//   Defines floating-point formats and operations
//   https://en.wikipedia.org/wiki/IEEE_754
//
// What Every Computer Scientist Should Know About Floating-Point Arithmetic:
//   Classic paper by David Goldberg
//   Essential reading for understanding floating-point
//
// Intel Software Developer Manual:
//   Volume 1, Chapter 10: Programming with Intel SSE
//   Comprehensive reference for SSE instructions
