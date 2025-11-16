// PEDAGOGICAL PURPOSE:
// This assembly file demonstrates manual modification of generated assembly code to change
// program behavior from addition to multiplication. This is an advanced exercise showing:
// 1. How high-level C operations map to assembly instructions
// 2. The structure of x86-64 assembly (stack frames, calling conventions)
// 3. How to modify assembly to change program logic
// 4. Understanding of the difference between 'mull' (unsigned) vs 'imul' (signed)
// 5. The relationship between compiler output and executable behavior
//
// KEY LEARNING OBJECTIVES:
// - Understand x86-64 calling conventions (argument passing via registers)
// - See how local variables are stored on the stack relative to %rbp
// - Learn about function prologues/epilogues (push/pop %rbp, stack adjustment)
// - Understand the difference between unsigned (mull) and signed (imul) multiplication
// - See how strings are stored in the .rodata section and referenced

	.file	"discussionDiscussion.c"
	.text
	.section	.rodata   // Read-only data section - stores string literals

// STRING LITERALS:
// These labels (.LC0, .LC1, etc.) mark locations in read-only memory
// The compiler creates these labels to reference strings efficiently
.LC0:
	.string	"Enter a number: "   // First prompt string
.LC1:
	.string	"%d"                 // Format string for scanf/printf
.LC2:
	.string	"Enter another: "    // Second prompt string
.LC3:
	.string	"Ta-da! %d\n"       // Final output format string

	.text                    // Code section begins
	.globl	main             // Make 'main' visible to linker
	.type	main, @function  // Declare 'main' as a function
main:
.LFB0:
	.cfi_startproc           // CFI = Call Frame Information (for debugging)

	// FUNCTION PROLOGUE:
	// Every function starts by saving the old base pointer and setting up a new stack frame
	pushq	%rbp             // Save the old base pointer on the stack
	                         // WHY? We need to restore it when we return
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16

	movq	%rsp, %rbp       // Set base pointer to current stack pointer
	                         // WHY? %rbp stays fixed, making it easy to reference local variables
	                         // %rsp will change as we push/pop, but %rbp stays constant
	.cfi_def_cfa_register 6

	subq	$16, %rsp        // Allocate 16 bytes on stack for local variables
	                         // WHY 16? We have 3 ints (12 bytes), but stack must be 16-byte aligned
	                         // STACK LAYOUT after this:
	                         // -4(%rbp)  = num3 (result)
	                         // -8(%rbp)  = num1 (first input)
	                         // -12(%rbp) = num2 (second input)

	// FIRST printf("Enter a number: ");
	// x86-64 calling convention: first argument goes in %rdi
	leaq	.LC0(%rip), %rax // Load address of "Enter a number: " into %rax
	                         // WHY leaq? It loads the *address* not the value
	                         // WHY %rip relative? Position-independent code for security
	movq	%rax, %rdi       // Move string address to %rdi (1st argument)
	                         // WHY %rdi? x86-64 calling convention:
	                         // %rdi = 1st arg, %rsi = 2nd, %rdx = 3rd, %rcx = 4th
	movl	$0, %eax         // Set %eax to 0
	                         // WHY? Indicates 0 vector registers used (for varargs)
	call	printf@PLT       // Call printf through Procedure Linkage Table
	                         // WHY @PLT? Allows dynamic linking to shared libraries

	// FIRST scanf("%d", &num1);
	// Second argument (&num1) goes in %rsi
	leaq	-8(%rbp), %rax   // Load address of num1 into %rax
	                         // -8(%rbp) is where num1 lives on the stack
	                         // WHY leaq? We need the ADDRESS (&num1), not the value
	movq	%rax, %rsi       // Move &num1 to %rsi (2nd argument)
	                         // WHY %rsi? Second argument in calling convention
	leaq	.LC1(%rip), %rax // Load address of "%d" format string
	movq	%rax, %rdi       // Move format string to %rdi (1st argument)
	movl	$0, %eax         // No vector registers
	call	__isoc99_scanf@PLT

	// SECOND printf("Enter another: ");
	leaq	.LC2(%rip), %rax // Load "Enter another: " address
	movq	%rax, %rdi       // First argument
	movl	$0, %eax
	call	printf@PLT

	// SECOND scanf("%d", &num2);
	leaq	-12(%rbp), %rax  // Load address of num2
	                         // -12(%rbp) is where num2 lives on the stack
	movq	%rax, %rsi       // Second argument (&num2)
	leaq	.LC1(%rip), %rax // Format string "%d"
	movq	%rax, %rdi       // First argument
	movl	$0, %eax
	call	__isoc99_scanf@PLT

	// THE MULTIPLICATION OPERATION:
	// This is the key modification! Original code had 'addl' for addition
	// We changed it to 'mull' for multiplication
	movl	-8(%rbp), %edx   // Load num1 into %edx
	                         // WHY %edx? We need it as the operand for mull
	movl	-12(%rbp), %eax  // Load num2 into %eax
	                         // WHY %eax? mull uses %eax as implicit destination

	// THE MODIFICATION: Changed from addl to mull
	// Original: addl	%edx, %eax  (would do: %eax = %eax + %edx)
	mull    %edx             // Multiply %eax by %edx, result in %eax
	                         // WHY mull not mul? mull = 32-bit unsigned multiply
	                         // mull has IMPLICIT destination: always %eax
	                         // Full operation: %eax = %eax * %edx (unsigned)
	                         // IMPORTANT: mull treats numbers as UNSIGNED
	                         // For SIGNED multiplication, use 'imul'

	movl	%eax, -4(%rbp)   // Store result into num3 at -4(%rbp)

	// FINAL printf("Ta-da! %d\n", num3);
	movl	-4(%rbp), %eax   // Load num3 into %eax
	                         // WHY again? We just stored it, but need it for printf
	movl	%eax, %esi       // Move num3 to %esi (2nd argument)
	                         // WHY %esi not %rsi? 32-bit value, use 32-bit register
	                         // %esi is the lower 32 bits of %rsi
	leaq	.LC3(%rip), %rax // Load "Ta-da! %d\n" format string
	movq	%rax, %rdi       // First argument
	movl	$0, %eax         // No vector registers
	call	printf@PLT

	// FUNCTION EPILOGUE:
	movl	$0, %eax         // Return value = 0 (success)
	                         // WHY %eax? Return values go in %rax/%eax
	leave                    // Clean up stack frame
	                         // 'leave' is equivalent to:
	                         //   movq %rbp, %rsp  (restore stack pointer)
	                         //   popq %rbp        (restore old base pointer)
	                         // WHY 'leave'? More efficient than two instructions
	.cfi_def_cfa 7, 8
	ret                      // Return to caller
	                         // Pops return address from stack and jumps to it
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Debian 12.2.0-14+deb12u1) 12.2.0"
	.section	.note.GNU-stack,"",@progbits

