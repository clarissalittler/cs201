# PEDAGOGICAL PURPOSE:
# This program demonstrates an alternative approach to array indexing using the lea
# instruction for address calculation. It reinforces array concepts while introducing
# the important distinction between lea (load address) and mov (load value).
#
# Key learning objectives:
# 1. Understanding lea (Load Effective Address) and what it does
# 2. Learning the difference between calculating an address vs dereferencing
# 3. Seeing an alternative way to access array elements using two steps
# 4. Understanding when lea does NOT dereference memory
# 5. Comparing direct indexed access vs address-then-dereference
# 6. Reinforcing array indexing and loop patterns
# 7. Understanding why one approach might be clearer or more verbose
#
# CONCEPTUAL OVERVIEW:
# This program does the same thing as data2.s but uses a different technique:
#   long arr1[] = {1, 2, 3, 4};
#   long sum = 0;
#   for (int i = 0; i < 4; i++) {
#       long *ptr = &arr1[i];    // lea calculates this address
#       sum = sum + *ptr;         // add dereferences it
#   }
#   return sum;  // Returns 10 (1+2+3+4)

        .section .data             # DATA SECTION: Initialized read-write data

arr1:   .long 1,2,3,4             # DEFINE ARRAY: Four consecutive 32-bit integers
                                  # Same as data2.s
                                  # Memory layout:
                                  # [arr1+0]=1, [arr1+4]=2, [arr1+8]=3, [arr1+12]=4

        .section .text             # CODE SECTION: Executable instructions
        .global _start             # GLOBAL DIRECTIVE: Entry point visible to linker

        ## PROGRAM TASK (same as data2.s):
        ## Write a for loop that adds up all the numbers in the array

_start:
        lea arr1(%rip),%rbx        # LOAD ARRAY ADDRESS: %rbx = &arr1[0]
                                  # Same initialization as data2.s
                                  # %rbx holds the base address of the array
                                  # RIP-relative addressing (%rip) -> arr1 + i (pointer arithmetic)

        mov $0,%rcx                # INITIALIZE LOOP COUNTER: int i = 0
                                  # %rcx is our loop index

        mov $0,%rax                # INITIALIZE ACCUMULATOR: int sum = 0
                                  # %rax will hold our running total

loopBody:
        # LOOP CONDITION: Check if i < 4
        cmp $4,%rcx                # COMPARE: Check %rcx - 4
                                  # Same condition as data2.s
                                  # If i >= 4, time to exit the loop

        jge done                   # CONDITIONAL EXIT: Jump to 'done' if i >= 4
                                  # Exit when we've processed all 4 elements

        ## ARRAY ACCESS - TWO STEP APPROACH:
        ## This is the KEY DIFFERENCE from data2.s!
        ##
        ## In data2.s we did:
        ##   add (%rbx,%rcx,4),%rax
        ## Which directly accesses and adds the array element in one step.
        ##
        ## Here we do it in TWO steps:
        ## Step 1: Calculate the address of arr1[i]
        ## Step 2: Dereference that address to get the value

        # STEP 1: CALCULATE ADDRESS
        lea (%rbx,%rcx,4),%rdx     # LOAD EFFECTIVE ADDRESS: %rdx = &arr1[i]
                                  # lea = Load Effective Address
                                  # CRITICAL: lea does NOT dereference!
                                  #
                                  # OPERATION:
                                  # Calculate: %rbx + (%rcx * 4)
                                  # This is: base_of_array + (index * element_size)
                                  # Result is the ADDRESS of arr1[i]
                                  #
                                  # WHAT'S IN %rdx?
                                  # Not the VALUE at arr1[i]
                                  # But the MEMORY ADDRESS where arr1[i] is stored
                                  #
                                  # ITERATION TRACE:
                                  # i=0: %rdx = %rbx + 0*4 = address of arr1[0]
                                  # i=1: %rdx = %rbx + 1*4 = address of arr1[1]
                                  # i=2: %rdx = %rbx + 2*4 = address of arr1[2]
                                  # i=3: %rdx = %rbx + 3*4 = address of arr1[3]
                                  #
                                  # C EQUIVALENT:
                                  # long *ptr = &arr1[i];
                                  # or equivalently:
                                  # long *ptr = arr1 + i;
                                  #
                                  # WHY "DOES NOT DEREFERENCE"?
                                  # lea computes an address but never reads from memory
                                  # It's pure address arithmetic, no memory access
                                  # This is different from mov!

        # STEP 2: DEREFERENCE AND ADD
        add (%rdx),%rax            # DEREFERENCE AND ADD: sum = sum + *ptr
                                  # NOW we dereference!
                                  # (%rdx) means "value at the address in %rdx"
                                  #
                                  # OPERATION:
                                  # 1. Read address from %rdx
                                  # 2. Read 32-bit value from that address
                                  # 3. Add that value to %rax
                                  # 4. Store result back in %rax
                                  #
                                  # ITERATION TRACE:
                                  # i=0: add (arr1[0]),%rax -> %rax = 0 + 1 = 1
                                  # i=1: add (arr1[1]),%rax -> %rax = 1 + 2 = 3
                                  # i=2: add (arr1[2]),%rax -> %rax = 3 + 3 = 6
                                  # i=3: add (arr1[3]),%rax -> %rax = 6 + 4 = 10
                                  #
                                  # C EQUIVALENT:
                                  # sum += *ptr;
                                  #
                                  # COMPARISON WITH data2.s:
                                  # data2.s: add (%rbx,%rcx,4),%rax  (one instruction)
                                  # data3.s: lea (%rbx,%rcx,4),%rdx  (two instructions)
                                  #          add (%rdx),%rax
                                  # Both achieve the same result!

        ## COMMENTED OUT ALTERNATIVE (from original):
        ## add (%rbx,%rcx,4),%rax
        ## This would be the data2.s approach
        ## It's commented out because we're demonstrating the two-step method

        ## MORE VERBOSE EXPLANATION (from original comments):
        ## %rbx holds arr1 (base address)
        ## lea(%rbx,%rcx,4) -> arr1 + i (pointer arithmetic)
        ## So %rdx holds arr1 + i (the address of element i)
        ## (%rdx) -> *(arr1 + i) (dereference to get the value)
        ## This is exactly what arr1[i] means in C!

        add $1,%rcx                # INCREMENT COUNTER: i++
                                  # Move to next array index

        jmp loopBody               # LOOP BACK: Jump to start of loop
                                  # Repeat until all elements processed

