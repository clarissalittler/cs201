// PEDAGOGICAL PURPOSE:
// This program demonstrates STACK OVERFLOW - a critical runtime error that occurs
// when recursive function calls consume all available stack space.
//
// Key learning objectives:
// 1. Understanding what causes stack overflow
// 2. Seeing how each function call consumes stack space
// 3. Understanding why infinite recursion is dangerous
// 4. Learning that stack space is LIMITED (typically 8MB on Linux)
// 5. Observing how local variables contribute to stack consumption
// 6. Understanding the difference between heap exhaustion and stack overflow
// 7. Recognizing why this recursion never terminates (n keeps INCREASING)

#include "stdio.h"

int counter = 0;        // Global variable to track recursion depth
                        // Global variables are stored in the data segment, NOT on the stack
                        // This counter shows us how deep the recursion went before crashing

// Function: badRec
// This is a DELIBERATELY BROKEN recursive function that will cause stack overflow
//
// WHY IT'S BAD:
// 1. The base case (n == 0) will NEVER be reached because n INCREASES each call
// 2. Each recursive call adds a new stack frame
// 3. Each stack frame holds: return address, old %rbp, stuff, moreStuff, parameters
// 4. The recursion continues FOREVER (or until stack overflow)
//
// WHAT HAPPENS ON THE STACK:
// Each call to badRec(n) allocates a stack frame containing:
//   - Return address (8 bytes) - where to return after function completes
//   - Saved %rbp (8 bytes) - caller's base pointer
//   - Local variable 'stuff' (4 bytes, but aligned to 8)
//   - Local variable 'moreStuff' (4 bytes, but aligned to 8)
//   - Parameter 'n' (passed in register, but might be spilled to stack)
//   Total: approximately 32-48 bytes per call (with alignment)
int badRec(int n){
  // LOCAL VARIABLES:
  // These are allocated on the stack in the current function's stack frame
  int stuff = 1;        // Local variable - stored in this call's stack frame
  int moreStuff = 2;    // Another local variable - more stack consumption

  // Increment the global counter to track recursion depth
  counter++;            // This runs before we check the base case
                        // Shows how many times we've called badRec

  // BASE CASE CHECK:
  // This checks if n == 0, but there's a CRITICAL BUG...
  if(n == 0){
    return 0;           // If we reach n == 0, we would stop recursing
  }
  else{
    // BEFORE RECURSING: Print the current recursion depth
    printf("%d\n",counter);     // Shows us how deep we are in the call stack
                                // This helps us see the progression toward stack overflow

    // THE BUG: We call badRec(n+1) instead of badRec(n-1)
    // This means n INCREASES with each call, not decreases
    // The base case (n == 0) will NEVER be reached
    // This creates INFINITE RECURSION (until stack overflow)
    //
    // WHAT SHOULD IT BE?
    // return badRec(n-1)+stuff+moreStuff;  // This would eventually reach base case
    //
    // STACK GROWTH:
    // Call 1: badRec(1000) -> pushes frame 1 on stack
    // Call 2: badRec(1001) -> pushes frame 2 on stack
    // Call 3: badRec(1002) -> pushes frame 3 on stack
    // ...
    // Eventually: STACK OVERFLOW - no more stack space available
    return badRec(n+1)+stuff+moreStuff;
  }
}

int main(){
  // Start the recursion with n=1000
  // This means we're starting far from the base case (0)
  // And we're moving in the WRONG direction (incrementing instead of decrementing)
  badRec(1000);

  // THIS LINE WILL NEVER EXECUTE
  // Why? The program will crash with a stack overflow before returning
  return 0;
}

// EXPECTED BEHAVIOR:
//
// When you run this program:
// 1. It prints increasing numbers (1, 2, 3, 4, ...)
// 2. The counter shows the recursion depth
// 3. Eventually (after hundreds of thousands of calls), you'll see:
//    Segmentation fault (core dumped)
//
// WHY SEGMENTATION FAULT?
// - The stack pointer (%rsp) grows downward toward lower addresses
// - There's a guard page below the stack
// - When %rsp crosses into the guard page, the OS sends SIGSEGV
// - This terminates the program with "Segmentation fault"

// TYPICAL STACK LIMITS:
// - Linux default: 8 MB stack size
// - With ~40 bytes per frame, we can have ~200,000 recursive calls
// - The exact number depends on:
//   * Stack size limit (ulimit -s)
//   * Size of each stack frame
//   * Other stack usage by the program

// STACK OVERFLOW VISUALIZATION:
//
// High Memory (stack bottom)
// +---------------------------+
// | main()'s stack frame      |  <- Start here
// +---------------------------+
// | badRec(1000) frame        |  <- First call
// | - return address          |
// | - saved %rbp              |
// | - stuff = 1               |
// | - moreStuff = 2           |
// +---------------------------+
// | badRec(1001) frame        |  <- Second call
// | - return address          |
// | - saved %rbp              |
// | - stuff = 1               |
// | - moreStuff = 2           |
// +---------------------------+
// | badRec(1002) frame        |  <- Third call
// |     ...                   |
// +---------------------------+
//        ... thousands more ...
// +---------------------------+
// | badRec(200000+) frame     |  <- %rsp is here when we crash
// +---------------------------+
// | GUARD PAGE                |  <- Causes SIGSEGV when accessed
// +---------------------------+
// Low Memory

// WHAT THIS TEACHES:
//
// 1. RECURSION MUST HAVE A REACHABLE BASE CASE
//    - Every recursive call should move TOWARD the base case
//    - This code moves AWAY from it (n increases, not decreases)
//
// 2. STACK SPACE IS LIMITED
//    - Unlike heap memory, stack size is fixed (typically 8MB)
//    - Each function call consumes stack space
//    - Too many calls = stack overflow
//
// 3. LOCAL VARIABLES CONSUME STACK SPACE
//    - 'stuff' and 'moreStuff' make each frame larger
//    - Larger frames = fewer possible calls before overflow
//
// 4. TAIL RECURSION OPTIMIZATION (not applied here)
//    - If this were tail-recursive, compiler could optimize it
//    - But return badRec(n+1)+stuff requires keeping the frame
//    - The addition happens AFTER the recursive call returns

// TRY IT:
// Compile: gcc overflowPractice.c -o overflowPractice
// Run: ./overflowPractice
//
// You'll see output like:
// 1
// 2
// 3
// ...
// 261978
// 261979
// Segmentation fault (core dumped)
//
// The exact number varies by system and stack size.
//
// EXPERIMENT:
// Check your stack limit: ulimit -s  (shows KB, default is 8192 = 8MB)
// Reduce it: ulimit -s 1024  (1MB stack)
// Run again: ./overflowPractice  (crashes sooner)
//
// COMPARE WITH FIX:
// Change line 14 to: return badRec(n-1)+stuff+moreStuff;
// Change line 19 to: badRec(10);  (start with smaller value)
// Now it works! It counts down from 10 to 0 and terminates.

// REAL-WORLD IMPLICATIONS:
// - Stack overflow is a common cause of crashes in recursive algorithms
// - Always ensure recursion has a terminating condition
// - For deep recursion, consider iterative solutions or tail-recursion
// - Some languages (not C) have unlimited stack via heap-allocated frames
