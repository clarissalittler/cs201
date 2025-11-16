// PEDAGOGICAL PURPOSE:
// This program demonstrates how to retrieve and interpret a child process's exit status.
// Key learning objectives:
// 1. How to capture a child's exit status using wait()
// 2. Understanding the status value returned by wait()
// 3. Using WEXITSTATUS() macro to extract the actual exit code
// 4. Why the status is encoded (contains more than just the exit code)
// 5. Parent-child communication through exit codes
// 6. The bit manipulation behind WEXITSTATUS

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Contains fork()
#include <sys/types.h>   // Contains pid_t
#include <sys/wait.h>    // Contains wait() and WEXITSTATUS()

/*
 * UNDERSTANDING EXIT STATUS:
 *
 * When a process exits, it returns an integer status code:
 * - 0 typically means "success"
 * - Non-zero typically means "error" or some specific result
 *
 * The parent can retrieve this status to know:
 * - Did the child succeed or fail?
 * - What was the specific result?
 *
 * WHY IS THIS USEFUL?
 * - Error handling: know if child task succeeded
 * - Communication: child can pass a small integer back to parent
 * - Process management: take action based on child's result
 */

int main(){

  /*
   * Create the child process
   */
  pid_t pid = fork(); // the parent forks, gets the pid in this variable

  if(pid != 0){
    /*
     * PARENT PROCESS
     */

    /*
     * Declare a variable to store the exit status
     *
     * TEACHING POINT: This variable will receive the raw status
     * information from wait(). It's NOT just the exit code!
     *
     * The status includes:
     * - Exit code (returned by child's return statement)
     * - Signal information (if child was terminated by a signal)
     * - Other process termination metadata
     */
    int result;

    printf("I'm the parent!\n");

    /*
     * Wait for child and capture its status
     *
     * TEACHING POINT: We pass &result (address of result variable)
     * so wait() can store the status information there.
     *
     * After wait() returns:
     * - Child is no longer a zombie (cleaned up)
     * - result contains encoded status information
     */
    wait(&result);

    /*
     * Extract the actual exit code using WEXITSTATUS macro
     *
     * TEACHING POINT: The WEXITSTATUS macro extracts the exit code
     * from the encoded status value.
     *
     * The comment shows how it works:
     * wexitstatus(r) ==> (r >> 8) & 255
     *
     * WHY THE BIT MANIPULATION?
     * - The exit code is stored in bits 8-15 of the status
     * - Bits 0-7 store signal information
     * - >> 8 shifts right by 8 bits (moves exit code to lower 8 bits)
     * - & 255 (0xFF) masks to keep only the lower 8 bits
     *
     * RESULT: We extract just the 8-bit exit code (0-255)
     */
    // wexitstatus(r) ==> (r >> 8) & 255
    printf("My child returned: %d\n",WEXITSTATUS(result));
  }
  else{
    /*
     * CHILD PROCESS
     */
    printf("I'm the child!\n");

    /*
     * Child returns 2 as its exit status
     *
     * TEACHING POINT: This return value becomes the child's exit code.
     * The parent can retrieve this using wait() and WEXITSTATUS().
     *
     * WHY RETURN 2?
     * This is arbitrary - we're just demonstrating that the parent
     * can see whatever value the child returns.
     *
     * In real programs:
     * - 0 usually means success
     * - 1 usually means generic error
     * - Other values indicate specific error codes or results
     */
    return 2;
    // Child exits with code 2
  }

  /*
   * TEACHING POINT: Notice this return is ONLY executed by the parent!
   * The child already returned in the else block above.
   *
   * Parent returns 0 (success) to whoever called it (usually the shell)
   */
  return 0;
}

// EXECUTION TRACE:
//
// Time  Process    Variable       Action
// ----  -------    --------       ------
// T0    Parent     -              Calls fork()
// T1    Parent     pid=1234       Takes if branch
//       Child      pid=0          Takes else branch
// T2    Parent     -              Prints "I'm the parent!"
//       Child      -              Prints "I'm the child!"
// T3    Parent     result=?       Calls wait(&result), BLOCKS
//       Child      -              Returns 2, exits (becomes zombie)
// T4    Parent     result=512     wait() returns (512 = 2 << 8)
// T5    Parent     -              WEXITSTATUS(512) extracts 2
// T6    Parent     -              Prints "My child returned: 2"
// T7    Parent     -              Returns 0, exits

// WHY IS result = 512?
// The exit code 2 is stored in bits 8-15 of the status:
// 2 << 8 = 512 (in binary: 0000 0010 0000 0000)
// WEXITSTATUS extracts it: (512 >> 8) & 255 = 2

// EXPECTED OUTPUT:
// I'm the parent!
// I'm the child!
// My child returned: 2
//
// OR (order may vary):
// I'm the child!
// I'm the parent!
// My child returned: 2

// TRY IT:
// Compile: gcc fork4.c -o fork4
// Run: ./fork4
// Observe that the parent correctly retrieves the child's exit code (2)
//
// EXPERIMENT:
// 1. Change "return 2" to "return 42" in the child
// 2. Recompile and run - you should see "My child returned: 42"
// 3. Try returning 255 (max unsigned char)
// 4. Try returning 256 - what happens? (Hint: only 8 bits are stored!)
//
// NEXT STEPS:
// See fork5.c for a more interactive example where the child's
// return value depends on user input!
