// PEDAGOGICAL PURPOSE:
// This program demonstrates an optimized version of recursive Fibonacci implementation.
// It's similar to fibber.s but with a subtle optimization that eliminates unnecessary
// stack operations. This shows how understanding register usage can lead to more
// efficient code while maintaining correctness.
//
// Key learning objectives:
// 1. Understanding when register preservation is necessary vs. unnecessary
// 2. Learning to optimize by eliminating redundant push/pop operations
// 3. Seeing how to analyze register liveness (when values are needed)
// 4. Understanding the difference between defensive and optimal programming
// 5. Learning stack operation optimization while maintaining correctness
// 6. Recognizing that not all pushed values need to be popped if not used again
// 7. Understanding the trade-off between code clarity and efficiency

      ### Program: Compute Fibonacci number recursively in assembly (OPTIMIZED)
### This program reads an integer n from the user,
### computes the nth Fibonacci number using recursion,
### and exits with the Fibonacci number as the exit code.
### The result is not printed to the screen, but returned as the exit code.

    .section .text            # Code section where the instructions are stored
    .global _start            # Define _start as global symbol; entry point of the program
    .extern readInt           # Declare that readInt function is defined externally
    .extern writeInt          # Declare that writeInt function is defined externally


// OPTIMIZED FIBONACCI RECURSIVE FUNCTION
// ======================================
// This version eliminates unnecessary register saves by analyzing when values
// are actually needed.
//
// COMPARISON WITH fibber.s:
// - fibber.s: pushes %rdi before BOTH recursive calls
// - fibber2.s: pushes %rdi only before first call (optimization)
//
// WHY THE OPTIMIZATION WORKS:
// After the second recursive call (fib(n-2)), we never use %rdi again.
// We only need the result in %rax and the saved fib(n-1) from memory.
// Therefore, there's no need to preserve %rdi around the second call.
//
// PARAMETERS:
//   %rdi - n (the Fibonacci index to compute)
//
// RETURNS:
//   %rax - fib(n) (the nth Fibonacci number)
//
fib:
    # FUNCTION PROLOGUE - Setup stack frame
    # ======================================
    push %rbp                 # Save the base pointer of the previous frame (caller)
                              # %rbp is callee-saved, must preserve

    mov %rsp, %rbp            # Set up the base pointer for this function's stack frame
                              # Establish frame pointer for local variables

    sub $16, %rsp             # Allocate 16 bytes of space on the stack for local variables
                              # ensure 16-byte alignment per System V ABI
                              # We use 8 bytes to store fib(n-1)
                              # Other 8 bytes for alignment

    # BASE CASE CHECK
    # ===============
    # fib(0) = 0, fib(1) = 1
    # Both handled by: return n when n <= 1

    cmp $1, %rdi              # Compare input n (%rdi) with 1
                              # Check if n <= 1

    jle .base                 # If n <= 1, jump to .base label (base case)
                              # For n=0 or n=1, return n directly

    # RECURSIVE CASE: fib(n) = fib(n-1) + fib(n-2)
    # ============================================

    # FIRST RECURSIVE CALL: fib(n-1)
    # ==============================
    # IMPORTANT: We must save %rdi here because:
    # - We're about to modify it (dec %rdi)
    # - We'll need the original n value after this call returns
    # - We use it to compute n-2 for the second recursive call

    push %rdi                 # Save current n on the stack before modifying it
                              # CRITICAL: Needed for computing n-2 later
                              # After fib(n-1) returns, we need to know what n was

    dec %rdi                  # Decrement n by 1; compute n - 1
                              # %rdi now contains n-1

    call fib                  # Recursive call to fib(n - 1)
                              # Makes recursive call with n-1
                              # Result returned in %rax

    mov %rax, -8(%rbp)        # Store fib(n - 1) result at offset -8(%rbp)
                              # Save fib(n-1) in local variable slot
                              # Must save before next call (which overwrites %rax)

    pop %rdi                  # Restore n from the stack
                              # %rdi now contains original n again
                              # We need this to compute n-2

    # SECOND RECURSIVE CALL: fib(n-2)
    # ===============================
    # OPTIMIZATION: No push %rdi here!
    # WHY? After this call, we don't use %rdi anymore.
    # We only need:
    # - %rax (result of fib(n-2))
    # - -8(%rbp) (saved fib(n-1))
    # So there's no point in saving %rdi

    # NO PUSH HERE - This is the key optimization!
    # In fibber.s, there's a push %rdi here, but it's unnecessary

    dec %rdi                  # Decrement n by 1; now n = n - 1
    dec %rdi                  # Decrement n by 1 again; now n = n - 2
                              # Two decrements: original n -> n-1 -> n-2
                              # We can freely modify %rdi now

    call fib                  # Recursive call to fib(n - 2)
                              # Makes recursive call with n-2
                              # Result in %rax

    # NO POP HERE - Nothing to restore!
    # In fibber.s, there's a pop %rdi here, but it was unnecessary

    # COMBINE RESULTS
    # ===============
    # At this point:
    # - fib(n-1) is at -8(%rbp) (from memory)
    # - fib(n-2) is in %rax (from register)
    # Add them to get fib(n)

    add -8(%rbp), %rax        # Add fib(n - 1) (from memory) to fib(n - 2) (in %rax)
                              # Result is fib(n) in %rax
                              # This is what we return

    # FUNCTION EPILOGUE - Cleanup and return
    # =======================================
    mov %rbp, %rsp            # Restore the stack pointer to the base pointer
                              # Deallocate local variables

    pop %rbp                  # Restore the base pointer of the caller
                              # Restore caller's frame pointer

    ret                       # Return to the caller
                              # Result is in %rax

    # BASE CASE HANDLER
    # =================
