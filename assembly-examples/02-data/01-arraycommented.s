# PEDAGOGICAL PURPOSE:
# This program demonstrates fundamental array operations in x86-64 assembly,
# including array declaration, indexed memory access, and iterating over arrays.
# It shows two complete loops: one that doubles each array element, and another
# that sums all elements. This is essential for understanding how arrays work
# at the hardware level and how high-level array operations translate to assembly.
#
# Key learning objectives:
# 1. Understanding how to declare arrays in the .data section
# 2. Learning the .quad directive for 64-bit (8-byte) integers
# 3. Mastering scaled indexed addressing: base(,index,scale)
# 4. Understanding memory addressing calculations: address = base + (index * scale)
# 5. Learning how to read from and write to memory using registers
# 6. Implementing loops with counters and conditional jumps
# 7. Understanding how array indexing in C maps to assembly
# 8. Learning to modify array elements in place
# 9. Accumulating values across loop iterations
# 10. Understanding register usage patterns in loops
#
# CONCEPTUAL OVERVIEW:
# This program performs two operations on a 5-element array:
#
# 1. DOUBLING LOOP: Multiplies each element by 2
#    for(i=0; i<5; i++) myArray[i] *= 2;
#
# 2. SUMMING LOOP: Adds all elements together
#    sum = 0;
#    for(i=0; i<5; i++) sum += myArray[i];
#    return sum;
#
# ARRAY MEMORY LAYOUT:
# myArray: [1] [2] [3] [4] [5]
#          +0  +8  +16 +24 +32  (byte offsets)
#
# Each element is 8 bytes (.quad), so:
# - Element 0 is at: myArray + (0 * 8) = myArray + 0
# - Element 1 is at: myArray + (1 * 8) = myArray + 8
# - Element 2 is at: myArray + (2 * 8) = myArray + 16
# - Element 3 is at: myArray + (3 * 8) = myArray + 24
# - Element 4 is at: myArray + (4 * 8) = myArray + 32

        .section .data             # DATA SECTION: Initialized data
                                  # This section holds variables with initial values
                                  # Unlike .rodata, this section is writable
                                  # Data persists for the lifetime of the program

myArray:         .quad 1, 2, 3, 4, 5
                                  # ARRAY DECLARATION: Five 64-bit integers
                                  # .quad = 8 bytes (quadword) per element
                                  # myArray is the LABEL (address of first element)
                                  #
                                  # MEMORY LAYOUT:
                                  # Address:  myArray+0  myArray+8  myArray+16 myArray+24 myArray+32
                                  # Value:    1          2          3          4          5
                                  # Bytes:    [8 bytes]  [8 bytes]  [8 bytes]  [8 bytes]  [8 bytes]
                                  #
                                  # Total size: 5 elements * 8 bytes = 40 bytes
                                  #
                                  # EQUIVALENT C CODE:
                                  # long myArray[] = {1, 2, 3, 4, 5};
                                  #
                                  # WHY .quad?
                                  # - Matches register size (64-bit)
                                  # - Efficient for modern processors
                                  # - Can hold large numbers (up to 2^63-1 for signed)

        .section .text             # CODE SECTION: Executable instructions
        .global _start             # GLOBAL DIRECTIVE: Make _start visible to linker


