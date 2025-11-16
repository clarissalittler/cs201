// PEDAGOGICAL PURPOSE:
// This program demonstrates correct extraction of child exit status using bit manipulation.
// Key learning objectives:
// 1. Understanding that wait() returns a packed status word
// 2. Exit codes are stored in bits 8-15 of the status
// 3. Manual extraction using bit shifting and masking
// 4. The relationship between WEXITSTATUS macro and bit operations
// 5. Comparing raw status vs. extracted exit code
// 6. Building understanding before using convenience macros

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Contains fork(), getpid()
#include <sys/types.h>   // Contains pid_t type definition
#include <sys/wait.h>    // Contains wait() and WEXITSTATUS()

int main(){

  // CREATE CHILD PROCESS:
  pid_t pid = fork();
  // Parent gets child's PID, child gets 0

  // VARIABLE TO STORE STATUS:
  int childReturn;
  // This will receive the packed status word from wait()

  // ERROR HANDLING:
  if(pid < 0){
    perror("Fork failed");
    return 1;
  }

  // CHILD PROCESS CODE:
  else if(pid ==0){
    // Child process logic

    // PROMPT FOR INPUT:
    printf("Hey, give me a number, will ya?\n");

    // READ INTEGER:
    int num;
    int success = scanf("%d",&num);
    // success = 1 if integer was read successfully
    // success = 0 if input wasn't a valid integer

    // RETURN BASED ON INPUT VALIDITY:
    if(success){
      // Input was valid
      return 0;
      // Exit with status 0 (success)
    }
    else{
      // Input was invalid
      return 1;
      // Exit with status 1 (failure)
    }
  }

  // PARENT PROCESS CODE:
  else{
    // Parent process logic

    // WAIT FOR CHILD AND GET STATUS:
    wait(&childReturn);
    // wait() stores the status information in childReturn
    //
    // WHAT'S IN childReturn?
    // It's NOT just the exit code - it's a packed integer:
    // Bits 0-6:   Signal number (if terminated by signal)
    // Bit 7:      Core dump flag
    // Bits 8-15:  Exit status (what child returned)
    // Bits 16+:   Other flags
    //
    // EXAMPLE:
    // If child returns 0: childReturn = 0x0000 (all bits zero)
    // If child returns 1: childReturn = 0x0100 (1 in bits 8-15)
    // If child returns 5: childReturn = 0x0500 (5 in bits 8-15)

    // EXTRACT EXIT CODE MANUALLY:
    // WEXITSTATUS(int) => (int >> 8) & 255
    printf("Here's the int returned by childReturn: %d\n",(childReturn >> 8) & 255);
    //
    // BREAKDOWN OF THE EXTRACTION:
    // 1. (childReturn >> 8)
    //    Right shift by 8 bits moves the exit code to the low byte
    //    Example: 0x0100 >> 8 = 0x0001
    //
    // 2. & 255
    //    Mask with 255 (0xFF) to keep only the low 8 bits
    //    This gives us the exit code (0-255 range)
    //    Example: 0x0001 & 0xFF = 0x0001 = 1
    //
    // COMPLETE EXAMPLE:
    // If child returns 1:
    //   childReturn = 256 (decimal) = 0x0100 (hex)
    //   childReturn >> 8 = 1
    //   1 & 255 = 1 ← correct exit code!
    //
    // WHY THIS WORKS:
    // This is exactly what the WEXITSTATUS macro does
    // We're implementing it manually to understand the mechanism

    return 0;
  }

  // UNREACHABLE CODE:
  return 0;
}

// BIT MANIPULATION EXPLAINED:
//
// Let's trace through with child returning 1:
//
// Step 1: Child exits with return 1
//   Kernel stores this in bits 8-15 of status word
//
// Step 2: Parent calls wait(&childReturn)
//   childReturn = 0000 0001 0000 0000 (binary)
//               = 0x0100 (hex)
//               = 256 (decimal)
//
// Step 3: Right shift by 8 bits
//   childReturn >> 8 = 0000 0000 0000 0001 (binary)
//                    = 0x0001 (hex)
//                    = 1 (decimal)
//
// Step 4: Mask with 255 (0xFF)
//   1 & 255 = 0000 0001 & 1111 1111
//           = 0000 0001
//           = 1
//
// Result: We extracted the exit code correctly!

// WHY & 255?
//
// The mask ensures we only keep the low 8 bits.
// This handles potential sign extension from the right shift.
//
// Without the mask:
// - If childReturn had high bits set (e.g., signal termination)
// - Right shift might propagate sign bit (implementation-defined)
// - Mask guarantees we only get the exit code byte
//
// Example with signal termination:
//   childReturn = 0x010B (exited with code 1, killed by signal 11)
//   childReturn >> 8 = 0x0001
//   0x0001 & 0xFF = 0x0001 = 1 (correct exit code extracted)

// COMPARISON WITH pic3.c:
//
// pic3.c (WRONG):
// printf("Here's the int returned by childReturn: %d", childReturn);
// Output: 256 (when child returns 1)
//
// pid3.c (CORRECT):
// printf("Here's the int returned by childReturn: %d", (childReturn >> 8) & 255);
// Output: 1 (when child returns 1)

