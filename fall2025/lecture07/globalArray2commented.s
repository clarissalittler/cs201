# PEDAGOGICAL PURPOSE:
# This program demonstrates indexed array access using scaled addressing modes.
# It shows how to access any array element using an index, not just the first one.
# Key learning objectives:
# 1. Understanding indexed addressing: base + (index * scale)
# 2. The scale factor corresponds to element size (8 bytes for .quad)
# 3. How assembly implements array indexing like arr[i]
# 4. The powerful addressing modes available in x86-64
# 5. Why the CPU has built-in multiplication for array access

.section .data               # DATA SECTION: Initialized global variables
arr:    .quad 1,2,3,4       # GLOBAL ARRAY: Four 64-bit integers
                            # Memory layout (each element is 8 bytes):
                            # arr[0]=1 at offset 0
                            # arr[1]=2 at offset 8
                            # arr[2]=3 at offset 16
                            # arr[3]=4 at offset 24

        .section .text      # CODE SECTION
        .global _start      # Make _start visible to linker

_start:                     # PROGRAM ENTRY POINT

        lea arr(%rip),%r9   # LOAD ARRAY ADDRESS: %r9 = address of arr
                            # %r9 now points to arr[0]
                            # C equivalent: long *r9 = arr;

        mov $1,%rcx         # LOAD INDEX: %rcx = 1
                            # This is the array index we want to access
                            # We're going to access arr[1], which has value 2
                            # WHY %rcx? Just a choice - any register could work
                            # C equivalent: int i = 1;

        mov (%r9,%rcx,8), %rdi  # INDEXED ACCESS: %rdi = arr[1]
                                # THIS IS THE KEY INSTRUCTION!
                                #
                                # SCALED INDEXED ADDRESSING MODE:
                                # Syntax: (base, index, scale)
                                # Calculates: base + (index * scale)
                                #
                                # BREAKING IT DOWN:
                                # - base = %r9 (address of arr[0])
                                # - index = %rcx (value 1)
                                # - scale = 8 (bytes per element)
                                # - Address = %r9 + (%rcx * 8)
                                # - Address = arr + (1 * 8) = arr + 8
                                # - This is the address of arr[1]
                                # - Load the 64-bit value there: 2
                                #
                                # WHY SCALE = 8?
                                # - Each array element is a .quad (8 bytes)
                                # - arr[0] is at offset 0
                                # - arr[1] is at offset 8 (1 * 8)
                                # - arr[2] is at offset 16 (2 * 8)
                                # - arr[3] is at offset 24 (3 * 8)
                                #
                                # VALID SCALE VALUES: 1, 2, 4, or 8
                                # - 1 for byte arrays
                                # - 2 for short (16-bit) arrays
                                # - 4 for int (32-bit) arrays
                                # - 8 for long (64-bit) arrays
                                #
                                # C EQUIVALENT:
                                # rdi = r9[rcx];
                                # // or: rdi = *(r9 + rcx);
                                # // or: rdi = arr[1];

        # *(%r9 + 1*8)      # Comment showing address calculation
                            # This is (%r9 + 8), which points to arr[1]

        # In C you'd just say *(%r9 + 1)
                            # C does the *8 multiplication automatically
                            # because it knows the element type is 'long' (8 bytes)
                            # Assembly makes it explicit with the scale factor

        mov $60, %rax       # SYSCALL NUMBER: 60 = exit

        syscall             # EXIT: Terminate with status 2
                            # Exit code is 2 (arr[1])
                            # Verify with: ./globalArray2; echo $?

# ADDRESSING MODE DEEP DIVE:
#
# The instruction: mov (%r9,%rcx,8), %rdi
# Uses the "base + (index * scale)" addressing mode
#
# General form: displacement(base, index, scale)
#   - displacement: constant offset (0 in our case, so omitted)
#   - base: base register (%r9)
#   - index: index register (%rcx)
#   - scale: 1, 2, 4, or 8
#
# Effective address = base + (index * scale) + displacement
#
# Examples:
#   (%r9,%rcx,8)          # base + index*8
#   16(%r9,%rcx,8)        # base + index*8 + 16
#   (%r9,%rcx,1)          # base + index*1 (for byte arrays)
#   (%r9,%rcx,4)          # base + index*4 (for int arrays)

