# PEDAGOGICAL PURPOSE:
# This is compiler-generated x86-64 assembly code for hello.c, demonstrating how
# a simple C program translates into machine instructions.
# Key learning objectives:
# 1. Understanding compiler-generated assembly structure
# 2. Function prologue and epilogue (stack frame setup/teardown)
# 3. How string literals are stored in .rodata
# 4. The x86-64 calling convention (arguments in %rdi)
# 5. Dynamic linking via PLT (Procedure Linkage Table)
# 6. Position-independent code using %rip-relative addressing
# 7. CFI (Call Frame Information) directives for debugging

	.file	"hello.c"       # SOURCE FILE: Debugging metadata
                                # Tells debugger this came from hello.c

	.text                   # CODE SECTION: All executable code goes here
                                # .text is read-only and executable

	.section	.rodata     # READ-ONLY DATA SECTION:
                                # For constant data like string literals
                                # Cannot be modified at runtime

.LC0:                           # LOCAL CONSTANT 0: Label for our string
                                # .LC0 is a compiler-generated label
                                # Marks the location of the string literal

	.string	"Hello, world!"     # STRING LITERAL: Null-terminated C string
                                # Stored as: 'H' 'e' 'l' 'l' 'o' ',' ' ' 'w' 'o' 'r' 'l' 'd' '!' '\0'
                                # Total: 14 bytes (13 visible + 1 null terminator)
                                # .string directive automatically adds null terminator
                                # This is the actual data that gets printed

	.text                   # BACK TO CODE SECTION
                                # Return to .text for function code

	.globl	main            # GLOBAL SYMBOL: Make 'main' visible to linker
                                # .globl (or .global) exports this symbol
                                # The C runtime needs to find main() to call it
                                # Without this, main would be local to this file only

	.type	main, @function # TYPE DIRECTIVE: Tell linker this is a function
                                # Helps linker distinguish functions from data
                                # Used by debuggers and profiling tools

main:                           # FUNCTION LABEL: Entry point of main() function
                                # This is where execution begins when C runtime calls main
                                # Label address becomes the function's address in memory

.LFB0:                          # LOCAL FUNCTION BEGIN: Compiler-generated label
                                # .LFB = "Local Function Begin"
                                # Used by debugger for stack unwinding
                                # Pairs with .LFE0 (Local Function End)

	.cfi_startproc          # CFI DIRECTIVE: Call Frame Information start
                                # .cfi = "Call Frame Information"
                                # Used by debuggers and exception handlers for stack unwinding
                                # Helps debugger reconstruct call stack during crashes
                                # Enables C++ exceptions to work across function boundaries

	endbr64                 # END BRANCH 64-bit: Security instruction
                                # Part of Intel CET (Control-flow Enforcement Technology)
                                # Marks valid target for indirect jumps/calls
                                # Prevents ROP (Return-Oriented Programming) attacks
                                # Only indirect jumps can land here (not direct jumps)
                                # On CPUs without CET support, this is a NOP

	# FUNCTION PROLOGUE: Setup stack frame for main()
	pushq	%rbp            # SAVE OLD BASE POINTER: Push caller's rbp onto stack
                                # %rbp = base pointer (frame pointer)
                                # Stack grows downward (toward lower addresses)
                                # This preserves caller's stack frame
                                # Stack pointer (%rsp) is decremented by 8 bytes

	.cfi_def_cfa_offset 16  # CFI: Define CFA (Canonical Frame Address) offset
                                # CFA is now 16 bytes from original stack pointer
                                # Used by debugger to reconstruct stack

	.cfi_offset 6, -16      # CFI: Register 6 (rbp) saved at offset -16
                                # Tells debugger where to find old rbp value
                                # Register numbers: 6 = rbp in DWARF standard

	movq	%rsp, %rbp      # ESTABLISH NEW BASE POINTER: %rbp = %rsp
                                # Creates stable reference for this function's stack frame
                                # Now %rbp points to base of main's stack frame
                                # %rbp won't change as %rsp moves (from push/pop operations)
                                # Local variables would be at %rbp-N (negative offsets)

	.cfi_def_cfa_register 6 # CFI: CFA is now defined relative to register 6 (rbp)
                                # Debugger uses %rbp as frame reference, not %rsp

	# PREPARE TO CALL puts():
	# The compiler optimized printf("Hello, world!\n") to puts("Hello, world!")
	# WHY? puts is simpler and faster for plain strings (no format string parsing)
	# puts automatically adds newline, so \n is included in the string

	leaq	.LC0(%rip), %rax    # LOAD EFFECTIVE ADDRESS: Get address of string
                                # lea = Load Effective Address (doesn't dereference)
                                # .LC0(%rip) = address of .LC0 relative to instruction pointer
                                # %rip = instruction pointer (program counter)
                                # This is POSITION-INDEPENDENT CODE (PIC)
                                # Enables ASLR (Address Space Layout Randomization)
                                #
                                # WHY %rip-relative?
                                # Absolute address: lea 0x12345678,%rax (not relocatable)
                                # RIP-relative: leaq .LC0(%rip),%rax (works at any address)
                                # .LC0(%rip) = address of .LC0 relative to next instruction
                                # Calculated at runtime: %rip + offset
                                #
                                # After this: %rax holds address of "Hello, world!" string

	movq	%rax, %rdi      # FIRST ARGUMENT: Move string address to %rdi
                                # x86-64 calling convention (System V ABI):
                                # - Integer/pointer args in: %rdi, %rsi, %rdx, %rcx, %r8, %r9
                                # - Floating-point args in: xmm0-xmm7
                                # - Additional args on stack
                                # - Return value in %rax (or xmm0 for float)
                                #
                                # puts takes one argument: const char* s
                                # First argument goes in %rdi
                                # We're passing the address of our string

	call	puts@PLT        # CALL FUNCTION: Call puts() through PLT
                                # call instruction:
                                #   1. Pushes return address (next instruction) onto stack
                                #   2. Jumps to target address
                                #
                                # @PLT = Procedure Linkage Table
                                # PLT is used for DYNAMIC LINKING
                                # puts is in libc.so (shared library)
                                # PLT resolves actual address of puts at runtime
                                # This allows:
                                #   - Sharing library code between processes
                                #   - Updating libraries without recompiling programs
                                #   - Lazy binding (resolve on first call)
                                #
                                # puts() will:
                                #   - Write "Hello, world!" to stdout
                                #   - Append newline
                                #   - Flush output
                                #   - Return number of characters written (in %rax)

	# PREPARE RETURN VALUE:
	movl	$0, %eax        # RETURN VALUE: Set return value to 0
                                # %eax is lower 32 bits of %rax
                                # Writing to %eax automatically zeros upper 32 bits of %rax
                                # Return value goes in %rax (64-bit) or %eax (32-bit)
                                # 0 indicates success (no errors)
                                # This is what "return 0;" in C becomes
                                #
                                # WHY %eax instead of %rax?
                                # mov $0,%eax is shorter instruction (saves bytes)
                                # Zeros upper 32 bits automatically (x86-64 rule)
                                # Equivalent to: mov $0,%rax

	# FUNCTION EPILOGUE: Teardown stack frame and return
	popq	%rbp            # RESTORE OLD BASE POINTER: Pop saved rbp from stack
                                # This restores caller's stack frame
                                # Stack pointer (%rsp) is incremented by 8 bytes
                                # Now %rbp has caller's value again

	.cfi_def_cfa 7, 8       # CFI: CFA is now defined relative to register 7 (%rsp)
                                # Offset is 8 (return address is at top of stack)
                                # Register 7 = %rsp in DWARF standard

	ret                     # RETURN: Pop return address and jump to it
                                # Pops 8 bytes from stack into %rip (instruction pointer)
                                # Execution continues at caller (C runtime)
                                # Return value is in %rax (we set it to 0)
                                #
                                # What happens after ret:
                                # 1. Control returns to C runtime startup code
                                # 2. C runtime calls exit(0) with main's return value
                                # 3. exit() performs cleanup and terminates process
                                # 4. OS reclaims resources

	.cfi_endproc            # CFI: End of procedure
                                # Marks end of Call Frame Information for main

