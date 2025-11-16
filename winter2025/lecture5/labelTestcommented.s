# PEDAGOGICAL PURPOSE:
# This program demonstrates the use of local labels (numbered labels) in assembly,
# which is an advanced topic for code organization. It shows how local labels can
# be reused within different scopes and how they interact with function calls.
#
# Key learning objectives:
# 1. Understanding local labels (numbered labels like 1:)
# 2. Learning forward (1f) and backward (1b) label references
# 3. Understanding label scope and reusability
# 4. Seeing how local labels work within functions
# 5. Understanding call and ret for function invocation
# 6. Learning how to organize code with multiple functions
# 7. Understanding why local labels are useful for local jumps
#
# CONCEPTUAL OVERVIEW:
# This program calls two functions that each use local labels:
#   biglabel2();  // prints "In the second sublabel\n"
#   biglabel1();  // prints "In the first sublabel\n"
#   exit(0);
#
# Both functions use "1:" as a local label without conflict

        .section .rodata           # RODATA SECTION: Read-only string data

msg1:   .asciz "In the first sublabel\n"
                                  # FIRST MESSAGE: String for biglabel1
                                  # Characters: 22 printable + 1 null = 23 bytes
msg1len = . - msg1                # LENGTH: Calculate bytes (23)

msg2:   .asciz "In the second sublabel\n"
                                  # SECOND MESSAGE: String for biglabel2
                                  # Characters: 23 printable + 1 null = 24 bytes
msg2len = . - msg2                # LENGTH: Calculate bytes (24)

        .section .text             # CODE SECTION: Executable instructions
        .globl _start              # GLOBAL DIRECTIVE: Entry point visible to linker

_start:
        call biglabel2             # CALL FUNCTION 2: Execute biglabel2
                                  # WHAT HAPPENS:
                                  # 1. Push return address (next instruction) onto stack
                                  # 2. Jump to biglabel2
                                  # 3. biglabel2 executes, prints second message
                                  # 4. ret pops return address and jumps back here
                                  #
                                  # OUTPUT ORDER:
                                  # This is called FIRST, so "second sublabel" prints first!

        call biglabel1             # CALL FUNCTION 1: Execute biglabel1
                                  # Called SECOND, so "first sublabel" prints second
                                  # Note the potentially confusing naming!
                                  #
                                  # AFTER BOTH CALLS:
                                  # Output will be:
                                  # "In the second sublabel"
                                  # "In the first sublabel"

        ## EXIT LABEL
1:                                # LOCAL LABEL 1: Exit label
                                  # THIS IS A LOCAL LABEL
                                  # Number labels (1:, 2:, 3:, etc.) are "local"
                                  # They can be reused in different parts of code
                                  # They're scoped to the nearest global label (non-numeric)
                                  #
                                  # IMPORTANT:
                                  # There's also a "1:" label in biglabel1 and biglabel2
                                  # They don't conflict because local labels are scoped!
                                  #
                                  # WHY HERE?
                                  # This is the exit point for the program
                                  # Could also be called "exit:" but using "1:" demonstrates locals

        mov $60,%rax               # SYSCALL NUMBER: 60 = exit
                                  # Prepare to terminate the program

        xor %rdi,%rdi              # ZERO %rdi: exit code = 0
                                  # xor trick for efficient zeroing

        syscall                    # INVOKE EXIT: Terminate with code 0

biglabel1:                        # FUNCTION 1: Prints first message
                                  # GLOBAL LABEL (not a number)
                                  # This creates a new scope for local labels
                                  # Any numbered labels after this are "owned" by biglabel1
                                  # until the next global label

        jmp 1f                     # JUMP FORWARD: Jump to local label "1" forward
                                  # SYNTAX: 1f = "local label 1, looking forward"
                                  # WHY FORWARD?
                                  # The "1:" we want is BELOW this line (ahead in the code)
                                  # Not the "1:" in _start (which is above/behind)
                                  #
                                  # IMPORTANT:
                                  # This jumps to the NEXT "1:" label in the code
                                  # Which is the "1:" on line 24 (below)
                                  # NOT the "1:" in _start or biglabel2

1:                                # LOCAL LABEL 1: Print first message
                                  # This is a different "1:" from _start's "1:"
                                  # They don't conflict due to scoping!
                                  # This "1:" belongs to biglabel1's scope

        lea msg1(%rip),%rsi        # LOAD MESSAGE 1 ADDRESS: %rsi = &msg1
                                  # Point to "In the first sublabel\n"

        mov $msg1len,%rdx          # LOAD MESSAGE 1 LENGTH: %rdx = 23
                                  # Number of bytes to write

        mov $1,%rax                # SYSCALL NUMBER: 1 = write
                                  # Prepare write syscall

        mov $0,%rdi                # FILE DESCRIPTOR: 0 = stdout
                                  # BUG: Should be 1 for stdout!
                                  # (Same fd issue as other programs)

        syscall                    # INVOKE WRITE: Print first message
                                  # Outputs: "In the first sublabel\n"

        ret                        # RETURN: Pop return address and jump
                                  # Returns to caller (_start)
                                  # Stack had return address from "call biglabel1"
                                  # Now we pop it and jump back

biglabel2:                        # FUNCTION 2: Prints second message
                                  # GLOBAL LABEL
                                  # Creates new scope for local labels
                                  # Previous "1:" labels are no longer accessible
                                  # We can reuse "1:" here without conflict!

        jmp 1f                     # JUMP FORWARD: Jump to THIS function's "1:"
                                  # Jumps to the "1:" on line 34 (below)
                                  # NOT the "1:" in _start or biglabel1
                                  # Local label lookup is scoped to this function

