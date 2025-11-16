# PEDAGOGICAL PURPOSE:
# This is a minimal x86-64 assembly program that demonstrates basic arithmetic
# and system calls without using the C library.
# Key learning objectives:
# 1. Understanding x86-64 assembly syntax and structure
# 2. The mov instruction for moving data between registers
# 3. The add instruction for arithmetic operations
# 4. Direct system calls using syscall instruction
# 5. The exit system call (number 60) and its return value
# 6. Program sections (.text) and entry points (_start)

        .section .text  # CODE SECTION: All executable code goes here
                        # .section directive defines different parts of the program
                        # .text is the standard section for code
                        # This section is read-only and executable

        .global _start  # GLOBAL SYMBOL: Make _start visible to the linker
                        # .global (or .globl) exports this symbol
                        # The linker needs _start as the entry point
                        # For standalone programs (not using C library),
                        # _start is the default entry point (not main)

_start:                 # ENTRY POINT: Program execution begins here
                        # Label marking the start of our program
                        # When the OS loads this program, it jumps to _start

        mov $11,%rdi            # MOVE IMMEDIATE TO REGISTER: Put 11 into %rdi
                                # mov destination, source (AT&T syntax)
                                # $ prefix means immediate value (literal number)
                                # %rdi is a 64-bit general-purpose register
                                # After this: %rdi = 11
                                # puts the number 11 in %rdi

        add %rdi,%rdi           # ADD REGISTER TO ITSELF: %rdi = %rdi + %rdi
                                # add source, destination
                                # destination = destination + source
                                # This effectively doubles %rdi: 11 + 11 = 22
                                # After this: %rdi = 22
                                # adds %rdi to itself and stores the result in %rdi
                                ## add r1,r2 ----> r2 = r2+r1 -----> r2 += r1
                                #
                                # WHY THIS SYNTAX?
                                # In AT&T syntax: instruction source, destination
                                # The destination is modified, source stays the same
                                # Think of it as: destination += source

        mov $60,%rax            # SETUP EXIT SYSCALL: Put 60 into %rax
                                # System call number 60 = exit
                                # %rax holds the system call number
                                # The syscall instruction looks at %rax to determine
                                # which system call to execute
                                # After this: %rax = 60

        syscall                 # EXECUTE SYSTEM CALL: Invoke the kernel
                                # syscall transfers control to the operating system
                                # The OS looks at %rax (60) to determine this is exit
                                # For exit, the OS also looks at %rdi for the exit code
                                # %rdi = 22, so the program exits with status 22
                                #
                                # System call convention (x86-64 Linux):
                                # - %rax: system call number
                                # - %rdi: first argument
                                # - %rsi: second argument
                                # - %rdx: third argument
                                # - %r10: fourth argument
                                # - %r8: fifth argument
                                # - %r9: sixth argument
                                #
                                # For exit syscall:
                                # - %rax = 60 (exit system call number)
                                # - %rdi = exit status code (0-255)

# WHAT THIS PROGRAM DOES:
# 1. Loads 11 into %rdi
# 2. Adds %rdi to itself (%rdi becomes 22)
# 3. Exits with status code 22

# TO ASSEMBLE AND RUN:
# as add1.s -o add1.o          # Assemble to object file
# ld add1.o -o add1            # Link to create executable
# ./add1                       # Run the program
# echo $?                      # Check exit status (should be 22)

# EXPECTED EXIT STATUS:
# 22 (11 + 11)

# KEY CONCEPTS:
#
# 1. REGISTERS:
#    - %rax: Accumulator register, also holds syscall numbers and return values
#    - %rdi: First argument register (also used for exit status)
#    - Registers are the CPU's fastest storage (faster than RAM)
#    - x86-64 has 16 general-purpose 64-bit registers
#
# 2. IMMEDIATE VALUES:
#    - $11 means the literal number 11
#    - Without $, it would mean memory address 11
#    - $ distinguishes values from addresses
#
# 3. AT&T SYNTAX:
#    - Source comes before destination: mov source, dest
#    - Registers have % prefix: %rax, %rdi
#    - Immediates have $ prefix: $11, $60
#    - Intel syntax is opposite: mov dest, source (no prefixes)
#
# 4. SYSTEM CALLS:
#    - Way for programs to request OS services
#    - Exit is syscall 60 on Linux x86-64
#    - Different from C library functions
#    - Direct communication with kernel

# REGISTER SUMMARY:
# After execution:
#   %rax = 60 (syscall number for exit)
#   %rdi = 22 (result of 11 + 11, also the exit code)

# COMPARISON TO C:
# This program is equivalent to:
#   int main() {
#       int rdi = 11;
#       rdi = rdi + rdi;  // rdi is now 22
#       return rdi;       // exit with status 22
#   }

# WHY WRITE IN ASSEMBLY?
# - Learn how computers work at the lowest level
# - Understand what compilers generate from C code
# - Performance-critical code optimization
# - Systems programming and OS development
# - Reverse engineering and security research
# - Full control over hardware
