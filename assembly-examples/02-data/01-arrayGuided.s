# ============================================================================
# Array Operations - Guided Version
# ============================================================================
# This program demonstrates fundamental array operations in assembly:
# - Declaring an array in the data section
# - Iterating over array elements with a loop
# - Reading from and writing to array elements
# - Using scaled indexed addressing mode
#
# WHAT IT DOES:
# 1. Doubles each element of myArray: [1,2,3,4,5] → [2,4,6,8,10]
# 2. Sums all doubled elements: 2+4+6+8+10 = 30
# 3. Exits with sum as status code (30)
# ============================================================================

.section .data

myArray:
    # .quad declares 64-bit (8-byte) integers
    # Creates array with 5 elements: [1, 2, 3, 4, 5]
    .quad 1, 2, 3, 4, 5

.section .text
.global _start

_start:
    # ========================================================================
    # LOOP 1: Double each array element
    # Equivalent C: for(i=0; i<5; i++) myArray[i] *= 2;
    # ========================================================================

    mov $0, %rcx            # %rcx = loop counter (i = 0)

.dubLoop:
    # Load array element: myArray[i]
    # Syntax: myArray(,index,scale) means: myArray + (index * scale)
    # Scale = 8 because each .quad is 8 bytes
    mov myArray(,%rcx,8), %rax   # %rax = myArray[i]

    imul $2, %rax                # %rax *= 2 (double the value)

    # Store back to array
    mov %rax, myArray(,%rcx,8)   # myArray[i] = %rax

    inc %rcx                     # i++
    cmp $5, %rcx                 # Compare i with 5
    jl .dubLoop                  # if (i < 5) goto .dubLoop

    # ========================================================================
    # LOOP 2: Sum all array elements
    # Equivalent C: sum=0; for(i=0; i<5; i++) sum += myArray[i];
    # ========================================================================

    mov $0, %rcx            # Reset loop counter (i = 0)
    mov $0, %rax            # %rax = sum accumulator (sum = 0)

.sumLoop:
    add myArray(,%rcx,8), %rax   # sum += myArray[i]
    inc %rcx                     # i++
    cmp $5, %rcx                 # Compare i with 5
    jl .sumLoop                  # if (i < 5) goto .sumLoop

    # ========================================================================
    # Exit with sum as status code
    # ========================================================================

    mov %rax, %rdi          # exit status = sum (should be 30)
    mov $60, %rax           # syscall: exit
    syscall

# ============================================================================
# KEY CONCEPTS:
#
# 1. ARRAY INDEXING: myArray(,index,scale)
#    - Base address: myArray
#    - Index register: %rcx
#    - Scale factor: 8 (size of each element in bytes)
#    - Effective address = myArray + (index * 8)
#
# 2. LOOP PATTERN:
#    - Initialize counter: mov $0, %rcx
#    - Loop body: do work with myArray(,%rcx,8)
#    - Increment: inc %rcx
#    - Test: cmp $5, %rcx
#    - Branch: jl .loopLabel
#
# 3. MEMORY vs REGISTERS:
#    - Read from array to register: mov myArray(,%rcx,8), %rax
#    - Operate on value in register: imul $2, %rax
#    - Write from register to array: mov %rax, myArray(,%rcx,8)
#
# ============================================================================
# TO BUILD AND RUN:
#
# as 01-arrayGuided.s -o array.o
# ld array.o -o array
# ./array
# echo $?                  # Should print: 30
#
# CALCULATION:
# Initial: [1, 2, 3, 4, 5]
# After doubling: [2, 4, 6, 8, 10]
# Sum: 2 + 4 + 6 + 8 + 10 = 30
# ============================================================================