.LFE0:                          # LOCAL FUNCTION END: Compiler-generated label
                                # Pairs with .LFB0 (Local Function Begin)
                                # Used by debugger

	.size	main, .-main    # SIZE DIRECTIVE: Size of main function
                                # .-main = current address - address of main
                                # Tells linker and debugger the function's size in bytes
                                # Used for bounds checking and optimization

	# COMPILER IDENTIFICATION:
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"
                                # Identifies the compiler version
                                # Embedded in object file for debugging/tracking

	# STACK PROTECTION METADATA:
	.section	.note.GNU-stack,"",@progbits
                                # Stack properties section
                                # Empty = no executable stack needed
                                # SECURITY: Prevents code execution from stack
                                # Prevents certain buffer overflow exploits
                                # The stack is marked non-executable (NX bit)

	# GNU PROPERTY NOTE (SECURITY FEATURES):
	.section	.note.gnu.property,"a"
                                # Section for GNU property notes
                                # "a" flag = allocatable (loaded into memory)

	.align 8                # ALIGNMENT: Align to 8-byte boundary
                                # Ensures data is properly aligned in memory

	.long	1f - 0f         # SIZE OF NAME: Size of following name string
	.long	4f - 1f         # SIZE OF DESCRIPTOR: Size of property descriptor
	.long	5               # TYPE: NT_GNU_PROPERTY_TYPE_0
0:
	.string	"GNU"           # NAME: Identifies this as a GNU extension
1:
	.align 8
	.long	0xc0000002      # PROPERTY TYPE: GNU_PROPERTY_X86_FEATURE_1_AND
	.long	3f - 2f         # PROPERTY SIZE
2:
	.long	0x3             # PROPERTY VALUE: IBT + SHSTK
                                # Bit 0: IBT (Indirect Branch Tracking)
                                # Bit 1: SHSTK (Shadow Stack)
                                # Both are Intel CET security features
3:
	.align 8
4:

# TO ASSEMBLE AND LINK:
# gcc -c hello.s -o hello.o    # Assemble to object file
# gcc hello.o -o hello         # Link with C library
# ./hello                      # Run
# Output: Hello, world!
#
# OR compile C source directly:
# gcc hello.c -o hello         # Generates this assembly as intermediate step

# COMPARISON TO C:
# The simple C program (7 lines) becomes ~50+ lines of assembly!
# Most of it is:
# - Debugging metadata (.cfi directives)
# - Security features (endbr64, stack protection)
# - Compiler identification
# - The actual work is just ~10 instructions
