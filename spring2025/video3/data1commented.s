# PEDAGOGICAL PURPOSE:
# This program demonstrates indirect memory access using lea (load effective address)
# and shows RIP-relative addressing for position-independent code.
# Key learning objectives:
# 1. The lea instruction for loading addresses (not values)
# 2. RIP-relative addressing for position-independent code
# 3. Dereferencing pointers with (%rbx) syntax
# 4. Difference between address and value
# 5. How to use pointers in assembly

        .section .data          # DATA SECTION: Initialized global variables
num1:   .long 200               # DEFINE 32-BIT INTEGER: num1 = 200
                                #exists for the lifetime of the program
                                ## .long means 32 bit

        .section .text          # CODE SECTION
        .global _start

_start:
        ## rip-relative addressing
        ## lea is used for the & operation

        lea num1(%rip),%rbx     # LOAD EFFECTIVE ADDRESS: Get &num1 into %rbx
                                # lea = Load Effective Address
                                # This loads the ADDRESS of num1, not its value!
                                # num1(%rip) = RIP-relative addressing
                                # &num1 -> %rbx
                                #
                                # WHAT IS RIP?
                                # %rip = Instruction Pointer (Program Counter)
                                # Points to the currently executing instruction
                                #
                                # RIP-RELATIVE ADDRESSING:
                                # num1(%rip) means "address of num1 relative to %rip"
                                # The CPU calculates: %rip + offset
                                # This makes code position-independent
                                #
                                # WHY RIP-RELATIVE?
                                # - ASLR (Address Space Layout Randomization)
                                # - Shared libraries
                                # - Code can load at any address
                                # - Offset from code to data is constant
                                #
                                # COMPARISON:
                                # lea num1(%rip),%rbx  - gets ADDRESS of num1
                                # mov num1,%rbx        - gets VALUE at num1 (200)
                                # mov $num1,%rbx       - gets absolute ADDRESS (not PIC)

        addl $10,(%rbx)         # ADD TO MEMORY VIA POINTER: Add 10 to *rbx
                                # (%rbx) means "dereference %rbx"
                                # Read the value at the address in %rbx
                                # Add 10 to it
                                # Write it back
                                # * (%rbx) = 10 + *(%rbx)
                                #
                                # WHAT HAPPENS:
                                # 1. %rbx contains address of num1
                                # 2. (%rbx) accesses the value at that address (200)
                                # 3. Add 10: 200 + 10 = 210
                                # 4. Write back to memory at (%rbx)
                                # 5. num1 now contains 210
                                #
                                # POINTER DEREFERENCING:
                                # In C: *ptr = 10 + *ptr;
                                # In asm: addl $10,(%rbx)
                                #
                                # PARENTHESES MEAN DEREFERENCE:
                                # %rbx      - the value in register rbx (an address)
                                # (%rbx)    - the value at the address in rbx
                                # $(%rbx)   - syntax error!

        mov num1,%rdi           # LOAD VALUE: Move num1's value to %rdi
                                # Reads the 32-bit value from num1
                                # num1 now contains 210, so %rdi = 210

        mov $60,%rax            # SETUP EXIT SYSCALL
        syscall                 # EXIT with status 210

# WHAT THIS PROGRAM DOES:
# 1. Defines num1 = 200
# 2. Loads address of num1 into %rbx (using lea)
# 3. Adds 10 to value at address %rbx (num1 becomes 210)
# 4. Loads num1 value into %rdi
# 5. Exits with status 210

# TO ASSEMBLE AND RUN:
# as data1.s -o data1.o
# ld data1.o -o data1
# ./data1
# echo $?  # Shows 210

# EXPECTED EXIT STATUS:
# 210 (200 + 10)

# LEA vs MOV:
#
# lea num1(%rip),%rbx  - Load ADDRESS of num1
#   Result: %rbx = memory address of num1
#   Like C: ptr = &num1;
#
# mov num1,%rbx        - Load VALUE from num1
#   Result: %rbx = 200 (the value stored at num1)
#   Like C: x = num1;
#
# mov $num1,%rbx       - Load absolute address (not PIC!)
#   Result: %rbx = fixed address of num1
#   Not position-independent!

# MEMORY ACCESS MODES:
#
# Direct:
#   mov num1,%rax       # Load value from num1
#   mov %rax,num1       # Store value to num1
#
# Indirect (via register):
#   mov (%rbx),%rax     # Load from address in %rbx
#   mov %rax,(%rbx)     # Store to address in %rbx
#
# Offset:
#   mov 8(%rbx),%rax    # Load from %rbx + 8
#   mov %rax,-4(%rbx)   # Store to %rbx - 4
#
# Indexed:
#   mov (%rbx,%rcx,4),%rax  # Load from %rbx + %rcx*4
#   Like C: x = array[index]

# RIP-RELATIVE ADDRESSING EXPLAINED:
#
# When this code is loaded at address 0x400000:
#   _start at 0x400100
#   num1 at 0x601000
#   Offset: 0x601000 - 0x400100 = 0x200F00
#
# The instruction stores the offset (0x200F00), not absolute address
# At runtime, CPU computes: current %rip + offset
#
# If ASLR loads code at 0x500000 instead:
#   _start at 0x500100
#   num1 at 0x701000
#   Offset still: 0x200F00
#   Calculation: 0x500100 + 0x200F00 = 0x701000 ✓
#
# The offset is constant; absolute addresses change!

# COMPARISON TO C:
# This program is equivalent to:
#   int num1 = 200;
#   int main() {
#       int *ptr = &num1;   // lea num1(%rip),%rbx
#       *ptr = *ptr + 10;   // addl $10,(%rbx)
#       return num1;        // mov num1,%rdi
#   }

# WHY USE LEA AND INDIRECT ACCESS?
#
# Direct access (data0.s):
#   addl $10,num1       # Simple and clear
#
# Indirect access (data1.s):
#   lea num1(%rip),%rbx
#   addl $10,(%rbx)     # More complex, why bother?
#
# Reasons for indirect access:
# 1. Pointers to variables (passing by reference)
# 2. Arrays and pointer arithmetic
# 3. Dynamic data structures
# 4. Position-independent code
# 5. Accessing different variables with same code

# THE LEA INSTRUCTION:
#
# lea source, dest
#   - Loads ADDRESS, not value
#   - Does not access memory (unlike mov)
#   - Can do arithmetic: lea 8(%rbx,%rcx,4),%rax
#   - Commonly used for:
#     * Getting addresses: lea var(%rip),%rax
#     * Arithmetic: lea (%rax,%rax,2),%rax  # %rax = %rax * 3
#     * Array indexing: lea array(%rip,%rdi,4),%rax

# DEREFERENCING SYNTAX:
#
# (%reg)        - Value at address in register
#   mov (%rbx),%rax     # %rax = *rbx
#
# offset(%reg)  - Value at (address + offset)
#   mov 8(%rbx),%rax    # %rax = *(rbx + 8)
#
# (%base,%index,scale) - Complex addressing
#   mov (%rbx,%rcx,4),%rax  # %rax = *(rbx + rcx*4)
#
# Examples:
#   addl $10,(%rbx)         # *rbx += 10
#   mov 16(%rsp),%rax       # %rax = *(rsp + 16)
#   mov (%rdi,%rsi,8),%rax  # %rax = *(rdi + rsi*8)