// EXECUTION TRACE (child enters valid input):
//
// Time    Process    Action                              State/Output
// ----    -------    ------                              ------------
// T0      Parent     fork()
// T1      Child      Created
// T2      Child      printf("Hey, give me...")           "Hey, give me a number..."
// T3      Child      scanf() waits
// [User enters: 42]
// T4      Child      scanf() reads 42, success = 1
// T5      Child      return 0                            Child exits with 0
// T6      Parent     wait(&childReturn)                  childReturn = 0x0000 = 0
// T7      Parent     (0 >> 8) & 255 = 0
// T8      Parent     printf("...%d\n", 0)                "...childReturn: 0"
// T9      Parent     return 0

// EXECUTION TRACE (child enters invalid input):
//
// Time    Process    Action                              State/Output
// ----    -------    ------                              ------------
// T0-T3   [Same as above]
// [User enters: hello]
// T4      Child      scanf() fails, success = 0
// T5      Child      return 1                            Child exits with 1
// T6      Parent     wait(&childReturn)                  childReturn = 0x0100 = 256
// T7      Parent     (256 >> 8) & 255
//                    = (1) & 255
//                    = 1
// T8      Parent     printf("...%d\n", 1)                "...childReturn: 1"
// T9      Parent     return 0

// STATUS WORD FORMAT:
//
// Bits    Purpose                     Example (child returns 5)
// ----    -------                     -------------------------
// 0-6     Signal number               0000000 (not signaled)
// 7       Core dump flag              0 (no core dump)
// 8-15    Exit status                 00000101 (exit code 5)
// 16-31   Other flags                 0000000000000000
//
// Full word: 0000 0000 0000 0101 0000 0000 = 0x0500 = 1280 (decimal)
// Extracted: (1280 >> 8) & 255 = 5 & 255 = 5 ← correct!

// CONCEPTUAL EXPLANATION:
//
// WHY PACK MULTIPLE VALUES?
// The status word efficiently encodes:
// 1. How did the process terminate? (normal exit vs signal)
// 2. If normal exit: what was the exit code?
// 3. If signaled: which signal? Was there a core dump?
// 4. Additional flags for stopped processes, continued, etc.
//
// ALTERNATIVES NOT USED:
// Could have separate fields:
//   struct { int exit_code; int signal; bool core_dump; ... }
// But a single packed integer is:
// - More efficient (one word instead of struct)
// - Compatible with old Unix systems
// - Simple to pass through system calls
//
// THE ABSTRACTION:
// Later examples will use WEXITSTATUS(childReturn) instead
// But understanding the bit manipulation helps you know:
// - How the macro works internally
// - Why you can't just print childReturn
// - How to debug strange status values

// COMMON QUESTIONS:
//
// Q: Why is the exit code in bits 8-15 instead of 0-7?
// A: Bits 0-7 are used for signal information.
//    This allows distinguishing normal exit from signal termination.
//
// Q: Can a process return values > 255?
// A: No. Only 8 bits are available (bits 8-15).
//    If you return 256, it wraps to 0 (256 & 0xFF = 0).
//
// Q: What if I only shift without masking?
// A: Usually works, but masking is safer:
//    - Handles potential sign extension
//    - Guarantees result is in 0-255 range
//    - More portable across platforms
//
// Q: Is this the same as WEXITSTATUS?
// A: Yes! WEXITSTATUS is typically defined as:
//    #define WEXITSTATUS(s) (((s) >> 8) & 0xFF)
//    We're doing exactly what the macro does.
//
// Q: When would bits 0-7 be non-zero?
// A: When the child is terminated by a signal:
//    - Bits 0-6 contain the signal number
//    - Bit 7 is set if a core dump was generated
//    Example: killed by SIGSEGV (11): bits 0-7 = 0x0B

// NEXT STEP: pid4.c
// The next example (pid4.c) uses the WEXITSTATUS macro
// and makes decisions based on the exit code.
// Now you understand what WEXITSTATUS does under the hood!

// TRY IT:
// Compile: gcc pid3.c -o pid3
//
// Test 1 - Valid input:
// $ ./pid3
// Hey, give me a number, will ya?
// 42
// Here's the int returned by childReturn: 0
//
// Test 2 - Invalid input:
// $ ./pid3
// Hey, give me a number, will ya?
// hello
// Here's the int returned by childReturn: 1
// ^^^^^^^ Now correctly shows 1, not 256!
//
// Experiments:
// 1. Compare with pic3.c output (which shows 256 instead of 1)
//
// 2. Modify child to return different values:
//    return 5;
//    Observe: Parent prints "...childReturn: 5"
//
// 3. Print both raw and extracted values:
//    printf("Raw: %d, Extracted: %d\n", childReturn, (childReturn >> 8) & 255);
//    See: "Raw: 256, Extracted: 1" for invalid input
//
// 4. Use WEXITSTATUS macro instead:
//    printf("...%d\n", WEXITSTATUS(childReturn));
//    Should give same result as manual extraction
