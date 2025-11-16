# PEDAGOGICAL PURPOSE:
# This program introduces RIP-relative addressing and the LEA (Load Effective Address)
# instruction, which are fundamental to modern x86-64 programming. It demonstrates
# how to work with pointers in assembly and why position-independent code is important
# for security and shared libraries. This builds on 02-data-section.s by showing a
# more modern and flexible way to access memory.
#
# Key learning objectives:
# 1. Understanding RIP-relative addressing: label(%rip)
# 2. Learning the LEA instruction for computing addresses
# 3. Understanding the difference between value and address (&)
# 4. Learning pointer dereferencing with parentheses: (%rbx)
# 5. Understanding position-independent code (PIC)
# 6. Learning why modern code uses %rip-relative addressing
# 7. Understanding Address Space Layout Randomization (ASLR)
# 8. Seeing the connection between C pointers and assembly
# 9. Understanding indirect memory access through registers
# 10. Learning when to use LEA vs MOV
#
# CONCEPTUAL OVERVIEW:
# This program does the same thing as 02-data-section.s:
# - Starts with num1 = 200
# - Adds 10 to it
# - Exits with the result (210)
#
# BUT it uses a different technique:
# - Gets a POINTER to num1 using LEA
# - Adds to memory through the pointer using indirect addressing
#
# In C terms:
#   int num1 = 200;
#   int *ptr = &num1;      // lea instruction
#   *ptr += 10;            // addl through pointer
#   return num1;
#
# KEY DIFFERENCE FROM 02-data-section.s:
# Old way:  addl $10,num1        (direct addressing)
# New way:  lea num1(%rip),%rbx  (get address with RIP-relative)
#           addl $10,(%rbx)      (indirect through pointer)
#
# POINTER VISUALIZATION:
#
# Memory:                Register:
# ┌──────────────┐       ┌──────────────┐
# │ num1: 200    │◄──────│ %rbx: &num1  │ (pointer)
# │ (4 bytes)    │       └──────────────┘
# └──────────────┘
#
# After add:
# ┌──────────────┐
# │ num1: 210    │
# └──────────────┘

        .section .data             # DATA SECTION: Initialized writable data

num1:   .long 200                  # VARIABLE DECLARATION: 32-bit integer = 200
                                  # Same as previous program
                                  # num1 is a label marking the memory address

        ## .long means 32 bit       # COMMENT: Size reminder

        .section .text             # CODE SECTION: Executable instructions
        .global _start             # GLOBAL DIRECTIVE: Make _start visible to linker

