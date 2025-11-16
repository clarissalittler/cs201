# PEDAGOGICAL PURPOSE:
# This program introduces the .data section and demonstrates how to declare and
# modify initialized variables in assembly. It covers data size specifiers,
# the distinction between .data and .text sections, and instruction size suffixes.
# This is the foundation for understanding how global and static variables work
# at the hardware level.
#
# Key learning objectives:
# 1. Understanding the purpose and structure of the .data section
# 2. Learning the .long directive for 32-bit integers
# 3. Understanding how variables persist for program lifetime
# 4. Learning instruction size suffixes (b, w, l, q)
# 5. Understanding direct memory addressing (no indexing)
# 6. Learning when size suffixes are required vs optional
# 7. Understanding implicit zero-extension of 32-bit operations
# 8. Seeing how assembly differs from high-level variable declarations
# 9. Understanding the memory layout of initialized data
# 10. Learning to modify memory directly from instructions
#
# CONCEPTUAL OVERVIEW:
# This program demonstrates the simplest possible use of the .data section:
# 1. Declare a 32-bit variable (num1) initialized to 200
# 2. Add 10 to that variable
# 3. Exit with the result (210)
#
# In C terms:
#   int num1 = 200;
#   num1 += 10;
#   return num1;
#
# MEMORY LAYOUT:
#
# .data section (in memory):
# ┌─────────────────┐
# │ num1: 200       │  <- 4 bytes (32-bit integer)
# │   (0x000000C8)  │
# └─────────────────┘
#
# After adding 10:
# ┌─────────────────┐
# │ num1: 210       │  <- Modified in place
# │   (0x000000D2)  │
# └─────────────────┘

        .section .data             # DATA SECTION: Initialized writable data
                                  # This section contains variables with initial values
                                  # Key characteristics:
                                  # - Writable (unlike .rodata which is read-only)
                                  # - Initialized at program load time
                                  # - Persists for entire program lifetime
                                  # - Located in data segment of memory
                                  # - Each process gets its own copy
                                  #
                                  # SECTION COMPARISON:
                                  # .text   - Code (executable, read-only)
                                  # .rodata - Constants (read-only data)
                                  # .data   - Initialized variables (read-write)
                                  # .bss    - Uninitialized variables (read-write, zero-filled)

num1:   .long 200                  # VARIABLE DECLARATION: 32-bit integer = 200
                                  # num1 is a LABEL that marks the address
                                  # .long specifies size: 4 bytes (32 bits)
                                  # 200 is the initial value
                                  #
                                  # SIZE DIRECTIVES:
                                  # .byte  - 1 byte  (8 bits)   - char
                                  # .word  - 2 bytes (16 bits)  - short
                                  # .long  - 4 bytes (32 bits)  - int
                                  # .quad  - 8 bytes (64 bits)  - long
                                  #
                                  # MEMORY REPRESENTATION:
                                  # Value 200 = 0x000000C8 in hex
                                  # Stored in little-endian:
                                  # Address:  num1   num1+1 num1+2 num1+3
                                  # Byte:     C8     00     00     00
                                  #
                                  # WHY .long?
                                  # - Matches C 'int' on most systems
                                  # - 4 bytes is efficient for arithmetic
                                  # - Adequate range: -2,147,483,648 to 2,147,483,647
                                  #
                                  # LIFETIME:
                                  # This variable exists for the entire program execution
                                  # It's not on the stack (no automatic cleanup)
                                  # Similar to C global or static variables

        ## .long means 32 bit       # COMMENT: Size clarification
                                  # This is a reminder that .long = 32 bits
                                  # Not to be confused with C "long" which is 64-bit on many systems
                                  # Assembly .long is always 32 bits regardless of platform

        .section .text             # CODE SECTION: Executable instructions
        .global _start             # GLOBAL DIRECTIVE: Make _start visible to linker