_start:
        # PHASE 1: DOUBLE EACH ARRAY ELEMENT
        # This loop multiplies every element by 2
        # C equivalent: for(int i=0; i<5; i++) myArray[i] *= 2;

        mov $0, %rcx               # INITIALIZE COUNTER: %rcx = 0 (loop index i)
                                  # %rcx serves as our array index
                                  # Starting at 0 for first element
                                  # Will increment to 1, 2, 3, 4 and stop at 5
                                  #
                                  # REGISTER CHOICE:
                                  # %rcx is conventional for loop counters
                                  # ('c' suggests 'counter')

        .dubLoop:                  # LOOP LABEL: Start of doubling loop
                                  # Local label (starts with .) - visible only in this file
                                  # Good practice for internal loop labels

        mov myArray(,%rcx,8), %rax
                                  # LOAD ARRAY ELEMENT: %rax = myArray[%rcx]
                                  # This is SCALED INDEXED ADDRESSING
                                  #
                                  # SYNTAX BREAKDOWN: myArray(,%rcx,8)
                                  # - myArray: base address (where array starts)
                                  # - (empty): no additional base register
                                  # - %rcx: index register (which element)
                                  # - 8: scale factor (bytes per element)
                                  #
                                  # ADDRESS CALCULATION:
                                  # effective_address = myArray + (0 + %rcx * 8)
                                  #                   = myArray + (%rcx * 8)
                                  #
                                  # EXAMPLE: When %rcx = 2 (third element):
                                  # address = myArray + (2 * 8) = myArray + 16
                                  # This loads the value at myArray[2] (which is 3)
                                  #
                                  # C EQUIVALENT:
                                  # rax = myArray[rcx];
                                  # or: rax = *(myArray + rcx);
                                  #
                                  # WHY THIS ADDRESSING MODE?
                                  # - Hardware computes (index * scale) automatically
                                  # - No separate multiply instruction needed
                                  # - Scale must be 1, 2, 4, or 8 (power of 2)
                                  # - Perfect for arrays of byte, word, long, quad

        imul $2, %rax              # DOUBLE THE VALUE: %rax = %rax * 2
                                  # imul = signed integer multiply
                                  # Two-operand form: destination *= source
                                  # %rax now holds the doubled value
                                  #
                                  # EXAMPLE: If %rax was 3, now it's 6
                                  #
                                  # WHY imul INSTEAD OF add?
                                  # - Demonstrates multiplication instruction
                                  # - Could also do: add %rax,%rax
                                  # - Or shift left: shl $1,%rax
                                  # - All three produce same result for *2

        mov %rax, myArray(,%rcx,8)
                                  # STORE BACK TO ARRAY: myArray[%rcx] = %rax
                                  # Write the doubled value back to memory
                                  # Uses same addressing mode as the load
                                  #
                                  # MEMORY UPDATE PATTERN:
                                  # 1. Load from memory to register
                                  # 2. Modify register value
                                  # 3. Store register back to memory
                                  #
                                  # WHY THREE STEPS?
                                  # - x86 can't multiply memory directly by immediate
                                  # - Registers are much faster than memory
                                  # - Common pattern: load-modify-store
                                  #
                                  # MEMORY STATE AFTER EACH ITERATION:
                                  # i=0: [2, 2, 3, 4, 5]
                                  # i=1: [2, 4, 3, 4, 5]
                                  # i=2: [2, 4, 6, 4, 5]
                                  # i=3: [2, 4, 6, 8, 5]
                                  # i=4: [2, 4, 6, 8, 10]

        inc %rcx                   # INCREMENT COUNTER: %rcx = %rcx + 1
                                  # Move to next array element
                                  # inc is faster than add $1,%rcx
                                  # After: %rcx goes from 0→1→2→3→4→5

        cmp $5, %rcx               # COMPARE: Is %rcx < 5?
                                  # cmp performs subtraction and sets flags
                                  # Effectively: %rcx - 5
                                  # Sets flags based on result:
                                  # - If %rcx < 5: SF (sign flag) = 1
                                  # - If %rcx = 5: ZF (zero flag) = 1
                                  # - If %rcx > 5: both flags = 0
                                  #
                                  # NOTE: cmp doesn't change %rcx!
                                  # It only sets flags for the next jump

        jl .dubLoop                # JUMP IF LESS: if(%rcx < 5) goto .dubLoop
                                  # jl = Jump if Less (signed comparison)
                                  # Checks if SF != OF (sign flag != overflow flag)
                                  #
                                  # LOOP CONTROL:
                                  # - When %rcx = 0,1,2,3,4: jump back (continue loop)
                                  # - When %rcx = 5: don't jump (exit loop)
                                  #
                                  # WHY jl INSTEAD OF jb?
                                  # - jl: "jump if less" (signed: -5 < 2)
                                  # - jb: "jump if below" (unsigned: 255 > 2)
                                  # - For positive counters, both work
                                  # - jl is clearer for counting loops
                                  #
                                  # TOTAL ITERATIONS: 5
                                  # Loop runs with %rcx = 0, 1, 2, 3, 4

        # PHASE 2: SUM ALL ARRAY ELEMENTS
        # This loop adds up all elements into %rax
        # C equivalent: sum=0; for(int i=0; i<5; i++) sum += myArray[i];

        mov $0,%rcx                # RESET COUNTER: %rcx = 0
                                  # Start from first element again
                                  # Reusing %rcx for the second loop

        mov $0,%rax                # INITIALIZE ACCUMULATOR: sum = 0
                                  # %rax will hold the running sum
                                  # Start with 0 and add each element
                                  #
                                  # ACCUMULATOR PATTERN:
                                  # - Initialize to 0 (neutral element for addition)
                                  # - Add each value in loop
                                  # - Final value is the total

        .sumLoop:                  # LOOP LABEL: Start of summing loop
                                  # Different label from .dubLoop
                                  # Same basic structure but different operation

        add myArray(,%rcx,8), %rax
                                  # ADD ARRAY ELEMENT: %rax += myArray[%rcx]
                                  # This adds memory directly to register!
                                  # Same addressing mode: myArray(,%rcx,8)
                                  #
                                  # NOTE: Unlike imul, add CAN work with memory operand
                                  # No need to load into register first
                                  #
                                  # MEMORY-TO-REGISTER ARITHMETIC:
                                  # x86 allows: add memory,register
                                  # But not: add memory,memory
                                  # At least one operand must be a register
                                  #
                                  # ACCUMULATION:
                                  # i=0: %rax = 0 + 2 = 2
                                  # i=1: %rax = 2 + 4 = 6
                                  # i=2: %rax = 6 + 6 = 12
                                  # i=3: %rax = 12 + 8 = 20
                                  # i=4: %rax = 20 + 10 = 30
                                  #
                                  # FINAL SUM: 2+4+6+8+10 = 30

        inc %rcx                   # INCREMENT COUNTER: %rcx++
                                  # Move to next element
                                  # Same pattern as first loop

        cmp $5, %rcx               # COMPARE: Is %rcx < 5?
                                  # Check if more elements remain

        jl .sumLoop                # JUMP IF LESS: if(%rcx < 5) goto .sumLoop
                                  # Continue until all 5 elements processed
                                  # Loop runs with %rcx = 0, 1, 2, 3, 4

        # PHASE 3: EXIT WITH RESULT
        # Return the sum as exit code

        mov %rax,%rdi              # SET EXIT CODE: %rdi = %rax (the sum)
                                  # Exit syscall expects exit code in %rdi
                                  # %rax contains our sum (30)
                                  #
                                  # TESTING:
                                  # After running: echo $?
                                  # Should print: 30
                                  #
                                  # NOTE: Exit codes are typically 0-255
                                  # But 30 is valid and visible

        mov $60,%rax               # SYSCALL NUMBER: 60 = exit
                                  # Prepare to terminate program
                                  # Overwrites the sum, but we saved it to %rdi

        syscall                    # INVOKE EXIT: Execute exit(30)
                                  # Program terminates with exit code 30
                                  # Parent process receives this value



