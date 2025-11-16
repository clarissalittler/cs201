# PEDAGOGICAL PURPOSE:
# This program demonstrates working with the data section, memory addressing,
# and dereferencing pointers in x86-64 assembly. It introduces the fundamental
# concepts of storing data in memory and manipulating it through pointers.
#
# Key learning objectives:
# 1. Understanding the .data section and how to define initialized data
# 2. Learning the difference between labels and values in memory
# 3. Understanding RIP-relative addressing for position-independent code
# 4. Learning pointer dereferencing using parentheses syntax
# 5. Understanding the addl instruction with memory operands
# 6. Seeing the difference between loading an address vs loading a value
# 7. Understanding that data can have specific sizes (.long = 4 bytes)
#
# CONCEPTUAL OVERVIEW:
# This program implements:
#   long num1 = 200;       // initialized data in .data section
#   long *ptr = &num1;      // get address of num1
#   *ptr = *ptr + 10;       // dereference and modify: 200 + 10 = 210
#   return num1;            // exit with the modified value (210)

        .section .data             # DATA SECTION: Initialized read-write data
                                  # This section contains variables with initial values
                                  # Unlike .rodata (read-only) or .bss (uninitialized)
                                  # .data holds variables that have initial values and can be modified

num1:   .long 200                 # DEFINE VARIABLE: Allocate and initialize a 4-byte integer
                                  # BREAKDOWN:
                                  # num1:       - Label marking the memory location
                                  # .long       - Directive: allocate 32-bit (4-byte) integer
                                  # 200         - Initial value stored at this location
                                  #
                                  # MEMORY LAYOUT:
                                  # Address: [num1] [num1+1] [num1+2] [num1+3]
                                  # Value:   [0xC8]  [0x00]   [0x00]   [0x00]
                                  # (200 in little-endian format)
                                  #
                                  # KEY DISTINCTION:
                                  # num1 is a LABEL (represents an address)
                                  # 200 is the VALUE stored at that address

        .section .text             # CODE SECTION: Executable instructions
        .global _start             # GLOBAL DIRECTIVE: Entry point visible to linker

_start:
        mov num1(%rip),%rbx        # LOAD ADDRESS: %rbx = address of num1
                                  # CRITICAL SYNTAX NOTE:
                                  # This does NOT load the value 200!
                                  # It loads the ADDRESS where 200 is stored
                                  #
                                  # RIP-RELATIVE ADDRESSING:
                                  # num1(%rip) means "num1 relative to instruction pointer"
                                  # %rip = current instruction pointer (program counter)
                                  # This is POSITION-INDEPENDENT CODE (PIC)
                                  #
                                  # WHY RIP-RELATIVE?
                                  # - Enables ASLR (Address Space Layout Randomization)
                                  # - Works regardless of where program is loaded in memory
                                  # - Modern security and shared library requirement
                                  #
                                  # WHAT'S IN %rbx AFTER THIS?
                                  # The memory address of num1 (e.g., 0x404000)
                                  # Think of this as: long *ptr = &num1;
                                  #
                                  # INCORRECT INTERPRETATION IN COMMENT:
                                  # The original comment says "# &num1 -> %rbx"
                                  # which is misleading. It should be:
                                  # "address of num1 -> %rbx" or "load effective address"
                                  # Actually, looking more carefully, this instruction
                                  # loads the ADDRESS, not the value!

        addl $10,(%rbx)            # DEREFERENCE AND ADD: *(%rbx) = *(%rbx) + 10
                                  # THIS IS THE CRITICAL LINE!
                                  # Let's break down the syntax carefully:
                                  #
                                  # addl - Add Long (32-bit operation)
                                  # $10  - Source operand: immediate value 10
                                  # (%rbx) - Destination: DEREFERENCE %rbx
                                  #
                                  # PARENTHESES MEAN DEREFERENCE:
                                  # (%rbx) means "the memory location pointed to by %rbx"
                                  # It's equivalent to C's *ptr syntax
                                  #
                                  # OPERATION STEP BY STEP:
                                  # 1. Read address from %rbx (address of num1)
                                  # 2. Read 32-bit value at that address (200)
                                  # 3. Add 10 to it (200 + 10 = 210)
                                  # 4. Write result back to that address
                                  #
                                  # MEMORY BEFORE: num1 contains 200
                                  # MEMORY AFTER:  num1 contains 210
                                  #
                                  # C EQUIVALENT:
                                  # *ptr = *ptr + 10;
                                  # or simply: *ptr += 10;
                                  #
                                  # WHY USE addl NOT addq?
                                  # Because num1 was defined with .long (32-bit)
                                  # We should use 32-bit operations to match
                                  # Using addq would work but access more bytes than necessary

        mov num1,%rdi              # LOAD VALUE FOR EXIT CODE: %rdi = value at num1
                                  # WAIT - SYNTAX LOOKS DIFFERENT!
                                  # This time we write: mov num1,%rdi (no %rip, no parens)
                                  # The assembler will actually generate this as:
                                  # movl num1(%rip),%edi (RIP-relative, load value)
                                  #
                                  # WHAT THIS REALLY DOES:
                                  # Load the 32-bit value FROM the memory location labeled num1
                                  # This reads the value 210 (after our addition)
                                  # And puts it into %edi (lower 32 bits of %rdi)
                                  #
                                  # C EQUIVALENT:
                                  # return num1;
                                  #
                                  # NOTE ON REGISTER SIZE:
                                  # Even though we use %rdi (64-bit), movl zeros the upper 32 bits
                                  # So %rdi = 0x00000000000000D2 (210 in hex is 0xD2)

        mov $60,%rax               # SYSCALL NUMBER: 60 = exit
                                  # Prepare to terminate the program

        syscall                    # INVOKE KERNEL: Execute exit(%rdi)
                                  # Program exits with code 210 (the modified value)
                                  # You can verify with: ./data1; echo $?
                                  # Shell will display: 210