_start:
        addl $10,num1              # ADD TO MEMORY: num1 += 10
                                  # This adds 10 directly to the memory location
                                  # Result: 200 + 10 = 210, stored back in num1
                                  #
                                  # INSTRUCTION BREAKDOWN:
                                  # addl - ADD Long (32-bit operation)
                                  # $10 - Immediate value (the constant 10)
                                  # num1 - Memory address (the variable location)
                                  #
                                  # SIZE SUFFIX 'l':
                                  # The 'l' is REQUIRED here because:
                                  # - num1 is just a label (assembler doesn't know size)
                                  # - Immediate $10 could be any size
                                  # - We must explicitly say "32-bit operation"
                                  #
                                  # WHY 'l' IS NEEDED:
                                  # Without suffix, assembler can't determine:
                                  # - Should it add to all 4 bytes? (addl)
                                  # - Or just 1 byte? (addb)
                                  # - Or 2 bytes? (addw)
                                  # The 'l' removes ambiguity
                                  #
                                  # MEMORY OPERATION:
                                  # This is a READ-MODIFY-WRITE operation:
                                  # 1. CPU reads 4 bytes from address num1 (value: 200)
                                  # 2. CPU adds 10 to the value (result: 210)
                                  # 3. CPU writes 4 bytes back to num1 (stores: 210)
                                  #
                                  # x86 ADVANTAGE:
                                  # x86 allows arithmetic directly on memory
                                  # Many RISC architectures require:
                                  #   load num1, register
                                  #   add 10, register
                                  #   store register, num1
                                  # x86 does all three in ONE instruction!

        ## need the l so that it understands that it's adding into a 32bit number
        ## l means long
        ## usually we can leave these off
        ## because the assembler can figure out
        ## if it's using the q (64 bit), l (32bit), w (16 bit), b (8 bit)
        ## q - 8 bytes, l - 4 bytes, w - 2 bytes, b - 1 byte
                                  # COMMENT EXPLANATION:
                                  # These comments explain why the 'l' suffix is needed
                                  #
                                  # WHEN SUFFIXES ARE OPTIONAL:
                                  # When using registers, assembler can infer size:
                                  #   mov %eax,%ebx    -> movl (both are 32-bit registers)
                                  #   mov %rax,%rbx    -> movq (both are 64-bit registers)
                                  #   mov %al,%bl      -> movb (both are 8-bit registers)
                                  #
                                  # WHEN SUFFIXES ARE REQUIRED:
                                  # When operand size is ambiguous:
                                  #   add $10,num1     -> ERROR! How many bytes?
                                  #   addl $10,num1    -> OK! 32-bit operation
                                  #   addq $10,num1    -> OK! 64-bit operation
                                  #
                                  # SIZE SUFFIX TABLE:
                                  # Suffix | Bits | Bytes | C Type      | Register examples
                                  # -------|------|-------|-------------|------------------
                                  # b      | 8    | 1     | char        | %al, %bl, %cl
                                  # w      | 16   | 2     | short       | %ax, %bx, %cx
                                  # l      | 32   | 4     | int         | %eax, %ebx, %ecx
                                  # q      | 64   | 8     | long/ptr    | %rax, %rbx, %rcx

        mov num1,%rdi              # LOAD TO REGISTER: %rdi = num1
                                  # This loads the 32-bit value from memory
                                  # No suffix needed - assembler sees %rdi (64-bit register)
                                  # and knows to do a 64-bit move
                                  #
                                  # ZERO-EXTENSION:
                                  # num1 is 32 bits (4 bytes)
                                  # %rdi is 64 bits (8 bytes)
                                  # What happens to the upper 32 bits?
                                  #
                                  # IMPORTANT x86-64 RULE:
                                  # 32-bit operations automatically ZERO-EXTEND to 64 bits
                                  # If num1 = 210 (0x000000D2):
                                  #   %edi gets: 0x000000D2 (32 bits)
                                  #   %rdi gets: 0x00000000000000D2 (64 bits)
                                  # Upper 32 bits are cleared to 0
                                  #
                                  # EXPLICIT NOTATION:
                                  # This could be written as: movl num1,%edi
                                  # Then: %rdi = zero_extend(%edi)
                                  # But mov num1,%rdi is more idiomatic
                                  #
                                  # WHY LOAD TO %rdi?
                                  # %rdi holds the first argument to syscalls
                                  # For exit, %rdi = exit code
                                  # We're preparing to exit with value 210

        mov $60,%rax               # SYSCALL NUMBER: 60 = exit
                                  # Prepare to terminate program
                                  # %rax holds the syscall number
                                  # exit syscall will use %rdi as the exit code

        syscall                    # INVOKE EXIT: Execute exit(210)
                                  # Program terminates with exit code 210
                                  #
                                  # EXIT CODE:
                                  # Shell receives: 210
                                  # Check with: echo $?
                                  #
                                  # NOTE: Exit codes are typically 0-255
                                  # 210 is outside normal range but valid
                                  # Shell may display it modulo 256