.base:
    mov %rdi, %rax            # Set return value to n (%rdi)
                              # Return n for n <= 1

    mov %rbp, %rsp            # Restore the stack pointer to the base pointer
    pop %rbp                  # Restore the base pointer of the caller
    ret                       # Return to the caller


// MAIN PROGRAM ENTRY POINT
// ========================
_start:
        call readInt              # Call external function to read an integer from the user
                                  # Returns integer in %rax

        mov %rax, %rdi            # Move input value from %rax to %rdi
                                  # Setup parameter for fib

        call fib                  # Call fib(n)
                                  # Computes nth Fibonacci number
                                  # Result will be in %rax

        mov %rax,%rdi             # Move result to %rdi for writeInt
        call writeInt             # Print the Fibonacci number

        # Prepare to exit the program with fib(n) as the exit code
        xor %rdi, %rdi            # Zero %rdi (exit code 0)
                                  # Move fib(n) result to %rdi (exit code expected in %rdi)
                                  # BUG: xor zeros %rdi, should be mov %rax,%rdi

        mov $60, %rax             # System call number for exit (60)
        syscall                   # Make the system call to exit the program


        ## FIBONACCI SEQUENCE REFERENCE:
        ## fib(0) = 0
        ## fib(1) = 1
        ## fib(2) = 1
        ## fib(3) = 2
        ## fib(4) = 3
        ## fib(5) = 5
        ## fib(6) = 8

// OPTIMIZATION ANALYSIS:
//
// CODE COMPARISON:
//
// fibber.s (original):
//   push %rdi          # Before first call
//   dec %rdi
//   call fib
//   mov %rax,-8(%rbp)
//   pop %rdi           # After first call
//
//   push %rdi          # Before second call (UNNECESSARY)
//   dec %rdi
//   dec %rdi
//   call fib
//   pop %rdi           # After second call (UNNECESSARY)
//
//   add -8(%rbp),%rax
//
// fibber2.s (optimized):
//   push %rdi          # Before first call
//   dec %rdi
//   call fib
//   mov %rax,-8(%rbp)
//   pop %rdi           # After first call
//
//   # NO PUSH            (OPTIMIZATION)
//   dec %rdi
//   dec %rdi
//   call fib
//   # NO POP             (OPTIMIZATION)
//
//   add -8(%rbp),%rax
//
// SAVINGS:
// - 2 fewer instructions per recursive call
// - Reduced stack operations
// - Slightly faster execution
// - Less stack space usage (8 bytes per frame)

// REGISTER LIVENESS ANALYSIS:
//
// After first recursive call (fib(n-1)):
//   %rdi needed? YES - to compute n-2
//   Action: Must save/restore
//
// After second recursive call (fib(n-2)):
//   %rdi needed? NO - never used again
//   Next operation: add -8(%rbp),%rax
//   %rdi not referenced
//   Action: Don't save/restore
//
// This is "liveness analysis" - a compiler optimization technique
// We determine when variables (registers) are "live" (needed)
// and when they're "dead" (no longer needed)

// WHEN IS THIS OPTIMIZATION VALID?
//
// Safe to skip save/restore when:
// 1. Register is not read after the call
// 2. Register is not passed to another call
// 3. Register is not returned
// 4. Function doesn't require preserved register
//
// In this case:
// - %rdi not read after second fib call
// - Only use %rax (result) and -8(%rbp) (saved value)
// - Therefore, saving %rdi is wasted effort

// PERFORMANCE IMPACT:
//
// For each recursive call:
// - Original: 2 push + 2 pop = 4 stack operations
// - Optimized: 1 push + 1 pop = 2 stack operations
// - Savings: 50% reduction in stack ops
//
// For fib(10) (total 177 calls):
// - Original: 177 × 2 = 354 unnecessary stack ops
// - Optimized: 0 unnecessary stack ops
//
// For fib(20) (total 21,891 calls):
// - Original: 21,891 × 2 = 43,782 unnecessary stack ops
// - Optimized: 0 unnecessary stack ops
//
// Stack operations are relatively fast, but eliminating them
// still provides measurable speedup (a few percent)

