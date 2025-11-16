# PEDAGOGICAL PURPOSE:
# This is the COMPILER-GENERATED assembly code for byteOrder.c
# It demonstrates how the C code translates to machine instructions.
# Key learning objectives:
# 1. Understanding the relationship between C and assembly code
# 2. Seeing how variables are stored on the stack
# 3. Understanding function prologue/epilogue (stack frame setup)
# 4. Learning how loops are implemented at the assembly level
# 5. Seeing how printf is called with arguments
# 6. Understanding compiler-generated optimizations and metadata
# 7. Introduction to stack-relative addressing with %rbp

	.file	"byteOrder.c"        # DEBUG INFO: Source file name for debugger
	.text                        # SECTION: Start of executable code section
	.section	.rodata          # SECTION: Read-only data (constants, strings)
.LC0:
	.string	"%x "                # STRING LITERAL: Format string for printf
	                             # This is the "%x " used in the loop
	.text                        # SECTION: Back to executable code
	.globl	main                 # DIRECTIVE: Make main visible to linker
	.type	main, @function      # DEBUG INFO: Declare main as a function
main:
.LFB0:                           # LABEL: Local Function Begin 0 (for debugging)
	.cfi_startproc               # DEBUG INFO: Call Frame Information starts

	# SECURITY FEATURE: endbr64 (End Branch 64-bit)
	# This is part of Intel's Control-flow Enforcement Technology (CET)
	# It prevents certain types of exploit attacks (return-oriented programming)
	endbr64

	# FUNCTION PROLOGUE: Set up stack frame
	# Every function needs to save the old base pointer and set up its own
	pushq	%rbp                 # Save caller's base pointer on stack
	.cfi_def_cfa_offset 16       # DEBUG INFO: Stack offset is now 16
	.cfi_offset 6, -16           # DEBUG INFO: rbp saved at offset -16

	movq	%rsp, %rbp           # Set rbp to current stack pointer
	.cfi_def_cfa_register 6      # DEBUG INFO: Now using rbp for stack frame

	# ALLOCATE STACK SPACE:
	# Subtract 32 from stack pointer to reserve local variable space
	# This gives us room for: num (4 bytes), pty (8 bytes), i (4 bytes)
	# Plus alignment and stack canary space
	subq	$32, %rsp

	# STACK CANARY PROTECTION:
	# Load a random value from thread-local storage and put it on stack
	# Before returning, we'll check if it changed (detects buffer overflows)
	movq	%fs:40, %rax         # Load stack canary from fs segment
	movq	%rax, -8(%rbp)       # Store it at -8(%rbp)
	xorl	%eax, %eax           # Clear eax (common compiler idiom)

	# INITIALIZE num VARIABLE:
	# Store 0x1a2b3c4d (439041101 in decimal) in memory
	# -24(%rbp) is the location of 'num' on the stack
	movl	$439041101, -24(%rbp)  # int num = 0x1a2b3c4d;

	# INITIALIZE pty POINTER:
	# Load the ADDRESS of num into rax, then store it in pty
	leaq	-24(%rbp), %rax      # LEA: Load Effective Address of num
	                             # This is equivalent to &num in C
	movq	%rax, -16(%rbp)      # Store address in pty variable
	                             # unsigned char* pty = (unsigned char*)&num;

	# INITIALIZE LOOP COUNTER:
	# Set i = 0 for the for loop
	movl	$0, -20(%rbp)        # int i = 0;

	jmp	.L2                      # Jump to loop condition check

