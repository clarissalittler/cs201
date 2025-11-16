// PEDAGOGICAL PURPOSE:
// This program demonstrates recursive function implementation in x86-64 assembly.
// It computes Fibonacci numbers using the classic recursive algorithm, showing how
// to manage the call stack, preserve registers, handle base cases, and make recursive calls.
//
// Key learning objectives:
// 1. Understanding recursive function calls in assembly
// 2. Learning proper stack frame management (prologue/epilogue)
// 3. Understanding how to preserve function parameters across recursive calls
// 4. Learning base case handling in recursive algorithms
// 5. Seeing how return values propagate through recursive call chains
// 6. Understanding local variable storage on the stack
// 7. Learning the importance of register preservation in recursion
// 8. Understanding why recursion requires careful stack management
// 9. Seeing how high-level recursion maps to low-level assembly
// 10. Understanding the performance implications of naive recursion

      ### Program: Compute Fibonacci number recursively in assembly
### This program reads an integer n from the user,
### computes the nth Fibonacci number using recursion,
### and exits with the Fibonacci number as the exit code.
### The result is not printed to the screen, but returned as the exit code.

    .section .text            # Code section where the instructions are stored
    .global _start            # Define _start as global symbol; entry point of the program
    .extern readInt           # Declare that readInt function is defined externally (provided elsewhere)
    .extern writeInt          # Declare that writeInt function is defined externally


