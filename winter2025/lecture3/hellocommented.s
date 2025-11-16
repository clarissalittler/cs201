# PEDAGOGICAL PURPOSE:
# This is compiler-generated x86-64 assembly code for hello.c, demonstrating how
# a simple C program translates into machine instructions.
# Key learning objectives:
# 1. Understanding assembly language structure and syntax
# 2. Function prologue and epilogue (stack frame setup/teardown)
# 3. Intel syntax vs AT&T syntax for x86-64 assembly
# 4. How string literals are stored in the .rodata section
# 5. The x86-64 calling convention (System V ABI)
# 6. Dynamic linking via PLT (Procedure Linkage Table)
# 7. Security features: endbr64, stack protection metadata
# 8. Position-independent code for ASLR

	.file	"hello.c"         # SOURCE FILE: Debugging metadata
	                          # Tells debugger this came from hello.c

	.intel_syntax noprefix    # SYNTAX DIRECTIVE: Use Intel syntax
	                          # Intel syntax: mov dest, src
	                          # AT&T syntax (default): movq %src, %dest
	                          # "noprefix" means no % before register names

	.text                     # CODE SECTION: All executable code goes here
	                          # .text is read-only and executable
	                          # This is where CPU instructions live

	.section	.rodata       # READ-ONLY DATA SECTION:
	                          # For constant data like string literals
	                          # Cannot be modified at runtime (protection)

.LC0:                         # LOCAL CONSTANT 0: Label for our string
	                          # .LC0 is a compiler-generated label
	                          # Marks the location of our string literal

	.string	"Hello CS201!"    # STRING LITERAL: Null-terminated C string
	                          # Stored as: 'H' 'e' 'l' 'l' 'o' ' ' 'C' 'S' '2' '0' '1' '!' '\0'
	                          # Total: 13 bytes (12 visible + 1 null terminator)
	                          # .string directive automatically adds null terminator
	                          # This is the actual data that gets printed

	.text                     # BACK TO CODE SECTION
	                          # Return to .text for function code

	.globl	main              # GLOBAL SYMBOL: Make 'main' visible to linker
	                          # .globl (or .global) exports this symbol
	                          # The linker needs to find main() to link with C runtime
	                          # Without this, main would be local to this file only

	.type	main, @function   # TYPE DIRECTIVE: Tell linker this is a function
	                          # Helps linker distinguish functions from data
	                          # Used by debuggers and profiling tools

main:                         # FUNCTION LABEL: Entry point of main() function
	                          # This is where execution begins when OS calls main
	                          # Label address becomes the function's address in memory