done:
        mov %rax,%rdi              # PREPARE RETURN VALUE: %rdi = sum (10)
                                  # Transfer result to exit code register

        mov $60,%rax               # SYSCALL NUMBER: 60 = exit
                                  # Prepare to terminate the program

        syscall                    # INVOKE KERNEL: Execute exit(10)
                                  # Program exits with code 10
                                  # Same result as data2.s!

# LEA vs MOV - CRITICAL DISTINCTION:
#
# lea (Load Effective Address):
#   - CALCULATES an address but does NOT access memory
#   - Takes an address expression and puts the ADDRESS in destination
#   - Example: lea (%rbx,%rcx,4),%rdx -> %rdx = %rbx + %rcx*4
#   - No memory read occurs!
#
# mov with address:
#   - ACCESSES memory and loads the VALUE
#   - Takes an address expression and puts the VALUE at that address in destination
#   - Example: mov (%rbx,%rcx,4),%rdx -> %rdx = *(%rbx + %rcx*4)
#   - Memory read occurs!
#
# WHEN TO USE lea:
#   - When you need to calculate an address for later use
#   - When you want to do arithmetic without affecting flags
#   - When you need a pointer for multiple operations
#   - For certain arithmetic tricks (like multiply by 3, 5, 9)
#
# WHEN TO USE mov:
#   - When you want the actual value from memory
#   - When you need to load data for computation
#   - Most common case for memory access

# TWO APPROACHES COMPARED:
#
# DIRECT APPROACH (data2.s):
#   add (%rbx,%rcx,4),%rax
#   - One instruction
#   - More compact
#   - Directly adds the array element
#
# TWO-STEP APPROACH (data3.s):
#   lea (%rbx,%rcx,4),%rdx
#   add (%rdx),%rax
#   - Two instructions
#   - More explicit
#   - Shows the address calculation separately
#   - Useful when you need the address for multiple operations
#
# WHY USE TWO-STEP?
#   - Pedagogical clarity (shows address vs value explicitly)
#   - Reusing the address (if accessing same element multiple times)
#   - Code clarity in complex addressing scenarios
#
# WHY USE DIRECT?
#   - More efficient (fewer instructions)
#   - More compact code
#   - Common pattern in optimized code

# PRACTICAL EXAMPLE WHERE TWO-STEP IS USEFUL:
#
# If we wanted to both read and write the same element:
#   lea (%rbx,%rcx,4),%rdx   # Get address once
#   mov (%rdx),%rax          # Read current value
#   add $10,%rax             # Modify it
#   mov %rax,(%rdx)          # Write back
#
# Versus doing it directly:
#   mov (%rbx,%rcx,4),%rax   # Read
#   add $10,%rax             # Modify
#   mov %rax,(%rbx,%rcx,4)   # Write (recalculates address!)

# ADDRESS CALCULATION EXAMPLES:
#
# For arr1[i] where each element is 4 bytes:
#
# Mathematical:    base_address + (i * 4)
# C pointer:       *(arr1 + i)
# lea syntax:      lea (arr1,%i_reg,4),%dest
# Result:          Memory address, not value!
#
# To get the value:
#   mov (address_reg),%dest
#   or directly:
#   mov (arr1,%i_reg,4),%dest

# EQUIVALENT C CODE:
#
# Approach 1 (data2.s - direct):
# for (i = 0; i < 4; i++) {
#     sum += arr1[i];
# }
#
# Approach 2 (data3.s - two-step):
# for (i = 0; i < 4; i++) {
#     long *ptr = &arr1[i];   // lea step
#     sum += *ptr;             // add step
# }
#
# Both produce identical results!

# TO ASSEMBLE, LINK, AND RUN:
# as data3.s -o data3.o
# ld data3.o -o data3
# ./data3
# echo $?                          # Should display 10 (same as data2.s)

# EXPERIMENT:
# Compare the size and performance:
#   objdump -d data2.o > data2.txt
#   objdump -d data3.o > data3.txt
#   diff data2.txt data3.txt
# You'll see data3.s has one extra instruction in the loop!
