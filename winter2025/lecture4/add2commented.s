# PEDAGOGICAL PURPOSE:
# This program demonstrates arithmetic operations (subtraction) and operand size suffixes
# in x86-64 assembly. It introduces the fundamental concept of register sizing and the
# AT&T syntax for arithmetic instructions.
#
# Key learning objectives:
# 1. Understanding operand size suffixes (q, l, w, b) and what they mean
# 2. Learning the AT&T syntax for binary operations (source, destination)
# 3. Understanding that destination must be a register or memory location
# 4. Seeing how computation results need to be transferred to the correct register
#    for system calls (calling convention requirements)
# 5. Understanding subtraction: destination = destination - source
# 6. Introduction to the exit syscall and its parameter passing requirements
#
# CONCEPTUAL OVERVIEW:
# This program implements: return (2 - 1);
# It performs: %rdx = %rdx - %rax (2 - 1 = 1)
# Then exits with code 1 to demonstrate the result

        .text                      # CODE SECTION: All executable instructions
        .global _start             # GLOBAL DIRECTIVE: Entry point visible to linker

        ## OPERAND SIZE SUFFIXES EXPLAINED:
        ## q - quad word (64 bits / 8 bytes) - Full register like %rax, %rbx
        ## l - long word (32 bits / 4 bytes) - Lower 32 bits like %eax, %ebx
        ## w - word (16 bits / 2 bytes)      - Lower 16 bits like %ax, %bx
        ## b - byte (8 bits / 1 byte)        - Lower 8 bits like %al, %bl
        ##
        ## ARITHMETIC INSTRUCTION SYNTAX:
        ## add/sub source,destination
        ## This means: destination = destination OP source
        ##
        ## SOURCE OPERAND can be:
        ##   - An immediate value (like $1, $100)
        ##   - A register (like %rax, %rbx)
        ##   - A memory location (like (%rbx), -8(%rbp))
        ##
        ## DESTINATION OPERAND must be:
        ##   - A register (like %rax, %rbx)
        ##   - A memory location (like (%rbx), -8(%rbp))
        ##   - BUT NOT an immediate value!
        ##
        ## WHY THIS RESTRICTION?
        ## The CPU needs a place to store the result. You can't store into
        ## a constant like $5 - it's not a storage location.

_start:
        mov $1,%rax                # LOAD FIRST VALUE: %rax = 1
                                  # This will be our subtrahend (what we subtract)
                                  # mov with $ means "move immediate value"
                                  # No size suffix needed - assembler infers 'q' from %rax

        mov $2,%rdx                # LOAD SECOND VALUE: %rdx = 2
                                  # This will be our minuend (what we subtract from)
                                  # We're setting up to compute 2 - 1

        sub %rax,%rdx              # SUBTRACT: %rdx = %rdx - %rax (2 - 1 = 1)
                                  # AT&T SYNTAX REMINDER: sub source,dest -> dest = dest - source
                                  # This is DIFFERENT from Intel syntax!
                                  # Intel would be: sub dest,source
                                  #
                                  # WHAT HAPPENS:
                                  # - Read value from %rax (1)
                                  # - Read value from %rdx (2)
                                  # - Compute 2 - 1 = 1
                                  # - Store result (1) back into %rdx
                                  # - %rax is unchanged (still 1)
                                  # - %rdx now contains 1

        mov %rdx,%rdi              # TRANSFER TO EXIT CODE REGISTER: %rdi = %rdx (1)
                                  # WHY THIS EXTRA MOVE?
                                  # The x86-64 calling convention requires the exit code
                                  # to be passed in %rdi, not %rdx
                                  # We stored our computation result in %rdx, but
                                  # the syscall to exit needs the value in %rdi
                                  #
                                  # THIS IS THE CALLING CONVENTION:
                                  # System calls expect arguments in specific registers:
                                  # - %rdi: first argument (exit code for exit syscall)
                                  # - %rsi: second argument
                                  # - %rdx: third argument
                                  # - and so on...
                                  #
                                  # We MUST follow this convention or the syscall
                                  # won't receive the correct parameters

        mov $60,%rax               # SYSCALL NUMBER: 60 = exit
                                  # Linux system call numbers are defined in the kernel
                                  # exit is syscall number 60
                                  # We put the syscall number in %rax before calling syscall

        syscall                    # INVOKE KERNEL: Call the exit syscall
                                  # This executes exit(%rdi) where %rdi = 1
                                  # The program terminates with exit code 1
                                  # You can verify with: ./add2; echo $?
                                  # The shell will display: 1

# DETAILED OPERAND SIZE EXAMPLES:
#
# If we used different size suffixes:
#   subb %al,%dl     - Subtract 8-bit values (byte)
#   subw %ax,%dx     - Subtract 16-bit values (word)
#   subl %eax,%edx   - Subtract 32-bit values (long)
#   subq %rax,%rdx   - Subtract 64-bit values (quad)
#
# NOTE: When you use a 32-bit register like %eax, the upper 32 bits
# of the corresponding 64-bit register (%rax) are automatically ZEROED.
# This is a special x86-64 behavior for compatibility.
#
# When you use 8-bit or 16-bit registers, the upper bits are UNCHANGED.

# REGISTER HIERARCHY VISUAL:
#
# 64-bit: %rax
#         ├─ 32-bit: %eax (lower 32 bits)
#         │  ├─ 16-bit: %ax (lower 16 bits)
#         │  │  ├─ 8-bit: %al (lower 8 bits)
#         │  │  └─ 8-bit: %ah (bits 8-15)
#
# Same pattern for %rbx/%ebx/%bx/%bl/%bh, %rcx/%ecx/%cx/%cl/%ch, %rdx/%edx/%dx/%dl/%dh
# And for %rsi/%esi/%si/%sil, %rdi/%edi/%di/%dil (no 'h' version for these)

# WHY MULTIPLE SIZES?
# 1. Compatibility - supports 8086, 286, 386, and x86-64 code
# 2. Efficiency - smaller operations can be faster for byte/word data
# 3. Memory savings - when working with arrays of small integers
# 4. C compatibility - char (8-bit), short (16-bit), int (32-bit), long (64-bit)

# CALLING CONVENTION SUMMARY FOR SYSCALLS:
# Register    Purpose
# %rax        Syscall number (input) / Return value (output)
# %rdi        Argument 1
# %rsi        Argument 2
# %rdx        Argument 3
# %r10        Argument 4
# %r8         Argument 5
# %r9         Argument 6
#
# For exit syscall:
#   %rax = 60 (syscall number)
#   %rdi = exit code

# ARITHMETIC OPERATIONS AVAILABLE:
# add  - Addition:        dest = dest + source
# sub  - Subtraction:     dest = dest - source
# imul - Multiplication:  dest = dest * source (signed)
# mul  - Multiplication:  (unsigned, different syntax)
# idiv - Division:        (signed, uses %rax and %rdx)
# div  - Division:        (unsigned)
# inc  - Increment:       dest = dest + 1
# dec  - Decrement:       dest = dest - 1
# neg  - Negate:          dest = -dest

# TO ASSEMBLE, LINK, AND RUN:
# as add2.s -o add2.o              # Assemble to object file
# ld add2.o -o add2                 # Link to executable
# ./add2                            # Run program
# echo $?                           # Check exit status (should be 1)

# EXPERIMENT:
# Try changing the values:
#   mov $10,%rax
#   mov $25,%rdx
#   sub %rax,%rdx
# Then reassemble and run. The exit code should be 15 (25 - 10).
