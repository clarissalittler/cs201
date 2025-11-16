// PEDAGOGICAL PURPOSE:
// This program demonstrates AVX-512 vector processing, the most advanced SIMD
// (Single Instruction Multiple Data) instruction set available on modern x86-64 CPUs.
// It shows how to perform parallel operations on 16 floating-point numbers simultaneously
// using 512-bit registers.
//
// Key learning objectives:
// 1. Understanding SIMD (Single Instruction Multiple Data) parallelism
// 2. Learning AVX-512 instruction set and ZMM registers (512 bits)
// 3. Understanding vector operations on packed single-precision floats
// 4. Seeing how one instruction can replace 16 separate operations
// 5. Learning RIP-relative addressing for position-independent code
// 6. Understanding the relationship between ZMM, YMM, and XMM registers
// 7. Converting between floating-point and integer representations
// 8. Understanding CPU feature requirements and compatibility

// WHAT IS AVX-512?
//
// AVX-512 = Advanced Vector Extensions with 512-bit registers
//
// Evolution of SIMD on x86:
//   MMX (1997):    64-bit registers (mm0-mm7)      - 8 bytes
//   SSE (1999):   128-bit registers (xmm0-xmm15)   - 16 bytes, 4 floats
//   AVX (2011):   256-bit registers (ymm0-ymm15)   - 32 bytes, 8 floats
//   AVX-512 (2016): 512-bit registers (zmm0-zmm31) - 64 bytes, 16 floats
//
// REGISTER HIERARCHY:
//   ZMM0 [512 bits] = 16 single-precision floats
//     |
//     └─ YMM0 [lower 256 bits] = 8 floats
//          |
//          └─ XMM0 [lower 128 bits] = 4 floats
//
// WHY USE AVX-512?
// - Process 16 values with ONE instruction
// - Massive speedup for scientific computing, graphics, machine learning
// - Example: Adding two arrays of 16 floats
//     Without AVX-512: 16 separate additions
//     With AVX-512:     1 vector addition (up to 16x faster!)

// PREREQUISITE: AVX-512 SUPPORT
//
// WARNING: Not all CPUs support AVX-512!
//
// Introduced in:
//   - Intel Xeon Phi (Knights Landing, 2016)
//   - Intel Skylake-X (2017)
//   - Intel Ice Lake (2019)
//   - AMD Zen 4 (2022)
//
// To check if your CPU supports AVX-512:
//   Linux:   grep avx512 /proc/cpuinfo
//   macOS:   sysctl -a | grep avx512
//
// If your CPU doesn't support AVX-512:
//   - Program will crash with "Illegal instruction" error
//   - Use AVX2 (256-bit) or SSE (128-bit) instead
//   - Emulators may not support AVX-512

.section .data
# Define the data section for initialized global variables

vec1:
    .float 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0,9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0
    # Define 'vec1' as an array of 16 single-precision floating-point numbers
    # .float = 32-bit IEEE 754 single-precision format (4 bytes each)
    # Total size: 16 floats × 4 bytes = 64 bytes (exactly 512 bits!)
    # Values: [1.0, 2.0, 3.0, ..., 16.0]
    # Memory layout (little-endian):
    #   Address 0: 0x3F800000 (1.0)
    #   Address 4: 0x40000000 (2.0)
    #   ... and so on

vec2:
    .float 16.0, 15.0, 14.0, 13.0, 12.0, 11.0, 10.0, 9.0,8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0
    # Define 'vec2' as an array of 16 floats in reverse order
    # Values: [16.0, 15.0, 14.0, ..., 1.0]
    # WHY reversed? To make the example more interesting
    # Each position will add to 17.0: 1+16=17, 2+15=17, etc.
    # This makes it easy to verify the vector addition worked

result:
    .space 64  # Reserve space for 16 floats (16 * 4 bytes)
    # Allocate 64 bytes of uninitialized space for the result
    # .space reserves memory without initializing it
    # This will store the output of our vector addition
    # Size matches vec1 and vec2: 64 bytes for 16 floats

.section .text
# Define the code section for executable instructions

.global _start
# Declare _start as the program entry point (required by linker)
# The OS will jump to this label when the program starts

