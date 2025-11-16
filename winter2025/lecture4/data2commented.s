# PEDAGOGICAL PURPOSE:
# This program demonstrates array processing using a for loop in assembly.
# It introduces fundamental concepts of arrays, indexed memory access, loops,
# and accumulation patterns.
#
# Key learning objectives:
# 1. Understanding how arrays are stored in memory (contiguous elements)
# 2. Learning the lea instruction for loading addresses
# 3. Understanding indexed memory access with scale factor: (%base,%index,scale)
# 4. Implementing for loops with initialization, condition, body, and increment
# 5. Using labels for loop control (loopBody, done)
# 6. Understanding accumulator pattern (sum += arr[i])
# 7. Learning how array indexing translates to memory addressing
#
# CONCEPTUAL OVERVIEW:
# This program implements:
#   long arr1[] = {1, 2, 3, 4};
#   long sum = 0;
#   for (int i = 0; i < 4; i++) {
#       sum = sum + arr1[i];
#   }
#   return sum;  // Returns 10 (1+2+3+4)

        .section .data             # DATA SECTION: Initialized read-write data

arr1:   .long 1,2,3,4             # DEFINE ARRAY: Four consecutive 32-bit integers
                                  # BREAKDOWN:
                                  # arr1:    - Label marking start of array
                                  # .long    - Each element is 32-bit (4 bytes)
                                  # 1,2,3,4  - Initial values
                                  #
                                  # MEMORY LAYOUT:
                                  # Address:  [arr1+0] [arr1+4] [arr1+8] [arr1+12]
                                  # Element:  arr1[0]  arr1[1]  arr1[2]  arr1[3]
                                  # Value:    1        2        3        4
                                  #
                                  # WHY +4 BETWEEN ELEMENTS?
                                  # Each .long is 4 bytes, so elements are 4 bytes apart
                                  # This is the "stride" or "element size"
                                  #
                                  # TOTAL SIZE: 4 elements × 4 bytes = 16 bytes

        .section .text             # CODE SECTION: Executable instructions
        .global _start             # GLOBAL DIRECTIVE: Entry point visible to linker

        ## PROGRAM TASK:
        ## Write a for loop that adds up all the numbers in the array

_start:
        lea arr1(%rip),%rbx        # LOAD ARRAY ADDRESS: %rbx = &arr1[0]
                                  # lea = Load Effective Address
                                  # This loads the ADDRESS of the array, not the values
                                  # RIP-relative addressing for position-independent code
                                  #
                                  # WHAT'S IN %rbx NOW?
                                  # The memory address where the array starts
                                  # Think of this as: long *arr = arr1;
                                  #
                                  # WHY USE lea?
                                  # We need to access different elements using indexing
                                  # Having the base address lets us do: arr[i] = *(arr + i)

        mov $0,%rcx                # INITIALIZE LOOP COUNTER: int i = 0
                                  # %rcx will be our loop index variable
                                  # We'll use it to track which array element we're on
                                  # Convention: %rcx often used for counters (c for count)

        mov $0,%rax                # INITIALIZE ACCUMULATOR: int sum = 0
                                  # %rax will hold our running sum
                                  # We'll add each array element to this
                                  # Convention: %rax often used for results/return values

        # FOR LOOP STRUCTURE:
        # for (i = 0; i < 4; i++) {
        #     sum = sum + arr1[i];
        # }

