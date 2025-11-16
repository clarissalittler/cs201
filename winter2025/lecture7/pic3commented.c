// PEDAGOGICAL PURPOSE:
// This program demonstrates improper handling of child process return values.
// Key learning objectives:
// 1. Understanding that wait() stores MORE than just the exit code
// 2. The status integer contains multiple pieces of information packed together
// 3. Raw status values are NOT the same as exit codes
// 4. Why we need macros like WEXITSTATUS() to extract exit codes
// 5. Error handling with fork() and scanf()
// 6. Parent-child coordination using wait()
// 7. This is a "what NOT to do" example to motivate proper status handling

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Contains fork(), getpid()
#include <sys/types.h>   // Contains pid_t type definition
#include <sys/wait.h>    // Contains wait() and WEXITSTATUS()

int main(){

  // CREATE A CHILD PROCESS:
  pid_t pid = fork();
  // After fork(), two processes exist:
  // - Parent: pid contains child's PID (positive number)
  // - Child: pid contains 0

  // VARIABLE TO STORE CHILD'S STATUS:
  int childReturn;
  // This will hold the status information from wait()
  // IMPORTANT: This is NOT just the exit code!
  // It's a packed integer with multiple fields

  // CHECK FOR FORK FAILURE:
  if(pid < 0){
    // fork() returns -1 on failure (out of resources, etc.)
    perror("Fork failed");
    // perror() prints error message with system error description
    return 1;
    // Exit with error status
  }

  // CHILD PROCESS CODE:
  else if(pid ==0){
    // We are the child process (fork() returned 0)

    // PROMPT FOR INPUT:
    printf("Hey, give me a number, will ya?\n");

    // READ INTEGER FROM USER:
    int num;
    int success = scanf("%d",&num);
    // scanf() returns the number of items successfully read
    // success = 1 if a number was read
    // success = 0 if input wasn't a valid integer
    // success = EOF if end-of-file or error occurred

    // CHECK IF INPUT WAS VALID:
    if(success){
      // scanf successfully read an integer
      // EXIT WITH SUCCESS:
      return 0;
      // Child exits with status 0 (success)
      // NOTE: We read the number but don't use it
      // This program only cares if input was valid
    }
    else{
      // scanf failed to read a valid integer
      // EXIT WITH FAILURE:
      return 1;
      // Child exits with status 1 (failure)
    }
  }

  // PARENT PROCESS CODE:
  else{
    // We are the parent process (pid > 0)

    // WAIT FOR CHILD TO FINISH:
    wait(&childReturn);
    // wait() does several things:
    // 1. Blocks until a child process terminates
    // 2. Collects the child's exit status
    // 3. Stores status information in childReturn
    // 4. Returns the PID of the child that exited
    //
    // WHAT'S IN childReturn?
    // It's NOT just the exit code!
    // It contains:
    // - Exit status (bits 8-15)
    // - Signal number if killed by signal (bits 0-6)
    // - Core dump flag (bit 7)
    // - Other status flags

    // THE BUG: PRINTING RAW STATUS:
    printf("Here's the int returned by childReturn: %d",childReturn);
    // THIS IS WRONG!
    // childReturn is a packed status word, not the actual exit code
    //
    // WHAT WILL BE PRINTED?
    // If child exited with status 0:
    //   childReturn will be 0 (happens to be correct)
    // If child exited with status 1:
    //   childReturn will be 256 (NOT 1!)
    //   Because exit status is stored in bits 8-15
    //   1 << 8 = 256
    //
    // THE RIGHT WAY:
    // Use WEXITSTATUS(childReturn) to extract the exit code
    // We'll see this in the next example (pid3.c or pid4.c)

    return 0;
    // Parent exits successfully
  }

  // UNREACHABLE CODE:
  // Both parent and child return before reaching here
  return 0;
}

// EXECUTION TRACE (Child enters valid number):
//
// Time    Process    Action                                  Output/State
// ----    -------    ------                                  ------------
// T0      Parent     main() starts
// T1      Parent     pid = fork()
// T2      Parent     fork() returns child's PID (e.g., 5001)
//         Child      fork() returns 0
// T3      Child      pid == 0, enter child branch
// T4      Child      printf("Hey, give me...")               "Hey, give me a number..."
// T5      Child      scanf() waits for input
// [User types: 42 <Enter>]
// T6      Child      scanf() reads 42, success = 1
// T7      Child      success is true, return 0               Child exits with 0
// T8      Parent     wait(&childReturn) collects status
//                    childReturn = 0 (no signal, exit code 0)
// T9      Parent     printf("...childReturn: %d", 0)         "...childReturn: 0"
// T10     Parent     return 0                                Parent exits