# PROGRAM EXECUTION FLOW:
#
# 1. START: Program begins at _start
# 2. DOUBLING LOOP (5 iterations):
#    - Initialize %rcx = 0
#    - Loop from i=0 to i=4:
#      * Load myArray[i] into %rax
#      * Multiply %rax by 2
#      * Store %rax back to myArray[i]
#      * Increment i
#    - Array is now: [2, 4, 6, 8, 10]
# 3. SUMMING LOOP (5 iterations):
#    - Initialize %rcx = 0, %rax = 0
#    - Loop from i=0 to i=4:
#      * Add myArray[i] to %rax
#      * Increment i
#    - %rax is now: 30
# 4. EXIT:
#    - Move sum (30) to %rdi
#    - Call exit syscall
#    - Program terminates with code 30

# SCALED INDEXED ADDRESSING MODES:
#
# General form: displacement(base,index,scale)
# Address = displacement + base + (index * scale)
#
# In this program: myArray(,%rcx,8)
# - displacement: myArray (array base address)
# - base: (none, empty)
# - index: %rcx (which element)
# - scale: 8 (bytes per element)
#
# OTHER EXAMPLES:
# array(,%rsi,4)     -> array[rsi] (4-byte elements)
# (%rbx,%rcx,8)      -> rbx[rcx] (8-byte elements, rbx is base)
# 16(%rsp,%rax,8)    -> (rsp+16)[rax] (offset array on stack)
#
# VALID SCALES: 1, 2, 4, 8 (corresponding to byte, word, long, quad)

# C ARRAY INDEXING VS ASSEMBLY:
#
# C code:               Assembly equivalent:
# int arr[5];           arr: .long 0,0,0,0,0
# arr[i]                arr(,%rcx,4)
# arr[i] = x            mov %eax,arr(,%rcx,4)
# x = arr[i]            mov arr(,%rcx,4),%eax
# &arr[i]               lea arr(,%rcx,4),%rax
#
# The compiler automatically:
# 1. Chooses appropriate scale (element size)
# 2. Generates index * scale calculation
# 3. Selects correct instruction size (b/w/l/q suffix)

