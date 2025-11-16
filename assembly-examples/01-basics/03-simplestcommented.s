# PEDAGOGICAL PURPOSE:
# This is an INCOMPLETE x86-64 assembly program that demonstrates what happens
# when you don't properly complete a syscall. It's intentionally broken to teach
# about program crashes, segmentation faults, and the importance of proper termination.
#
# Key learning objectives:
# 1. Understanding what makes a program incomplete
# 2. Why programs need proper exit calls
# 3. What happens when execution runs past your code (segmentation fault)
# 4. The importance of the syscall instruction
# 5. How the CPU executes instructions sequentially
# 6. Debugging techniques for assembly programs
# 7. The difference between "assembles" and "runs correctly"

        .section .text          # CODE SECTION: Contains executable instructions
                                # This section is readable and executable
                                # The CPU will try to execute anything here
                                # Even if it's not valid instructions!

        .global _start          # GLOBAL SYMBOL: Export _start for the linker
                                # Makes _start visible so ld can find entry point
                                # Without this, linking would fail

_start:                         # ENTRY POINT: Where program execution begins
                                # OS loads program and jumps here
                                # But we don't properly exit...

        mov $60, %rax           # Load syscall number 60 into %rax
                                # AT&T syntax: mov source, destination
                                # $60 is immediate value (literal number 60)
                                # 60 = exit syscall on Linux x86-64
                                #
                                # This LOOKS like we're setting up an exit...
                                # But we never complete it!
                                #
                                # TO PROPERLY EXIT, WE WOULD NEED:
                                # 1. mov $60, %rax     (syscall number) ✓ We have this
                                # 2. mov $0, %rdi      (exit status)    ✗ MISSING!
                                # 3. syscall           (invoke kernel)  ✗ MISSING!

        # WHAT HAPPENS NEXT?
        # The program doesn't stop here!
        # The CPU continues executing whatever bytes come after this instruction.
        # Since we didn't include any more code, it executes:
        #   - Padding bytes (usually 0x00)
        #   - Or whatever random data follows
        #   - Eventually hits invalid instruction or protected memory
        #   - Result: SEGMENTATION FAULT

# WHY THIS PROGRAM CRASHES:
#
# 1. CPU executes: mov $60, %rax
#    - %rax now contains 60
#    - Instruction pointer (%rip) advances to next instruction
#
# 2. No more valid instructions!
#    - CPU tries to decode whatever bytes follow
#    - Might be padding (0x00 bytes)
#    - Might be invalid opcodes
#
# 3. Eventually tries to execute invalid memory
#    - Accesses address outside program's memory space
#    - Memory Management Unit (MMU) detects illegal access
#    - CPU triggers a hardware exception
#    - Kernel sends SIGSEGV signal to process
#    - Process terminates with "Segmentation fault"

# WHAT IS A SEGMENTATION FAULT?
#
# A segmentation fault (segfault) occurs when a program tries to:
# - Access memory it doesn't own
# - Execute data as code
# - Write to read-only memory
# - Dereference a null pointer
#
# In this case: CPU executes past our code into invalid memory
#
# The kernel terminates the process with exit status 139:
#   139 = 128 + 11
#   128 = base for signals
#   11 = SIGSEGV (segmentation fault signal number)

# THE CPU'S PERSPECTIVE:
#
# The CPU doesn't know our program is "done" after mov $60, %rax.
# It just keeps executing:
#
# Address    Bytes           Instruction
# 0x401000:  48 c7 c0 3c ... mov $60, %rax     ← We are here
# 0x401007:  00 00 00 00     (padding bytes)   ← CPU tries to execute these!
# 0x40100b:  ??              (who knows?)
# ...eventually...
# 0x??????:  (unmapped memory) ← SEGFAULT!

# COMPREHENSIVE SUMMARY:
#
# WHAT THIS PROGRAM DOES:
# 1. Loads 60 into %rax
# 2. Doesn't invoke syscall
# 3. CPU continues past our code
# 4. Crashes with segmentation fault
#
# WHAT'S MISSING:
# 1. Exit status argument (mov $0, %rdi)
# 2. Syscall instruction (syscall)
#
# ACTUAL BEHAVIOR:
# - Assembles successfully (syntactically valid)
# - Links successfully (has _start symbol)
# - Crashes when run (no proper exit)

