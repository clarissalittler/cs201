# PEDAGOGICAL PURPOSE:
# This program demonstrates working with global variables stored in the .data section
# and introduces position-independent code (PIC) using RIP-relative addressing.
# It performs the same computation as for1.s but stores the accumulator in memory.
#
# Key learning objectives:
# 1. Understanding the .data section for initialized global variables
# 2. RIP-relative addressing: (%rip) for position-independent code
# 3. Memory operands in arithmetic instructions
# 4. The difference between registers and memory locations
# 5. Why modern code uses (%rip) instead of absolute addresses
# 6. Working with .quad (64-bit) data declarations
# 7. The performance difference between register and memory operations
#
# CONCEPTUAL OVERVIEW:
# This implements:
#   long num = 0;      // Global variable in .data section
#   long num2 = 0;     // Another global variable
#   for (int i = 1; i < 11; i++) {
#       num += i;      // Memory operation!
#       num2++;        // Count iterations
#   }
#   return num;        // Returns 55

        .section .data          # DATA SECTION: Initialized global/static variables
                               # This section is loaded into memory as read-write data
                               # Unlike .text (read-only code) or .bss (uninitialized)

num:    .quad 0                # DEFINE GLOBAL VARIABLE: 64-bit integer, initial value 0
                               # .quad = 8 bytes (64 bits) = size of a long in C
                               # 'num' is the label (symbol) for this memory location
                               # This is equivalent to: long num = 0;
                               # The assembler/linker will assign this a memory address

        ## i.e. long num = 0;
        # ^^^ HELPFUL COMMENT: Shows C equivalent

num2:   .quad 0                # DEFINE SECOND GLOBAL: long num2 = 0;
                               # This will be stored at address (num + 8)
                               # Used to count loop iterations in this program

        # MEMORY LAYOUT IN .data SECTION:
        # If .data starts at address 0x404000:
        #   num:  0x404000-0x404007 (8 bytes)
        #   num2: 0x404008-0x40400f (8 bytes)

        .section .text          # CODE SECTION: Executable instructions

        .global _start         # GLOBAL DIRECTIVE: Entry point

        # ORIGINAL COMMENTS:
        ## let's have a loop that adds all the numbers between 1 and 10 and puts that as the exit code

_start:
        # ORIGINAL COMMENTS:
        ## let's store our computations in %r8
        ## where do we store the index of our for-loop?
        ## By convention! Use %rcx for that
        ## for(%rcx = 1; %rcx<11; %rcx++){
        ##   num = num + %rcx
        ## }

        # LOOP INITIALIZATION:
        mov $1,%rcx            # INITIALIZE LOOP COUNTER: %rcx = 1
                               # Same as for1.s: %rcx is the loop index
                               # NOTE: We do NOT initialize num here!
                               # num is already initialized to 0 in the .data section
                               # Unlike for1.s, we don't use %r8 for accumulation

loop:                          # LOOP LABEL: Entry point for each iteration

        #while loop style, tests before the loop body
        # ^^^ Same loop style as for1.s: test condition first

        cmp $11,%rcx           # TEST LOOP CONDITION: Is %rcx >= 11?
                               # Same termination condition as for1.s

        jge finish             # CONDITIONAL EXIT: Exit loop if %rcx >= 11
                               # Same loop exit logic as for1.s

        # LOOP BODY WITH MEMORY OPERATIONS:

        add %rcx,num(%rip)     # MEMORY OPERATION: num = num + %rcx
                               # THE KEY INSTRUCTION: Add to a memory location!
                               # This is MORE COMPLEX than "add %rcx,%r8"
                               #
                               # ADDRESSING MODE: num(%rip) = RIP-relative
                               # %rip = Instruction Pointer (program counter)
                               # num(%rip) = address of num relative to current instruction
                               # The CPU calculates: address = %rip + offset_to_num
                               #
                               # WHY RIP-RELATIVE?
                               # This enables POSITION-INDEPENDENT CODE (PIC)
                               # The code can be loaded at any memory address
                               # Required for: shared libraries, ASLR security, PIE executables
                               #
                               # WHAT THE CPU DOES:
                               # 1. Calculate address: addr = %rip + offset
                               # 2. Read value from memory: temp = [addr]
                               # 3. Add register: temp = temp + %rcx
                               # 4. Write back to memory: [addr] = temp
                               #
                               # This is SLOWER than register operations!
                               # Memory access takes many CPU cycles
                               # Registers are accessed in 1 cycle

        #???? is the (%rip)
        # ^^^ ORIGINAL COMMENT: Student question about (%rip) syntax!
        # This is what this commented version aims to explain

        addq $1,num2(%rip)     # MEMORY INCREMENT: num2 = num2 + 1
                               # Another RIP-relative memory operation
                               # addq = "add quad-word" (64-bit)
                               # The 'q' suffix specifies operand size (necessary here!)
                               # Without 'q', assembler wouldn't know if num2 is byte/word/long/quad
                               #
                               # This increments the iteration counter in memory
                               # Equivalent to: num2++; in C
                               #
                               # WHY THE 'q' SUFFIX?
                               # With registers, size is implied: %rcx is 64-bit
                               # With memory, we must specify: b(byte), w(word), l(long), q(quad)
                               # addq = 64-bit add
                               # addl = 32-bit add
                               # addw = 16-bit add
                               # addb = 8-bit add

        add $1,%rcx            # INCREMENT LOOP COUNTER: %rcx = %rcx + 1
                               # Same as for1.s: advance to next iteration

        jmp loop               # UNCONDITIONAL JUMP: Repeat loop
                               # Same as for1.s: create the loop

        # LOOP EXIT:
finish:
        mov num(%rip),%rdi     # LOAD RETURN VALUE: %rdi = num
                               # Read the final sum from memory
                               # RIP-relative addressing again
                               # This transfers the result from memory to register
                               # for passing to the exit syscall

        mov $60,%rax           # SYSCALL NUMBER: exit() = 60

        syscall                # INVOKE KERNEL: Execute exit(55)
                               # Program terminates with exit code 55
                               # Same result as for1.s, but computed in memory!

# RIP-RELATIVE ADDRESSING EXPLAINED:
# Modern x86-64 code uses RIP-relative addressing for globals
#
# What is %rip?
#   %rip = Instruction Pointer (also called Program Counter)
#   It holds the address of the NEXT instruction to execute
#   It advances automatically as instructions execute
#
# Why RIP-relative?
#   1. POSITION-INDEPENDENT CODE (PIC): Code works at any address
#   2. ASLR: Address Space Layout Randomization (security feature)
#   3. SHARED LIBRARIES: Multiple processes share same code
#   4. SMALLER CODE: Relative offset is 32-bit, absolute address is 64-bit
#
# How does num(%rip) work?
#   The assembler calculates: offset = address_of_num - address_of_instruction
#   The instruction encodes this offset
#   At runtime: effective_address = %rip + offset
#   The offset is constant, but %rip varies with load address
#
# Example:
#   If code loaded at 0x400000:
#     Instruction at: 0x401050
#     num at:        0x404000
#     offset =       0x404000 - 0x401050 = 0x2fb0
#   If code loaded at 0x600000 (different address!):
#     Instruction at: 0x601050
#     num at:        0x604000
#     offset =       0x604000 - 0x601050 = 0x2fb0 (SAME!)
#   The offset is constant regardless of load address!

# ALTERNATIVE ADDRESSING MODES:
# num(%rip)         - RIP-relative (modern, position-independent)
# num               - Absolute addressing (old style, not PIC)
# (%rax)            - Indirect: use address in %rax
# 8(%rax)           - Indirect with offset: address = %rax + 8
# (%rax,%rbx)       - Indexed: address = %rax + %rbx
# (%rax,%rbx,4)     - Scaled indexed: address = %rax + %rbx*4
# 8(%rax,%rbx,4)    - Full form: address = %rax + %rbx*4 + 8

# MEMORY VS REGISTER OPERATIONS:
# Register to register:
#   add %rcx,%r8              - FAST (1 cycle)
#   Both operands in CPU registers
#
# Immediate to memory:
#   addq $1,num(%rip)         - SLOW (many cycles)
#   Must: read memory, add, write memory
#   Memory access goes through cache hierarchy
#
# Register to memory:
#   add %rcx,num(%rip)        - SLOW (many cycles)
#   Must: read memory, add to register, write memory
#
# This is why for1.s (register-only) is faster than for2.s!

# DATA SECTION DIRECTIVES:
# .quad value    - 64-bit (8 bytes) - long, pointer
# .long value    - 32-bit (4 bytes) - int
# .word value    - 16-bit (2 bytes) - short
# .byte value    - 8-bit (1 byte)   - char
#
# .ascii "text"  - String without null terminator
# .asciz "text"  - String with null terminator (same as .string)
# .string "text" - String with null terminator
#
# .space n       - Reserve n bytes of uninitialized space
# .align n       - Align next item to n-byte boundary

# SECTION TYPES:
# .text   - Executable code (read-only, executable)
# .data   - Initialized global/static variables (read-write)
# .rodata - Read-only data (constants, string literals)
# .bss    - Uninitialized global/static variables (read-write, zero-initialized)
#
# .bss is special: doesn't take space in the executable file
# OS zero-fills .bss when program loads

# WHY USE MEMORY INSTEAD OF REGISTERS?
# 1. More variables than registers (x86-64 has ~16 general registers)
# 2. Variables must persist across function calls
# 3. Large data structures (arrays, structs) don't fit in registers
# 4. Pointers: need address, registers don't have addresses
# 5. Sharing data between functions
# 6. Thread-local storage
#
# But registers are MUCH faster, so compilers try to use them when possible

# COMPARISON: for1.s vs for2.s
# for1.s:
#   - Accumulator in %r8 (register)
#   - Faster execution
#   - No global variables
#   - Can't observe intermediate values from outside
#
# for2.s:
#   - Accumulator in num (memory)
#   - Slower execution (memory access overhead)
#   - Uses global variable
#   - Debugger can watch 'num' change during loop
#   - Demonstrates real-world pattern

# EQUIVALENT C CODE:
# long num = 0;      // .data section
# long num2 = 0;
#
# int main() {
#     for (int i = 1; i < 11; i++) {
#         num += i;
#         num2++;
#     }
#     return num;    // Returns 55
# }

# TO ASSEMBLE, LINK, AND RUN:
# as for2.s -o for2.o
# ld for2.o -o for2
# ./for2
# echo $?                      # Should display 55
#
# TO EXAMINE GLOBAL VARIABLES:
# objdump -s -j .data for2.o   # Show .data section contents
# nm for2.o                    # Show symbols (num, num2, _start)
# gdb for2                     # Debug and watch 'num' change in loop