# LOOP PATTERNS:
#
# COUNTING LOOP (like our code):
#   mov $0,%rcx          # i = 0
# loop:
#   (body using %rcx)
#   inc %rcx             # i++
#   cmp $N,%rcx          # i < N?
#   jl loop              # continue if yes
#
# COUNTDOWN LOOP (alternative):
#   mov $N,%rcx          # i = N
# loop:
#   dec %rcx             # --i
#   (body using %rcx)
#   jnz loop             # continue if not zero
#
# Both patterns are common in assembly

# DATA DIRECTIVES:
#
# .byte value            - 1 byte  (8 bits)   Range: -128 to 127 or 0 to 255
# .word value            - 2 bytes (16 bits)  Range: -32768 to 32767
# .long value            - 4 bytes (32 bits)  Range: -2^31 to 2^31-1
# .quad value            - 8 bytes (64 bits)  Range: -2^63 to 2^63-1
#
# Arrays:
# .byte 1,2,3            - Three bytes: [1][2][3]
# .long 100,200,300      - Three longs: [100][200][300] (12 bytes total)
# .quad 1,2,3,4,5        - Five quads: [1][2][3][4][5] (40 bytes total)

# REGISTER SIZE SUFFIXES:
#
# Instruction suffixes indicate operand size:
# movb - byte (8 bits)    %al, %bl, etc.
# movw - word (16 bits)   %ax, %bx, etc.
# movl - long (32 bits)   %eax, %ebx, etc.
# movq - quad (64 bits)   %rax, %rbx, etc.
#
# For array access, match suffix to array element size:
# .byte array -> movb array(,%rcx,1),%al
# .word array -> movw array(,%rcx,2),%ax
# .long array -> movl array(,%rcx,4),%eax
# .quad array -> movq array(,%rcx,8),%rax
#
# Or let assembler infer from register:
# mov myArray(,%rcx,8),%rax  -> automatically movq

# EQUIVALENT C CODE:
#
# long myArray[] = {1, 2, 3, 4, 5};
#
# int main() {
#     // Phase 1: Double each element
#     for(int i = 0; i < 5; i++) {
#         myArray[i] *= 2;
#     }
#
#     // Phase 2: Sum all elements
#     long sum = 0;
#     for(int i = 0; i < 5; i++) {
#         sum += myArray[i];
#     }
#
#     return sum;  // Returns 30
# }

# TO ASSEMBLE, LINK, AND RUN:
# as 01-array.s -o 01-array.o
# ld 01-array.o -o 01-array
# ./01-array
# echo $?
# (outputs: 30, which is 2+4+6+8+10)

# DEBUGGING WITH GDB:
# gdb ./01-array
# (gdb) break _start
# (gdb) run
# (gdb) display/5gd &myArray     # Show array as 5 decimal quad-words
# (gdb) stepi                    # Step through each instruction
# (gdb) info registers rcx rax   # Show register values
# (gdb) x/5gd &myArray           # Examine array memory
#
# Watch how array values change during doubling loop!

# COMMON MISTAKES:
#
# 1. Wrong scale factor:
#    mov myArray(,%rcx,4),%rax   # WRONG! Elements are 8 bytes, not 4
#
# 2. Forgetting to initialize counter:
#    (no mov $0,%rcx)            # WRONG! Starts at random value
#
# 3. Off-by-one errors:
#    cmp $6,%rcx                 # WRONG! Would skip last element
#    jl .dubLoop                 # or access beyond array
#
# 4. Using wrong jump condition:
#    jg .dubLoop                 # WRONG! Would never loop (5 > 5 is false)
#
# 5. Forgetting to store back:
#    imul $2,%rax                # Modify value
#    (no store instruction)      # WRONG! Change only in register, not memory

# PERFORMANCE NOTES:
#
# - Scaled indexed addressing is ONE instruction (no separate multiply)
# - Modern CPUs execute this very efficiently
# - inc is often faster than add $1 (fewer bytes, may fuse with cmp)
# - Array access patterns like this are well-optimized by CPU cache
# - Sequential access (0,1,2,3,4) is cache-friendly (prefetching)
#
# OPTIMIZATION OPPORTUNITY:
# Could combine loops to save one pass through array:
#   for(i=0; i<5; i++) {
#       myArray[i] *= 2;
#       sum += myArray[i];
#   }
# But our version is clearer for learning!
