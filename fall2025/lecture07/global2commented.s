# PEDAGOGICAL PURPOSE:
# This program demonstrates pointer operations in assembly: getting the address
# of a variable and dereferencing it to access/modify the value.
# It shows the distinction between a memory address and the value at that address.
# Key learning objectives:
# 1. Understanding lea (load effective address) vs mov
# 2. Pointer operations: getting an address (&) and dereferencing (*)
# 3. Indirect addressing using registers
# 4. The relationship between C pointers and assembly
# 5. Why we sometimes use pointers instead of direct memory access

.section .data               # DATA SECTION: Initialized global variables
num:    .quad 10            # GLOBAL VARIABLE: 64-bit integer initialized to 10
                            # 'num' is the label (address) where value 10 is stored

        .section .text      # CODE SECTION
        .global _start      # Make _start visible to linker

_start:                     # PROGRAM ENTRY POINT

        lea num(%rip),%r9   # LOAD EFFECTIVE ADDRESS: %r9 = address of num
                            # LEA EXPLAINED:
                            # - lea = Load Effective Address
                            # - Does NOT access memory - just computes an address
                            # - num(%rip) computes the address of num
                            # - Stores that ADDRESS in %r9 (not the value!)
                            #
                            # POINTER ANALOGY IN C:
                            # long num = 10;
                            # long *r9 = &num;  // r9 is a pointer to num
                            #
                            # After this instruction:
                            # - %r9 contains the memory address where num is stored
                            # - %r9 is now a POINTER to num
                            # - We haven't read the value 10 yet

        # %r9 = &num         # C-style comment showing pointer notation
                            # %r9 holds the ADDRESS (not the value)

        # what's the equivalent of *
                            # How do we DEREFERENCE the pointer?
                            # Answer: Use parentheses syntax: (%r9)

        addq $10,(%r9)      # ADD THROUGH POINTER: *r9 += 10
                            # INDIRECT ADDRESSING:
                            # - (%r9) means: "the memory location pointed to by %r9"
                            # - This is DEREFERENCING the pointer
                            # - Read the value at address %r9, add 10, write back
                            #
                            # STEP BY STEP:
                            # 1. Read %r9 to get the address (let's say 0x404000)
                            # 2. Read the 64-bit value at address 0x404000 (currently 10)
                            # 3. Add 10 to it (result: 20)
                            # 4. Write 20 back to address 0x404000
                            #
                            # C EQUIVALENT:
                            # *r9 = *r9 + 10;
                            # // or simply: *r9 += 10;
                            #
                            # After this: the value at num's address is now 20

        # *(%r9) = *(%r9) + 10  # C-style comment showing the operation
                                # Dereference, add, store back

        movq (%r9), %rdi    # LOAD THROUGH POINTER: %rdi = *r9
                            # Again using indirect addressing
                            # (%r9) dereferences the pointer
                            # We read the VALUE at the address stored in %r9
                            # That value is now 20 (after our addition)
                            # %rdi now contains 20
                            #
                            # C EQUIVALENT:
                            # rdi = *r9;

        movq $60, %rax      # SYSCALL NUMBER: 60 = exit

        syscall             # EXIT: Terminate with status 20
                            # Exit code is 20 (the modified value)
                            # Verify with: ./global2; echo $?

# UNDERSTANDING LEA VS MOV:
#
# lea num(%rip), %r9    # Computes and loads the ADDRESS
#                       # %r9 = address of num
#                       # Does NOT access memory - just arithmetic
#
# mov num(%rip), %r9    # Would load the VALUE at that address
#                       # %r9 = 10 (the value stored at num)
#                       # This DOES access memory
#
# Analogy in C:
#   long *ptr = &num;   // lea: ptr gets the address
#   long val = num;     // mov: val gets the value

# POINTER NOTATION SUMMARY:
#
# Assembly          C Equivalent       Meaning
# ─────────────────────────────────────────────────
# lea num(%rip),%r9  long *r9 = &num   Get address of num
# (%r9)              *r9                Dereference r9
# movq (%r9),%rdi    rdi = *r9         Load value through pointer
# addq $10,(%r9)     *r9 += 10         Add to value through pointer

# WHY USE POINTERS?
#
# Compare with global1.s:
#   global1.s: addq $10,num(%rip)     # Direct access
#   global2.s: lea num(%rip),%r9      # Get pointer
#              addq $10,(%r9)          # Indirect access
#
# Why would we ever use the pointer approach?
#
# 1. FUNCTION PARAMETERS: Can pass addresses to functions
#    - Example: adder(long *ptr) { *ptr += 10; }
#
# 2. POINTER ARITHMETIC: Can calculate array addresses
#    - Example: ptr = &array[0]; *(ptr + i) accesses array[i]
#
# 3. MULTIPLE ACCESSES: Save the address once, use many times
#    - lea arr(%rip),%r9
#    - mov (%r9),%rax      # array[0]
#    - mov 8(%r9),%rbx     # array[1]
#    - mov 16(%r9),%rcx    # array[2]
#
# 4. DYNAMIC/COMPUTED ADDRESSES: Address not known at compile time
#    - Example: based on user input or runtime calculations

# MEMORY ADDRESSING MODES:
#
# Direct (RIP-relative):
#   movq num(%rip), %rdi      # Address computed at compile time + runtime offset
#
# Indirect (register):
#   movq (%r9), %rdi          # Address is in %r9
#
# Indirect with offset:
#   movq 8(%r9), %rdi         # Address is %r9 + 8
#
# Indexed with scale:
#   movq (%r9,%rcx,8), %rdi   # Address is %r9 + %rcx * 8

# COMPARISON: DIRECT VS INDIRECT ACCESS:
#
# GLOBAL1.S (Direct):
#   addq $10,num(%rip)        # CPU computes: %rip + offset_to_num
#                             # One memory access to modify num
#
# GLOBAL2.S (Indirect):
#   lea num(%rip),%r9         # CPU computes: %rip + offset_to_num, stores in %r9
#   addq $10,(%r9)            # CPU reads %r9, accesses memory at that address
#                             # Still one memory access to modify num
#                             # But now we have the address saved in %r9 for reuse

# TO ASSEMBLE, LINK, AND RUN:
# as global2.s -o global2.o
# ld global2.o -o global2
# ./global2
# echo $?                  # Should show 20