1:                                # LOCAL LABEL 1: Print second message
                                  # This is YET ANOTHER "1:" label
                                  # Three different "1:" labels in this program:
                                  # - One in _start (exit)
                                  # - One in biglabel1 (print first)
                                  # - One in biglabel2 (print second)
                                  # All coexist without conflict due to scoping!

        lea msg2(%rip),%rsi        # LOAD MESSAGE 2 ADDRESS: %rsi = &msg2
                                  # Point to "In the second sublabel\n"

        mov $msg2len,%rdx          # LOAD MESSAGE 2 LENGTH: %rdx = 24
                                  # Number of bytes to write

        mov $1,%rax                # SYSCALL NUMBER: 1 = write
                                  # Prepare write syscall

        mov $0,%rdi                # FILE DESCRIPTOR: 0 = stdout
                                  # BUG: Should be 1 for stdout!

        syscall                    # INVOKE WRITE: Print second message
                                  # Outputs: "In the second sublabel\n"

        ret                        # RETURN: Return to caller (_start)

# LOCAL LABELS EXPLAINED:
#
# LOCAL LABELS (numbered):
#   - Written as: 1: 2: 3: etc.
#   - Referenced as:
#     - 1b = "label 1, searching backward"
#     - 1f = "label 1, searching forward"
#   - Scoped to nearest global label
#   - Can be reused in different functions
#   - Useful for small jumps within a function
#
# GLOBAL LABELS (named):
#   - Written as: name:
#   - Referenced as: name
#   - Visible throughout the file (or program if .globl)
#   - Cannot be reused (would cause "duplicate symbol" error)
#   - Used for functions and major code points

# LOCAL LABEL SCOPING:
#
# Scope 1 (_start):
#   _start:
#     call biglabel2
#     call biglabel1
#   1:              <- Local to _start
#     mov $60,%rax
#
# Scope 2 (biglabel1):
#   biglabel1:
#     jmp 1f
#   1:              <- Local to biglabel1 (different from _start's 1:)
#     lea msg1(%rip),%rsi
#
# Scope 3 (biglabel2):
#   biglabel2:
#     jmp 1f
#   1:              <- Local to biglabel2 (different from both above)
#     lea msg2(%rip),%rsi

# FORWARD (f) vs BACKWARD (b) REFERENCES:
#
# Example:
#   global1:
#     jmp 1f        # Jump to next "1:" (forward)
#     jmp 1b        # Would jump to previous "1:" (backward)
#   1:              # Target for "jmp 1f"
#     ret
#   1:              # Another "1:" label (can have multiple!)
#     ret
#
# The assembler searches in the specified direction
# until it finds the numbered label.

# WHY USE LOCAL LABELS?
#
# Advantages:
#   - Avoid naming many small labels (loop1, loop2, done1, done2, etc.)
#   - Keep labels local to their function scope
#   - Cleaner code for small jumps
#   - Can reuse common patterns (1:, 2:, 3:)
#
# When to use GLOBAL labels instead:
#   - Function names (must be unique)
#   - Labels you need to reference from other files
#   - Labels used far from their definition
#   - When clarity is more important than brevity

# PROGRAM EXECUTION TRACE:
#
# 1. _start:
# 2. call biglabel2  -> Jump to biglabel2, push return address
# 3. biglabel2:
# 4. jmp 1f          -> Jump forward to biglabel2's "1:"
# 5. 1: (in biglabel2)
# 6. Output: "In the second sublabel\n"
# 7. ret             -> Return to _start (after first call)
# 8. call biglabel1  -> Jump to biglabel1, push return address
# 9. biglabel1:
# 10. jmp 1f         -> Jump forward to biglabel1's "1:"
# 11. 1: (in biglabel1)
# 12. Output: "In the first sublabel\n"
# 13. ret            -> Return to _start (after second call)
# 14. 1: (in _start)
# 15. exit(0)

# WHY THE EXTRA jmp 1f?
#
# In both functions, there's a seemingly pointless:
#   jmp 1f
# 1:
#   (actual code)
#
# This demonstrates local label usage but isn't functionally necessary.
# The code would work the same without the jump:
#   biglabel1:
#     lea msg1(%rip),%rsi
#     ...
#
# It's pedagogical - showing how local labels and forward jumps work!

# EQUIVALENT C CODE:
#
# #include <unistd.h>
#
# void biglabel1() {
#     const char *msg1 = "In the first sublabel\n";
#     write(1, msg1, 23);
# }
#
# void biglabel2() {
#     const char *msg2 = "In the second sublabel\n";
#     write(1, msg2, 24);
# }
#
# int main() {
#     biglabel2();   // Prints second message first!
#     biglabel1();   // Prints first message second!
#     return 0;
# }

# ACTUAL OUTPUT:
# In the second sublabel
# In the first sublabel
#
# Note the order! biglabel2 is called first.

# TO ASSEMBLE, LINK, AND RUN:
# as labelTest.s -o labelTest.o
# ld labelTest.o -o labelTest
# ./labelTest
# (outputs both messages in order they're called)

# EXPERIMENT:
# Try adding more local labels:
#   2:
#     mov $1,%rax
#   3:
#     syscall
# And jump to them:
#   jmp 2f
#   jmp 3f
# See how forward/backward references work!
