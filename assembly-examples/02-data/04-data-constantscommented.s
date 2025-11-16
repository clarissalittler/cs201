# PEDAGOGICAL PURPOSE:
# This program synthesizes multiple concepts: arrays, RIP-relative addressing,
# pointers, and scaled indexed addressing. It demonstrates how to combine these
# techniques to iterate through an array using modern position-independent code.
# This represents real-world assembly programming style and serves as a bridge
# between basic examples and production code.
#
# Key learning objectives:
# 1. Combining RIP-relative addressing with arrays
# 2. Using LEA to get array base address
# 3. Implementing loops with both counter and accumulator
# 4. Understanding complex addressing: (%base,%index,scale)
# 5. Seeing how array[i] translates with pointer arithmetic
# 6. Learning the relationship between C array indexing and assembly
# 7. Understanding why compilers generate RIP-relative code
# 8. Comparing different loop exit strategies (jge vs jl)
# 9. Practicing register allocation for multiple variables
# 10. Understanding how address calculation and memory access combine
#
# CONCEPTUAL OVERVIEW:
# This program sums the elements of a 32-bit integer array:
#
# int arr1[] = {1, 2, 3, 4};
# int sum = 0;
# for(int i = 0; i < 4; i++) {
#     sum += arr1[i];
# }
# return sum;
#
# TECHNIQUE PROGRESSION:
# 01-array.s:         Direct addressing with scaled indexing
# 02-data-section.s:  Direct addressing with single variable
# 03-data-types.s:    RIP-relative + pointer to single variable
# 04-data-constants.s: RIP-relative + pointer + scaled indexing (THIS FILE)
#
# ARRAY MEMORY LAYOUT:
# arr1: [1]  [2]  [3]  [4]
#       +0   +4   +8   +12  (byte offsets from arr1)
#
# Each element is 4 bytes (.long), so:
# - Element 0 is at: %rbx + (0 * 4) = %rbx + 0
# - Element 1 is at: %rbx + (1 * 4) = %rbx + 4
# - Element 2 is at: %rbx + (2 * 4) = %rbx + 8
# - Element 3 is at: %rbx + (3 * 4) = %rbx + 12
#
# ADDRESSING VISUALIZATION:
#
# %rbx points here ↓
# ┌─────┬─────┬─────┬─────┐
# │  1  │  2  │  3  │  4  │  arr1[] in memory
# └─────┴─────┴─────┴─────┘
#   ↑     ↑     ↑     ↑
#   +0    +4    +8    +12   (byte offsets)
#   i=0   i=1   i=2   i=3   (when %rcx = i)
#
# Access: (%rbx,%rcx,4) = *(%rbx + %rcx * 4) = arr1[%rcx]

        .section .data             # DATA SECTION: Initialized writable data

arr1:   .long 1,2,3,4              # ARRAY DECLARATION: Four 32-bit integers
                                  # .long = 4 bytes per element
                                  # arr1 is the label (address of first element)
                                  # Comma-separated values create consecutive elements
                                  #
                                  # MEMORY LAYOUT:
                                  # Address:  arr1+0  arr1+4  arr1+8  arr1+12
                                  # Value:    1       2       3       4
                                  # Size:     4 bytes each
                                  # Total:    16 bytes (4 elements × 4 bytes)
                                  #
                                  # EQUIVALENT C:
                                  # int arr1[] = {1, 2, 3, 4};
                                  # Or explicitly:
                                  # int arr1[4] = {1, 2, 3, 4};
                                  #
                                  # WHY .long?
                                  # - Standard integer size (32 bits)
                                  # - Efficient for CPU operations
                                  # - Matches C 'int' on most platforms
                                  # - Smaller than .quad (saves memory for small values)

        .section .text             # CODE SECTION: Executable instructions
        .global _start             # GLOBAL DIRECTIVE: Make _start visible to linker

        ## write a for loop that adds up all the
        ## numbers in the array
                                  # COMMENT: Task description
                                  # This is a classic accumulator pattern
                                  # Initialize sum to 0, iterate through array, add each element

