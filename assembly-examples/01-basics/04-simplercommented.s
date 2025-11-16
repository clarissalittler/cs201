# PEDAGOGICAL PURPOSE:
# This program demonstrates an INCOMPLETE exit syscall that's missing the exit status
# argument. It will run without crashing but produces unpredictable exit codes because
# it uses whatever garbage value happens to be in %rdi. This teaches the importance
# of initializing all syscall arguments.
#
# Key learning objectives:
# 1. Understanding the importance of initializing all syscall arguments
# 2. What happens when you use uninitialized registers
# 3. The difference between crashing and incorrect behavior
# 4. Debugging techniques for finding uninitialized variables
# 5. Why exit codes might be unpredictable
# 6. The syscall interface requires ALL arguments, not just the syscall number
# 7. Register state persistence from program startup

        .section .text          # CODE SECTION: Contains executable instructions
                                # This is where our program code lives
                                # Read-only and executable memory region

        .global _start          # GLOBAL SYMBOL: Export _start for linker
                                # Makes _start visible so linker can find entry point
                                # Required for ld to link successfully

_start:                         # ENTRY POINT: Where program execution begins
                                # OS loads program and jumps here
                                # Registers have whatever values the OS set up

        # PREPARE EXIT SYSCALL (INCOMPLETE!)
        # The exit syscall needs:
        #   %rax = 60 (syscall number)
        #   %rdi = exit status (0-255)
        # We only set %rax, not %rdi!

        mov $60, %rax           # Load syscall number 60 into %rax
                                # AT&T syntax: mov source, destination
                                # $ means immediate value (literal 60)
                                # %rax holds the syscall number
                                # 60 = exit syscall on Linux x86-64
                                #
                                # This part is CORRECT.

        # MISSING: mov $0, %rdi
        # We should set %rdi to our desired exit status!
        # Without this, %rdi contains whatever was there before.
        #
        # What's in %rdi at program start?
        # - Depends on OS and runtime environment
        # - Might be argc (argument count) on some systems
        # - Might be 0
        # - Might be some other value
        # - IT'S UNPREDICTABLE!

        syscall                 # INVOKE SYSTEM CALL: Call kernel
                                # This executes exit(%rdi)
                                # Kernel reads %rax (60) → exit syscall
                                # Kernel reads %rdi → exit status (UNDEFINED!)
                                # Program terminates with unpredictable status
                                #
                                # The syscall WORKS (doesn't crash)
                                # But the exit status is WRONG (or unpredictable)

# WHY THIS PROGRAM DOESN'T CRASH:
#
# Unlike 03-simplest.s, this program includes the syscall instruction.
# The kernel successfully executes exit(), so the program terminates cleanly.
# However, the exit status is undefined because we never set %rdi.
#
# CRASH vs INCORRECT BEHAVIOR:
# - 03-simplest.s: Missing syscall → CRASHES (segfault)
# - 04-simpler.s:  Missing argument → WORKS but exit status is random

# WHAT'S IN %rdi AT STARTUP?
#
# When the OS starts a program, it sets up initial register values.
# Different systems do this differently:
#
# On Linux x86-64:
# - %rsp points to argc on the stack
# - %rdx might contain address of fini function
# - Other registers are typically zeroed or contain small values
# - %rdi might contain argc (argument count)
#
# Common scenarios:
# 1. ./04-simpler         → %rdi might be 1 (argc=1)
# 2. ./04-simpler arg1    → %rdi might be 2 (argc=2)
# 3. Varies by OS version, kernel, and runtime

# ACTUAL BEHAVIOR (VARIES):
#
# Possibility 1: %rdi = 0 at startup
#   Result: exit(0) → success status
#   $ ./04-simpler
#   $ echo $?
#   0
#
# Possibility 2: %rdi = 1 at startup (argc)
#   Result: exit(1) → error status
#   $ ./04-simpler
#   $ echo $?
#   1
#
# Possibility 3: %rdi = some other value
#   Result: exit(that_value % 256) → unpredictable
#   $ ./04-simpler
#   $ echo $?
#   ??? (could be anything)

# COMPREHENSIVE SUMMARY:
#
# WHAT THIS PROGRAM DOES:
# 1. Sets syscall number to 60 (exit)
# 2. Does NOT set exit status in %rdi
# 3. Calls kernel with undefined exit status
# 4. Terminates successfully but with unpredictable exit code
#
# WHAT'S CORRECT:
# - .section .text (has code section)
# - .global _start (exports entry point)
# - _start: label (defines entry point)
# - mov $60, %rax (sets syscall number)
# - syscall (invokes kernel)
#
# WHAT'S MISSING:
# - mov $0, %rdi (or any exit status initialization)
#
# RESULT:
# - Program exits cleanly (no crash)
# - Exit status is whatever was in %rdi
# - Unpredictable behavior (non-deterministic)

