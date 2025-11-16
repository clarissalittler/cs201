# PEDAGOGICAL PURPOSE:
# This program demonstrates array access and looping in assembly language.
# It computes the sum of an array using a for loop.
# Key learning objectives:
# 1. Defining arrays in .data section
# 2. Looping with labels and jumps
# 3. Array indexing with scaled addressing (%rbx,%rcx,4)
# 4. Loop counters and accumulators
# 5. Conditional jumps for loop termination (jge)
# 6. How C for-loops translate to assembly

        .section .data          # DATA SECTION
arr1:   .long 1,2,3,4           # DEFINE ARRAY: Four 32-bit integers
                                # arr1[0] = 1, arr1[1] = 2, arr1[2] = 3, arr1[3] = 4
                                # Stored contiguously in memory:
                                # Address:  arr1+0  arr1+4  arr1+8  arr1+12
                                # Value:    1       2       3       4
                                # Total size: 4 elements × 4 bytes = 16 bytes

        .section .text          # CODE SECTION
        .global _start

        ## write a for loop that adds up all the
        ## numbers in the array

_start:
        lea arr1(%rip),%rbx     # LOAD ARRAY ADDRESS: %rbx = &arr1
                                # lea = load effective address
                                # %rbx now points to the first element
                                # load the address of the array into %rbx (relative to %rip)
                                # This gives us the base address for array indexing

        mov $0,%rcx             # INITIALIZE LOOP COUNTER: i = 0
                                # %rcx will be our loop counter (array index)
                                # int i = 0

        mov $0,%rax             # INITIALIZE ACCUMULATOR: sum = 0
                                # %rax will hold the running sum
                                # int sum = 0

loopBody:                       # LOOP START LABEL: Beginning of loop body
                                # This is where each iteration begins

        cmp $4,%rcx             # COMPARE: Is i >= 4?
                                # Compare loop counter with array size
                                # Computes: %rcx - 4
                                # Sets flags based on result
                                # i < 4

        jge done                # EXIT LOOP IF: i >= 4
                                # jge = jump if greater or equal (signed)
                                # If %rcx >= 4, we've processed all elements
                                # Jump to 'done' to exit loop
                                # This implements: if (i >= 4) break;

        add (%rbx,%rcx,4),%rax  # ADD ARRAY ELEMENT: sum += arr1[i]
                                # (%rbx,%rcx,4) is scaled indexed addressing
                                # Means: memory at address (%rbx + %rcx * 4)
                                # %rbx = base address of array
                                # %rcx = index (0, 1, 2, 3)
                                # 4 = scale (sizeof(long) = 4 bytes)
                                # Address = %rbx + (4 * %rcx)
                                ## *(%rbx + 4 * %rcx)
                                ## arr1[rcx]
                                ## arr1[rcx] -> *(arr1 + rcx)
                                #
                                # ITERATION BY ITERATION:
                                # i=0: (%rbx + 0*4) = (%rbx + 0)  = arr1[0] = 1
                                # i=1: (%rbx + 1*4) = (%rbx + 4)  = arr1[1] = 2
                                # i=2: (%rbx + 2*4) = (%rbx + 8)  = arr1[2] = 3
                                # i=3: (%rbx + 3*4) = (%rbx + 12) = arr1[3] = 4

        add $1,%rcx             # INCREMENT COUNTER: i++
                                # Adds 1 to loop counter
                                # Next iteration will process next array element
                                #i++

        jmp loopBody            # REPEAT LOOP: Go back to start of loop
                                # Unconditional jump back to loopBody
                                # This creates the loop!

done:                           # LOOP EXIT LABEL: After loop finishes
                                # Reached when %rcx >= 4
                                # At this point: %rax = 1 + 2 + 3 + 4 = 10

        mov %rax,%rdi           # SET EXIT CODE: %rdi = sum (10)
                                # The accumulated sum becomes our exit code

        mov $60,%rax            # SETUP EXIT SYSCALL: %rax = 60
                                # (Note: this overwrites our sum, but we already copied it to %rdi)

        syscall                 # EXIT: Exit with status 10