# TO ASSEMBLE, LINK, AND RUN (with crash):
as 03-simplest.s -o 03-simplest.o    # Assembles fine
ld 03-simplest.o -o 03-simplest      # Links fine
./03-simplest                        # CRASHES: Segmentation fault
echo $?                              # Shows: 139 (128 + SIGSEGV)

# EXPECTED OUTPUT:
# Segmentation fault (core dumped)
#
# Exit status: 139
#
# 139 = 128 + 11 (SIGSEGV signal number)

# WHY DOES IT ASSEMBLE/LINK BUT CRASH?
#
# ASSEMBLY TIME:
# - Assembler checks syntax only
# - "mov $60, %rax" is syntactically valid
# - Creates valid machine code bytes
# - Doesn't check if program logic is complete
#
# LINK TIME:
# - Linker finds _start symbol (good)
# - Resolves all references (none to resolve)
# - Creates executable file
# - Doesn't check if program will run correctly
#
# RUN TIME:
# - OS loads program into memory
# - OS jumps to _start
# - CPU executes mov $60, %rax
# - CPU tries to execute next bytes
# - CRASH!

# HOW TO FIX THIS PROGRAM:
#
# Option 1: Add missing syscall components
        # .section .text
        # .global _start
        # _start:
        #     mov $60, %rax      # Syscall number
        #     mov $0, %rdi       # Exit status
        #     syscall            # Invoke kernel
#
# Option 2: Use xor for exit status
        # .section .text
        # .global _start
        # _start:
        #     mov $60, %rax      # Syscall number
        #     xor %rdi, %rdi     # Exit status = 0
        #     syscall            # Invoke kernel

# DEBUGGING THE CRASH:
#
# Use gdb to see what happens:
#
# $ gdb ./03-simplest
# (gdb) run
# Program received signal SIGSEGV, Segmentation fault.
# 0x0000000000401007 in ?? ()
#
# (gdb) x/10i $pc-7
# Shows the mov instruction and then garbage bytes
#
# (gdb) info registers
# Shows %rax = 60, but we crashed before using it

# USING strace:
#
# $ strace ./03-simplest
# execve("./03-simplest", ["./03-simplest"], ...) = 0
# --- SIGSEGV {si_signo=SIGSEGV, si_code=SEGV_MAPERR, si_addr=0x401007} ---
# +++ killed by SIGSEGV (core dumped) +++
#
# Notice: No exit syscall appears!
# Program crashes before making any syscalls.

# WHAT'S IN THE EXECUTABLE?
#
# $ objdump -d 03-simplest
#
# Disassembly of section .text:
#
# 0000000000401000 <_start>:
#   401000:  48 c7 c0 3c 00 00 00    mov    $0x3c,%rax    # Our instruction
#   401007:  ...                     (whatever comes next)
#
# The mov instruction is 7 bytes long.
# After address 0x401007, there might be:
#   - Padding zeros added by linker
#   - Start of next section
#   - Unmapped memory
#
# Any of these will cause a crash when CPU tries to execute them.

# COMPARISON: COMPLETE vs INCOMPLETE
#
# INCOMPLETE (this program):
#   mov $60, %rax
#   (crashes)
#
# MINIMAL COMPLETE:
#   mov $60, %rax
#   mov $0, %rdi
#   syscall
#   (exits cleanly with status 0)
#
# The difference: 2 instructions and proper termination!

# PEDAGOGICAL VALUE:
#
# This broken program teaches:
# 1. Assembly doesn't hold your hand
# 2. Syntactically correct ≠ logically correct
# 3. Programs need explicit termination
# 4. CPU blindly executes instructions sequentially
# 5. Importance of proper error handling
# 6. How to debug crashes with gdb/strace