_start:
	# STEP 1: Load first vector into ZMM0
	vmovups vec1(%rip), %zmm0        # Load vec1 into zmm0
	# vmovups = Vector Move Unaligned Packed Single-precision
	# Breakdown:
	#   v      = VEX/EVEX prefix (AVX encoding)
	#   mov    = move operation
	#   u      = unaligned (doesn't require 64-byte alignment)
	#   p      = packed (multiple values)
	#   s      = single-precision (32-bit floats)
	#
	# vec1(%rip) = RIP-relative addressing
	#   %rip = instruction pointer (program counter)
	#   vec1(%rip) = address of vec1 relative to current instruction
	#   WHY? Position-independent code (works at any memory address)
	#
	# %zmm0 = destination register (512-bit ZMM register)
	# After this instruction:
	#   ZMM0[0] = 1.0, ZMM0[1] = 2.0, ..., ZMM0[15] = 16.0
	#
	# WHY 'u' (unaligned)?
	#   AVX-512 has both aligned (vmovaps) and unaligned (vmovups) moves
	#   Aligned is faster but requires data at 64-byte boundaries
	#   Unaligned works anywhere (safer, slightly slower)

	# STEP 2: Load second vector into ZMM1
	vmovups vec2(%rip), %zmm1        # Load vec2 into zmm1
	# Same as previous instruction, but loads vec2 into ZMM1
	# After this instruction:
	#   ZMM1[0] = 16.0, ZMM1[1] = 15.0, ..., ZMM1[15] = 1.0
	#
	# Now we have two vectors loaded:
	#   ZMM0: [1.0, 2.0, 3.0, ..., 16.0]
	#   ZMM1: [16.0, 15.0, 14.0, ..., 1.0]

	# STEP 3: Perform parallel vector addition
	vaddps  %zmm1, %zmm0, %zmm2      # Add zmm0 and zmm1, store in zmm2
	# vaddps = Vector Add Packed Single-precision
	# Breakdown:
	#   v      = VEX/EVEX prefix
	#   add    = addition operation
	#   p      = packed (operates on all elements)
	#   s      = single-precision floats
	#
	# AT&T syntax: source2, source1, destination
	#   %zmm1 = second source operand
	#   %zmm0 = first source operand
	#   %zmm2 = destination
	#
	# THIS ONE INSTRUCTION PERFORMS 16 ADDITIONS IN PARALLEL:
	#   ZMM2[0]  = ZMM0[0]  + ZMM1[0]  = 1.0  + 16.0 = 17.0
	#   ZMM2[1]  = ZMM0[1]  + ZMM1[1]  = 2.0  + 15.0 = 17.0
	#   ZMM2[2]  = ZMM0[2]  + ZMM1[2]  = 3.0  + 14.0 = 17.0
	#   ...
	#   ZMM2[15] = ZMM0[15] + ZMM1[15] = 16.0 + 1.0  = 17.0
	#
	# Result: ZMM2 contains [17.0, 17.0, 17.0, ..., 17.0]
	#
	# WHY THIS IS POWERFUL:
	#   Without SIMD: would need 16 separate addss instructions
	#   With AVX-512: ONE instruction does all 16 additions
	#   This is the essence of SIMD parallelism!

	# STEP 4: Store result vector to memory
	vmovups %zmm2, result(%rip)      # Store the result
	# Move the contents of ZMM2 to the 'result' array in memory
	# Writes all 64 bytes (16 floats) at once
	# result array now contains: [17.0, 17.0, 17.0, ..., 17.0]
	#
	# Memory layout after this instruction:
	#   result[0] = 17.0 (0x41880000 in hex)
	#   result[1] = 17.0
	#   ... (all 16 elements are 17.0)

	# STEP 5: Extract one element for the exit code
	lea result(%rip),%rbx
	# Load the effective address of 'result' into %rbx
	# lea = Load Effective Address (doesn't access memory, just calculates address)
	# %rbx now points to the beginning of the result array
	# We'll use %rbx as the base pointer for indexed addressing

	mov $4,%rcx
	# Move the value 4 into %rcx
	# We'll use this as an index to access result[4]
	# WHY 4? Arbitrary choice to demonstrate accessing an element
	# Could be any value from 0 to 15

	# STEP 6: Load single float from result array
	movss (%rbx,%rcx,4),%xmm0
	# movss = Move Scalar Single-precision
	# Loads ONE float (not a vector) into XMM0
	#
	# Address calculation: (%rbx,%rcx,4)
	#   Base: %rbx (points to result array)
	#   Index: %rcx (4)
	#   Scale: 4 (size of float in bytes)
	#   Effective address: %rbx + %rcx * 4 = result + 4*4 = result + 16 bytes
	#   This is the address of result[4] (the 5th element, 0-indexed)
	#
	# %xmm0 = lower 128 bits of %zmm0
	# Only the lowest 32 bits of XMM0 are written (one float)
	# After this: XMM0[0] = 17.0
	#
	# WHY movss instead of vmovss?
	#   Both work, movss is the older SSE instruction
	#   vmovss is the AVX version (3-operand form)
	#   For scalar operations, they're equivalent

	# STEP 7: Convert float to integer
	cvttss2si %xmm0,%eax
	# cvttss2si = Convert with Truncation Scalar Single-precision to Signed Integer
	# Breakdown:
	#   cvtt    = convert with truncation (round toward zero)
	#   ss      = scalar single-precision (one 32-bit float)
	#   2si     = to signed integer
	#
	# Source: %xmm0 (contains 17.0)
	# Destination: %eax (32-bit integer register)
	#
	# Conversion: 17.0 → 17 (truncates fractional part)
	# Truncation means: 17.9 → 17, 17.1 → 17, -17.9 → -17
	#
	# After this: %eax = 17
	#
	# WHY cvtt (truncate) instead of cvt (round)?
	#   Truncation is simpler and faster
	#   Matches C language type casting behavior: (int)17.9 = 17

	# STEP 8: Prepare exit syscall
	mov %eax,%edi
	# Move the integer result from %eax to %edi
	# %edi will be the exit code for the program
	# After this: %edi = 17
	#
	# WHY %edi?
	#   Linux syscall convention: first parameter goes in %rdi
	#   For exit syscall, this is the exit code
	#   Using %edi (32-bit) instead of %rdi (64-bit) is fine
	#   The upper 32 bits are automatically zeroed

	mov $60,%rax
	# Move syscall number 60 into %rax
	# Syscall 60 = sys_exit (terminate the program)
	#
	# Linux syscall numbers (x86-64):
	#   0  = read
	#   1  = write
	#   60 = exit

	syscall
	# Invoke the syscall
	# Control transfers to the kernel
	# Kernel executes sys_exit(17)
	# Program terminates with exit code 17
	#
	# To verify the exit code:
	#   Run the program: ./01-avx512
	#   Check exit code: echo $?
	#   Should print: 17

