# PEDAGOGICAL PURPOSE:
# This program demonstrates conditional logic combined with I/O operations.
# It introduces checking input values, conditional branching based on data content,
# and outputting different messages based on conditions. The title "echoBroken" suggests
# this is intentionally incomplete or demonstrates a specific learning point about
# what happens when you don't read input first.
#
# Key learning objectives:
# 1. Understanding the .rodata section for read-only string data
# 2. Learning the .asciz directive for null-terminated strings
# 3. Understanding the '.' (current location counter) for calculating string length
# 4. Checking the first byte of a buffer with cmpb (byte comparison)
# 5. Implementing conditional message output
# 6. Understanding why this is "broken" (doesn't read input first!)
# 7. Learning about uninitialized buffer contents
#
# CONCEPTUAL OVERVIEW:
# This program is BROKEN because it checks the buffer without reading into it first!
# It implements:
#   char buff[128];              // uninitialized (all zeros)
#   if (buff[0] == 0) {
#       write(stdout, "Was zero\n", 9);
#   } else {
#       write(stdout, "Not zero\n", 9);
#   }
#   return 0;
#
# Since buff is in .bss (zeroed), it will ALWAYS print "Was zero"

        .section .bss              # BSS SECTION: Uninitialized data (zeroed by OS)
buff:   .skip 128                 # ALLOCATE BUFFER: Reserve 128 bytes
                                  # All bytes initialized to 0 by the OS
                                  # This is the KEY to understanding the "broken" behavior

        .section .rodata           # RODATA SECTION: Read-only data
                                  # This section holds constants that cannot be modified
                                  # String literals go here
                                  # Attempting to write to .rodata causes segfault
                                  # Memory protection: read-only, not writable

msg1:   .asciz "Was zero\n"       # DEFINE STRING 1: Null-terminated string
                                  # .asciz = ASCII string with Zero terminator
                                  # Characters: 'W','a','s',' ','z','e','r','o','\n','\0'
                                  # Total: 10 bytes (9 visible + 1 null terminator)
                                  #
                                  # WHY .asciz?
                                  # - Compatible with C string functions
                                  # - Null terminator marks end of string
                                  # - .ascii would NOT add the null terminator

msg1len = . - msg1                # CALCULATE STRING LENGTH
                                  # '.' = current location counter (current address)
                                  # msg1 = address where string starts
                                  # '. - msg1' = (current address) - (start address)
                                  # This computes the length INCLUDING null terminator
                                  # msg1len = 10
                                  #
                                  # IMPORTANT: This is an assembler calculation
                                  # It happens at assembly time, not runtime
                                  # msg1len becomes a constant in the assembled code
                                  #
                                  # WHY THIS WORKS:
                                  # After .asciz, '.' points to the next byte after the string
                                  # Subtracting msg1 gives us the string length

msg2:   .asciz "Not zero\n"       # DEFINE STRING 2: Null-terminated string
                                  # Characters: 'N','o','t',' ','z','e','r','o','\n','\0'
                                  # Total: 10 bytes

msg2len = . - msg2                # CALCULATE STRING 2 LENGTH
                                  # Same technique as msg1len
                                  # msg2len = 10

        .section .text             # CODE SECTION: Executable instructions
        .globl _start              # GLOBAL DIRECTIVE: Entry point visible to linker