# PROGRAM EXECUTION FLOW:
#
# 1. INITIALIZATION: Program loads with num1 = 200 in .data section
# 2. MODIFICATION: addl $10,num1 changes num1 to 210
# 3. PREPARATION: mov num1,%rdi loads 210 into %rdi
# 4. EXIT: syscall terminates with exit code 210

# SECTION PURPOSES:
#
# .text (code section):
# - Contains executable instructions
# - Read-only and executable
# - Shared between multiple instances of same program
# - Stored in separate memory pages from data
#
# .data (initialized data section):
# - Contains variables with initial values
# - Read-write (can be modified)
# - Each process instance gets its own copy
# - Loaded from executable file at program start
#
# .rodata (read-only data section):
# - Contains constants and string literals
# - Read-only (write causes segmentation fault)
# - Can be shared between processes
# - Memory-protected by operating system
#
# .bss (uninitialized data section):
# - Contains variables without initial values (or initialized to 0)
# - Not stored in executable file (saves disk space)
# - OS allocates and zeros memory at load time
# - More efficient for large zero-initialized arrays

# DATA SECTION EXAMPLES:
#
# .section .data
# byte_var:    .byte 42          # 1 byte: 42
# word_var:    .word 1000        # 2 bytes: 1000
# long_var:    .long 50000       # 4 bytes: 50000
# quad_var:    .quad 2000000000  # 8 bytes: 2000000000
#
# Multiple values:
# array:       .long 1,2,3,4     # 16 bytes total (4 elements × 4 bytes)
#
# String:
# message:     .asciz "Hello"    # 6 bytes: 'H','e','l','l','o','\0'
#
# Alignment:
# .align 8                       # Align next item to 8-byte boundary
# aligned_var: .quad 12345       # Starts at address divisible by 8

# DIRECT MEMORY ADDRESSING:
#
# This program uses DIRECT (absolute) addressing:
#   addl $10,num1
#
# Address of num1 is a compile-time constant
# Assembler/linker fills in the actual address
#
# ALTERNATIVE: RIP-RELATIVE ADDRESSING:
#   addl $10,num1(%rip)
#
# More common in modern code because:
# - Enables position-independent code (PIC)
# - Required for shared libraries
# - Better for ASLR security
# - More portable across load addresses
#
# But direct addressing is simpler for learning!

# SIZE SUFFIX RULES:
#
# REQUIRED (ambiguous):
# addl $10,num1      # Must specify: 32-bit add
# movb $5,myvar      # Must specify: 8-bit move
#
# OPTIONAL (size clear from registers):
# mov %eax,%ebx      # Both 32-bit → movl inferred
# mov %rax,%rbx      # Both 64-bit → movq inferred
# add %ecx,%edx      # Both 32-bit → addl inferred
#
# BEST PRACTICE:
# - Include suffix when working with memory
# - Optional with register-only operations
# - Include for clarity in complex code
# - Required by some assemblers (e.g., AT&T syntax)

# REGISTER SIZE VARIANTS:
#
# 64-bit: %rax, %rbx, %rcx, %rdx, %rsi, %rdi, %rsp, %rbp, %r8-%r15
# 32-bit: %eax, %ebx, %ecx, %edx, %esi, %edi, %esp, %ebp, %r8d-%r15d
# 16-bit: %ax,  %bx,  %cx,  %dx,  %si,  %di,  %sp,  %bp,  %r8w-%r15w
# 8-bit:  %al,  %bl,  %cl,  %dl,  %sil, %dil, %spl, %bpl, %r8b-%r15b
#
# IMPORTANT PROPERTY:
# Writing to 32-bit register (%eax) ZEROS upper 32 bits of 64-bit register (%rax)
# Example:
#   mov $5,%eax       # %rax = 0x0000000000000005
#   mov $-1,%rax      # %rax = 0xFFFFFFFFFFFFFFFF
#   mov $5,%eax       # %rax = 0x0000000000000005 (upper bits cleared!)
#
# Writing to 16-bit or 8-bit does NOT affect upper bits:
#   mov $-1,%rax      # %rax = 0xFFFFFFFFFFFFFFFF
#   mov $5,%ax        # %rax = 0xFFFFFFFFFFFF0005 (only low 16 bits changed)

