# PEDAGOGICAL PURPOSE:
# This program demonstrates accessing and modifying global variables in assembly.
# It introduces position-independent code and RIP-relative addressing.
# Key learning objectives:
# 1. Understanding the .data section for initialized global variables
# 2. RIP-relative addressing for accessing globals in position-independent code
# 3. Direct memory operations (adding to memory without loading to register first)
# 4. The difference between the address of a variable and its value
# 5. How global variables persist in memory throughout program execution

.section .data               # DATA SECTION: Initialized global/static variables go here
                            # This section is readable and writable at runtime
                            # Data here persists for the entire program execution

num:    .quad 10            # GLOBAL VARIABLE: Define a 64-bit integer initialized to 10
                            # .quad means "quadword" (8 bytes = 64 bits)
                            # 'num' is a label that represents the memory address
                            # At that address, we store the value 10
                            # WHY .quad? To match the size of our registers (%rdi, %rax, etc.)

        .section .text      # CODE SECTION: Switch to executable code section
        .global _start      # Make _start visible to linker

_start:                     # PROGRAM ENTRY POINT

        addq $10,num(%rip)  # ADD TO MEMORY: Add 10 directly to the value at num
                            # This is a DIRECT MEMORY OPERATION - we modify memory
                            # without first loading into a register
                            #
                            # BREAKING DOWN THE SYNTAX:
                            # - addq: add quadword (64-bit)
                            # - $10: immediate value (the number 10)
                            # - num(%rip): RIP-relative addressing
                            #
                            # RIP-RELATIVE ADDRESSING EXPLAINED:
                            # %rip = instruction pointer (program counter)
                            # num(%rip) means: "the address of num relative to current instruction"
                            # WHY? For position-independent code (PIE/PIC)
                            # - The absolute address of num might be anywhere in memory
                            # - But the OFFSET from the current instruction is fixed
                            # - This allows the code to run at any address (ASLR security)
                            #
                            # WHAT HAPPENS:
                            # 1. CPU calculates: address = current %rip + offset_to_num
                            # 2. CPU reads the 64-bit value at that address (10)
                            # 3. CPU adds 10 to it (result: 20)
                            # 4. CPU writes 20 back to that address
                            #
                            # After this instruction: num contains 20

        movq num(%rip), %rdi # LOAD FROM MEMORY: Load the value at num into %rdi
                            # Again using RIP-relative addressing
                            # We're reading the MODIFIED value (20)
                            # %rdi now contains 20
                            #
                            # WHY move to %rdi?
                            # - We want to use this as the exit code
                            # - %rdi holds the first argument for syscalls
                            # - For exit, that's the status code

        movq $60, %rax      # SYSCALL NUMBER: 60 = exit
                            # Preparing to terminate the program

        syscall             # EXIT: Terminate with status 20
                            # The exit code will be 20 (the modified value of num)
                            # Verify with: ./global1; echo $?
                            # Should display 20

# MEMORY LAYOUT:
#
# When this program loads into memory:
#
# .text section (read-only, executable):
#   _start:
#     addq $10,num(%rip)
#     movq num(%rip),%rdi
#     ...
#
# .data section (read-write, not executable):
#   num: [0x0A 0x00 0x00 0x00 0x00 0x00 0x00 0x00]  (10 in little-endian)
#        ^
#        └─ This is the address labeled 'num'
#
# After addq instruction:
#   num: [0x14 0x00 0x00 0x00 0x00 0x00 0x00 0x00]  (20 in little-endian)

# RIP-RELATIVE ADDRESSING IN DEPTH:
#
# Traditional absolute addressing (not used here):
#   movq num, %rdi     # Would use absolute address (not position-independent)
#
# RIP-relative addressing (what we use):
#   movq num(%rip), %rdi  # Uses offset from current instruction
#
# Example calculation:
#   If current instruction is at address 0x401000
#   And num is at address 0x402000
#   Then the assembler encodes offset = 0x402000 - 0x401000 = 0x1000
#   At runtime, CPU computes: 0x401000 + 0x1000 = 0x402000
#
# WHY this matters:
#   - With ASLR, the actual addresses change each run
#   - But the RELATIVE offset stays constant
#   - So the code works regardless of where it's loaded

# COMPARISON WITH C:
#
# This assembly is roughly equivalent to:
#
# long num = 10;           // .data section, initialized global
#
# int main() {
#     num += 10;          // addq $10,num(%rip)
#     return num;         // movq num(%rip),%rdi; exit syscall
# }

# TO ASSEMBLE, LINK, AND RUN:
# as global1.s -o global1.o
# ld global1.o -o global1
# ./global1
# echo $?                  # Should show 20

# KEY CONCEPTS DEMONSTRATED:
#
# 1. GLOBAL VARIABLES: Stored in .data section, persist throughout execution
# 2. DIRECT MEMORY OPERATIONS: Can modify memory without using registers
# 3. RIP-RELATIVE ADDRESSING: Essential for position-independent code
# 4. INITIALIZATION: .quad 10 initializes the variable before program starts
# 5. MEMORY VS REGISTERS: num is in memory, %rdi/%rax are registers
