# PEDAGOGICAL PURPOSE:
# This program introduces the .data section for storing global variables
# and demonstrates direct memory access without pointers.
# Key learning objectives:
# 1. The .data section for initialized global variables
# 2. Using .long directive to define 32-bit integers
# 3. Direct memory access (addl $10,num1)
# 4. Operating on memory locations vs registers
# 5. Difference between .text (code) and .data (variables)
# 6. The 'l' suffix for 32-bit operations

        .section .data          # DATA SECTION: For initialized global variables
                                # Variables here exist for the program's lifetime
                                # They are stored in the data segment of memory
                                # Unlike the stack, these have fixed addresses

num1:   .long 200               # DEFINE 32-BIT INTEGER: Create variable named num1
                                # .long means 32-bit integer (4 bytes)
                                # num1 is a label that represents the memory address
                                # The value 200 is stored at that address
                                #exists for the lifetime of the program
                                ## .long means 32 bit
                                #
                                # WHY .long?
                                # - .byte = 1 byte (8 bits)
                                # - .word = 2 bytes (16 bits) - historically "word"
                                # - .long = 4 bytes (32 bits) - "long word"
                                # - .quad = 8 bytes (64 bits) - "quad word"

        .section .text          # CODE SECTION: Executable instructions
        .global _start

_start:
        addl $10,num1           # ADD TO MEMORY: Add 10 to the value at num1
                                # addl = add long (32-bit addition)
                                # Reads the value at num1 (200)
                                # Adds 10 to it
                                # Writes result back to num1 (210)
                                #need the l so that it understands that it's adding into a 32bit number
                                ## l means long
                                ## usually we can leave these off
                                ## because the assembler can figure out
                                ## if it's using the q (64 bit), l (32bit), w (16 bit), b (8 bit)
                                ## q - 8 bytes, l - 4 bytes, w - 2 bytes, b - 1 byte
                                #
                                # MEMORY OPERATION:
                                # This is different from adding to a register!
                                # Compare:
                                #   add $10,%rax   - adds to register
                                #   addl $10,num1  - adds to memory location
                                #
                                # What happens:
                                # 1. CPU reads value from memory address num1 → 200
                                # 2. CPU adds 10 → 210
                                # 3. CPU writes result back to num1 → memory now has 210

        mov num1,%rdi           # LOAD VALUE: Move num1's value into %rdi
                                # This reads the 32-bit value from memory at num1
                                # Since num1 now contains 210, %rdi = 210
                                # This is our exit code
                                #
                                # NOTE: mov num1,%rdi (without $) loads the VALUE
                                # If we wrote mov $num1,%rdi, it would load the ADDRESS
                                #
                                # Actually, on x86-64:
                                # mov num1,%rdi loads from [num1]
                                # The assembler knows num1 is a memory address

        mov $60,%rax            # SETUP EXIT SYSCALL: %rax = 60
        syscall                 # EXIT: Exit with status 210 (but truncated to 210 % 256 = 210)

# WHAT THIS PROGRAM DOES:
# 1. Defines num1 = 200 in .data section
# 2. Adds 10 to num1 in memory (num1 becomes 210)
# 3. Loads num1 into %rdi (exit code)
# 4. Exits with status 210

# TO ASSEMBLE AND RUN:
# as data0.s -o data0.o
# ld data0.o -o data0
# ./data0
# echo $?  # Shows 210

# EXPECTED EXIT STATUS:
# 210 (200 + 10)

# THE .data SECTION:
#
# Memory layout of a program:
#   +-------------------+  High addresses
#   | Stack             |  ← grows down
#   | (local variables) |
#   +-------------------+
#   | Heap              |  ← grows up
#   | (dynamic memory)  |
#   +-------------------+
#   | .bss              |  Uninitialized globals (zeroed)
#   | (int x;)          |
#   +-------------------+
#   | .data             |  Initialized globals
#   | (int x = 5;)      |  ← num1 lives here!
#   +-------------------+
#   | .text             |  Code (read-only)
#   | (instructions)    |
#   +-------------------+  Low addresses

# DIRECTIVES FOR DEFINING DATA:
#
# .byte value     # 1 byte  (8 bits):   0 to 255
# .word value     # 2 bytes (16 bits):  0 to 65535
# .long value     # 4 bytes (32 bits):  0 to 4294967295
# .quad value     # 8 bytes (64 bits):  0 to 2^64-1
#
# Examples:
#   myByte: .byte 42         # One byte with value 42
#   myWord: .word 1000       # Two bytes with value 1000
#   myLong: .long 200        # Four bytes with value 200
#   myQuad: .quad 9999999999 # Eight bytes

# SIZE SUFFIXES FOR INSTRUCTIONS:
#
# b = byte (8-bit):   addb, movb, cmpb
# w = word (16-bit):  addw, movw, cmpw
# l = long (32-bit):  addl, movl, cmpl
# q = quad (64-bit):  addq, movq, cmpq
#
# Examples:
#   addb $1,%al     # Add 1 to 8-bit register
#   addw $1,%ax     # Add 1 to 16-bit register
#   addl $1,%eax    # Add 1 to 32-bit register
#   addq $1,%rax    # Add 1 to 64-bit register

# WHY THE SIZE MATTERS:
#
# If num1 is .long (32-bit), we need addl (not addq or addb):
#   addl $10,num1   ✓ Correct - operates on 32 bits
#   addq $10,num1   ✗ Wrong - tries to add to 64 bits
#   addb $10,num1   ✗ Wrong - only adds to first 8 bits
#
# Using the wrong size can cause:
# - Data corruption
# - Segmentation faults
# - Unexpected behavior

# DIRECT MEMORY ACCESS:
#
# addl $10,num1 directly modifies memory
# This is called a "memory operand"
#
# Alternative approach using registers:
#   mov num1,%eax    # Load from memory to register
#   add $10,%eax     # Add in register
#   mov %eax,num1    # Store back to memory
#
# Direct memory access is shorter but slower
# (memory access is much slower than register access)

# COMPARISON TO C:
# This program is equivalent to:
#   int num1 = 200;   // Global variable
#   int main() {
#       num1 = num1 + 10;  // Or: num1 += 10;
#       return num1;       // Returns 210
#   }

# MEMORY vs REGISTER OPERATIONS:
#
# Register-only (fast):
#   mov $200,%rdi
#   add $10,%rdi
#
# Memory operation (slower):
#   addl $10,num1
#   mov num1,%rdi
#
# Registers are 100-1000x faster than RAM!
# But we need memory for:
# - Large amounts of data
# - Data that persists across function calls
# - Sharing data between functions