loopBody:
        # LOOP CONDITION: Check if i < 4
        cmp $4,%rcx                # COMPARE: Check if %rcx - $4
                                  # This computes: i - 4
                                  # If i < 4, result is negative (i hasn't reached 4 yet)
                                  # If i >= 4, result is zero or positive (loop should end)
                                  #
                                  # FLAGS SET:
                                  # When i=0: 0-4=-4 (negative, SF=1, continue loop)
                                  # When i=3: 3-4=-1 (negative, SF=1, continue loop)
                                  # When i=4: 4-4=0  (zero, ZF=1, exit loop)
                                  # When i>4: positive (exit loop)

        jge done                   # CONDITIONAL EXIT: Jump to 'done' if i >= 4
                                  # jge = "jump if greater or equal" (signed comparison)
                                  # Checks if %rcx >= 4
                                  # If true, we've processed all elements, so exit loop
                                  # If false, continue to loop body
                                  #
                                  # ITERATION TRACE:
                                  # i=0: 0 >= 4? No, continue
                                  # i=1: 1 >= 4? No, continue
                                  # i=2: 2 >= 4? No, continue
                                  # i=3: 3 >= 4? No, continue
                                  # i=4: 4 >= 4? Yes, jump to done

        ## LOOP BODY: Accumulate array element
        add (%rbx,%rcx,4),%rax     # ARRAY ACCESS AND ADD: sum = sum + arr1[i]
                                  # THIS IS THE MOST IMPORTANT LINE!
                                  # Let's break down the addressing mode:
                                  #
                                  # (%rbx,%rcx,4) = INDEXED ADDRESSING
                                  # Syntax: (base,index,scale)
                                  # Computes: base + (index * scale)
                                  # = %rbx + (%rcx * 4)
                                  # = address_of_arr1 + (i * 4)
                                  # = &arr1[i]
                                  #
                                  # WHY SCALE = 4?
                                  # Each array element is 4 bytes (.long)
                                  # To get arr1[i], we need arr1 + i*sizeof(element)
                                  #
                                  # ITERATION TRACE:
                                  # i=0: (%rbx + 0*4) = arr1[0] = 1, sum = 0+1 = 1
                                  # i=1: (%rbx + 1*4) = arr1[1] = 2, sum = 1+2 = 3
                                  # i=2: (%rbx + 2*4) = arr1[2] = 3, sum = 3+3 = 6
                                  # i=3: (%rbx + 3*4) = arr1[3] = 4, sum = 6+4 = 10
                                  #
                                  # WHAT add DOES:
                                  # 1. Calculate address: %rbx + %rcx*4
                                  # 2. Read 32-bit value from that address
                                  # 3. Add it to %rax
                                  # 4. Store result back in %rax
                                  #
                                  # C EQUIVALENT:
                                  # sum += arr1[i];
                                  #
                                  # ALTERNATIVE NOTATION:
                                  # The comment shows: *(%rbx + 4 * %rcx)
                                  # This is the same thing in pointer notation

        add $1,%rcx                # INCREMENT COUNTER: i++
                                  # Add 1 to loop index
                                  # Move to next array element
                                  #
                                  # PROGRESSION:
                                  # First iteration:  i: 0 -> 1
                                  # Second iteration: i: 1 -> 2
                                  # Third iteration:  i: 2 -> 3
                                  # Fourth iteration: i: 3 -> 4
                                  # (Then loop exits because 4 >= 4)

        jmp loopBody               # LOOP BACK: Unconditional jump to loop start
                                  # This creates the loop!
                                  # Always jump back to check condition again
                                  # Loop continues until jge exits to 'done'

done:
        mov %rax,%rdi              # PREPARE RETURN VALUE: %rdi = sum (10)
                                  # Transfer sum to exit code register
                                  # After 4 iterations: %rax = 1+2+3+4 = 10
                                  # This will be our program's exit code

        mov $60,%rax               # SYSCALL NUMBER: 60 = exit
                                  # Prepare to terminate the program
                                  # Note: This overwrites our sum, but we saved it in %rdi

        syscall                    # INVOKE KERNEL: Execute exit(10)
                                  # Program exits with code 10 (the sum)
                                  # You can verify with: ./data2; echo $?

