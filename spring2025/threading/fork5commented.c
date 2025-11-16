// PEDAGOGICAL PURPOSE:
// This program demonstrates interactive parent-child communication using exit codes.
// Key learning objectives:
// 1. Child process can read input and return status based on that input
// 2. Understanding variable scope: 'returned' is uninitialized in child
// 3. Error handling with scanf() return value
// 4. Practical use of exit codes to communicate success/failure
// 5. The importance of checking function return values
// 6. A subtle bug: using uninitialized variables

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Contains fork()
#include <sys/types.h>   // Contains pid_t
#include <sys/wait.h>    // Contains wait() and WEXITSTATUS()

int main(){

  /*
   * Create child process
   */
  pid_t pid = fork();

  /*
   * Declare variable to hold wait() status
   *
   * TEACHING POINT: This variable is declared AFTER fork()
   * This means it exists in BOTH parent and child processes
   * But each process has its own separate copy!
   *
   * IMPORTANT BUG: This variable is UNINITIALIZED
   * - In parent: gets set by wait()
   * - In child: NEVER gets set (undefined behavior!)
   * We'll see how this causes a problem...
   */
  int returned;

  if(pid == 0){
    /*
     * CHILD PROCESS
     */

    // in the child process
    /*
     * Variables for reading user input
     *
     * blah: will hold the integer the user types
     * results: will hold scanf()'s return value
     */
    int blah;
    int results;

    /*
     * Prompt user for input
     *
     * TEACHING POINT: The child process can interact with the user!
     * Even though it's a separate process, it shares the same
     * stdin/stdout as the parent.
     */
    printf("Say somethin', will ya: ");

    /*
     * Read an integer from user
     *
     * TEACHING POINT: scanf() returns the number of items successfully read
     * - Returns 1 if one integer was successfully read
     * - Returns 0 if no integer could be read (invalid input)
     * - Returns EOF on end-of-file
     *
     * WHY CHECK THE RETURN VALUE?
     * If the user types "hello" instead of a number, scanf() fails.
     * We need to check this to handle errors properly.
     */
    results = scanf("%d",&blah);

    /*
     * Check if scanf() succeeded
     */
    if(results < 1){
      /*
       * scanf() failed (user didn't enter a valid integer)
       *
       * Return 1 to indicate error
       * This is a common convention: 0 = success, non-zero = error
       */
      return 1;
    }
    else {
      /*
       * scanf() succeeded (user entered a valid integer)
       *
       * Return 0 to indicate success
       *
       * TEACHING POINT: We read the integer into 'blah' but don't
       * use it! We're only checking whether the user typed a valid
       * number, not what that number is.
       */
      return 0;
    }
    // Child exits here with code 0 or 1
  }
  else {
    /*
     * PARENT PROCESS
     */

    // in the parent process
    /*
     * Wait for child to finish and get its exit status
     *
     * TEACHING POINT: The parent waits for the child to:
     * 1. Read user input
     * 2. Process it
     * 3. Return success (0) or error (1)
     * Then parent retrieves that status
     */
    wait(&returned);
  }

  /*
   * Print the exit status
   *
   * SUBTLE BUG ALERT!
   * This line executes in BOTH parent and child... but wait!
   * The child already returned in the if block above.
   * So this only executes in the PARENT.
   *
   * But there's still a problem with the original code structure:
   * If this line COULD execute in both, the child would use an
   * uninitialized 'returned' variable!
   *
   * TEACHING POINT: Variable initialization matters!
   * - Parent: 'returned' was set by wait(), prints correct value
   * - Child: (doesn't reach here, but if it did, 'returned' is undefined)
   */
  printf("This was returned: %d\n",WEXITSTATUS(returned) );

  return 0;
}

// EXECUTION TRACE:
//
// Time  Process    Variable        Action
// ----  -------    --------        ------
// T0    Parent     returned=?      Calls fork() (returned uninitialized)
// T1    Parent     pid=1234        Takes else branch
//       Child      returned=?      Takes if branch (returned uninitialized)
// T2    Parent     -               Calls wait(), BLOCKS
//       Child      -               Prints "Say somethin', will ya: "
// T3    Child      -               Waits for user input
// T4    Child      results=1       User types "42", scanf succeeds
// T5    Child      -               Returns 0 (success)
// T6    Parent     returned=0      wait() returns, status stored
// T7    Parent     -               Prints "This was returned: 0"
// T8    Parent     -               Exits

// EXPECTED OUTPUT (if user types a valid number):
// Say somethin', will ya: 42
// This was returned: 0

// EXPECTED OUTPUT (if user types invalid input):
// Say somethin', will ya: hello
// This was returned: 1

// TRY IT:
// Compile: gcc fork5.c -o fork5
// Run: ./fork5
//
// Test 1: Type a number (e.g., "42")
// Expected: "This was returned: 0"
//
// Test 2: Type text (e.g., "hello")
// Expected: "This was returned: 1"
//
// WHY DOES THIS WORK?
// The child process determines its exit code based on whether
// the user provided valid input. The parent retrieves this
// exit code and can act accordingly.
//
// REAL-WORLD APPLICATIONS:
// - Shell scripts check exit codes to know if commands succeeded
// - Build systems use exit codes to detect compilation failures
// - Automation tools use exit codes for error handling
//
// DISCUSSION QUESTION:
// The 'returned' variable is uninitialized at declaration.
// Why doesn't this cause problems in this program?
// Answer: Because the child returns before using it, and the
// parent sets it via wait() before using it. But this is fragile!