_start:
        lea arr1(%rip),%rbx        # LOAD ARRAY BASE ADDRESS: %rbx = &arr1[0]
                                  # This gets a POINTER to the start of the array
                                  #
                                  # INSTRUCTION BREAKDOWN:
                                  # lea          - Load Effective Address (compute address)
                                  # arr1(%rip)   - Address of arr1, RIP-relative
                                  # %rbx         - Destination (will hold pointer)
                                  #
                                  # RIP-RELATIVE ADDRESSING: arr1(%rip)
                                  # - Position-independent code (PIC)
                                  # - Works with ASLR (Address Space Layout Randomization)
                                  # - Required for modern security standards
                                  # - Enables shared libraries
                                  #
                                  # CALCULATION:
                                  # Assembler: offset = arr1_address - next_instruction_address
                                  # Runtime: %rbx = %rip + offset = address of arr1
                                  #
                                  # WHY LEA?
                                  # We need the ADDRESS to use with indexed addressing later
                                  # mov arr1(%rip),%ebx would load the VALUE (1), not address
                                  #
                                  # C EQUIVALENT:
                                  # int *rbx = arr1;
                                  # or:
                                  # int *rbx = &arr1[0];
                                  #
                                  # ALTERNATIVE APPROACHES:
                                  # Without RIP-relative (old style):
                                  #   lea arr1,%rbx          (direct - breaks with ASLR)
                                  # Without LEA (embedded in each access):
                                  #   mov arr1(%rip,%rcx,4),%eax  (works but less clear)
                                  #
                                  # WHY %rbx?
                                  # - Holds base address for the entire loop
                                  # - Doesn't change during iteration
                                  # - %rbx is callee-saved (preserved across function calls)

        # load the address of the array into %rbx (relative to %rip)
                                  # COMMENT: Explanation of previous instruction
                                  # Emphasizes RIP-relative aspect

        mov $0,%rcx                # INITIALIZE LOOP COUNTER: int i = 0
                                  # %rcx will track which element we're processing
                                  # Standard loop pattern: start at 0, increment each iteration
                                  #
                                  # REGISTER ALLOCATION:
                                  # %rbx - base address of array (doesn't change)
                                  # %rcx - loop counter i (0, 1, 2, 3)
                                  # %rax - accumulator sum (running total)
                                  #
                                  # WHY %rcx?
                                  # - Convention: 'c' suggests 'counter'
                                  # - Works well with loop instructions (though not used here)
                                  # - Can be used in addressing mode calculations

        # int i = 0                 # COMMENT: C equivalent

        mov $0,%rax                # INITIALIZE ACCUMULATOR: int sum = 0
                                  # %rax will hold the running sum
                                  # Start with 0 (neutral element for addition)
                                  # Each iteration adds one array element to this
                                  #
                                  # ACCUMULATOR PATTERN:
                                  # 1. Initialize to 0 (or identity element)
                                  # 2. Loop: accumulator += current_element
                                  # 3. After loop: accumulator holds result
                                  #
                                  # WHY %rax?
                                  # - Convention: 'a' suggests 'accumulator'
                                  # - Return value register (we return sum to exit code)
                                  # - First choice for temporary results

        # int sum = 0               # COMMENT: C equivalent