# DETAILED ADDRESSING MODES EXPLANATION:
#
# LOADING AN ADDRESS (effective address):
#   lea num1(%rip),%rbx          - Load address of num1 into %rbx
#   mov num1(%rip),%rbx          - Also loads address in this context (depends on assembler)
#
# LOADING A VALUE (direct from memory):
#   mov num1(%rip),%eax          - Load the VALUE at num1 into %eax
#   mov num1,%eax                - Assembler converts to num1(%rip),%eax
#
# DEREFERENCING A POINTER (indirect through register):
#   mov (%rbx),%eax              - Load value from memory address in %rbx
#   add $10,(%rbx)               - Add 10 to value at memory address in %rbx
#   mov %eax,(%rbx)              - Store %eax to memory address in %rbx

# MEMORY SECTIONS EXPLAINED:
#
# .data section:
#   - Contains initialized read-write data
#   - Variables with starting values
#   - Stored in the data segment of the executable
#   - Example: .long 200, .quad 1000, .byte 'A'
#
# .rodata section:
#   - Contains initialized read-only data
#   - Constants and string literals
#   - Cannot be modified at runtime (memory protection)
#   - Example: .string "Hello", .long 42
#
# .bss section:
#   - Contains uninitialized data (Block Started by Symbol)
#   - Variables without initial values (implicitly zero)
#   - Doesn't take space in executable (only at runtime)
#   - Example: .skip 128 (reserve 128 bytes)
#
# .text section:
#   - Contains executable code
#   - Read-only and executable (not writable)
#   - Your instructions go here

# DATA DIRECTIVES:
#
# .byte   - 1 byte  (8 bits)    Example: .byte 65        (letter 'A')
# .word   - 2 bytes (16 bits)   Example: .word 1000
# .long   - 4 bytes (32 bits)   Example: .long 200       (used in this program)
# .quad   - 8 bytes (64 bits)   Example: .quad 1000000
#
# .ascii  - String without null terminator
# .asciz  - String with null terminator (C-style string)
# .string - Same as .asciz
#
# .skip N - Reserve N uninitialized bytes
# .space N - Same as .skip

# POINTER OPERATIONS IN ASSEMBLY:
#
# C Code                   | Assembly Equivalent
# -------------------------|----------------------------------
# int x = 200;            | num1: .long 200
# int *ptr;               | (use a register like %rbx)
# ptr = &x;               | mov num1(%rip),%rbx  or  lea num1(%rip),%rbx
# *ptr = *ptr + 10;       | addl $10,(%rbx)
# return x;               | mov num1,%edi

# POSITION-INDEPENDENT CODE (PIC):
#
# Modern executables use PIC for security (ASLR):
#   - Program can load at different addresses each run
#   - Prevents certain types of exploits
#   - Required for shared libraries
#
# RIP-relative addressing makes this possible:
#   - Instead of absolute address: 0x404000
#   - Use offset from instruction: instruction_addr + offset
#   - Works regardless of load address

# COMMON PITFALL:
#
# WRONG:  mov num1,%rbx        # Loads VALUE (200), not address!
#                              # (Assembler makes it: movl num1(%rip),%ebx)
# RIGHT:  lea num1(%rip),%rbx  # Explicitly loads ADDRESS
#
# For clarity, use 'lea' when you want an address!

# EQUIVALENT C CODE:
#
# long num1 = 200;          // initialized global variable
#
# int main() {
#     long *ptr = &num1;    // get pointer to num1
#     *ptr = *ptr + 10;     // add 10 to the value
#     return num1;          // return modified value (210)
# }

# TO ASSEMBLE, LINK, AND RUN:
# as data1.s -o data1.o
# ld data1.o -o data1
# ./data1
# echo $?                          # Should display 210

# EXPERIMENT:
# Try changing the initial value:
#   num1: .long 50
# Or change the addition:
#   addl $100,(%rbx)
# The exit code should reflect: initial_value + added_value
