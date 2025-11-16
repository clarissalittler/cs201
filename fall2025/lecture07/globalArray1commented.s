# PEDAGOGICAL PURPOSE:
# This program demonstrates declaring and accessing arrays in assembly.
# It introduces the concept of arrays as contiguous memory and pointer arithmetic.
# Key learning objectives:
# 1. How to declare arrays in the .data section
# 2. Understanding that array names are pointers to the first element
# 3. Accessing the first element of an array (array[0])
# 4. Memory layout of arrays: elements stored sequentially
# 5. The relationship between arrays and pointers in assembly

.section .data               # DATA SECTION: Initialized global variables
arr:    .quad 1,2,3,4       # GLOBAL ARRAY: Four 64-bit integers
                            # MEMORY LAYOUT:
                            # Address: arr+0   arr+8   arr+16  arr+24
                            # Value:   1       2       3       4
                            #
                            # .quad 1,2,3,4 means:
                            # - Allocate 4 quadwords (4 * 8 bytes = 32 bytes total)
                            # - Initialize them to 1, 2, 3, 4 respectively
                            # - 'arr' is the address of the first element
                            #
                            # WHY 8-byte spacing? Each .quad is 64 bits = 8 bytes
                            # So arr[0] is at arr+0, arr[1] is at arr+8, etc.
                            #
                            # C EQUIVALENT:
                            # long arr[4] = {1, 2, 3, 4};

        .section .text      # CODE SECTION
        .global _start      # Make _start visible to linker

_start:                     # PROGRAM ENTRY POINT

        lea arr(%rip),%r9   # LOAD ARRAY ADDRESS: %r9 = address of array
                            # lea computes the address of 'arr'
                            # %r9 now points to the first element
                            #
                            # IMPORTANT CONCEPT:
                            # In assembly (and C), an array name represents
                            # the ADDRESS of the first element
                            # So 'arr' and '&arr[0]' mean the same thing
                            #
                            # C EQUIVALENT:
                            # long *r9 = arr;
                            # // or: long *r9 = &arr[0];

        mov (%r9), %rdi     # LOAD FIRST ELEMENT: %rdi = *r9 = arr[0]
                            # (%r9) dereferences the pointer
                            # This reads the 64-bit value at address %r9
                            # That's the first element: 1
                            # %rdi now contains 1
                            #
                            # BREAKING IT DOWN:
                            # - %r9 contains the address (e.g., 0x404000)
                            # - (%r9) accesses the memory at that address
                            # - We read the 8-byte value there: 1
                            #
                            # C EQUIVALENT:
                            # rdi = *r9;
                            # // or: rdi = arr[0];

        # *arr -> 1         # Comment showing that *arr gives us 1
                            # In C: *arr is the same as arr[0]

        mov $60, %rax       # SYSCALL NUMBER: 60 = exit

        syscall             # EXIT: Terminate with status 1
                            # Exit code is 1 (first element of array)
                            # Verify with: ./globalArray1; echo $?

# ARRAY MEMORY LAYOUT IN DETAIL:
#
# When arr is defined with: .quad 1,2,3,4
#
# Memory (assuming arr starts at address 0x404000):
#
# Address       Value (hex)        Value (decimal)    Array notation
# ───────────────────────────────────────────────────────────────────
# 0x404000:     0x0000000000000001     1               arr[0]
# 0x404008:     0x0000000000000002     2               arr[1]
# 0x404010:     0x0000000000000003     3               arr[2]
# 0x404018:     0x0000000000000004     4               arr[3]
#
# Each element is 8 bytes (64 bits) because we used .quad
# Elements are stored sequentially (contiguously) in memory

# UNDERSTANDING ARRAY ACCESS:
#
# arr               # The label, represents the address of arr[0]
# arr(%rip)         # RIP-relative address of arr[0]
# lea arr(%rip),%r9 # Load that address into %r9
# (%r9)             # Dereference: access the value at address %r9
#                   # This gives us arr[0]

# DIFFERENT WAYS TO ACCESS arr[0]:
#
# Method 1 (used in this program):
#   lea arr(%rip),%r9    # Get pointer to array
#   mov (%r9),%rdi       # Dereference to get arr[0]
#
# Method 2 (direct access):
#   mov arr(%rip),%rdi   # Load arr[0] directly
#
# Both produce the same result, but Method 1 is useful when:
# - You need the array address for further operations
# - You want to iterate through the array
# - You need to pass the array to a function

# ARRAY VS SINGLE VARIABLE:
#
# Single variable:
#   num: .quad 10           # One 64-bit value at address 'num'
#
# Array:
#   arr: .quad 1,2,3,4      # Four 64-bit values starting at address 'arr'
#
# Accessing single variable:
#   mov num(%rip),%rdi      # Load the value 10
#
# Accessing array element:
#   lea arr(%rip),%r9       # Get array address
#   mov (%r9),%rdi          # Load arr[0] = 1
#   mov 8(%r9),%rdi         # Load arr[1] = 2
#   mov 16(%r9),%rdi        # Load arr[2] = 3
#   mov 24(%r9),%rdi        # Load arr[3] = 4

# WHY ARRAYS MATTER:
#
# Arrays allow us to:
# 1. Store multiple related values efficiently
# 2. Access elements by index (position)
# 3. Iterate through data in loops
# 4. Pass multiple values with a single pointer
#
# Without arrays, we'd need separate variables:
#   val0: .quad 1
#   val1: .quad 2
#   val2: .quad 3
#   val3: .quad 4
#
# Arrays give us contiguous memory and predictable addressing

# POINTER ARITHMETIC PREVIEW:
#
# Since elements are 8 bytes apart:
#   (%r9)       = arr[0]  (offset 0)
#   8(%r9)      = arr[1]  (offset 8)
#   16(%r9)     = arr[2]  (offset 16)
#   24(%r9)     = arr[3]  (offset 24)
#
# General formula for arr[i] where each element is 8 bytes:
#   address = arr + (i * 8)
#
# We'll see indexed addressing in globalArray2.s

# COMPARISON WITH C:
#
# C code:
#   long arr[4] = {1, 2, 3, 4};
#   return arr[0];  // Returns 1
#
# Assembly equivalent (this program):
#   arr: .quad 1,2,3,4
#   lea arr(%rip),%r9
#   mov (%r9),%rdi
#   mov $60,%rax
#   syscall

# TO ASSEMBLE, LINK, AND RUN:
# as globalArray1.s -o globalArray1.o
# ld globalArray1.o -o globalArray1
# ./globalArray1
# echo $?                  # Should show 1 (first element)