_start:
        ## rip-relative addressing   # COMMENT: Key technique being demonstrated
                                    # RIP = Instruction Pointer (program counter)
                                    # RIP-relative = address relative to current instruction
                                    #
                                    # WHY RIP-RELATIVE?
                                    # Essential for modern x86-64 programming because:
                                    # 1. Position-Independent Code (PIC)
                                    # 2. Shared libraries (.so files)
                                    # 3. Address Space Layout Randomization (ASLR)
                                    # 4. Security hardening
                                    # 5. Better code density
                                    #
                                    # SECURITY BENEFIT (ASLR):
                                    # With ASLR, program loads at random address each run
                                    # Absolute addressing: breaks (hardcoded addresses)
                                    # RIP-relative: works (offsets stay constant)
                                    #
                                    # Example without ASLR:
                                    # Run 1: num1 at 0x00404000
                                    # Run 2: num1 at 0x00404000 (same - predictable!)
                                    # Attacker can hardcode addresses in exploits
                                    #
                                    # Example with ASLR:
                                    # Run 1: num1 at 0x7f3e5000
                                    # Run 2: num1 at 0x7f8a2000 (different - unpredictable!)
                                    # RIP-relative still works: offset from code is constant

        ## lea is used for the & operation
                                  # COMMENT: LEA = Load Effective Address
                                  # This is the assembly equivalent of C's & operator
                                  #
                                  # C:           Assembly:
                                  # &num1        lea num1(%rip),%rbx
                                  # &array[i]    lea array(,%rsi,8),%rbx
                                  # &struct.field lea 16(%rdi),%rbx
                                  #
                                  # LEA does NOT access memory!
                                  # It only computes the address
                                  # Think: "Load Effective Address" = Calculate address

        lea num1(%rip),%rbx        # GET ADDRESS: %rbx = &num1
                                  # LEA computes the address of num1 and stores it in %rbx
                                  # %rbx now contains a POINTER to num1
                                  #
                                  # INSTRUCTION BREAKDOWN:
                                  # lea          - Load Effective Address (compute address)
                                  # num1(%rip)   - Address relative to %rip
                                  # %rbx         - Destination (will hold the address)
                                  #
                                  # RIP-RELATIVE ADDRESSING: num1(%rip)
                                  # Format: displacement(%rip)
                                  # Address = %rip + displacement
                                  #
                                  # HOW IT WORKS:
                                  # 1. Assembler calculates: offset = num1 - next_instruction
                                  # 2. At runtime: %rip points to next instruction
                                  # 3. CPU computes: %rbx = %rip + offset = address of num1
                                  #
                                  # CONCRETE EXAMPLE:
                                  # Suppose after assembly:
                                  # - This instruction at address 0x1000
                                  # - num1 is at address 0x2000
                                  # - Next instruction at 0x1007 (instruction is 7 bytes)
                                  # - Offset = 0x2000 - 0x1007 = 0xFF9
                                  # - Encoded as: lea 0xFF9(%rip),%rbx
                                  #
                                  # At runtime (with ASLR, loaded at 0x7fff00000000):
                                  # - This instruction at 0x7fff00001000
                                  # - %rip = 0x7fff00001007 (next instruction)
                                  # - %rbx = 0x7fff00001007 + 0xFF9 = 0x7fff00002000
                                  # - num1 is actually at 0x7fff00002000
                                  # - It works! Offset stayed the same even though addresses changed
                                  #
                                  # VS ABSOLUTE ADDRESSING (not RIP-relative):
                                  # lea num1,%rbx  (direct - would use absolute address)
                                  # Problem: hardcodes address, breaks with ASLR
                                  #
                                  # LEA VS MOV:
                                  # lea num1(%rip),%rbx  -> %rbx = address of num1 (pointer)
                                  # mov num1(%rip),%ebx  -> %ebx = value of num1 (200)
                                  #
                                  # C EQUIVALENT:
                                  # int *rbx = &num1;
                                  # or:
                                  # rbx = (void*)&num1;
                                  #
                                  # WHY %rbx?
                                  # Any general-purpose register works for pointers
                                  # %rbx is callee-saved (useful across function calls)
                                  # %rax, %rcx, %rdx are often used for other purposes

        addl $10,(%rbx)            # ADD THROUGH POINTER: *(%rbx) = 10 + *(%rbx)
                                  # This adds 10 to the memory location pointed to by %rbx
                                  # The parentheses mean: DEREFERENCE the pointer
                                  #
                                  # INSTRUCTION BREAKDOWN:
                                  # addl         - ADD Long (32-bit operation)
                                  # $10          - Immediate value to add
                                  # (%rbx)       - Memory at address in %rbx (INDIRECT)
                                  #
                                  # POINTER DEREFERENCING: (%rbx)
                                  # Parentheses around a register = "address stored in register"
                                  # Like C's * operator:
                                  #   C:        *ptr
                                  #   Assembly: (%rbx)
                                  #
                                  # OPERATION STEPS:
                                  # 1. Read address from %rbx (points to num1)
                                  # 2. Load 4 bytes from that address (value: 200)
                                  # 3. Add 10 (result: 210)
                                  # 4. Store 4 bytes back to that address (num1 = 210)
                                  #
                                  # ADDRESSING MODE:
                                  # This is INDIRECT addressing or REGISTER INDIRECT
                                  # Address comes from a register, not from instruction
                                  # Very flexible - same code works with different addresses
                                  #
                                  # WHY USE A POINTER?
                                  # Advantages:
                                  # 1. Can work with different variables using same code
                                  # 2. Essential for arrays and data structures
                                  # 3. Needed for passing addresses to functions
                                  # 4. Enables runtime address computation
                                  #
                                  # COMPARISON TO 02-data-section.s:
                                  # Old: addl $10,num1       (direct - name is hardcoded)
                                  # New: addl $10,(%rbx)     (indirect - address in register)
                                  #
                                  # C EQUIVALENT:
                                  # *rbx += 10;
                                  # or:
                                  # *rbx = *rbx + 10;
                                  #
                                  # SIZE SUFFIX 'l':
                                  # Required because (%rbx) could point to any size data
                                  # 'l' specifies 32-bit operation to match .long declaration

        mov num1,%rdi              # LOAD RESULT: %rdi = num1
                                  # Load the modified value to pass as exit code
                                  # Could also do: mov (%rbx),%edi
                                  # Both read the same memory location
                                  #
                                  # ADDRESSING COMPARISON:
                                  # mov num1,%rdi       - Direct addressing
                                  # mov num1(%rip),%rdi - RIP-relative addressing
                                  # mov (%rbx),%rdi     - Indirect addressing
                                  # All three can read num1, but in different ways!

        mov $60,%rax               # SYSCALL NUMBER: 60 = exit

        syscall                    # INVOKE EXIT: Execute exit(210)



