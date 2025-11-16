# PEDAGOGICAL PURPOSE:
# This is compiler-generated assembly for adding.c, showing how the C compiler
# translates high-level code into machine instructions.
# Key learning objectives:
# 1. Understanding the x86-64 calling convention (how arguments are passed)
# 2. Stack frame setup and teardown (function prologue and epilogue)
# 3. How local variables are stored on the stack
# 4. The relationship between C code and assembly code
# 5. Understanding compiler-generated metadata (.cfi directives)
# 6. Position-independent code (PIE) and the PLT (Procedure Linkage Table)
# 7. How the compiler organizes code and data sections

	.file	"adding.c"         # SOURCE FILE: Debugging info - which C file this came from
	.text                      # CODE SECTION: All executable code goes here
	.globl	adder              # GLOBAL SYMBOL: Make adder visible to other object files
	.type	adder, @function   # TYPE DIRECTIVE: Tell linker this is a function (not data)

adder:                         # FUNCTION LABEL: The entry point for the adder function
.LFB0:                         # LOCAL LABEL: "Local Function Begin" - used by debugger
	.cfi_startproc             # CFI: Call Frame Information start - for stack unwinding
	                          # Used by debuggers and exception handlers

	endbr64                    # ENDBR64: "End Branch 64-bit" - security feature
	                          # Part of Intel CET (Control-flow Enforcement Technology)
	                          # Prevents certain types of ROP (Return-Oriented Programming) attacks
	                          # Valid target for indirect jumps/calls

	# FUNCTION PROLOGUE: Setup stack frame
	pushq	%rbp               # SAVE OLD BASE POINTER: Push caller's %rbp onto stack
	                          # This preserves the caller's stack frame
	.cfi_def_cfa_offset 16    # CFI: Stack is now 16 bytes from original position
	.cfi_offset 6, -16        # CFI: Register 6 (%rbp) is saved at offset -16

	movq	%rsp, %rbp         # ESTABLISH NEW BASE POINTER: %rbp = %rsp
	                          # Creates a stable reference point for local variables
	                          # Now %rbp points to the base of this function's stack frame
	.cfi_def_cfa_register 6   # CFI: %rbp is now the frame reference register

	# SAVE PARAMETERS TO STACK:
	# x86-64 calling convention: first 6 integer args in %rdi, %rsi, %rdx, %rcx, %r8, %r9
	# For adder(int n1, int n2):
	#   n1 arrives in %edi (32-bit portion of %rdi)
	#   n2 arrives in %esi (32-bit portion of %rsi)
	# The compiler saves them to stack (though it could keep them in registers)

	movl	%edi, -20(%rbp)    # STORE PARAMETER n1: Save %edi to stack at %rbp-20
	                          # This is the first parameter (n1)
	                          # movl = move 32-bit (l = long = 4 bytes)

	movl	%esi, -24(%rbp)    # STORE PARAMETER n2: Save %esi to stack at %rbp-24
	                          # This is the second parameter (n2)

	# PERFORM ADDITION: res = n1 + n2
	movl	-20(%rbp), %edx    # LOAD n1: Read first parameter from stack into %edx
	movl	-24(%rbp), %eax    # LOAD n2: Read second parameter from stack into %eax

	addl	%edx, %eax         # ADD: %eax = %eax + %edx (n2 + n1)
	                          # This is the actual addition operation!
	                          # Result is in %eax

	movl	%eax, -4(%rbp)     # STORE RESULT: Save sum to local variable 'res' at %rbp-4
	                          # This corresponds to: res = n1 + n2;

	# PREPARE RETURN VALUE:
	movl	-4(%rbp), %eax     # LOAD RETURN VALUE: Move 'res' into %eax
	                          # x86-64 calling convention: return value goes in %rax (or %eax for 32-bit)
	                          # The caller will read the return value from here

	# FUNCTION EPILOGUE: Teardown stack frame and return
	popq	%rbp               # RESTORE OLD BASE POINTER: Pop saved %rbp from stack
	                          # This restores the caller's stack frame
	.cfi_def_cfa 7, 8         # CFI: Stack pointer (%rsp) is frame reference, offset 8

	ret                        # RETURN: Pop return address from stack and jump to it
	                          # Control goes back to the caller (main)
	                          # Return value is in %eax
	.cfi_endproc               # CFI: End of procedure
.LFE0:                         # LOCAL LABEL: "Local Function End"
	.size	adder, .-adder     # SIZE DIRECTIVE: Size of adder function (for debugger)

	# DATA SECTION: Read-only data
	.section	.rodata        # READ-ONLY DATA: Constants like string literals
.LC0:                          # LOCAL CONSTANT 0: Label for format string
	.string	"%d\n"             # STRING LITERAL: The printf format string
	                          # Stored in read-only memory to prevent modification
	                          # .string automatically null-terminates

	.text                      # BACK TO CODE SECTION
	.globl	main               # GLOBAL SYMBOL: Make main visible to linker
	.type	main, @function    # TYPE: This is a function

