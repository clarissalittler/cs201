// PEDAGOGICAL PURPOSE:
// This program demonstrates using WEXITSTATUS macro and making decisions based on exit codes.
// Key learning objectives:
// 1. Using the WEXITSTATUS() macro to extract exit codes properly
// 2. Making decisions in parent based on child's exit status
// 3. Understanding exit code conventions (0 = success, non-zero = failure)
// 4. Parent-child communication via exit codes
// 5. Proper abstraction using system-provided macros
// 6. Humorous error messages to make learning fun

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Contains fork(), getpid()
#include <sys/types.h>   // Contains pid_t type definition
#include <sys/wait.h>    // Contains wait() and WEXITSTATUS()

int main(){

  // CREATE CHILD PROCESS:
  pid_t pid = fork();
  // Parent: pid = child's PID
  // Child: pid = 0

  // VARIABLE TO STORE STATUS:
  int childReturn;
  // Will hold the packed status word from wait()

  // ERROR HANDLING:
  if(pid < 0){
    perror("Fork failed");
    return 1;
  }

  // CHILD PROCESS CODE:
  else if(pid ==0){
    // Child asks for input and validates it

    // PROMPT USER:
    printf("Hey, give me a number, will ya?\n");
    // Child is requesting an integer from the user

    // READ INTEGER:
    int num;
    int success = scanf("%d",&num);
    // scanf("%d", ...) returns:
    // - 1 if successfully read an integer
    // - 0 if input doesn't match format
    // - EOF if end of file or error

    // EXIT BASED ON SUCCESS:
    if(success){
      // Input was a valid integer
      return 0;
      // EXIT WITH SUCCESS STATUS
      // By convention, 0 means "everything went well"
      //
      // Note: We don't actually use the number
      // We only care if the user PROVIDED a valid number
    }
    else{
      // Input was NOT a valid integer
      return 1;
      // EXIT WITH FAILURE STATUS
      // By convention, non-zero means "something went wrong"
      // The specific value (1) can indicate the type of error
    }
  }

  // PARENT PROCESS CODE:
  else{
    // Parent waits for child and responds to its exit status

    // WAIT FOR CHILD:
    wait(&childReturn);
    // Blocks until child exits
    // Stores packed status word in childReturn

    // EXTRACT EXIT CODE USING WEXITSTATUS:
    // WEXITSTATUS(int) => (int >> 8) & 255
    if(WEXITSTATUS(childReturn) == 0){
      // Child exited with status 0 (success)
      //
      // WHY WEXITSTATUS?
      // - childReturn is a packed status word
      // - WEXITSTATUS extracts just the exit code
      // - Same as: (childReturn >> 8) & 255
      // - But more portable and readable
      //
      // WHAT THIS MEANS:
      // Child successfully read a valid integer
      // User cooperated with the child's request

      // everything was cool
      printf("Thanks for being nice to my child!\n");
      // Humorous acknowledgment that user provided valid input
      // Parent is "grateful" that child's task succeeded
    }
    else{
      // Child exited with non-zero status (failure)
      //
      // WHAT THIS MEANS:
      // Child failed to read a valid integer
      // User didn't provide what child asked for

      // DRAMATIC ERROR MESSAGE:
      printf("They've massacred my boy!\n");
      // Humorous reference to "The Godfather"
      // Treating invalid input as a "tragedy"
      // Makes the concept memorable and fun
      //
      // IN REALITY:
      // This just means the user typed something that wasn't a number
      // The "massacre" is just scanf() failing
    }

    return 0;
    // Parent exits successfully regardless of child's status
    // We reported the child's status but don't propagate the error
  }

  // UNREACHABLE CODE:
  return 0;
}

// EXECUTION TRACE (Valid input):
//
// Time    Process    Action                                  Output
// ----    -------    ------                                  ------
// T0      Parent     fork()
// T1      Child      Created, pid = 0
// T2      Parent     wait(&childReturn) - BLOCKS
// T3      Child      printf("Hey, give me...")               "Hey, give me a number..."
// T4      Child      scanf() waits for input
// [User types: 42 <Enter>]
// T5      Child      scanf() reads 42, success = 1
// T6      Child      success is true, return 0               Child exits with 0
// T7      Parent     wait() returns, childReturn = 0
// T8      Parent     WEXITSTATUS(0) = 0
// T9      Parent     0 == 0, true condition
// T10     Parent     printf("Thanks for...")                 "Thanks for being nice..."
// T11     Parent     return 0

// EXECUTION TRACE (Invalid input):
//
// Time    Process    Action                                  Output
// ----    -------    ------                                  ------
// T0-T4   [Same as above]
// [User types: hello <Enter>]
// T5      Child      scanf() fails, success = 0
// T6      Child      success is false, return 1              Child exits with 1
// T7      Parent     wait() returns, childReturn = 256
// T8      Parent     WEXITSTATUS(256) = (256 >> 8) & 255 = 1
// T9      Parent     1 == 0, false condition
// T10     Parent     printf("They've massacred...")          "They've massacred my boy!"
// T11     Parent     return 0