// COMPLETE EXECUTION TRACE:
//
// Initial state:
//   vec1 in memory: [1.0, 2.0, 3.0, ..., 16.0]
//   vec2 in memory: [16.0, 15.0, 14.0, ..., 1.0]
//   result in memory: [uninitialized]
//
// After vmovups vec1(%rip), %zmm0:
//   ZMM0 = [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0,
//           9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0]
//
// After vmovups vec2(%rip), %zmm1:
//   ZMM1 = [16.0, 15.0, 14.0, 13.0, 12.0, 11.0, 10.0, 9.0,
//           8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0]
//
// After vaddps %zmm1, %zmm0, %zmm2:
//   ZMM2 = [17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0,
//           17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0]
//   (Each element is the sum of corresponding elements from ZMM0 and ZMM1)
//
// After vmovups %zmm2, result(%rip):
//   result in memory: [17.0, 17.0, 17.0, ..., 17.0]
//
// After lea result(%rip),%rbx:
//   %rbx = address of result
//
// After mov $4,%rcx:
//   %rcx = 4
//
// After movss (%rbx,%rcx,4),%xmm0:
//   %xmm0[0] = result[4] = 17.0
//   (Loads the 5th element, since arrays are 0-indexed)
//
// After cvttss2si %xmm0,%eax:
//   %eax = 17 (integer)
//
// After mov %eax,%edi:
//   %edi = 17
//
// After mov $60,%rax; syscall:
//   Program exits with code 17

// UNDERSTANDING THE PERFORMANCE BENEFIT:
//
// Traditional scalar code (processing one element at a time):
//   movss vec1(%rip), %xmm0
//   addss vec2(%rip), %xmm0
//   movss %xmm0, result(%rip)
//   movss vec1+4(%rip), %xmm0
//   addss vec2+4(%rip), %xmm0
//   movss %xmm0, result+4(%rip)
//   ... (repeat 14 more times!)
//   Total: 48 instructions (3 × 16)
//
// AVX-512 vector code (this program):
//   vmovups vec1(%rip), %zmm0
//   vmovups vec2(%rip), %zmm1
//   vaddps %zmm1, %zmm0, %zmm2
//   vmovups %zmm2, result(%rip)
//   Total: 4 instructions
//
// Speedup: 48/4 = 12x fewer instructions!
// Real-world speedup is less due to memory bandwidth, but still significant