# PROGRAM EXECUTION FLOW:
#
# 1. SETUP: Program loads with num1 = 200 in .data section
# 2. GET ADDRESS: lea num1(%rip),%rbx computes address of num1 → %rbx
# 3. MODIFY: addl $10,(%rbx) adds 10 through pointer (num1 becomes 210)
# 4. LOAD RESULT: mov num1,%rdi loads 210 into %rdi
# 5. EXIT: syscall terminates with exit code 210

# RIP-RELATIVE ADDRESSING DEEP DIVE:
#
# %rip (Instruction Pointer):
# - Points to the NEXT instruction to execute
# - Automatically updated by CPU as instructions run
# - Cannot be directly read or written in user code
# - Only usable in address calculations: label(%rip)
#
# ENCODING:
# label(%rip) is encoded as a PC-relative offset:
# - Assembler: offset = label_address - next_instruction_address
# - Runtime: effective_address = %rip + offset
#
# BENEFITS:
# 1. Position-independent code (works at any load address)
# 2. Smaller instruction encoding (32-bit offset vs 64-bit absolute)
# 3. Better for CPU branch predictors and caches
# 4. Required for shared libraries
# 5. Compatible with ASLR security
#
# WHEN TO USE:
# - Accessing global variables: mov myvar(%rip),%eax
# - Getting addresses: lea myvar(%rip),%rbx
# - Calling functions: call function(%rip)
# - Jump tables: jmp *table(%rip,%rax,8)
#
# DEFAULT IN MODERN CODE:
# Most compilers generate RIP-relative by default on x86-64
# GCC: -fPIC (position-independent code) uses RIP-relative
# Direct addressing is rare in modern binaries

# LEA INSTRUCTION EXPLAINED:
#
# LEA computes addresses without accessing memory
# "Load Effective Address" is somewhat misleading!
#
# COMMON USES:
#
# 1. Get address (& operator):
#    lea array(%rip),%rsi     # %rsi = &array
#
# 2. Array indexing:
#    lea array(,%rcx,8),%rax  # %rax = &array[rcx]
#
# 3. Pointer arithmetic:
#    lea 16(%rdi),%rsi        # %rsi = rdi + 16
#
# 4. Efficient arithmetic (side effect!):
#    lea (%rdi,%rdi,2),%rax   # %rax = rdi * 3
#    lea 8(%rdi,%rsi,4),%rax  # %rax = 8 + rdi + (rsi*4)
#
# 5. Structure field access:
#    lea 24(%rbx),%rax        # %rax = &(struct->field) if field at offset 24
#
# LEA VS MOV:
# lea label(%rip),%rax    # %rax = address of label (pointer)
# mov label(%rip),%rax    # %rax = value at label (dereference)
#
# LEA never accesses memory!
# It only computes the address that WOULD be accessed