main:                          # FUNCTION LABEL: Entry point of the program
.LFB1:                         # Local Function Begin
	.cfi_startproc             # CFI start

	endbr64                    # Security: CET branch target

	# FUNCTION PROLOGUE:
	pushq	%rbp               # Save caller's base pointer
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp         # Establish stack frame
	.cfi_def_cfa_register 6

	subq	$16, %rsp          # ALLOCATE STACK SPACE: Reserve 16 bytes for local variables
	                          # This space will hold n1 and n2
	                          # Aligned to 16 bytes (x86-64 ABI requirement)

	# INITIALIZE LOCAL VARIABLES:
	movl	$10, -8(%rbp)      # INITIALIZE n1: Store 10 at %rbp-8
	                          # This is: int n1 = 10;

	movl	$20, -4(%rbp)      # INITIALIZE n2: Store 20 at %rbp-4
	                          # This is: int n2 = 20;

	# PREPARE FUNCTION CALL: adder(n1, n2)
	# x86-64 calling convention: args in %rdi, %rsi, %rdx, %rcx, %r8, %r9
	movl	-4(%rbp), %edx     # LOAD n2: Read n2 from stack into %edx (temporary)
	movl	-8(%rbp), %eax     # LOAD n1: Read n1 from stack into %eax (temporary)

	movl	%edx, %esi         # ARGUMENT 2: Move n2 into %esi (second parameter register)
	movl	%eax, %edi         # ARGUMENT 1: Move n1 into %edi (first parameter register)

	call	adder              # CALL FUNCTION: Jump to adder, pushing return address
	                          # After adder returns, %eax contains the return value (30)

	# PREPARE PRINTF CALL: printf("%d\n", result)
	movl	%eax, %esi         # ARGUMENT 2: Move adder's return value to %esi
	                          # This is the value to print (30)

	leaq	.LC0(%rip), %rax   # LOAD FORMAT STRING ADDRESS: Load address of "%d\n"
	                          # leaq = Load Effective Address
	                          # %rip = instruction pointer (program counter)
	                          # This is POSITION-INDEPENDENT CODE (PIC)
	                          # .LC0(%rip) means: address of .LC0 relative to current instruction
	                          # Enables ASLR (Address Space Layout Randomization) for security

	movq	%rax, %rdi         # ARGUMENT 1: Move format string address to %rdi
	                          # printf's first argument is the format string

	movl	$0, %eax           # ZERO %eax: Required before calling variadic functions
	                          # For printf, %eax indicates number of vector registers used
	                          # We're not using any, so it's 0

	call	printf@PLT         # CALL PRINTF: Call through PLT (Procedure Linkage Table)
	                          # @PLT enables dynamic linking
	                          # PLT resolves the actual address of printf in libc at runtime
	                          # This allows sharing of library code between processes

	# PREPARE RETURN:
	movl	$0, %eax           # RETURN VALUE: Set return value to 0 (success)

	# FUNCTION EPILOGUE:
	leave                      # LEAVE: Equivalent to: movq %rbp, %rsp; popq %rbp
	                          # Efficiently tears down stack frame
	                          # Restores %rsp and %rbp to caller's values
	.cfi_def_cfa 7, 8

	ret                        # RETURN: Return to caller (OS)
	.cfi_endproc
.LFE1:                         # Local Function End
	.size	main, .-main       # Size of main function

	# METADATA SECTIONS:
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"  # Compiler identification

	.section	.note.GNU-stack,"",@progbits              # Stack properties
	                                                     # Empty = no executable stack needed (security)

	# GNU PROPERTY NOTE: Additional metadata about the binary
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f          # Size of name
	.long	4f - 1f          # Size of descriptor
	.long	5                # Type: NT_GNU_PROPERTY_TYPE_0
0:
	.string	"GNU"            # Name
1:
	.align 8
	.long	0xc0000002       # Property type: GNU_PROPERTY_X86_FEATURE_1_AND
	.long	3f - 2f          # Property size
2:
	.long	0x3              # Property value: IBT (Indirect Branch Tracking) + SHSTK (Shadow Stack)
	                        # These are security features (Intel CET)
3:
	.align 8
4:

# SUMMARY OF CALLING CONVENTION:
#
# When main calls adder(10, 20):
#   1. Arguments placed in registers: %edi = 10, %esi = 20
#   2. call instruction pushes return address and jumps to adder
#   3. adder sets up stack frame, performs addition
#   4. adder places result (30) in %eax
#   5. adder restores stack frame and returns
#   6. main finds return value in %eax
#
# When main calls printf("%d\n", 30):
#   1. Format string address in %rdi
#   2. Value to print (30) in %esi
#   3. %eax = 0 (no vector args)
#   4. call through PLT for dynamic linking
#
# STACK LAYOUT FOR main:
#   %rbp + 0:  Saved %rbp from caller
#   %rbp - 4:  n2 (20)
#   %rbp - 8:  n1 (10)
#   %rbp - 16: (padding for alignment)
#
# STACK LAYOUT FOR adder:
#   %rbp + 0:  Saved %rbp from main
#   %rbp - 4:  res (local variable)
#   %rbp - 20: n1 parameter (copy)
#   %rbp - 24: n2 parameter (copy)

# TO ASSEMBLE AND LINK:
# gcc adding.s -o adding       # Compile assembly to executable
# ./adding                      # Run (outputs: 30)
#
# Or manually:
# as adding.s -o adding.o       # Assemble to object file
# gcc adding.o -o adding        # Link (gcc handles libc linking)
# ./adding