# COMMON STUDENT MISTAKES:
#
# Mistake 1: Forgetting syscall
#   mov $60, %rax
#   mov $0, %rdi
#   # Missing syscall!
#   Result: Segfault
#
# Mistake 2: Wrong order
#   syscall
#   mov $60, %rax     # Too late!
#   Result: Random syscall with garbage registers
#
# Mistake 3: Only setting %rax (this program)
#   mov $60, %rax
#   # Missing argument and syscall
#   Result: Segfault

# SIGNALS AND EXIT CODES:
#
# When a program is killed by a signal, shell exit code is:
#   exit_code = 128 + signal_number
#
# Common signals:
#   SIGSEGV (11): Segmentation fault → 139
#   SIGILL  (4):  Illegal instruction → 132
#   SIGABRT (6):  Abort → 134
#   SIGFPE  (8):  Floating point exception → 136
#   SIGKILL (9):  Kill (uncatchable) → 137
#   SIGTERM (15): Termination → 143
#
# Our program exits with 139 (SIGSEGV).

# HOW THE KERNEL HANDLES THIS:
#
# 1. CPU executes mov $60, %rax (succeeds)
# 2. CPU tries to fetch next instruction
# 3. CPU accesses invalid memory address
# 4. Memory Management Unit (MMU) raises exception
# 5. CPU traps to kernel (exception handler)
# 6. Kernel checks: was this access valid?
# 7. Kernel determines: NO, segmentation violation
# 8. Kernel sends SIGSEGV to the process
# 9. Process has no signal handler
# 10. Kernel terminates process
# 11. Kernel generates core dump (if enabled)
# 12. Kernel returns to shell with exit code 139

# CORE DUMPS:
#
# When a program crashes, the kernel may create a core dump:
# - Snapshot of program's memory at crash time
# - Useful for debugging
# - Can be analyzed with gdb
#
# Enable core dumps:
# $ ulimit -c unlimited
# $ ./03-simplest
# Segmentation fault (core dumped)
# $ ls -lh core
# -rw------- 1 user user 352K ... core
#
# Analyze with gdb:
# $ gdb ./03-simplest core
# (gdb) bt        # Backtrace
# (gdb) info reg  # Registers at crash

# RELATION TO C:
#
# This assembly program has no sensible C equivalent.
# It's like writing:
#
# int main() {
#     // Set up exit, but never call it
#     int syscall_number = 60;
#     // Then fall off the end... undefined behavior!
# }
#
# In C, falling off the end of main() is undefined.
# In assembly, it's explicitly wrong and crashes.

# WHAT IF WE ADD A NOP?
#
# Even adding a nop (no operation) doesn't help:
#   mov $60, %rax
#   nop
#
# CPU will execute nop, then continue to whatever comes after.
# Still crashes! Must explicitly exit.

# INFINITE LOOP ALTERNATIVE:
#
# If you want a program that doesn't exit, loop forever:
#   _start:
#       mov $60, %rax
#   loop:
#       jmp loop        # Jump to itself
#
# This wouldn't crash, but would hang forever.
# You'd need Ctrl-C to kill it (SIGINT).

# THE LESSON:
#
# Assembly programming requires explicit control flow.
# - Every path must end properly (syscall, ret, jmp)
# - No implicit exits or safety nets
# - You control every instruction
# - With great power comes great responsibility!

# FIX IT YOURSELF:
#
# Exercise: Modify this program to exit cleanly.
# Add these lines after mov $60, %rax:
#
#   xor %rdi, %rdi     # Exit status 0
#   syscall            # Actually exit
#
# Then reassemble, relink, and run:
#   as 03-simplest.s -o 03-simplest.o
#   ld 03-simplest.o -o 03-simplest
#   ./03-simplest      # No crash!
#   echo $?            # Shows 0

# NEXT STEPS:
#
# After understanding why this crashes:
# 1. Fix it by adding missing instructions
# 2. Experiment with different invalid instructions
# 3. Use gdb to step through and see the crash
# 4. Learn about other ways programs can crash
# 5. Build complete, robust assembly programs