# TO ASSEMBLE, LINK, AND RUN:
as 04-simpler.s -o 04-simpler.o    # Assemble (succeeds)
ld 04-simpler.o -o 04-simpler      # Link (succeeds)
./04-simpler                       # Run (exits cleanly)
echo $?                            # Shows: ??? (unpredictable)

# EXPECTED OUTPUT:
# (no terminal output)
#
# Exit status: Unpredictable (commonly 0 or 1)
#
# On many modern Linux systems, %rdi is 0 at startup, so you might see:
# $ ./04-simpler
# $ echo $?
# 0
#
# But this is NOT guaranteed! It's undefined behavior.

# DEBUGGING UNINITIALIZED REGISTERS:
#
# Use gdb to see what's in %rdi:
#
# $ gdb ./04-simpler
# (gdb) break _start
# (gdb) run
# Breakpoint 1, 0x0000000000401000 in _start ()
#
# (gdb) info registers rdi
# rdi            0x0      0
#
# (gdb) stepi               # Step past mov $60, %rax
# (gdb) info registers rax
# rax            0x3c     60
#
# (gdb) stepi               # Step past syscall (program exits)
# [Inferior 1 (process ...) exited with code 0]
#
# This shows %rdi was 0, so exit status was 0.

# USING strace TO SEE THE SYSCALL:
#
# $ strace ./04-simpler
# execve("./04-simpler", ["./04-simpler"], ...) = 0
# exit(0)                                 = ?
# +++ exited with 0 +++
#
# Or if %rdi was 1:
# exit(1)                                 = ?
# +++ exited with 1 +++
#
# strace shows the actual argument passed to exit().
# This helps debug what value was in %rdi.

# WHY IS THIS DANGEROUS?
#
# 1. NON-DETERMINISM:
#    Program behaves differently on different systems
#    Same binary, different exit codes
#    Breaks scripts that check exit status
#
# 2. HARD TO DEBUG:
#    Sometimes works (exit 0), sometimes fails
#    "Works on my machine" syndrome
#    Heisenbug: changes when you observe it
#
# 3. SECURITY:
#    If %rdi contains sensitive data, it leaks via exit code
#    Shell scripts might make wrong decisions
#
# 4. CORRECTNESS:
#    Scripts expect 0 for success, non-zero for failure
#    Random exit codes break automation

# COMPARISON TO OTHER PROGRAMS:
#
# 02-simple.s (CORRECT):
#   mov $60, %rax    # Syscall number
#   mov $-1, %rdi    # Exit status (explicit)
#   syscall          # Invoke
#   → Always exits with 255
#
# 03-simplest.s (CRASHES):
#   mov $60, %rax    # Syscall number
#   # Missing: exit status
#   # Missing: syscall
#   → Segmentation fault (139)
#
# 04-simpler.s (UNPREDICTABLE - this file):
#   mov $60, %rax    # Syscall number
#   # Missing: exit status
#   syscall          # Invoke
#   → Exits with undefined status (varies)

# HOW TO FIX THIS PROGRAM:
#
# Add ONE instruction before syscall:
#
# Method 1: Explicit zero
#   mov $60, %rax
#   mov $0, %rdi     # ← ADD THIS
#   syscall
#
# Method 2: XOR trick (faster, shorter)
#   mov $60, %rax
#   xor %rdi, %rdi   # ← ADD THIS (sets %rdi = 0)
#   syscall
#
# Method 3: Custom exit code
#   mov $60, %rax
#   mov $42, %rdi    # ← ADD THIS (exit with 42)
#   syscall

# THE LESSON ABOUT SYSCALL ARGUMENTS:
#
# Linux syscall convention requires ALL arguments to be set:
#
# write(fd, buf, count):
#   %rax = 1
#   %rdi = fd    ← MUST set
#   %rsi = buf   ← MUST set
#   %rdx = count ← MUST set
#
# exit(status):
#   %rax = 60
#   %rdi = status ← MUST set
#
# The kernel doesn't check if you forgot an argument.
# It just uses whatever's in the register.
# ALWAYS initialize all required arguments!

# REGISTER INITIALIZATION BEST PRACTICES:
#
# 1. ALWAYS set all syscall arguments explicitly
# 2. Don't assume registers are zero at startup
# 3. Don't assume registers keep values between calls
# 4. Initialize before use, not after
# 5. Use xor %reg, %reg to zero a register efficiently