loopBody:                          # LOOP LABEL: Start of loop body
                                  # Not a local label (no leading .)
                                  # Visible throughout the file
                                  # Control flow will jump here for each iteration

        cmp $4,%rcx                # COMPARE: Is i >= 4?
                                  # Check if we've processed all elements
                                  # Performs: %rcx - 4 and sets flags
                                  #
                                  # COMPARISON FLAGS:
                                  # If %rcx = 0,1,2,3: %rcx < 4, ZF=0, SF≠OF
                                  # If %rcx = 4: %rcx = 4, ZF=1
                                  # If %rcx > 4: %rcx > 4, ZF=0, SF=OF
                                  #
                                  # NOTE: cmp doesn't modify %rcx!
                                  # Only sets condition flags for next jump

        # i < 4                     # COMMENT: C equivalent condition

        jge done                   # JUMP IF GREATER OR EQUAL: if(i >= 4) goto done
                                  # Exit loop when all elements processed
                                  # jge = Jump if Greater or Equal (signed comparison)
                                  #
                                  # CONDITION:
                                  # Jumps when %rcx >= 4
                                  # Checks SF = OF (sign flag equals overflow flag)
                                  #
                                  # LOOP EXIT:
                                  # When %rcx = 4: jumps to done (exits loop)
                                  # When %rcx < 4: doesn't jump (continues loop)
                                  #
                                  # INVERSE LOGIC:
                                  # C loop: while(i < 4) { body; }
                                  # Assembly: if(i >= 4) exit; body; goto loop;
                                  #
                                  # ALTERNATIVE STYLES:
                                  # Could use: jl loopBody after comparison
                                  # Or: dec %rcx and jnz at end (countdown loop)
                                  # This style (test at top) is most common
                                  #
                                  # WHY jge NOT jae?
                                  # jge: signed comparison (treats as signed integers)
                                  # jae: unsigned comparison (treats as unsigned)
                                  # For positive counters 0-4, both work
                                  # jge is more intuitive for counting loops

        add (%rbx,%rcx,4),%rax     # ACCUMULATE: sum += arr1[i]
                                  # Add current array element to running sum
                                  #
                                  # INSTRUCTION BREAKDOWN:
                                  # add              - Add values
                                  # (%rbx,%rcx,4)    - Source: array element (memory)
                                  # %rax             - Destination: accumulator (register)
                                  #
                                  # COMPLEX ADDRESSING: (%rbx,%rcx,4)
                                  # This is BASE + (INDEX * SCALE) addressing
                                  # Format: (base,index,scale)
                                  # Address calculation: %rbx + (%rcx * 4)
                                  #
                                  # BREAKDOWN:
                                  # %rbx  - base address (pointer to arr1[0])
                                  # %rcx  - index (which element: 0, 1, 2, 3)
                                  # 4     - scale (bytes per element: sizeof(int))
                                  #
                                  # CONCRETE EXAMPLES:
                                  # When %rcx = 0: address = %rbx + (0*4) = %rbx + 0  → arr1[0] = 1
                                  # When %rcx = 1: address = %rbx + (1*4) = %rbx + 4  → arr1[1] = 2
                                  # When %rcx = 2: address = %rbx + (2*4) = %rbx + 8  → arr1[2] = 3
                                  # When %rcx = 3: address = %rbx + (3*4) = %rbx + 12 → arr1[3] = 4
                                  #
                                  # C EQUIVALENT:
                                  # rax += arr1[rcx];
                                  # or more explicitly:
                                  # rax += *(rbx + rcx);  (if rbx is int* pointing to arr1)
                                  #
                                  # POINTER ARITHMETIC:
                                  # In C: arr1[i] ≡ *(arr1 + i)
                                  # In assembly: (%rbx,%rcx,4) ≡ *(%rbx + %rcx * sizeof(element))
                                  #
                                  # WHY SCALE 4?
                                  # arr1 elements are .long (4 bytes each)
                                  # Must multiply index by element size
                                  # Scale must be 1, 2, 4, or 8 (hardware limitation)
                                  #
                                  # ACCUMULATION TRACE:
                                  # Initially: %rax = 0
                                  # i=0: %rax = 0 + 1 = 1
                                  # i=1: %rax = 1 + 2 = 3
                                  # i=2: %rax = 3 + 3 = 6
                                  # i=3: %rax = 6 + 4 = 10
                                  # Final: %rax = 10

        ## *(%rbx + 4 * %rcx)
        ## arr1[rcx]
        ## arr1[rcx] -> *(arr1 + rcx)
                                  # COMMENTS: Multiple explanations of addressing
                                  # 1. Direct formula: address = %rbx + 4*%rcx
                                  # 2. Array notation: arr1[rcx]
                                  # 3. Pointer notation: *(arr1 + rcx)
                                  #
                                  # These all express the same concept in different ways
                                  # Helps connect assembly to higher-level thinking

        add $1,%rcx                # INCREMENT COUNTER: i++
                                  # Move to next array element
                                  # %rcx goes: 0 → 1 → 2 → 3 → 4
                                  #
                                  # ALTERNATIVE INSTRUCTIONS:
                                  # inc %rcx     - Equivalent, slightly shorter encoding
                                  # add $1,%rcx  - More explicit, same effect
                                  # Both are fine; inc is traditional for counters
                                  #
                                  # AFFECTS FLAGS:
                                  # Sets OF, SF, ZF, AF, PF (but not CF)
                                  # But we don't use these flags here
                                  # Next iteration starts with new cmp

        #i++                       # COMMENT: C equivalent

        jmp loopBody               # UNCONDITIONAL JUMP: goto loopBody
                                  # Jump back to start of loop for next iteration
                                  # Forms the bottom of the loop
                                  #
                                  # LOOP STRUCTURE:
                                  # loopBody:
                                  #     if(condition) exit
                                  #     body
                                  #     increment
                                  #     goto loopBody
                                  #
                                  # This is a "while" style loop:
                                  # Check condition first, then execute body
                                  #
                                  # ALTERNATIVE: "do-while" style:
                                  # loopBody:
                                  #     body
                                  #     increment
                                  #     if(!condition) goto loopBody
                                  #
                                  # PERFORMANCE:
                                  # Modern CPUs predict backward branches well
                                  # Loop overhead is minimal on modern processors
                                  # Branch predictor learns the pattern quickly