_start:
        ## THE CRITICAL MISSING CODE:
        ## Notice there's NO read syscall here!
        ## A proper echo would read input first
        ## This is why it's called "echoBroken"
        ##
        ## Missing code should be:
        ##   mov $0,%rax           # read syscall
        ##   mov $0,%rdi           # from stdin
        ##   lea buff(%rip),%rsi   # into buff
        ##   mov $128,%rdx         # up to 128 bytes
        ##   syscall

        lea buff(%rip),%rbx        # LOAD BUFFER ADDRESS: %rbx = &buff[0]
                                  # Get pointer to our buffer
                                  # We'll check its first byte

        cmpb $0,(%rbx)             # COMPARE BYTE: Check if buff[0] == 0
                                  # BREAKDOWN:
                                  # cmpb    - Compare Byte (8-bit comparison)
                                  # $0      - Immediate value 0
                                  # (%rbx)  - Dereference %rbx (first byte of buff)
                                  #
                                  # OPERATION:
                                  # Read one byte from address in %rbx
                                  # Compare it to 0 (compute byte - 0)
                                  # Set flags based on result
                                  #
                                  # SINCE buff IS IN .bss:
                                  # All bytes are initialized to 0 by the OS
                                  # buff[0] = 0
                                  # Comparison: 0 - 0 = 0
                                  # ZF (Zero Flag) = 1
                                  #
                                  # WHY cmpb NOT cmp?
                                  # We only care about the first byte
                                  # cmpb compares 8 bits
                                  # cmp would compare 64 bits (entire quad word)

        je isZero                  # JUMP IF EQUAL: Branch to isZero if ZF=1
                                  # je = "jump if equal" = "jump if ZF=1"
                                  # Since buff[0] = 0, ZF=1, jump WILL occur
                                  # This is the "Was zero" path
                                  #
                                  # PREDICTION:
                                  # This program will ALWAYS take this branch
                                  # Because .bss is always zeroed
                                  # And we never wrote to buff

        jmp notZero                # JUMP TO NOT ZERO: Unconditional jump
                                  # This line is only reached if je didn't jump
                                  # That would mean buff[0] != 0
                                  # But since buff is zeroed, this is unreachable!
                                  # (Unless something else wrote to buff)

        ## COMMENTED OUT CODE (from original):
        ## This would be the write syscall for echoing
        ## But it's disabled because we're doing conditional output instead
        ##
        ## set up the write
        ## the number of bytes is in %rax here
        ## mov $128,%rdx           # move the number of bytes to %rdx
        ## mov $1,%rax
        ## lea buff(%rip),%rsi
        ## mov $0,%rdi
        ## syscall

isZero:
        ## ZERO PATH: Output "Was zero" message
        lea msg1(%rip),%rsi        # LOAD MESSAGE ADDRESS: %rsi = &msg1
                                  # Point to "Was zero\n" string
                                  # This is the buffer to write from

        mov $msg1len,%rdx          # LOAD MESSAGE LENGTH: %rdx = 10
                                  # This many bytes to write
                                  # Uses the calculated constant from assembly time

        jmp writeLabel             # JUMP TO COMMON WRITE CODE
                                  # Both branches converge at writeLabel
                                  # Avoids duplicating the write syscall code

notZero:
        ## NON-ZERO PATH: Output "Not zero" message
        ## This code is unreachable in practice (buff[0] is always 0)

        lea msg2(%rip),%rsi        # LOAD MESSAGE ADDRESS: %rsi = &msg2
                                  # Point to "Not zero\n" string

        mov $msg2len,%rdx          # LOAD MESSAGE LENGTH: %rdx = 10
                                  # This many bytes to write

writeLabel:
        ## COMMON WRITE CODE: Both paths converge here
        ## At this point:
        ## - %rsi points to the message to write
        ## - %rdx contains the message length

        mov $1,%rax                # SYSCALL NUMBER: 1 = write
                                  # Prepare to write to file descriptor

        mov $0,%rdi                # FILE DESCRIPTOR: stdout = 0
                                  # BUG: Should be 1 for stdout!
                                  # Same issue as echo.s
                                  # Correct: mov $1,%rdi

        syscall                    # INVOKE WRITE: Execute write(0, msg, len)
                                  # Outputs the selected message
                                  # Since buff[0] was 0, outputs "Was zero\n"

        ## EXIT WITH EXIT CODE 0
        xor %rdi,%rdi              # ZERO %rdi: %rdi = 0
                                  # xor trick to set exit code to 0

        mov $60,%rax               # SYSCALL NUMBER: 60 = exit
                                  # Prepare to terminate

        syscall                    # INVOKE EXIT: Execute exit(0)
                                  # Program terminates successfully