# LOOP BODY:
.L3:
	# PREPARE TO ACCESS pty[i]:
	# We need to calculate the address: pty + i
	movl	-20(%rbp), %eax      # Load i into eax
	movslq	%eax, %rdx           # Sign-extend i to 64-bit (in rdx)
	                             # movslq = move and sign-extend long to quad
	movq	-16(%rbp), %rax      # Load pty pointer into rax
	addq	%rdx, %rax           # Add i to pty (pointer arithmetic)
	                             # Now rax = pty + i

	# READ ONE BYTE FROM MEMORY:
	movzbl	(%rax), %eax         # Load byte at (pty+i), zero-extend to 32-bit
	                             # movzbl = move zero-extended byte to long
	                             # This is the pty[i] access
	movzbl	%al, %eax            # Redundant zero-extension (compiler artifact)

	# PREPARE printf CALL:
	# printf takes arguments in registers: %rdi, %rsi, %rdx, ...
	# First arg (%rdi): format string
	# Second arg (%rsi): the byte value to print
	movl	%eax, %esi           # Move byte value to %esi (second argument)
	leaq	.LC0(%rip), %rax     # Load address of "%x " format string
	                             # %rip-relative addressing (position-independent)
	movq	%rax, %rdi           # Move format string to %rdi (first argument)
	movl	$0, %eax             # Set %eax = 0 (required for variadic functions)

	call	printf@PLT           # Call printf (through PLT for dynamic linking)
	                             # PLT = Procedure Linkage Table

	# INCREMENT LOOP COUNTER:
	addl	$1, -20(%rbp)        # i++ (increment i by 1)

# LOOP CONDITION:
.L2:
	cmpl	$3, -20(%rbp)        # Compare i with 3
	jle	.L3                      # If i <= 3, jump back to loop body
	                             # This implements: for(...; i<4; ...)

	# PRINT NEWLINE:
	# Call putchar(10) where 10 is ASCII newline '\n'
	movl	$10, %edi            # Load 10 into first argument register
	call	putchar@PLT          # Call putchar (optimized from printf("\n"))

	# PREPARE RETURN VALUE:
	movl	$0, %eax             # Set return value to 0 (success)

	# STACK CANARY CHECK:
	# Verify the canary hasn't been corrupted (security check)
	movq	-8(%rbp), %rdx       # Load saved canary value
	subq	%fs:40, %rdx         # Subtract original canary value
	je	.L5                      # If equal (zero), skip to normal return
	call	__stack_chk_fail@PLT # If not equal, stack was corrupted! Abort!

.L5:
	# FUNCTION EPILOGUE: Restore stack and return
	leave                        # Equivalent to: movq %rbp, %rsp; popq %rbp
	                             # Restores stack pointer and base pointer
	.cfi_def_cfa 7, 8            # DEBUG INFO: Stack frame info
	ret                          # Return to caller
	.cfi_endproc                 # DEBUG INFO: Call Frame Information ends
.LFE0:                           # LABEL: Local Function End 0
	.size	main, .-main         # DEBUG INFO: Size of main function

	# COMPILER/LINKER METADATA:
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"  # Compiler version
	.section	.note.GNU-stack,"",@progbits              # Stack is not executable

	# GNU PROPERTY NOTES (for security features):
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002           # Property type (indicates CET support)
	.long	3f - 2f
2:
	.long	0x3                  # CET flags
3:
	.align 8
4:

# KEY CONCEPTS DEMONSTRATED:
# 1. STACK LAYOUT: Variables stored at negative offsets from %rbp
#    -8(%rbp):  stack canary
#    -16(%rbp): pty pointer (8 bytes)
#    -20(%rbp): loop counter i (4 bytes)
#    -24(%rbp): num variable (4 bytes)
#
# 2. CALLING CONVENTION (System V AMD64 ABI):
#    First 6 args in: %rdi, %rsi, %rdx, %rcx, %r8, %r9
#    Return value in: %rax
#
# 3. BYTE ACCESS: movzbl loads a byte and zero-extends it
#
# 4. POSITION-INDEPENDENT CODE: Uses %rip-relative addressing for .LC0
#
# 5. SECURITY: Stack canary and endbr64 instruction

# TO COMPILE THIS YOURSELF:
# gcc -S byteOrder.c -o byteOrder.s
# (The -S flag tells gcc to stop after generating assembly)