// EXIT CODE CONVENTIONS:
//
// STANDARD MEANINGS:
// 0     - Success, everything went as planned
// 1     - General failure
// 2     - Misuse of shell command
// 126   - Command can't execute
// 127   - Command not found
// 128+N - Fatal error signal N (e.g., 130 = killed by Ctrl+C)
//
// IN THIS PROGRAM:
// 0 - Child successfully read an integer
// 1 - Child failed to read an integer
//
// USING EXIT CODES:
// Exit codes allow child to communicate simple status to parent
// Parent can make decisions based on child's success/failure
// This is a fundamental form of inter-process communication

// WEXITSTATUS EXPLAINED:
//
// MACRO DEFINITION (typical):
// #define WEXITSTATUS(status) (((status) >> 8) & 0xFF)
//
// WHY USE THE MACRO?
// 1. Portable across different Unix systems
// 2. Self-documenting code (clearer intent)
// 3. Handles implementation differences
// 4. Less error-prone than manual bit manipulation
//
// COMPARISON:
// Manual:  if(((childReturn >> 8) & 255) == 0)
// Macro:   if(WEXITSTATUS(childReturn) == 0)
// The macro version is clearer and more maintainable

// RELATED MACROS:
//
// WIFEXITED(status)
// - Returns true if child exited normally (via return or exit())
// - Returns false if child was killed by signal
// - Use before WEXITSTATUS to ensure status is valid
//
// WIFSIGNALED(status)
// - Returns true if child was terminated by a signal
// - Example: killed by segmentation fault
//
// WTERMSIG(status)
// - Returns the signal number that terminated the child
// - Only valid if WIFSIGNALED returned true
//
// ROBUST EXIT CODE CHECKING:
// if (WIFEXITED(status)) {
//     int exit_code = WEXITSTATUS(status);
//     printf("Exited with code %d\n", exit_code);
// } else if (WIFSIGNALED(status)) {
//     int sig = WTERMSIG(status);
//     printf("Killed by signal %d\n", sig);
// }

// HUMOR IN ERROR MESSAGES:
//
// "Thanks for being nice to my child!"
// - Anthropomorphizes the processes
// - Parent is grateful when child succeeds
// - Makes dry systems programming more engaging
//
// "They've massacred my boy!"
// - Reference to classic movie line
// - Treats scanf() failure as dramatic tragedy
// - Memorable way to learn about exit codes
//
// PEDAGOGICAL VALUE:
// Humor helps students remember concepts
// The dramatic contrast highlights the two code paths
// Makes testing more fun (students try to "massacre" the child)

// COMMON QUESTIONS:
//
// Q: Why not just check if(childReturn == 0)?
// A: That only works for exit code 0.
//    For exit code 1, childReturn is 256, not 1.
//    WEXITSTATUS correctly extracts the actual exit code.
//
// Q: Could the child return different values for different errors?
// A: Yes! For example:
//    return 1 for invalid input
//    return 2 for EOF
//    Parent can check WEXITSTATUS(childReturn) and handle each case
//
// Q: What if child is killed by a signal?
// A: WIFEXITED(childReturn) would return false
//    WIFSIGNALED(childReturn) would return true
//    This program doesn't check for that case
//
// Q: Should I always use WEXITSTATUS?
// A: When you care about the exit code, yes!
//    If you only care that child finished, wait(NULL) is fine
//
// Q: What happens if I don't call wait()?
// A: Child becomes a zombie until parent exits
//    Good practice: always wait() for your children

// COMPARISON WITH pid3.c:
//
// pid3.c:
// - Manually extracts exit code: (childReturn >> 8) & 255
// - Educational: shows how WEXITSTATUS works
// - More verbose and error-prone
//
// pid4.c:
// - Uses WEXITSTATUS macro
// - Cleaner, more readable code
// - Industry-standard approach
// - Makes decisions based on exit code

// PARENT-CHILD COMMUNICATION:
//
// This demonstrates a simple communication pattern:
// 1. Child performs a task (reading input)
// 2. Child reports success/failure via exit code
// 3. Parent checks exit code
// 4. Parent takes action based on result
//
// MORE COMPLEX COMMUNICATION:
// Exit codes are limited (0-255)
// For richer communication, processes can use:
// - Pipes
// - Shared memory
// - Signals
// - Sockets
// - Files
// We'll learn these in later examples

// TRY IT:
// Compile: gcc pid4.c -o pid4
//
// Test 1 - Valid input (success):
// $ ./pid4
// Hey, give me a number, will ya?
// 42
// Thanks for being nice to my child!
//
// Test 2 - Invalid input (failure):
// $ ./pid4
// Hey, give me a number, will ya?
// hello
// They've massacred my boy!
//
// Test 3 - EOF (Ctrl+D):
// $ ./pid4
// Hey, give me a number, will ya?
// [Press Ctrl+D]
// They've massacred my boy!
//
// Experiments:
// 1. Try different inputs (numbers, words, special characters)
//
// 2. Modify child to return different codes:
//    if (!success && feof(stdin)) return 2;
//    else if (!success) return 1;
//    Then parent can distinguish EOF from invalid input
//
// 3. Add WIFEXITED check:
//    if (!WIFEXITED(childReturn)) {
//        printf("Child didn't exit normally!\n");
//    }
//
// 4. Print the actual exit code:
//    printf("Child exited with code: %d\n", WEXITSTATUS(childReturn));
//
// 5. Compare with pid3.c - both should behave the same
//    but pid4.c uses the cleaner macro approach