.LFB0:                        # LOCAL FUNCTION BEGIN: Compiler-generated label
	                          # .LFB = "Local Function Begin"
	                          # Used by debugger for stack unwinding
	                          # Pairs with .LFE0 (Local Function End)

	.cfi_startproc            # CFI DIRECTIVE: Call Frame Information start
	                          # .cfi = "Call Frame Information"
	                          # Used by debuggers and exception handlers for stack unwinding
	                          # Helps debugger reconstruct call stack during crashes
	                          # Enables C++ exceptions to work across function boundaries

	endbr64                   # END BRANCH 64-bit: Security instruction
	                          # Part of Intel CET (Control-flow Enforcement Technology)
	                          # Marks valid target for indirect jumps/calls
	                          # Prevents ROP (Return-Oriented Programming) attacks
	                          # Only indirect jumps can land here (not direct jumps)
	                          # On CPUs without CET support, this is a NOP (no operation)

	# FUNCTION PROLOGUE: Setup stack frame for main()
	push	rbp               # SAVE OLD BASE POINTER: Push caller's rbp onto stack
	                          # rbp = base pointer (frame pointer)
	                          # Stack grows downward (toward lower addresses)
	                          # This preserves caller's stack frame
	                          # Stack pointer (rsp) is decremented by 8 bytes

	.cfi_def_cfa_offset 16    # CFI: Define CFA (Canonical Frame Address) offset
	                          # CFA is now 16 bytes from original stack pointer
	                          # Used by debugger to reconstruct stack

	.cfi_offset 6, -16        # CFI: Register 6 (rbp) saved at offset -16
	                          # Tells debugger where to find old rbp value
	                          # Register numbers: 6 = rbp in DWARF standard

	mov	rbp, rsp              # ESTABLISH NEW BASE POINTER: rbp = rsp
	                          # Creates stable reference for this function's stack frame
	                          # Now rbp points to base of main's stack frame
	                          # rbp won't change as rsp moves (from push/pop operations)
	                          # Local variables would be at rbp-N (negative offsets)

	.cfi_def_cfa_register 6   # CFI: CFA is now defined relative to register 6 (rbp)
	                          # Debugger uses rbp as frame reference, not rsp

	# PREPARE TO CALL puts():
	# The compiler optimized printf("Hello CS201!\n") to puts("Hello CS201!")
	# WHY? puts is simpler and faster for plain strings (no format string parsing)
	# puts automatically adds newline, so \n is removed from string

	lea	rax, .LC0[rip]        # LOAD EFFECTIVE ADDRESS: Get address of string
	                          # lea = Load Effective Address (doesn't dereference)
	                          # .LC0[rip] = address of .LC0 relative to instruction pointer
	                          # rip = instruction pointer (program counter)
	                          # This is POSITION-INDEPENDENT CODE (PIC)
	                          # Enables ASLR (Address Space Layout Randomization)
	                          #
	                          # WHY rip-relative?
	                          # Absolute address: lea rax, 0x12345678 (not relocatable)
	                          # RIP-relative: lea rax, .LC0[rip] (works at any address)
	                          # .LC0[rip] = address of .LC0 relative to next instruction
	                          # Calculated at runtime: rip + offset
	                          #
	                          # After this: rax holds address of "Hello CS201!" string

	mov	rdi, rax              # FIRST ARGUMENT: Move string address to rdi
	                          # x86-64 calling convention (System V ABI):
	                          # - Integer/pointer args in: rdi, rsi, rdx, rcx, r8, r9
	                          # - Floating-point args in: xmm0-xmm7
	                          # - Additional args on stack
	                          # - Return value in rax (or xmm0 for float)
	                          #
	                          # puts takes one argument: const char* s
	                          # First argument goes in rdi
	                          # We're passing the address of our string

	call	puts@PLT          # CALL FUNCTION: Call puts() through PLT
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
	                          #   - Write "Hello CS201!" to stdout
	                          #   - Append newline
	                          #   - Flush output
	                          #   - Return number of characters written (in rax)

	# PREPARE RETURN VALUE:
	mov	eax, 0                # RETURN VALUE: Set return value to 0
	                          # eax is lower 32 bits of rax
	                          # Writing to eax automatically zeros upper 32 bits of rax
	                          # Return value goes in rax (64-bit) or eax (32-bit)
	                          # 0 indicates success (no errors)
	                          # This is what "return 0;" in C becomes
	                          #
	                          # WHY eax instead of rax?
	                          # mov eax, 0 is shorter instruction (saves bytes)
	                          # Zeros upper 32 bits automatically (x86-64 rule)
	                          # Equivalent to: mov rax, 0

	# FUNCTION EPILOGUE: Teardown stack frame and return
	pop	rbp                   # RESTORE OLD BASE POINTER: Pop saved rbp from stack
	                          # This restores caller's stack frame
	                          # Stack pointer (rsp) is incremented by 8 bytes
	                          # Now rbp has caller's value again

	.cfi_def_cfa 7, 8         # CFI: CFA is now defined relative to register 7 (rsp)
	                          # Offset is 8 (return address is at top of stack)
	                          # Register 7 = rsp in DWARF standard

	ret                       # RETURN: Pop return address and jump to it
	                          # Pops 8 bytes from stack into rip (instruction pointer)
	                          # Execution continues at caller (C runtime)
	                          # Return value is in rax (we set it to 0)
	                          #
	                          # What happens after ret:
	                          # 1. Control returns to C runtime startup code
	                          # 2. C runtime calls exit(0) with main's return value
	                          # 3. exit() performs cleanup and terminates process
	                          # 4. OS reclaims resources

	.cfi_endproc              # CFI: End of procedure
	                          # Marks end of Call Frame Information for main