# INDIRECT ADDRESSING MODES:
#
# (%reg)                  - Simple indirect: address in register
#   Example: mov (%rbx),%eax
#   C: eax = *rbx
#
# offset(%reg)            - Indirect with displacement
#   Example: mov 8(%rbx),%eax
#   C: eax = *(rbx + 8) or eax = rbx[2] (if int array)
#
# (%base,%index,scale)    - Base + scaled index
#   Example: mov (%rbx,%rcx,4),%eax
#   C: eax = base[rcx] or eax = *(base + rcx*4)
#
# offset(%base,%index,scale) - Full form
#   Example: mov 16(%rbx,%rcx,8),%rax
#   C: rax = *(rbx + 16 + rcx*8)
#
# THIS PROGRAM USES:
# (%rbx) - Simple indirect addressing
# Address comes from %rbx, which holds &num1

# POSITION-INDEPENDENT CODE (PIC):
#
# PROBLEM: Hardcoded addresses break when code loads at different addresses
#
# Example (non-PIC):
# myvar is at absolute address 0x404000
# mov 0x404000,%eax        # Hardcoded address
# If ASLR loads program at different address, this breaks!
#
# Solution (PIC with RIP-relative):
# mov myvar(%rip),%eax     # Offset-based, not absolute
# Works regardless of load address!
#
# WHY IT MATTERS:
# 1. Shared libraries must work at different addresses in each process
# 2. ASLR randomizes addresses for security
# 3. Modern systems require PIC for security
# 4. Kernel might load program anywhere in address space
#
# x86-64 makes PIC easier:
# - RIP-relative addressing built into ISA
# - 32-bit offsets reach ±2GB from code
# - Most programs fit in this range
# - Older x86-32 needed complex GOT (Global Offset Table)

# ADDRESS SPACE LAYOUT RANDOMIZATION (ASLR):
#
# SECURITY TECHNIQUE:
# Randomize load addresses of:
# - Executable code
# - Shared libraries
# - Stack
# - Heap
#
# DEFENSE AGAINST:
# Buffer overflow exploits that rely on predictable addresses
# Return-to-libc attacks
# ROP (Return-Oriented Programming) chains
#
# EXAMPLE:
# Without ASLR (predictable):
# ./program
# Code at: 0x00400000
# Stack at: 0x7ffffffde000
# libc at: 0x7ffff7a0d000
# (same every time - attacker can hardcode addresses!)
#
# With ASLR (randomized):
# ./program (run 1)
# Code at: 0x555555554000
# Stack at: 0x7fffffffde00
# libc at: 0x7ffff7dd4000
#
# ./program (run 2)
# Code at: 0x555555554000 (PIE not enabled in this example)
# Stack at: 0x7fffffffdf00 (different!)
# libc at: 0x7ffff7dd6000 (different!)
#
# RIP-relative code works in all cases!

# POINTER OPERATIONS IN ASSEMBLY:
#
# C CODE:                    ASSEMBLY:
# int *ptr;                  # (no declaration needed)
# ptr = &var;                lea var(%rip),%rbx
# *ptr = 10;                 movl $10,(%rbx)
# x = *ptr;                  mov (%rbx),%eax
# *ptr += 5;                 addl $5,(%rbx)
# ptr++;                     add $4,%rbx (for int*, 4 bytes)
# ptr += 2;                  add $8,%rbx (for int*, 2*4 bytes)
# ptr[i] = x;                mov %eax,(%rbx,%rcx,4)
#
# POINTER ARITHMETIC:
# In C, ptr++ advances by sizeof(*ptr)
# In assembly, must manually multiply by size:
# - byte pointer: add $1
# - int pointer (4 bytes): add $4
# - long pointer (8 bytes): add $8

# EQUIVALENT C CODE:
#
# int num1 = 200;
#
# int main() {
#     int *ptr = &num1;     // lea instruction
#     *ptr += 10;           // addl through pointer
#     return num1;
# }
#
# Or more explicitly matching the assembly:
# int num1 = 200;
#
# int _start() {
#     int *rbx;
#     rbx = &num1;          // lea num1(%rip),%rbx
#     *rbx = *rbx + 10;     // addl $10,(%rbx)
#     int rdi = num1;       // mov num1,%rdi
#     exit(rdi);            // mov $60,%rax; syscall
# }