// FIBONACCI RECURSIVE FUNCTION
// ============================
// Computes the nth Fibonacci number using the classic recursive definition:
//   fib(0) = 0
//   fib(1) = 1
//   fib(n) = fib(n-1) + fib(n-2)  for n > 1
//
// ALGORITHM:
//   if n <= 1:
//       return n
//   else:
//       return fib(n-1) + fib(n-2)
//
// WHY THIS ALGORITHM?
// - Direct translation of mathematical definition
// - Pedagogically clear (shows recursion structure)
// - Demonstrates multiple recursive calls
// - Shows stack frame management
//
// PERFORMANCE WARNING:
// - This is exponentially slow: O(2^n) time complexity
// - fib(40) makes ~200 million recursive calls
// - Production code should use iterative or memoized version
// - But perfect for teaching recursion!
//
// PARAMETERS:
//   %rdi - n (the Fibonacci index to compute)
//
// RETURNS:
//   %rax - fib(n) (the nth Fibonacci number)
//
// REGISTERS USED:
//   %rdi - parameter n (must preserve across calls)
//   %rax - return value
//   %rbp - base pointer (callee-saved, must preserve)
//   %rsp - stack pointer
//
fib:
    # FUNCTION PROLOGUE - Setup stack frame
    # ======================================
    # Every function that uses local variables or calls other functions
    # needs to set up a stack frame. The prologue establishes this frame.

    push %rbp                 # Save the base pointer of the previous frame (caller)
                              # %rbp is a callee-saved register
                              # We must preserve it and restore before returning
                              # The caller's %rbp value is now on the stack

    mov %rsp, %rbp            # Set up the base pointer for this function's stack frame
                              # %rbp now points to our stack frame base
                              # WHY? %rbp provides stable reference while %rsp changes
                              # Local variables accessed relative to %rbp
                              # Example: -8(%rbp), -16(%rbp)

    sub $16, %rsp             # Allocate 16 bytes of space on the stack for local variables
                              # ensure 16-byte alignment (System V ABI requirement)
                              # STACK LAYOUT:
                              # %rbp + 0  : saved %rbp
                              # %rbp - 8  : local variable (will store fib(n-1))
                              # %rbp - 16 : padding for alignment
                              # WHY 16 BYTES? System V ABI requires 16-byte stack alignment
                              # before calls. We use 8 bytes for data, 8 for alignment.

    # BASE CASE CHECK
    # ===============
    # Fibonacci definition:
    #   fib(0) = 0
    #   fib(1) = 1
    # Both handled by same code: return n when n <= 1

    cmp $1, %rdi              # Compare input n (%rdi) with 1
                              # Sets flags based on (%rdi - 1)
                              # This checks if n <= 1

    jle .base                 # If n <= 1, jump to .base label (base case)
                              # jle = jump if less or equal (signed)
                              # For n=0 or n=1, we just return n
                              # For n>=2, we continue to recursive case

    # RECURSIVE CASE: fib(n) = fib(n-1) + fib(n-2)
    # ============================================
    # We need to:
    # 1. Compute fib(n-1)
    # 2. Save the result
    # 3. Compute fib(n-2)
    # 4. Add the two results
    #
    # CHALLENGE: Preserve n across recursive calls
    # - fib(n-1) will modify %rdi and %rax
    # - We need original n to compute fib(n-2)
    # - Solution: push/pop %rdi around calls

    # FIRST RECURSIVE CALL: fib(n-1)
    # ==============================
    push %rdi                 # Save current n on the stack before modifying it
                              # CRITICAL: We need n for the second recursive call
                              # Stack: [..., saved_rbp, fib_n-1_space, padding, n]
                              # After push: %rsp -= 8

    dec %rdi                  # Decrement n by 1; compute n - 1
                              # dec = decrement (subtract 1)
                              # %rdi now contains n-1
                              # This is the parameter for fib(n-1)

    call fib                  # Recursive call to fib(n - 1)
                              # This will:
                              # - Push return address
                              # - Jump to fib
                              # - Execute fib(n-1) recursively
                              # - Return with result in %rax
                              # Result returned in %rax

    mov %rax, -8(%rbp)        # Store fib(n - 1) result at offset -8(%rbp)
                              # This is our local variable storage
                              # We need to save it before the next recursive call
                              # WHY? The next call will overwrite %rax
                              # -8(%rbp) is the first local variable slot

    pop %rdi                  # Restore n from the stack
                              # %rdi now contains original n again
                              # Stack pointer moves up: %rsp += 8

    # SECOND RECURSIVE CALL: fib(n-2)
    # ===============================
    push %rdi                 # Save n on the stack again before modifying it
                              # WHY AGAIN? We'll need it for the add instruction
                              # Actually, this isn't strictly necessary here
                              # since we don't use %rdi after the call
                              # But it's defensive programming

    dec %rdi                  # Decrement n by 1; now n = n - 1
    dec %rdi                  # Decrement n by 1 again; now n = n - 2
                              # Two decrements: n -> n-1 -> n-2
                              # Could also: sub $2,%rdi
                              # %rdi now contains n-2

    call fib                  # Recursive call to fib(n - 2)
                              # Another recursive call
                              # Goes down the recursion tree
                              # Result returned in %rax

    pop %rdi                  # Restore n from the stack
                              # Balance the earlier push
                              # Maintains stack hygiene

    # COMBINE RESULTS
    # ===============
    # At this point:
    # - fib(n-1) is stored at -8(%rbp)
    # - fib(n-2) is in %rax
    # Need to add them: %rax = fib(n-1) + fib(n-2)

    add -8(%rbp), %rax        # Add fib(n - 1) (from memory) to fib(n - 2) (in %rax)
                              # Result is fib(n) = fib(n-1) + fib(n-2)
                              # %rax now contains the final result
                              # This is the value we'll return

    # FUNCTION EPILOGUE - Cleanup and return
    # =======================================
    # Restore stack frame and return to caller

    mov %rbp, %rsp            # Restore the stack pointer to the base pointer
                              # Deallocates local variables
                              # Equivalent to: add $16,%rsp
                              # But mov %rbp,%rsp works regardless of local var size

    pop %rbp                  # Restore the base pointer of the caller
                              # Pops the saved %rbp from prologue
                              # Now %rbp has caller's frame pointer

    ret                       # Return to the caller
                              # Pops return address from stack
                              # Jumps to that address
                              # Result is in %rax (fib(n))

    # BASE CASE HANDLER
    # =================
    # When n <= 1, we return n directly
    # fib(0) = 0, fib(1) = 1

.base:
    mov %rdi, %rax            # Set return value to n (%rdi)
                              # For n=0: return 0
                              # For n=1: return 1
                              # This matches Fibonacci definition

    mov %rbp, %rsp            # Restore the stack pointer to the base pointer
                              # Same as in normal epilogue
                              # Deallocate local variables

    pop %rbp                  # Restore the base pointer of the caller
                              # Restore caller's frame pointer

    ret                       # Return to the caller
                              # Result is in %rax (either 0 or 1)


// MAIN PROGRAM ENTRY POINT
// ========================
// Reads n from user, computes fib(n), prints result, exits