done:                              # LOOP EXIT LABEL: Loop finished
                                  # Control arrives here when i >= 4
                                  # %rax contains the sum (1+2+3+4 = 10)
                                  # %rcx contains 4 (final counter value)
                                  # %rbx still points to arr1 (unchanged)

        mov %rax,%rdi              # SET EXIT CODE: %rdi = sum
                                  # Copy sum from %rax to %rdi
                                  # Exit syscall expects exit code in %rdi
                                  #
                                  # RESULT:
                                  # %rdi = 10 (the sum of 1+2+3+4)
                                  #
                                  # TESTING:
                                  # After program runs: echo $?
                                  # Should display: 10
                                  #
                                  # WHY MOVE?
                                  # %rax must hold syscall number (60)
                                  # Can't use %rax for both sum and syscall number
                                  # So we save sum to %rdi first

        mov $60,%rax               # SYSCALL NUMBER: 60 = exit
                                  # Prepare to terminate program
                                  # This overwrites the sum in %rax
                                  # But we already saved it to %rdi

        syscall                    # INVOKE EXIT: Execute exit(10)
                                  # Program terminates with exit code 10
                                  # Return value is 1+2+3+4 = 10



# PROGRAM EXECUTION FLOW:
#
# 1. SETUP:
#    - lea arr1(%rip),%rbx  → %rbx points to arr1
#    - mov $0,%rcx          → i = 0
#    - mov $0,%rax          → sum = 0
#
# 2. LOOP ITERATION 0 (i=0):
#    - cmp $4,%rcx          → 0 < 4, continue
#    - add (%rbx,0,4),%rax  → sum = 0 + arr1[0] = 0 + 1 = 1
#    - add $1,%rcx          → i = 1
#    - jmp loopBody         → restart loop
#
# 3. LOOP ITERATION 1 (i=1):
#    - cmp $4,%rcx          → 1 < 4, continue
#    - add (%rbx,4,4),%rax  → sum = 1 + arr1[1] = 1 + 2 = 3
#    - add $1,%rcx          → i = 2
#    - jmp loopBody         → restart loop
#
# 4. LOOP ITERATION 2 (i=2):
#    - cmp $4,%rcx          → 2 < 4, continue
#    - add (%rbx,8,4),%rax  → sum = 3 + arr1[2] = 3 + 3 = 6
#    - add $1,%rcx          → i = 3
#    - jmp loopBody         → restart loop
#
# 5. LOOP ITERATION 3 (i=3):
#    - cmp $4,%rcx          → 3 < 4, continue
#    - add (%rbx,12,4),%rax → sum = 6 + arr1[3] = 6 + 4 = 10
#    - add $1,%rcx          → i = 4
#    - jmp loopBody         → restart loop
#
# 6. LOOP EXIT (i=4):
#    - cmp $4,%rcx          → 4 >= 4, exit!
#    - jge done             → jump to done
#
# 7. CLEANUP & EXIT:
#    - mov %rax,%rdi        → %rdi = 10
#    - mov $60,%rax         → syscall number
#    - syscall              → exit(10)

# ADDRESSING MODE COMPARISON:
#
# This program combines TWO addressing techniques:
#
# 1. RIP-RELATIVE (for position independence):
#    lea arr1(%rip),%rbx
#    - Gets base address using PC-relative offset
#    - Works with ASLR and shared libraries
#    - Modern, secure, required for production code
#
# 2. BASE+INDEX*SCALE (for array access):
#    add (%rbx,%rcx,4),%rax
#    - Accesses array element using base + offset
#    - %rbx: array base (from step 1)
#    - %rcx: index (which element)
#    - 4: scale (sizeof(int))
#
# COMBINED:
# The RIP-relative addressing gets us the array location
# The indexed addressing accesses individual elements
# Together: position-independent array processing!