# EQUIVALENT C CODE:
#
# int num1 = 200;  // Global variable in .data section
#
# int main() {
#     num1 += 10;
#     return num1;
# }
#
# Or more explicitly:
# int num1 = 200;
#
# int _start() {
#     num1 = num1 + 10;
#     exit(num1);
# }

# MEMORY OPERATIONS:
#
# READ-MODIFY-WRITE (this program):
# addl $10,num1
# - CPU loads 4 bytes from num1
# - Performs addition in ALU
# - Stores 4 bytes back to num1
# - All in one instruction!
#
# ALTERNATIVE (explicit steps):
# mov num1,%eax      # Load
# add $10,%eax       # Modify
# mov %eax,num1      # Store
# - Three instructions
# - Uses a register
# - More flexible (can use result multiple times)
# - Common in RISC architectures

# TO ASSEMBLE, LINK, AND RUN:
# as 02-data-section.s -o 02-data-section.o
# ld 02-data-section.o -o 02-data-section
# ./02-data-section
# echo $?
# (outputs: 210, which is 200 + 10)

# DEBUGGING WITH GDB:
# gdb ./02-data-section
# (gdb) break _start
# (gdb) run
# (gdb) print/d num1        # Print num1 as decimal
# (outputs: 200)
# (gdb) stepi               # Execute addl instruction
# (gdb) print/d num1        # Print num1 again
# (outputs: 210)
# (gdb) info variables      # List all data symbols
# (gdb) x/dw &num1          # Examine memory: decimal word
# (outputs: 210)

# VARIATIONS TO TRY:
#
# 1. Change .long to .quad:
#    num1: .quad 200
#    addq $10,num1         # 64-bit operation
#    mov num1,%rdi         # Already 64-bit
#
# 2. Use a register explicitly:
#    mov num1,%eax         # Load to register
#    add $10,%eax          # Add in register
#    mov %eax,num1         # Store back
#    mov %eax,%edi         # Pass to exit
#
# 3. Multiple variables:
#    num1: .long 200
#    num2: .long 10
#    movl num2,%eax
#    addl %eax,num1
#
# 4. Use different sizes:
#    small: .byte 200
#    addb $10,small        # 8-bit operation (wraps at 255)

# COMMON MISTAKES:
#
# 1. Forgetting size suffix:
#    add $10,num1          # ERROR: Operand size ambiguous
#    FIX: addl $10,num1
#
# 2. Wrong section:
#    .section .text
#    num1: .long 200       # WRONG: Data in code section
#    FIX: Move to .section .data
#
# 3. Trying to modify .rodata:
#    .section .rodata
#    num1: .long 200
#    addl $10,num1         # SEGFAULT: .rodata is write-protected
#    FIX: Use .data instead
#
# 4. Size mismatch:
#    num1: .long 200       # 32-bit
#    addq $10,num1         # 64-bit operation on 32-bit data
#    FIX: Match sizes (both l or both q)

# WHY USE .data INSTEAD OF .rodata?
#
# .rodata (read-only):
# - For constants that never change
# - String literals: .asciz "Hello"
# - Lookup tables that are immutable
# - Operating system write-protects this memory
# - Trying to write causes segmentation fault
#
# .data (read-write):
# - For variables that can change
# - Counters, flags, mutable state
# - This program modifies num1, so .data is required
# - Writable but initialized at program load

# ASSEMBLY VS C VARIABLES:
#
# C:                        Assembly:
# int x = 5;                .section .data
#                           x: .long 5
#
# x += 10;                  addl $10,x
#
# int y = x;                mov x,%eax
#                           mov %eax,y
#
# C hides complexity:
# - Section management (automatic)
# - Size specifications (inferred from type)
# - Memory addressing (automatic)
# - Load/store operations (implicit)
#
# Assembly exposes reality:
# - Must explicitly choose section
# - Must specify size (.long, .quad, etc.)
# - Must explicitly address memory
# - Must explicitly load/store