_start:
        call readInt              # Call external function to read an integer from the user
                                  # User types a number
                                  # readInt parses it and returns in %rax
                                  # Example: user types "5" -> %rax = 5

        mov %rax, %rdi            # Move input value from %rax to %rdi
                                  # %rdi is the first parameter for fib
                                  # Now %rdi = n (the Fibonacci index)

        call fib                  # Call fib(n)
                                  # Computes nth Fibonacci number recursively
                                  # Result will be in %rax
                                  # Example: fib(5) = 5

        mov %rax,%rdi             # Move result to %rdi for writeInt
                                  # writeInt expects parameter in %rdi
        call writeInt             # Print the Fibonacci number
                                  # Converts to ASCII and outputs

        # Prepare to exit the program with fib(n) as the exit code
        xor %rdi, %rdi            # Move fib(n) result to %rdi (exit code expected in %rdi)
                                  # Actually, this zeros %rdi (xor with itself)
                                  # BUG: Should be "mov %rax,%rdi" to exit with fib(n)
                                  # As written, exits with 0 regardless of result

        mov $60, %rax             # System call number for exit (60)
        syscall                   # Make the system call to exit the program


        ## FIBONACCI SEQUENCE REFERENCE:
        ## fib(0) = 0
        ## fib(1) = 1
        ## fib(2) = 1   (0 + 1)
        ## fib(3) = 2   (1 + 1)
        ## fib(4) = 3   (1 + 2)
        ## fib(5) = 5   (2 + 3)
        ## fib(6) = 8   (3 + 5)

// RECURSION TREE VISUALIZATION:
//
// For fib(4):
//
//                    fib(4)
//                   /      \
//              fib(3)      fib(2)
//              /    \      /    \
//          fib(2) fib(1) fib(1) fib(0)
//          /   \
//      fib(1) fib(0)
//
// Call order (depth-first):
// 1. fib(4) calls fib(3)
// 2. fib(3) calls fib(2)
// 3. fib(2) calls fib(1) -> base case, returns 1
// 4. fib(2) calls fib(0) -> base case, returns 0
// 5. fib(2) returns 1+0 = 1
// 6. fib(3) calls fib(1) -> base case, returns 1
// 7. fib(3) returns 1+1 = 2
// 8. fib(4) calls fib(2)
// 9. fib(2) calls fib(1) -> base case, returns 1
// 10. fib(2) calls fib(0) -> base case, returns 0
// 11. fib(2) returns 1+0 = 1
// 12. fib(4) returns 2+1 = 3
//
// Total calls: 9 (including base cases)

// STACK EVOLUTION EXAMPLE:
//
// Computing fib(3):
//
// Initial: _start calls fib(3)
//   Stack: [return_to_start]
//   %rdi = 3
//
// fib(3) prologue:
//   push %rbp -> Stack: [return_to_start, old_rbp]
//   mov %rsp,%rbp
//   sub $16,%rsp -> Stack: [return_to_start, old_rbp, local, padding]
//
// fib(3) calls fib(2):
//   push %rdi -> Stack: [..., 3]
//   %rdi = 2
//   call fib -> Stack: [..., 3, return_to_fib3]
//
// fib(2) prologue:
//   push %rbp -> Stack: [..., 3, return_to_fib3, old_rbp]
//   sub $16,%rsp
//
// fib(2) calls fib(1):
//   push %rdi -> Stack: [..., 2]
//   %rdi = 1
//   call fib
//
// fib(1) executes base case:
//   %rax = 1
//   returns
//
// Back in fib(2):
//   mov %rax,-8(%rbp) -> store 1
//   calls fib(0)
//   %rax = 0
//   add -8(%rbp),%rax -> %rax = 1
//   epilogue, returns
//
// Back in fib(3):
//   mov %rax,-8(%rbp) -> store 1
//   calls fib(1)
//   %rax = 1
//   add -8(%rbp),%rax -> %rax = 2
//   epilogue, returns
//
// Final result: %rax = 2

// IMPORTANT CONCEPTS:
//
// 1. RECURSION IN ASSEMBLY:
//    - Same function can call itself
//    - Each call gets its own stack frame
//    - Frames are nested (stack grows down)
//    - Base case stops recursion
//    - Return values propagate back up
//
// 2. STACK FRAME STRUCTURE:
//    - Saved %rbp (links to previous frame)
//    - Local variables (accessed via %rbp offsets)
//    - Padding for alignment
//    - Saved parameters (via push/pop)
//    - Return address (pushed by call)
//
// 3. REGISTER PRESERVATION:
//    - %rbp is callee-saved (must preserve)
//    - %rdi is caller-saved (we save it explicitly)
//    - %rax is caller-saved (used for return value)
//    - push/pop ensure registers restored
//
// 4. BASE CASE CRITICAL:
//    - Without base case, infinite recursion
//    - Stack overflow eventually
//    - Here: n <= 1 stops recursion
//    - Must be reachable from any input
//
// 5. PERFORMANCE WARNING:
//    - Exponential time: O(2^n)
//    - Exponential stack depth: O(n)
//    - Many redundant calculations
//    - fib(5) calculates fib(3) twice
//    - fib(6) calculates fib(4) twice, fib(3) three times
//    - Solution: memoization or iteration