// OTHER AVX-512 INSTRUCTIONS:
//
// Arithmetic:
//   vaddps   - Add packed single-precision floats
//   vsubps   - Subtract
//   vmulps   - Multiply
//   vdivps   - Divide
//   vsqrtps  - Square root
//
// Double precision (64-bit floats, 8 values per ZMM):
//   vaddpd   - Add packed double-precision
//   vmulpd   - Multiply double-precision
//
// Integer operations (16 32-bit ints or 8 64-bit ints):
//   vpaddd   - Add packed 32-bit integers
//   vpmulld  - Multiply packed 32-bit integers
//
// Comparison and masking:
//   vcmpps   - Compare floats, produce mask
//   vblendmps - Blend two vectors using mask
//
// Memory:
//   vmovaps  - Move aligned packed single (requires 64-byte alignment)
//   vmovups  - Move unaligned packed single (works anywhere)

// LIMITATIONS AND CONSIDERATIONS:
//
// 1. CPU SUPPORT REQUIRED:
//    - Program will crash on CPUs without AVX-512
//    - Need runtime detection in production code
//    - Alternative: compile multiple versions
//
// 2. POWER AND THERMAL:
//    - AVX-512 uses more power than scalar code
//    - May cause CPU frequency throttling (thermal limits)
//    - Intel's "AVX-512 frequency penalty"
//    - Some CPUs downclock when AVX-512 is active
//
// 3. CONTEXT SWITCHING:
//    - ZMM registers are large (512 bits each, 32 registers = 2KB)
//    - Saving/restoring on context switch is expensive
//    - OS must support AVX-512 context management
//
// 4. MEMORY ALIGNMENT:
//    - Best performance with 64-byte aligned data
//    - Use vmovaps for aligned data (faster than vmovups)
//    - Can use .align 64 directive in assembly
//
// 5. CODE SIZE:
//    - AVX-512 instructions are longer (EVEX encoding)
//    - May increase instruction cache pressure

// COMPILING AND RUNNING:
//
// Assemble:
//   as 01-avx512.s -o 01-avx512.o
//
// Link:
//   ld 01-avx512.o -o 01-avx512
//
// Run:
//   ./01-avx512
//
// Check exit code:
//   echo $?
//   (should print 17)
//
// If you get "Illegal instruction":
//   Your CPU doesn't support AVX-512
//   Try the SSE or AVX2 versions instead

// EXTENDING THIS EXAMPLE:
//
// 1. Different operations:
//    Replace vaddps with vmulps to multiply vectors
//    Result would be: [16.0, 30.0, 42.0, 52.0, ...]
//
// 2. More complex algorithms:
//    Dot product: multiply vectors, then sum all elements
//    Matrix multiplication using vector operations
//    Image processing (16 pixels at once)
//
// 3. Mixed precision:
//    Use vcvtps2pd to convert floats to doubles
//    Use vcvttpd2dq to convert doubles to integers
//
// 4. Masking (AVX-512 unique feature):
//    Use mask registers (k0-k7) for conditional operations
//    Example: vaddps %zmm1, %zmm0, %zmm2{%k1}
//    Only updates elements where mask bit is 1

// REAL-WORLD APPLICATIONS:
//
// 1. Scientific Computing:
//    - Physics simulations
//    - Weather modeling
//    - Computational fluid dynamics
//
// 2. Machine Learning:
//    - Neural network training
//    - Matrix operations
//    - Batch processing
//
// 3. Graphics and Image Processing:
//    - Video encoding/decoding
//    - Image filters
//    - Ray tracing
//
// 4. Signal Processing:
//    - Audio processing (16 samples at once)
//    - Fourier transforms
//    - Compression algorithms
//
// 5. Database Operations:
//    - Parallel scanning
//    - Vectorized sorting
//    - Aggregation functions

// FURTHER READING:
//
// Intel Intrinsics Guide:
//   https://software.intel.com/sites/landingpage/IntrinsicsGuide/
//
// AVX-512 Wikipedia:
//   https://en.wikipedia.org/wiki/AVX-512
//
// Intel 64 and IA-32 Architectures Software Developer Manuals:
//   Volume 1: Chapter 15 (Programming with Intel AVX-512)