# ALL WAYS TO ACCESS arr[1]:
#
# Method 1 (indexed - what we use):
#   lea arr(%rip),%r9       # r9 = address of arr
#   mov $1,%rcx             # rcx = index 1
#   mov (%r9,%rcx,8),%rdi   # rdi = arr[1]
#
# Method 2 (direct offset):
#   lea arr(%rip),%r9       # r9 = address of arr
#   mov 8(%r9),%rdi         # rdi = arr[1] (offset 8 bytes)
#
# Method 3 (RIP-relative with offset):
#   mov arr+8(%rip),%rdi    # rdi = arr[1] directly
#
# Method 1 is most useful when the index is variable (e.g., in a loop)
# Method 2 works when you know the offset at compile time
# Method 3 is simplest for constant access

# WHY HARDWARE SUPPORT FOR SCALING?
#
# Array access is SO common that CPUs have built-in support:
# - One instruction computes: base + (index * scale) + offset
# - No need for separate multiply instruction
# - Makes array access very efficient
#
# Without scaling, you'd need:
#   mov %rcx,%rax           # Copy index
#   imul $8,%rax            # Multiply by element size
#   add %r9,%rax            # Add base address
#   mov (%rax),%rdi         # Load from computed address
#
# With scaling:
#   mov (%r9,%rcx,8),%rdi   # One instruction does it all!

# SCALE FACTOR TABLE:
#
# Data Type         Size    .directive    Scale Factor
# ────────────────────────────────────────────────────
# char/byte         1 byte  .byte         1
# short             2 bytes .word         2
# int               4 bytes .long/.int    4
# long/pointer      8 bytes .quad         8
# float             4 bytes .float        4
# double            8 bytes .double       8

# COMPARISON: FIXED VS VARIABLE INDEX
#
# Fixed index (compile-time constant):
#   # Access arr[1] - we know it's element 1
#   mov 8(%r9),%rdi         # Direct offset: element 1 is at +8
#
# Variable index (runtime value):
#   # Access arr[i] - i is in %rcx
#   mov (%r9,%rcx,8),%rdi   # Computed offset: element i is at +(i*8)
#
# Variable indexing is essential for:
# - Loops that iterate through arrays
# - Functions that take an index as parameter
# - Any case where the index isn't known until runtime

# ACCESSING DIFFERENT ELEMENTS:
#
# Given: arr[4] = {1, 2, 3, 4} at address %r9
#
# arr[0]:  mov (%r9), %rdi            # or: mov $0,%rcx; mov (%r9,%rcx,8),%rdi
# arr[1]:  mov 8(%r9), %rdi           # or: mov $1,%rcx; mov (%r9,%rcx,8),%rdi
# arr[2]:  mov 16(%r9), %rdi          # or: mov $2,%rcx; mov (%r9,%rcx,8),%rdi
# arr[3]:  mov 24(%r9), %rdi          # or: mov $3,%rcx; mov (%r9,%rcx,8),%rdi
#
# The indexed form (%r9,%rcx,8) works for any index value

# C ARRAY INDEXING VS ASSEMBLY:
#
# C code:
#   long arr[4] = {1, 2, 3, 4};
#   int i = 1;
#   long value = arr[i];    # Gets arr[1] = 2
#
# What C does behind the scenes:
#   1. Calculate address: &arr[i] = arr + (i * sizeof(long))
#   2. Dereference to get value
#
# Assembly equivalent (this program):
#   arr: .quad 1,2,3,4      # Declare array
#   lea arr(%rip),%r9       # r9 = arr
#   mov $1,%rcx             # rcx = i (index)
#   mov (%r9,%rcx,8),%rdi   # rdi = arr[i]
#
# The scaling by 8 is automatic in C (based on type)
# but explicit in assembly (we specify scale=8)

# TO ASSEMBLE, LINK, AND RUN:
# as globalArray2.s -o globalArray2.o
# ld globalArray2.o -o globalArray2
# ./globalArray2
# echo $?                  # Should show 2 (arr[1])
