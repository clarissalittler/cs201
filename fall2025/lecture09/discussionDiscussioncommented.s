// PEDAGOGICAL PURPOSE:
// This assembly file demonstrates the MODIFIED version of discussionDiscussion.c
// where the addition has been changed to SIGNED multiplication (imul).
// This version uses 'imul' instead of 'addl', showing:
// 1. How assembly modification changes program behavior
// 2. The difference between signed (imul) and unsigned (mull) multiplication
// 3. That assembly is human-readable text that can be edited
// 4. How compiler-generated assembly can be hand-modified before assembling
//
// KEY LEARNING OBJECTIVES:
// - Understanding that assembly is an intermediate step (not final code)
// - Learning the 'imul' instruction for signed multiplication
// - Seeing how one instruction change completely alters program behavior
// - Understanding x86-64 calling conventions through concrete example
// - Appreciating the difference between signed and unsigned arithmetic
//
// COMPARISON WITH dd2.s:
// dd2.s uses 'mull' (unsigned multiplication with implicit destination)
// This file uses 'imul' (signed multiplication with explicit destination)

	.file	"discussionDiscussion.c"
	.text
	.section	.rodata   // Read-only data section for string constants

// STRING LITERAL STORAGE:
// The compiler stores all string literals in read-only memory
// Each label (.LC0, .LC1, etc.) marks the start of a string
.LC0:
	.string	"Enter a number: "   // First prompt
.LC1:
	.string	"%d"                 // Format specifier for integers
.LC2:
	.string	"Enter another: "    // Second prompt
.LC3:
	.string	"Ta-da! %d\n"       // Output format string

	.text                    // Code section begins here
	.globl	main             // Make main() visible to linker (entry point)
	.type	main, @function  // Declare main as a function symbol
main:
.LFB0:
	.cfi_startproc           // CFI directives for debugger/unwinder

	// FUNCTION PROLOGUE:
	// Standard x86-64 function entry sequence
	pushq	%rbp             // Save caller's base pointer
	                         // WHY? We must preserve it and restore on return
	                         // The stack now has the old %rbp value
	.cfi_def_cfa_offset 16   // Debug info: stack offset is now 16
	.cfi_offset 6, -16       // Debug info: %rbp saved at offset -16

	movq	%rsp, %rbp       // Create new base pointer
	                         // %rbp = current stack pointer
	                         // WHY? %rbp provides stable reference for local variables
	                         // Even as we push/pop, %rbp stays constant
	.cfi_def_cfa_register 6  // Debug info: use %rbp for stack tracking

	subq	$16, %rsp        // Allocate 16 bytes for local variables
	                         // WHY 16 bytes for 3 ints (12 bytes)?
	                         // x86-64 ABI requires 16-byte stack alignment
	                         // Stack must be 16-byte aligned before 'call' instructions
	                         // LAYOUT:
	                         // -4(%rbp)  = num3 (result)
	                         // -8(%rbp)  = num1 (first input)
	                         // -12(%rbp) = num2 (second input)
	                         // -16(%rbp) = padding for alignment

	// PRINTF CALL #1: printf("Enter a number: ");
	// x86-64 calling convention for arguments:
	// %rdi = arg1, %rsi = arg2, %rdx = arg3, %rcx = arg4, %r8 = arg5, %r9 = arg6
	leaq	.LC0(%rip), %rax // Load Effective Address of string
	                         // %rip-relative addressing for position-independent code
	                         // %rax now contains address of "Enter a number: "
	                         // WHY leaq? Gets the ADDRESS, not the string itself
	movq	%rax, %rdi       // Move string address to first argument register
	                         // WHY %rdi? First argument in calling convention
	movl	$0, %eax         // Set %al (lower byte of %rax) to 0
	                         // WHY? Indicates number of vector registers used
	                         // For varargs functions like printf, this matters
	call	printf@PLT       // Call printf via Procedure Linkage Table
	                         // @PLT enables dynamic linking to libc

	// SCANF CALL #1: scanf("%d", &num1);
	// Need to pass ADDRESS of num1 (second argument)
	leaq	-8(%rbp), %rax   // Load address of num1
	                         // num1 lives at offset -8 from base pointer
	                         // leaq calculates: %rax = %rbp + (-8)
	                         // WHY leaq? We need &num1, not the value of num1
	movq	%rax, %rsi       // Move &num1 to second argument register
	                         // WHY %rsi? Second argument in calling convention
	leaq	.LC1(%rip), %rax // Load address of "%d" format string
	movq	%rax, %rdi       // Move format string to first argument
	movl	$0, %eax         // No vector registers
	call	__isoc99_scanf@PLT // Call ISO C99 version of scanf

	// PRINTF CALL #2: printf("Enter another: ");
	leaq	.LC2(%rip), %rax // Load "Enter another: "
	movq	%rax, %rdi       // First argument
	movl	$0, %eax         // No vector registers
	call	printf@PLT

	// SCANF CALL #2: scanf("%d", &num2);
	leaq	-12(%rbp), %rax  // Load address of num2
	                         // num2 lives at offset -12 from %rbp
	movq	%rax, %rsi       // Second argument (&num2)
	leaq	.LC1(%rip), %rax // Format string "%d"
	movq	%rax, %rdi       // First argument
	movl	$0, %eax         // No vector registers
	call	__isoc99_scanf@PLT

	// THE MULTIPLICATION: THIS IS THE KEY MODIFICATION!
	// Original C code: int num3 = num1 + num2;
	// Original assembly: addl %edx, %eax
	// Modified assembly: imul %edx, %eax
	//
	// WHAT CHANGED? Just one instruction!
	movl	-8(%rbp), %edx   // Load num1 into %edx
	                         // movl = move 32-bit value
	                         // %edx = lower 32 bits of %rdx
	movl	-12(%rbp), %eax  // Load num2 into %eax
	                         // %eax = lower 32 bits of %rax

	imul	%edx, %eax       // SIGNED integer multiplication
	                         // Operation: %eax = %eax * %edx
	                         // WHY 'imul' not 'mull'?
	                         // imul: SIGNED multiplication (handles negative numbers correctly)
	                         // mull: UNSIGNED multiplication (treats values as positive)
	                         //
	                         // EXAMPLE WITH NEGATIVE NUMBERS:
	                         // If num1 = -5 and num2 = 3:
	                         // imul gives: -15 (correct!)
	                         // mull gives: garbage (treats -5 as huge positive number)
	                         //
	                         // INSTRUCTION FORMAT:
	                         // imul has TWO operands: source and destination
	                         // mull has ONE operand (destination is implicit: always %eax)

	movl	%eax, -4(%rbp)   // Store result in num3
	                         // num3 lives at offset -4 from %rbp

	// PRINTF CALL #3: printf("Ta-da! %d\n", num3);
	movl	-4(%rbp), %eax   // Load num3 into %eax
	movl	%eax, %esi       // Move to second argument register
	                         // WHY %esi not %rsi?
	                         // %esi is 32-bit register (for int)
	                         // %rsi is 64-bit register (for pointers)
	                         // Writing to %esi automatically zero-extends to %rsi
	leaq	.LC3(%rip), %rax // Load "Ta-da! %d\n"
	movq	%rax, %rdi       // First argument (format string)
	movl	$0, %eax         // No vector registers
	call	printf@PLT       // Call printf

	// FUNCTION EPILOGUE:
	movl	$0, %eax         // Set return value to 0
	                         // WHY %eax? Return values go in %rax/%eax
	                         // 0 indicates success to the operating system
	leave                    // Tear down stack frame
	                         // 'leave' is shorthand for:
	                         //   movq %rbp, %rsp  (restore stack pointer)
	                         //   popq %rbp        (restore old base pointer)
	                         // WHY use 'leave'? Efficient single instruction
	.cfi_def_cfa 7, 8        // Debug info: stack restored
	ret                      // Return to caller
	                         // Pops return address from stack and jumps there
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Debian 12.2.0-14+deb12u1) 12.2.0"
	.section	.note.GNU-stack,"",@progbits