# WHAT DIFFERENT KERNELS/VERSIONS MIGHT DO:
#
# Older kernels:
#   Might leave %rdi with various values
#   Could be argc, could be 0, could be random
#
# Newer kernels with security features:
#   Might zero most registers
#   Reduces information leakage
#   But still not guaranteed!
#
# Different OSes (FreeBSD, OpenBSD, etc.):
#   Different startup conventions
#   Different register initialization
#   Same program, different behavior
#
# NEVER rely on uninitialized values!

# ANALOGY TO C:
#
# This assembly is like this buggy C code:
#
# int main() {
#     int status;           // Uninitialized!
#     exit(status);         // Uses garbage value
# }
#
# Or:
#
# int main() {
#     int status;           // Uninitialized!
#     return status;        // Returns garbage
# }
#
# Compilers often warn: "variable 'status' is uninitialized"
# But assembly has no such warnings!

# TESTING FOR UNINITIALIZED VARIABLES:
#
# Valgrind can detect some uninitialized values:
# $ valgrind ./04-simpler
# (But might not catch register issues in pure assembly)
#
# Better approach: Code review and testing
# - Review: Does every syscall set all arguments?
# - Test: Run multiple times, check if exit code varies
# - Debug: Use gdb to inspect registers before syscall

# REAL-WORLD SCENARIO:
#
# Imagine a script that checks program success:
#
# #!/bin/bash
# ./04-simpler
# if [ $? -eq 0 ]; then
#     echo "Success! Continuing..."
#     ./important_operation
# else
#     echo "Failed! Aborting."
#     exit 1
# fi
#
# If %rdi happens to be 0: Script continues (might be wrong!)
# If %rdi happens to be 1: Script aborts (might be wrong!)
#
# Unpredictable behavior breaks production systems!

# EXIT STATUS BYTE TRUNCATION:
#
# Remember: Exit status is only 8 bits (0-255)
# If %rdi contains a large value, only lower byte is used:
#
# %rdi = 0x0000000000000001 → exit status 1
# %rdi = 0x0000000000000100 → exit status 0 (256 % 256 = 0)
# %rdi = 0x00000000000001FF → exit status 255 (511 % 256 = 255)
# %rdi = 0xFFFFFFFFFFFFFFFF → exit status 255 (-1 as unsigned byte)
#
# Only the lowest byte matters!

# MAKING IT DETERMINISTIC:
#
# Even if we don't know what's in %rdi, we can test:
#
# $ for i in {1..10}; do ./04-simpler; echo $?; done
# 0
# 0
# 0
# ...
# 0
#
# If it's always the same: Consistent (but still wrong!)
# If it varies: Truly random/dependent on environment

# PEDAGOGICAL VALUE:
#
# This program teaches:
# 1. Importance of initializing all variables
# 2. Difference between crashes and wrong behavior
# 3. Undefined behavior can "work" by accident
# 4. Assembly gives you no safety nets
# 5. Syscalls require precise argument setup
# 6. Debugging techniques for register values

# THE CORRECT VERSION:
#
# Always write it like this:
#
#        .section .text
#        .global _start
#
# _start:
#        mov $60, %rax     # Exit syscall number
#        xor %rdi, %rdi    # Exit status = 0 (explicit!)
#        syscall           # Invoke kernel
#
# This is:
# - Deterministic (always exits with 0)
# - Portable (works on all systems)
# - Clear (obvious intent)
# - Correct (follows syscall convention)

# EXPERIMENT YOURSELF:
#
# Try this modified version to see %rdi value:
#
# _start:
#     mov %rdi, %rax      # Copy %rdi to %rax
#     # Now %rax has whatever %rdi started with
#     # Could print it or examine in debugger
#
#     mov $60, %rax       # Setup exit
#     # Don't modify %rdi
#     syscall             # Exit with original %rdi value
#
# Run and check: echo $?
# That's what %rdi was at startup!

# SUMMARY TABLE:
#
# Program          | Exit Status | Behavior
# -----------------|-------------|------------------
# 02-simple.s      | 255         | Deterministic
# 03-simplest.s    | 139         | Crashes (segfault)
# 04-simpler.s     | ???         | Unpredictable
# (correct version)| 0           | Deterministic

# NEXT STEPS:
#
# After understanding this issue:
# 1. Always initialize syscall arguments
# 2. Use gdb to inspect register values
# 3. Use strace to verify syscall arguments
# 4. Write defensive assembly code
# 5. Test on multiple systems/environments
# 6. Read syscall documentation carefully