// TRADE-OFFS:
//
// PROS of optimization:
// + Fewer instructions
// + Less stack usage
// + Slightly faster
// + Shows understanding of register usage
//
// CONS of optimization:
// - Slightly harder to read (less symmetric)
// - Requires careful analysis
// - Easy to make mistakes
// - Original version is "safer" (defensive programming)
//
// WHEN TO OPTIMIZE:
// - Performance-critical code
// - After profiling shows bottleneck
// - When you fully understand the code
// - When correctness is verified
//
// WHEN NOT TO OPTIMIZE:
// - Code is rarely executed
// - Clarity is more important
// - Risk of introducing bugs
// - Premature optimization

// COMPILER OPTIMIZATION:
//
// A good compiler would automatically:
// 1. Perform liveness analysis
// 2. Identify dead stores/loads
// 3. Eliminate unnecessary operations
// 4. Generate optimized code
//
// This is why high-level languages can be as fast as hand-written assembly:
// Compilers are excellent at these micro-optimizations
//
// However, learning to do it manually:
// - Teaches how compilers work
// - Develops optimization thinking
// - Helps understand generated assembly
// - Useful for debugging compiler output

// CORRECTNESS VERIFICATION:
//
// How do we know this optimization is correct?
//
// Method 1: Manual analysis
// - Trace through code
// - Check all uses of %rdi
// - Confirm it's not needed after second call
//
// Method 2: Test both versions
// - Run fibber.s and fibber2.s with same inputs
// - Compare outputs
// - If identical, optimization preserves correctness
//
// Method 3: Formal methods
// - Prove equivalence mathematically
// - Use symbolic execution
// - Advanced technique for critical code

// EQUIVALENT C CODE:
//
// Both fibber.s and fibber2.s correspond to:
//
// long fib(long n) {
//     if (n <= 1) {
//         return n;
//     } else {
//         long a = fib(n - 1);
//         long b = fib(n - 2);
//         return a + b;
//     }
// }
//
// The C compiler would generate code similar to fibber2.s
// (with the optimization) at -O2 or higher optimization levels.

// TRY IT:
//
// Compile both versions:
//   as fibber.s -o fibber.o
//   ld fibber.o readInt.o writeInt.o -o fibber
//
//   as fibber2.s -o fibber2.o
//   ld fibber2.o readInt.o writeInt.o -o fibber2
//
// Test for correctness:
//   ./fibber
//   10
//   55
//
//   ./fibber2
//   10
//   55
//
// Time both versions (subtle difference):
//   time ./fibber
//   30
//   832040
//   (Note the time)
//
//   time ./fibber2
//   30
//   832040
//   (Slightly faster, but difference is small)
//
// The optimization saves ~2% execution time
// Most time is spent in computation, not stack ops

// LEARNING EXERCISES:
//
// 1. Verify the optimization:
//    - Add print statements before/after each call
//    - Show when %rdi is actually used
//    - Confirm it's dead after second call
//
// 2. Try incorrect optimization:
//    - Remove push/pop from first call
//    - Observe incorrect results
//    - Understand why that breaks
//
// 3. Measure performance:
//    - Use 'time' command
//    - Compare fibber vs fibber2
//    - Calculate percentage improvement
//
// 4. Disassemble with objdump:
//    - objdump -d fibber
//    - objdump -d fibber2
//    - Compare instruction counts
//
// 5. Profile with perf:
//    - perf stat ./fibber
//    - perf stat ./fibber2
//    - Compare instruction counts

// COMMON MISCONCEPTIONS:
//
// "We should always optimize"
// - No! Optimize when needed, after measuring
// - Premature optimization is root of evil
// - Clear code > clever code (usually)
//
// "This optimization makes a huge difference"
// - Not really. Savings are modest (~2%)
// - Algorithm choice matters more
// - Iterative version is 1000x faster!
//
// "Assembly is always faster than C"
// - False! Compilers are very good
// - They do optimizations humans miss
// - Hand-written assembly rarely beats -O3
//
// "I should apply this everywhere"
// - Dangerous! Easy to introduce bugs
// - Only when you understand fully
// - Verify correctness first

// BROADER LESSON:
//
// This optimization demonstrates:
// 1. Understanding beats blind coding
// 2. Small optimizations can accumulate
// 3. Trade-offs exist (clarity vs efficiency)
// 4. Correctness comes first
// 5. Profile before optimizing
//
// In practice:
// - Write clear code first
// - Profile to find bottlenecks
// - Optimize hot paths only
// - Test thoroughly
// - Document optimizations
//
// For recursive Fibonacci specifically:
// - This optimization: ~2% improvement
// - Better algorithm (iterative): ~1000x improvement
// - Always choose better algorithm first!