# WHY IS THIS "BROKEN"?
#
# The program is called "echoBroken" for a reason:
# 1. It never reads input from the user
# 2. It checks an uninitialized buffer
# 3. Since .bss is zeroed, buff[0] is always 0
# 4. It will ALWAYS print "Was zero"
# 5. It's not actually an echo program!
#
# TO FIX IT:
# Add a read syscall before the comparison:
#   mov $0,%rax
#   mov $0,%rdi
#   lea buff(%rip),%rsi
#   mov $128,%rdx
#   syscall
#
# Then it would:
# - Read user input
# - Check if first character is null (0)
# - Print "Was zero" if user just pressed Enter
# - Print "Not zero" if user typed something

# CURRENT LOCATION COUNTER ('.'):
#
# The '.' symbol is special in assembly:
#   - Represents the current assembly address
#   - Changes as the assembler processes each line
#   - Used for calculating offsets and sizes
#
# Example:
#   msg: .asciz "Hello"    # '.' now points after "Hello\0"
#   len = . - msg          # len = 6 (5 chars + null)
#
# This is evaluated at assembly time, not runtime!

# .rodata vs .data:
#
# .rodata (Read-Only Data):
#   - Cannot be modified at runtime
#   - Memory is marked read-only by OS
#   - String literals, constants
#   - Attempting to write causes segfault
#   - Can be shared between processes (memory efficient)
#
# .data (Read-Write Data):
#   - Can be modified at runtime
#   - Initialized variables
#   - Each process gets its own copy

# .bss INITIALIZATION:
#
# When your program loads:
# 1. OS allocates memory for .bss section
# 2. OS zeros all bytes in .bss
# 3. This is guaranteed by the OS loader
# 4. No need to explicitly initialize to zero
#
# In C:
#   static char buff[128];     // Goes to .bss, zeroed
#   static char buff[128] = {0}; // Same, explicitly zeroed
#   static char buff[128] = {1}; // Goes to .data, not .bss!

# BYTE COMPARISON vs QUAD COMPARISON:
#
# cmpb - Compare Byte (8 bits)
#   cmpb $0,(%rbx)    # Compare one byte
#
# cmpw - Compare Word (16 bits)
#   cmpw $0,(%rbx)    # Compare two bytes
#
# cmpl - Compare Long (32 bits)
#   cmpl $0,(%rbx)    # Compare four bytes
#
# cmpq - Compare Quad (64 bits)
#   cmpq $0,(%rbx)    # Compare eight bytes
#
# Choose based on data size you're checking!

# CONTROL FLOW ANALYSIS:
#
# Normal execution path:
#   _start
#   -> lea buff(%rip),%rbx
#   -> cmpb $0,(%rbx)         [buff[0] = 0, sets ZF=1]
#   -> je isZero              [ZF=1, jump occurs]
#   -> isZero:
#   -> lea msg1(%rip),%rsi
#   -> mov $msg1len,%rdx
#   -> jmp writeLabel
#   -> writeLabel:
#   -> write syscall          [outputs "Was zero\n"]
#   -> exit
#
# Unreachable path:
#   -> jmp notZero            [never reached]
#   -> notZero:               [unreachable code]
#   -> lea msg2(%rip),%rsi    [would output "Not zero\n"]

# EQUIVALENT C CODE (BROKEN VERSION):
#
# #include <unistd.h>
# #include <string.h>
#
# char buff[128];  // .bss section, zeroed
#
# int main() {
#     // BUG: No read here!
#     // Should be: read(0, buff, 128);
#
#     const char *msg;
#     int len;
#
#     if (buff[0] == 0) {
#         msg = "Was zero\n";
#         len = 9;
#     } else {
#         msg = "Not zero\n";
#         len = 9;
#     }
#
#     write(1, msg, len);  // Note: code has wrong fd
#     return 0;
# }

# TO ASSEMBLE, LINK, AND RUN:
# as echoBroken.s -o echoBroken.o
# ld echoBroken.o -o echoBroken
# ./echoBroken
# (outputs: Was zero)
# (always outputs "Was zero" because buff is uninitialized/zeroed)

# EXPERIMENT:
# To make it work properly:
# 1. Add read syscall before the comparison
# 2. Fix file descriptors (stdin=0, stdout=1)
# 3. Test by typing text vs just pressing Enter
# 4. Empty input (just Enter) -> first byte is '\n' (10), not 0
# 5. To detect empty input, check for '\n' not 0