# SCALED INDEXED ADDRESSING DEEP DIVE:
#
# GENERAL FORM: displacement(base,index,scale)
# Address = displacement + base + (index * scale)
#
# THIS PROGRAM: (%rbx,%rcx,4)
# - displacement: 0 (omitted)
# - base: %rbx (array base address)
# - index: %rcx (which element)
# - scale: 4 (bytes per element)
# Address = 0 + %rbx + (%rcx * 4) = %rbx + (%rcx * 4)
#
# VALID SCALES: 1, 2, 4, 8
# Correspond to: byte, word, long, quad
# Hardware can multiply by these in one cycle
#
# EXAMPLES:
# (%rbx,%rcx,1)      → byte array: rbx[rcx]
# (%rbx,%rcx,2)      → word array: rbx[rcx] (short)
# (%rbx,%rcx,4)      → long array: rbx[rcx] (int) ← THIS PROGRAM
# (%rbx,%rcx,8)      → quad array: rbx[rcx] (long/pointer)
#
# WITH DISPLACEMENT:
# 16(%rbx,%rcx,4)    → (rbx+16)[rcx] (skip 16 bytes, then index)
#
# USAGE PATTERNS:
# - Arrays: (%base,%index,element_size)
# - Structures: offset(%base) for each field
# - Array of structures: offset(%base,%index,struct_size)
# - Matrices: base(,%row,row_size) then add column offset

# C ARRAY EQUIVALENTS:
#
# C CODE:                    ASSEMBLY:
# int arr[10];               arr: .long 0,0,0,0,0,0,0,0,0,0
# int *ptr = arr;            lea arr(%rip),%rbx
# int x = arr[i];            mov (%rbx,%rcx,4),%eax
# arr[i] = x;                mov %eax,(%rbx,%rcx,4)
# arr[i] += 5;               addl $5,(%rbx,%rcx,4)
# x = arr[2];                mov 8(%rbx),%eax (or (%rbx) with offset)
# ptr[i] = x;                mov %eax,(%ptr,%rcx,4)
#
# ARRAY INDEXING FORMULA:
# C: arr[i] means *(arr + i)
# Assembly: (%rbx,%rcx,4) means *(%rbx + %rcx * 4)
#
# The scale (4) is sizeof(element)
# Compiler chooses correct scale based on type

# LOOP PATTERN VARIATIONS:
#
# THIS PROGRAM (while-style, count up):
# mov $0,%rcx
# loop:
#   cmp $4,%rcx
#   jge done
#   (body)
#   add $1,%rcx
#   jmp loop
# done:
#
# ALTERNATIVE 1 (do-while-style):
# mov $0,%rcx
# loop:
#   (body)
#   add $1,%rcx
#   cmp $4,%rcx
#   jl loop
#
# ALTERNATIVE 2 (countdown):
# mov $4,%rcx
# loop:
#   sub $1,%rcx
#   (body using %rcx)
#   jnz loop
#
# ALTERNATIVE 3 (pointer walking):
# lea arr1(%rip),%rbx
# lea arr1+16(%rip),%r8    # end address
# loop:
#   cmp %r8,%rbx
#   jge done
#   add (%rbx),%rax
#   add $4,%rbx              # advance pointer
#   jmp loop
#
# All valid! Choice depends on:
# - Personal preference
# - Performance characteristics
# - Code clarity
# - Register pressure

# EQUIVALENT C CODE:
#
# int arr1[] = {1, 2, 3, 4};
#
# int main() {
#     int sum = 0;
#     for(int i = 0; i < 4; i++) {
#         sum += arr1[i];
#     }
#     return sum;
# }
#
# Or with explicit pointer (closer to assembly):
# int arr1[] = {1, 2, 3, 4};
#
# int _start() {
#     int *rbx = arr1;       // lea arr1(%rip),%rbx
#     int rcx = 0;           // mov $0,%rcx
#     int rax = 0;           // mov $0,%rax
#
#     while(rcx < 4) {       // cmp/jge pattern
#         rax += rbx[rcx];   // add (%rbx,%rcx,4),%rax
#         rcx += 1;          // add $1,%rcx
#     }
#
#     exit(rax);             // mov %rax,%rdi; syscall
# }

# COMPARISON WITH 01-array.s:
#
# 01-array.s (direct addressing):
# mov myArray(,%rcx,8),%rax
# - Simpler (one addressing mode)
# - Array name embedded in instruction
# - Direct/absolute addressing
# - Less flexible
# - Not position-independent
#
# 04-data-constants.s (RIP-relative + indexed):
# lea arr1(%rip),%rbx
# add (%rbx,%rcx,4),%rax
# - Two-step process
# - Base address in register
# - Position-independent (RIP-relative)
# - More flexible (can change base)
# - Modern style
# - Required for shared libraries
#
# WHEN TO USE EACH:
# Simple standalone programs: Direct is fine
# Shared libraries: Must use RIP-relative
# Position-independent executables (PIE): Must use RIP-relative
# Modern Linux: RIP-relative is default