// EXECUTION TRACE (Child enters invalid input):
//
// Time    Process    Action                                  Output/State
// ----    -------    ------                                  ------------
// T0-T5   [Same as above]
// [User types: hello <Enter>]
// T6      Child      scanf() fails to read int, success = 0
// T7      Child      success is false, return 1              Child exits with 1
// T8      Parent     wait(&childReturn) collects status
//                    childReturn = 256 (exit code 1 << 8)
// T9      Parent     printf("...childReturn: %d", 256)       "...childReturn: 256"
// T10     Parent     return 0                                Parent exits

// THE PROBLEM DEMONSTRATED:
//
// When child exits with status code N, wait() stores N in bits 8-15:
// Exit Code    childReturn Value    Binary Representation
// ---------    -----------------    ---------------------
//     0              0               0000000000000000
//     1            256               0000000100000000  (1 << 8)
//     2            512               0000001000000000  (2 << 8)
//   127          32512               0111111100000000  (127 << 8)
//
// Notice: Only exit code 0 gives childReturn = 0
// All other exit codes are multiplied by 256!
//
// WHY THIS ENCODING?
// The status word encodes multiple pieces of information:
// Bits 0-6:   Signal number (if terminated by signal)
// Bit 7:      Core dump flag (1 if core dumped)
// Bits 8-15:  Exit status (what the program returned)
// Other bits: Additional flags
//
// This allows wait() to distinguish:
// - Normal exit with code N: bits 8-15 = N
// - Killed by signal S: bits 0-6 = S
// - Stopped (not terminated): different pattern

// THE SOLUTION:
//
// Use the WEXITSTATUS() macro to extract the exit code:
//
// int exit_code = WEXITSTATUS(childReturn);
// printf("Child exited with code: %d\n", exit_code);
//
// WEXITSTATUS implementation (conceptually):
// #define WEXITSTATUS(status) (((status) >> 8) & 0xFF)
//
// This shifts right by 8 bits and masks to get the exit code.
//
// OTHER USEFUL MACROS:
// WIFEXITED(status)    - Returns true if child exited normally
// WIFSIGNALED(status)  - Returns true if child was killed by signal
// WTERMSIG(status)     - Returns signal number that killed child
// WIFSTOPPED(status)   - Returns true if child is stopped
// WSTOPSIG(status)     - Returns signal that stopped child

// COMMON QUESTIONS:
//
// Q: Why not just return the exit code directly?
// A: Because wait() needs to convey more information:
//    - Did the process exit normally or was it killed?
//    - If killed, which signal?
//    - Did it produce a core dump?
//    Packing this into one integer is efficient.
//
// Q: What if child exits with code 256?
// A: Exit codes are 8-bit (0-255 only).
//    If you return 256, it wraps to 0 (256 % 256 = 0).
//
// Q: What happens if child is killed by a signal?
// A: WIFEXITED() returns false, WIFSIGNALED() returns true,
//    and WTERMSIG() gives you the signal number.
//
// Q: Can I just divide childReturn by 256?
// A: That works for normal exits but fails for signal termination.
//    Always use the WEXITSTATUS() macro for portability.

// COMPARISON WITH CORRECT VERSION:
//
// This (pic3.c) - WRONG:
// printf("Here's the int returned by childReturn: %d", childReturn);
// Output when child returns 1: "...childReturn: 256"
//
// Next example (pid3.c or pid4.c) - CORRECT:
// printf("Here's the int returned by childReturn: %d", WEXITSTATUS(childReturn));
// Output when child returns 1: "...childReturn: 1"

// TRY IT:
// Compile: gcc pic3.c -o pic3
//
// Test 1 - Valid input:
// $ ./pic3
// Hey, give me a number, will ya?
// 42
// Here's the int returned by childReturn: 0
//
// Test 2 - Invalid input:
// $ ./pic3
// Hey, give me a number, will ya?
// hello
// Here's the int returned by childReturn: 256
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ BUG!
// Should be 1, not 256!
//
// Experiments:
// 1. Run with different inputs and observe childReturn values
// 2. Compare with pid3.c or pid4.c which use WEXITSTATUS()
// 3. Modify child to return different values (2, 5, 10)
//    and observe childReturn is always (return_value * 256)