// STACK FRAME VISUALIZATION:
//
// Higher addresses
// +----------------+
// | Return address |  (pushed by 'call')
// +----------------+
// | Saved %rbp     |  <- pushed by 'pushq %rbp'
// +----------------+  <- %rbp points here after 'movq %rsp, %rbp'
// | (padding)      |  -4(%rbp)
// +----------------+
// | num3 (result)  |  -4(%rbp)  (4 bytes)
// +----------------+
// | num1           |  -8(%rbp)  (4 bytes)
// +----------------+
// | num2           |  -12(%rbp) (4 bytes)
// +----------------+  <- %rsp points here after 'subq $16, %rsp'
// | (unused)       |
// +----------------+
// Lower addresses

// KEY DIFFERENCES: mull vs imul vs addl
//
// addl %edx, %eax     - Add (signed or unsigned, same for both)
//                       Result: %eax = %eax + %edx
//
// imul %edx, %eax     - Signed multiply (interprets as signed integers)
//                       Result: %eax = %eax * %edx (signed)
//                       Example: -5 * 3 = -15 (correct with imul)
//
// mull %edx           - Unsigned multiply (interprets as unsigned)
//                       Result: %eax = %eax * %edx (unsigned)
//                       Implicit destination (always %eax)
//                       Example: If %eax = -5 (0xFFFFFFFB as unsigned = 4,294,967,291)
//                                -5 * 3 with mull would give wrong signed result!

// EXERCISE:
// 1. Compare this file to discussionDiscussioncommented.s (which uses imul)
// 2. What happens if you input negative numbers?
// 3. Try changing mull back to imul or addl and recompile
// 4. What's the largest number you can multiply before overflow?

// COMPILATION:
// gcc -c dd2.s -o dd2.o     (assemble to object file)
// gcc dd2.o -o dd2          (link to executable)
// ./dd2                      (run it!)