.LFE0:                        # LOCAL FUNCTION END: Compiler-generated label
	                          # Pairs with .LFB0 (Local Function Begin)
	                          # Used by debugger

	.size	main, .-main      # SIZE DIRECTIVE: Size of main function
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

	.align 8                  # ALIGNMENT: Align to 8-byte boundary
	                          # Ensures data is properly aligned in memory
	                          # Required for efficient CPU access

	.long	1f - 0f           # SIZE OF NAME: Size of following name string
	                          # 1f = label "1:" forward, 0f = label "0:" forward
	                          # Calculates size dynamically

	.long	4f - 1f           # SIZE OF DESCRIPTOR: Size of property descriptor

	.long	5                 # TYPE: NT_GNU_PROPERTY_TYPE_0
	                          # Indicates this is a GNU property note

0:                            # LABEL: Start of name
	.string	"GNU"             # NAME: Null-terminated "GNU" string
	                          # Identifies this as a GNU extension

1:                            # LABEL: End of name
	.align 8                  # ALIGNMENT: Align to 8-byte boundary

	.long	0xc0000002        # PROPERTY TYPE: GNU_PROPERTY_X86_FEATURE_1_AND
	                          # Indicates x86-64 feature flags

	.long	3f - 2f           # PROPERTY SIZE: Size of property value

2:                            # LABEL: Start of property value
	.long	0x3               # PROPERTY VALUE: IBT + SHSTK
	                          # Bit 0 (0x1): IBT (Indirect Branch Tracking)
	                          # Bit 1 (0x2): SHSTK (Shadow Stack)
	                          # Both are Intel CET security features
	                          # Prevents ROP and other control-flow hijacking attacks

3:                            # LABEL: End of property value
	.align 8                  # ALIGNMENT: Align to 8-byte boundary

4:                            # LABEL: End of descriptor

# COMPREHENSIVE SUMMARY:
#
# WHAT THIS CODE DOES:
# 1. Defines a string constant "Hello CS201!" in read-only memory
# 2. Implements main() function that:
#    a. Sets up stack frame (push rbp, mov rbp, rsp)
#    b. Loads address of string into rdi (first argument)
#    c. Calls puts() to print the string
#    d. Sets return value to 0 (success)
#    e. Restores stack frame (pop rbp)
#    f. Returns to caller (C runtime)
# 3. Includes security features (CET, NX stack)
# 4. Uses position-independent code (PIC) for ASLR
# 5. Links dynamically to libc via PLT

# REGISTER USAGE SUMMARY:
#
# rax: General-purpose accumulator, return value
# rbp: Base pointer (frame pointer), points to stack frame base
# rsp: Stack pointer, points to top of stack
# rdi: First integer/pointer argument (string address for puts)
# rip: Instruction pointer (program counter), current instruction address
#
# eax: Lower 32 bits of rax (used for 32-bit operations)

# STACK LAYOUT:
#
# Before prologue:
#   [return address] ← rsp
#
# After "push rbp":
#   [old rbp]        ← rsp
#   [return address]
#
# After "mov rbp, rsp":
#   [old rbp]        ← rsp, rbp
#   [return address]
#
# After "call puts":
#   [puts return addr]
#   [old rbp]        ← rbp
#   [return address]
#
# After "pop rbp":
#   [return address] ← rsp

# TO ASSEMBLE AND LINK:
# as hello.s -o hello.o         # Assemble to object file
# gcc hello.o -o hello           # Link with C library
# ./hello                        # Run
# Output: Hello CS201!
#
# OR compile C source directly:
# gcc hello.c -o hello
# (Generates this assembly as intermediate step)
#
# TO SEE GENERATED ASSEMBLY:
# gcc -S hello.c                 # Creates hello.s (AT&T syntax)
# gcc -S -masm=intel hello.c     # Creates hello.s (Intel syntax)