# INDEXED ADDRESSING MODE EXPLAINED:
#
# General form: displacement(base,index,scale)
# Calculates:   displacement + base + (index * scale)
#
# Examples used in this program:
#   (%rbx,%rcx,4)     = 0 + %rbx + (%rcx * 4)
#   arr1(%rip)        = address of arr1 + %rip
#
# Common addressing modes:
#   (%rax)            = value at address in %rax
#   8(%rax)           = value at address (%rax + 8)
#   (%rax,%rbx,1)     = value at address (%rax + %rbx * 1)
#   (%rax,%rbx,4)     = value at address (%rax + %rbx * 4)
#   8(%rax,%rbx,4)    = value at address (8 + %rax + %rbx * 4)
#
# Valid scale factors: 1, 2, 4, 8 (matches common data sizes)

# ARRAY INDEXING IN DIFFERENT LANGUAGES:
#
# C:           arr1[i]
# C pointer:   *(arr1 + i)
# Assembly:    (%rbx,%rcx,4)  where %rbx=arr1, %rcx=i, 4=sizeof(long)
#
# All three mean the same thing: access element i of the array

# FOR LOOP COMPONENTS:
#
# 1. INITIALIZATION (before loop):
#    mov $0,%rcx              # i = 0
#    mov $0,%rax              # sum = 0
#
# 2. CONDITION (at loop start):
#    loopBody:
#    cmp $4,%rcx              # check i < 4
#    jge done                 # exit if i >= 4
#
# 3. BODY (loop work):
#    add (%rbx,%rcx,4),%rax   # sum += arr1[i]
#
# 4. INCREMENT (end of body):
#    add $1,%rcx              # i++
#    jmp loopBody             # loop back

# LOOP EXECUTION TRACE:
#
# Initial: %rcx=0, %rax=0
#
# Iteration 1:
#   cmp $4,%rcx -> 0 < 4, continue
#   add (%rbx,0,4),%rax -> %rax = 0 + arr1[0] = 0 + 1 = 1
#   add $1,%rcx -> %rcx = 1
#
# Iteration 2:
#   cmp $4,%rcx -> 1 < 4, continue
#   add (%rbx,1,4),%rax -> %rax = 1 + arr1[1] = 1 + 2 = 3
#   add $1,%rcx -> %rcx = 2
#
# Iteration 3:
#   cmp $4,%rcx -> 2 < 4, continue
#   add (%rbx,2,4),%rax -> %rax = 3 + arr1[2] = 3 + 3 = 6
#   add $1,%rcx -> %rcx = 3
#
# Iteration 4:
#   cmp $4,%rcx -> 3 < 4, continue
#   add (%rbx,3,4),%rax -> %rax = 6 + arr1[3] = 6 + 4 = 10
#   add $1,%rcx -> %rcx = 4
#
# Loop exit:
#   cmp $4,%rcx -> 4 >= 4, jump to done
#
# Final: %rcx=4, %rax=10

# WHY THIS PATTERN IS IMPORTANT:
#
# The accumulator pattern (sum += value) is fundamental:
# - Calculating averages
# - Finding totals
# - Counting elements that match criteria
# - Many algorithms use variations of this pattern
#
# Indexed addressing is essential for:
# - Array processing
# - String manipulation
# - Data structure traversal
# - Matrix operations

# EQUIVALENT C CODE:
#
# long arr1[] = {1, 2, 3, 4};
#
# int main() {
#     int i = 0;
#     long sum = 0;
#
#     for (i = 0; i < 4; i++) {
#         sum = sum + arr1[i];
#     }
#
#     return sum;  // returns 10
# }

# TO ASSEMBLE, LINK, AND RUN:
# as data2.s -o data2.o
# ld data2.o -o data2
# ./data2
# echo $?                          # Should display 10

# EXPERIMENT:
# Try different array values:
#   arr1: .long 5,10,15,20
# The exit code should be 50 (5+10+15+20)
#
# Try different array size:
#   arr1: .long 1,2,3,4,5,6
#   Change cmp $4,%rcx to cmp $6,%rcx
# The exit code should be 21 (1+2+3+4+5+6)