// STACK FRAME VISUALIZATION:
//
// When main() is executing, the runtime stack looks like:
//
// Higher Memory Addresses
// +------------------+
// | ...              |
// +------------------+
// | Return address   | <- Where to jump when main() returns
// +------------------+
// | Saved %rbp       | <- Caller's base pointer (saved by pushq %rbp)
// +------------------+ <- %rbp points here (after movq %rsp, %rbp)
// | (unused/padding) | -4(%rbp) area
// +------------------+
// | num3             | -4(%rbp) [4 bytes]
// +------------------+
// | num1             | -8(%rbp) [4 bytes]
// +------------------+
// | num2             | -12(%rbp) [4 bytes]
// +------------------+
// | (padding)        | -16(%rbp) [4 bytes for alignment]
// +------------------+ <- %rsp points here (after subq $16, %rsp)
// | ...              |
// +------------------+
// Lower Memory Addresses

// KEY INSTRUCTION COMPARISON:
//
// ADDITION (original):
//   addl %edx, %eax        ; %eax = %eax + %edx
//
// SIGNED MULTIPLICATION (this file):
//   imul %edx, %eax        ; %eax = %eax * %edx (signed)
//
// UNSIGNED MULTIPLICATION (dd2.s):
//   mull %edx              ; %eax = %eax * %edx (unsigned)
//                          ; Note: implicit destination, always uses %eax
//
// WHY THE DIFFERENCE MATTERS:
// For positive numbers: imul and mull give same result
// For negative numbers: imul interprets as signed, mull as unsigned
//
// Example: num1 = -5 (0xFFFFFFFB), num2 = 3
// imul: -5 * 3 = -15 (0xFFFFFFF1) ✓ Correct
// mull: 4294967291 * 3 = overflow  ✗ Wrong for signed math

// MODIFICATION PROCESS:
//
// 1. Start with C source: discussionDiscussion.c (with addition)
// 2. Compile to assembly: gcc -S discussionDiscussion.c
// 3. Edit assembly: Change 'addl' to 'imul' on line 47
// 4. Assemble to object: gcc -c discussionDiscussion.s
// 5. Link to executable: gcc discussionDiscussion.o -o discussion
// 6. Run: ./discussion
// 7. Now it multiplies instead of adds!
//
// WHAT THIS TEACHES:
// - Assembly is human-readable intermediate representation
// - One instruction change = completely different behavior
// - Understanding the compilation pipeline
// - Assembly is BEFORE machine code, so we can modify it!

// CONCEPTUAL QUESTIONS:
// 1. Why can we modify assembly but not machine code?
//    (Machine code is binary, assembly is text)
//
// 2. What happens if we use 'mull' instead of 'imul'?
//    (See dd2.s - wrong results for negative numbers)
//
// 3. Could we add division? subtraction?
//    (Yes! divl, subl, etc.)
//
// 4. Why does the compiler use these specific registers?
//    (x86-64 calling convention - standard across all programs)
//
// 5. What if we change the string literals in assembly?
//    (Totally works! Students often make "Ta-da!" say something else)

// TRY IT YOURSELF:
// Compile: gcc discussionDiscussion.s -o discussion_mult
// Run: ./discussion_mult
// Input: 5 and 3
// Output: Ta-da! 15  (instead of 8 with addition)
//
// Try with negative numbers:
// Input: -5 and 3
// Output: Ta-da! -15 (imul handles negatives correctly)