# TO ASSEMBLE, LINK, AND RUN:
# as 04-data-constants.s -o 04-data-constants.o
# ld 04-data-constants.o -o 04-data-constants
# ./04-data-constants
# echo $?
# (outputs: 10, which is 1+2+3+4)

# DEBUGGING WITH GDB:
# gdb ./04-data-constants
# (gdb) break _start
# (gdb) run
# (gdb) display/4dw $rbx          # Show 4 words at array
# (gdb) display/d $rcx            # Show counter
# (gdb) display/d $rax            # Show sum
# (gdb) commands
#   >silent
#   >printf "i=%d, sum=%d, arr[i]=%d\n", $rcx, $rax, *($rbx+$rcx*4)
#   >continue
# (gdb) continue
#
# This will show each iteration:
# i=0, sum=0, arr[i]=1
# i=1, sum=1, arr[i]=2
# i=2, sum=3, arr[i]=3
# i=3, sum=6, arr[i]=4

# EXPERIMENTS TO TRY:
#
# 1. Change array values:
#    arr1: .long 10,20,30,40
#    Expected exit code: 100
#
# 2. Change array size:
#    arr1: .long 1,2,3,4,5,6
#    Change: cmp $6,%rcx
#    Expected exit code: 21
#
# 3. Different element size:
#    arr1: .quad 1,2,3,4
#    Change scale: (%rbx,%rcx,8)
#    Expected: same result (10)
#
# 4. Verify RIP-relative:
#    (gdb) disassemble _start
#    Look for: lea 0x...(%rip),%rbx
#    Compare to: objdump -d ./04-data-constants
#
# 5. Remove RIP-relative:
#    Change: lea arr1,%rbx
#    Assemble and compare behavior
#    May work but not position-independent!

# COMMON MISTAKES:
#
# 1. Wrong scale factor:
#    add (%rbx,%rcx,8),%rax     # WRONG: 8 bytes, but .long is 4
#    FIX: add (%rbx,%rcx,4),%rax
#
# 2. Off-by-one in loop:
#    cmp $3,%rcx                # WRONG: only processes 3 elements
#    FIX: cmp $4,%rcx           # Process all 4
#
# 3. Forgetting to initialize:
#    (no mov $0,%rax)           # WRONG: sum starts at garbage
#    FIX: mov $0,%rax           # Initialize to 0
#
# 4. Wrong jump direction:
#    jl done                    # WRONG: exits immediately
#    FIX: jge done              # Exit when i >= 4
#
# 5. Incrementing wrong register:
#    add $1,%rax                # WRONG: changes sum, not counter
#    FIX: add $1,%rcx           # Increment counter
#
# 6. Using wrong addressing:
#    add arr1(%rip,%rcx,4),%rax # WRONG: can't combine RIP + index
#    FIX: Use two-step with lea first

# PERFORMANCE CONSIDERATIONS:
#
# LOOP OVERHEAD:
# - 5 instructions per iteration (cmp, jge, add, add, jmp)
# - Modern CPUs: ~1-2 cycles per iteration (with good prediction)
# - Branch predictor learns pattern quickly
#
# MEMORY ACCESS:
# - Sequential access pattern (cache-friendly)
# - CPU prefetcher detects pattern
# - Data likely stays in L1 cache
# - Base+index addressing: no extra cost
#
# OPTIMIZATIONS:
# 1. Loop unrolling (process multiple elements per iteration)
# 2. SIMD instructions (process 4-8 elements at once)
# 3. Compiler would likely vectorize this
# 4. For learning, simple version is best!
#
# COMPILER OUTPUT:
# Modern compilers might generate:
# - Vectorized version using SSE/AVX
# - Unrolled loop
# - Strength reduction (eliminate multiply)
# - But basic structure would be similar

# REAL-WORLD USAGE:
#
# This pattern appears in:
# - String processing (strlen, strcpy)
# - Array algorithms (sort, search, reduce)
# - Data structure traversal
# - Buffer operations
# - System calls with array parameters
# - Image processing (pixel arrays)
# - Scientific computing (vector operations)
#
# FOUNDATION FOR:
# - Multi-dimensional arrays
# - Structures and unions
# - Linked lists and trees
# - Hash tables
# - Dynamic arrays
# - Buffer management
# - Memory allocation
#
# Master this pattern, and you understand
# how virtually all array code works!