// CALL COUNT ANALYSIS:
//
// Calls needed for fib(n):
//   fib(0) = 1 call
//   fib(1) = 1 call
//   fib(2) = 3 calls (1 + 1 + 1)
//   fib(3) = 5 calls (3 + 1 + 1)
//   fib(4) = 9 calls (5 + 3 + 1)
//   fib(5) = 15 calls (9 + 5 + 1)
//   fib(n) ≈ 2 * fib(n) - 1 calls (approximately)
//
// This grows exponentially!
// fib(20) ≈ 20,000 calls
// fib(30) ≈ 2,000,000 calls
// fib(40) ≈ 200,000,000 calls (takes seconds)

// EQUIVALENT C CODE:
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
// int main() {
//     long n = readInt();
//     long result = fib(n);
//     writeInt(result);
//     return 0;  // Note: assembly version has bug here
// }

// TRY IT:
//
// Compile and link:
//   as fibber.s -o fibber.o
//   as readInt.s -o readInt.o
//   as writeInt.s -o writeInt.o
//   ld fibber.o readInt.o writeInt.o -o fibber
//
// Run with small inputs:
//   ./fibber
//   5           (user types)
//   5           (output: fib(5) = 5)
//
//   ./fibber
//   10          (user types)
//   55          (output: fib(10) = 55)
//
// Try timing large inputs:
//   time ./fibber
//   30          (takes a moment)
//   832040      (fib(30) = 832,040)
//
//   time ./fibber
//   40          (takes several seconds!)
//   102334155   (fib(40) = 102,334,155)
//
// Don't try fib(50)! It would take hours.

// ITERATIVE VERSION (for comparison):
//
// An iterative version would be O(n) instead of O(2^n):
//
// long fib_iterative(long n) {
//     if (n <= 1) return n;
//     long a = 0, b = 1;
//     for (long i = 2; i <= n; i++) {
//         long temp = a + b;
//         a = b;
//         b = temp;
//     }
//     return b;
// }
//
// This computes fib(40) instantly instead of taking seconds!

// MEMOIZED VERSION (for comparison):
//
// Store computed results to avoid redundant calculations:
//
// long memo[100] = {0};  // Initialized to 0
//
// long fib_memo(long n) {
//     if (n <= 1) return n;
//     if (memo[n] != 0) return memo[n];  // Already computed
//     memo[n] = fib_memo(n-1) + fib_memo(n-2);
//     return memo[n];
// }
//
// This is still recursive but O(n) time!
// Each fib(k) is computed only once.

// DEBUGGING TIPS:
//
// 1. Add print statements:
//    - Print n on entry to each fib call
//    - Print return value before returning
//    - See the call tree visually
//
// 2. Use a debugger (gdb):
//    - Set breakpoint at fib
//    - Step through recursion
//    - Examine stack frames (bt command)
//    - Inspect registers and memory
//
// 3. Trace stack depth:
//    - Add counter for recursive depth
//    - Print current depth
//    - Verify it doesn't exceed expectations
//
// 4. Start with small inputs:
//    - fib(0), fib(1) - base cases
//    - fib(2) - simplest recursive case
//    - fib(3) - two recursive calls
//    - Gradually increase

// COMMON BUGS:
//
// 1. Forgetting to save %rdi:
//    - First recursive call modifies it
//    - Second call gets wrong value
//    - Results in incorrect answer
//
// 2. Not preserving %rbp:
//    - Corrupts stack frame chain
//    - Caller's local variables get overwritten
//    - Crash or wrong results
//
// 3. Stack misalignment:
//    - Not allocating 16-byte aligned space
//    - Can cause issues with some functions
//    - System V ABI violation
//
// 4. Missing base case:
//    - Infinite recursion
//    - Stack overflow
//    - Segmentation fault
//
// 5. Wrong base case condition:
//    - Using < instead of <=
//    - fib(1) would recurse to fib(0), fib(-1)
//    - Negative numbers cause problems