# WHAT THIS PROGRAM DOES:
# 1. Defines array: arr1 = {1, 2, 3, 4}
# 2. Loads array base address into %rbx
# 3. Initializes i=0, sum=0
# 4. Loop:
#    - If i >= 4, exit loop
#    - sum += arr1[i]
#    - i++
#    - Repeat
# 5. Exits with status = sum = 10

# TO ASSEMBLE AND RUN:
# as data2.s -o data2.o
# ld data2.o -o data2
# ./data2
# echo $?  # Shows 10

# EXPECTED EXIT STATUS:
# 10 (1 + 2 + 3 + 4)

# LOOP EXECUTION TRACE:
#
# Initial: %rcx=0, %rax=0
#
# Iteration 1:
#   cmp $4,%rcx  → 0 < 4, don't jump
#   add (%rbx,%rcx,4),%rax  → %rax = 0 + arr1[0] = 0 + 1 = 1
#   add $1,%rcx  → %rcx = 1
#   jmp loopBody
#
# Iteration 2:
#   cmp $4,%rcx  → 1 < 4, don't jump
#   add (%rbx,%rcx,4),%rax  → %rax = 1 + arr1[1] = 1 + 2 = 3
#   add $1,%rcx  → %rcx = 2
#   jmp loopBody
#
# Iteration 3:
#   cmp $4,%rcx  → 2 < 4, don't jump
#   add (%rbx,%rcx,4),%rax  → %rax = 3 + arr1[2] = 3 + 3 = 6
#   add $1,%rcx  → %rcx = 3
#   jmp loopBody
#
# Iteration 4:
#   cmp $4,%rcx  → 3 < 4, don't jump
#   add (%rbx,%rcx,4),%rax  → %rax = 6 + arr1[3] = 6 + 4 = 10
#   add $1,%rcx  → %rcx = 4
#   jmp loopBody
#
# Iteration 5:
#   cmp $4,%rcx  → 4 >= 4, jump to done!
#   %rax = 10 (final sum)

# SCALED INDEXED ADDRESSING:
#
# Format: offset(%base,%index,scale)
# Address = base + (index × scale) + offset
#
# (%rbx,%rcx,4):
#   base = %rbx (array start address)
#   index = %rcx (loop counter)
#   scale = 4 (size of each element in bytes)
#   offset = 0 (implied)
#   Address = %rbx + %rcx × 4
#
# This is perfect for array access!
#   C: array[i]
#   Assembly: (%rbx,%rcx,scale)
#     where scale = sizeof(array element)

# COMMON ARRAY ACCESS PATTERNS:
#
# Byte array (char):
#   (%rbx,%rcx,1)  or just (%rbx,%rcx)
#
# Word array (short):
#   (%rbx,%rcx,2)
#
# Long array (int):
#   (%rbx,%rcx,4)
#
# Quad array (long):
#   (%rbx,%rcx,8)

# COMPARISON TO C:
# This program implements:
#   int arr1[] = {1, 2, 3, 4};
#   int main() {
#       int sum = 0;
#       for (int i = 0; i < 4; i++) {
#           sum += arr1[i];
#       }
#       return sum;  // Returns 10
#   }

# FOR-LOOP STRUCTURE IN ASSEMBLY:
#
# C for-loop:
#   for (init; condition; increment) {
#       body;
#   }
#
# Assembly pattern:
#   init
# loop:
#   test condition
#   jump if false to done
#   body
#   increment
#   jump to loop
# done:
#
# Our loop:
#   mov $0,%rcx           # init: i = 0
# loopBody:
#   cmp $4,%rcx           # test: i < 4?
#   jge done              # exit if i >= 4
#   add (...),%rax        # body: sum += arr1[i]
#   add $1,%rcx           # increment: i++
#   jmp loopBody          # repeat
# done:

# ALTERNATIVE LOOP STRUCTURES:
#
# While loop:
#   while (condition) { body; }
#   →
# loop:
#   test condition
#   je done
#   body
#   jmp loop
# done:
#
# Do-while loop:
#   do { body; } while (condition);
#   →
# loop:
#   body
#   test condition
#   jne loop