# COMPARISON: DIRECT VS INDIRECT:
#
# DIRECT (02-data-section.s):
# addl $10,num1
# - Simpler
# - Fewer instructions
# - Variable name hardcoded
# - Less flexible
#
# INDIRECT (this program):
# lea num1(%rip),%rbx
# addl $10,(%rbx)
# - More instructions
# - Uses a register
# - Can change which variable to access
# - More flexible
# - Demonstrates pointer concept
# - Foundation for more complex programs

# TO ASSEMBLE, LINK, AND RUN:
# as 03-data-types.s -o 03-data-types.o
# ld 03-data-types.o -o 03-data-types
# ./03-data-types
# echo $?
# (outputs: 210, same as 02-data-section.s)

# DEBUGGING WITH GDB:
# gdb ./03-data-types
# (gdb) break _start
# (gdb) run
# (gdb) stepi                    # Execute lea instruction
# (gdb) print/x $rbx             # Show address in %rbx
# (gdb) print &num1              # Show address of num1
# (gdb) print/d num1             # Show value of num1
# (outputs: 200)
# (gdb) stepi                    # Execute addl instruction
# (gdb) print/d num1             # Show value again
# (outputs: 210)
# (gdb) x/dw $rbx                # Examine memory at %rbx
# (outputs: 210)

# EXPERIMENTS TO TRY:
#
# 1. Verify RIP-relative addressing:
#    (gdb) disassemble _start
#    Look for: lea    0x... (%rip),%rbx
#    The 0x... is the offset from %rip to num1
#
# 2. Compare with direct addressing:
#    Change: lea num1(%rip),%rbx
#    To:     lea num1,%rbx
#    Assemble and compare object files
#
# 3. Use different pointer operations:
#    mov $5,(%rbx)              # *ptr = 5
#    mov (%rbx),%eax            # eax = *ptr
#    add %eax,(%rbx)            # *ptr += eax
#
# 4. Multiple indirections:
#    lea num1(%rip),%rbx        # rbx = &num1
#    lea num1(%rip),%rcx        # rcx = &num1
#    mov (%rbx),%eax            # eax = *rbx
#    mov %eax,(%rcx)            # *rcx = eax (copies num1 to num1!)

# COMMON MISTAKES:
#
# 1. Forgetting parentheses:
#    addl $10,%rbx              # WRONG: adds to pointer, not to value
#    FIX: addl $10,(%rbx)       # Correct: dereference pointer
#
# 2. LEA vs MOV confusion:
#    mov num1(%rip),%rbx        # Gets VALUE (200), not address
#    FIX: lea num1(%rip),%rbx   # Gets ADDRESS (pointer)
#
# 3. Size mismatch:
#    lea num1(%rip),%rbx        # %rbx = address
#    addq $10,(%rbx)            # WRONG: 64-bit add to 32-bit data
#    FIX: addl $10,(%rbx)       # Match .long (32-bit)
#
# 4. Uninitialized pointer:
#    (forget lea instruction)
#    addl $10,(%rbx)            # WRONG: %rbx contains garbage
#    FIX: Initialize with lea first
#
# 5. Pointer arithmetic confusion:
#    lea num1(%rip),%rbx        # %rbx points to 4-byte int
#    add $1,%rbx                # Advances by 1 BYTE
#    addl $10,(%rbx)            # WRONG: now pointing to wrong location
#    FIX: Be careful with pointer arithmetic sizes

# ADVANCED: WHY USE LEA + INDIRECT?
#
# This program could be simpler with direct addressing.
# So why use pointers?
#
# PREPARES FOR:
# 1. Arrays - same code for different elements
# 2. Function parameters - pass addresses
# 3. Data structures - navigate linked lists, trees
# 4. Dynamic allocation - work with malloc'd memory
# 5. System calls - pass buffers to kernel
#
# EXAMPLE: Same code, different targets:
# lea var1(%rip),%rbx
# addl $10,(%rbx)            # Modifies var1
#
# lea var2(%rip),%rbx
# addl $10,(%rbx)            # Modifies var2 (same code!)
#
# This is the foundation of all pointer-based programming!
