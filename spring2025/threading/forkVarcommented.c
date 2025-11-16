// PEDAGOGICAL PURPOSE:
// This program demonstrates process memory independence - how parent and child have separate memory spaces.
// Key learning objectives:
// 1. Understanding that fork() creates SEPARATE memory spaces
// 2. Changes to variables in one process DON'T affect the other process
// 3. Process isolation - a fundamental operating system concept
// 4. Contrasting with threads (which SHARE memory - we'll see this later)
// 5. Why processes are good for isolation and safety
// 6. The copy-on-write optimization (mentioned conceptually)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Contains fork()
#include <sys/types.h>   // Contains pid_t

/*
 * PROCESS MEMORY INDEPENDENCE:
 *
 * When fork() creates a child process, it creates a COPY of the parent's
 * memory space. This means:
 * - Child gets copies of all variables
 * - Child and parent have SEPARATE memory
 * - Changing a variable in one process doesn't affect the other
 *
 * This is fundamentally different from threads, which SHARE memory!
 *
 * WHY SEPARATE MEMORY?
 * PROS:
 * - Isolation: Processes can't accidentally corrupt each other's data
 * - Security: One process can't read another's secrets
 * - Stability: If one process crashes, it doesn't affect others
 *
 * CONS:
 * - Memory overhead: Each process needs its own copy
 * - Communication is harder: Processes can't directly share data
 */

int main(){
  /*
   * Declare a variable BEFORE forking
   *
   * TEACHING POINT: This variable exists in the parent's memory.
   * When we fork(), the child gets a COPY of this variable with
   * the same initial value (100).
   *
   * After fork():
   * - Parent has 'thingy' at memory address 0xABCD (example)
   * - Child has 'thingy' at memory address 0xABCD (same virtual address!)
   * - But these addresses map to DIFFERENT physical memory
   *
   * COPY-ON-WRITE OPTIMIZATION:
   * Modern OSes don't actually copy memory immediately. They use
   * copy-on-write: memory is shared until one process modifies it,
   * then that page is copied. This saves memory when processes
   * don't modify shared data.
   */
  int thingy = 100;

  /*
   * Create child process
   * Both parent and child now have their own 'thingy' variable
   * Both start with value 100
   */
  pid_t pid = fork();

  if(pid != 0){
    /*
     * PARENT PROCESS
     *
     * Modify the parent's copy of 'thingy'
     */

    /*
     * Increment thingy twice
     *
     * TEACHING POINT: These modifications happen to the PARENT's
     * copy of thingy. The child's copy is unaffected!
     *
     * Parent's thingy: 100 -> 101 -> 102
     * Child's thingy: still 100
     */
    thingy++;   // thingy = 101
    thingy++;   // thingy = 102

    /*
     * Print parent's value
     *
     * Expected output: "Thingy: 102"
     */
    printf("Thingy: %d\n",thingy);
  }
  else{
    /*
     * CHILD PROCESS
     *
     * Modify the child's copy of 'thingy'
     */

    /*
     * Increment thingy once
     *
     * TEACHING POINT: This modification happens to the CHILD's
     * copy of thingy. The parent's copy is unaffected!
     *
     * Child's thingy: 100 -> 101
     * Parent's thingy: (will be 102 when parent modifies it)
     */
    thingy++;   // thingy = 101

    /*
     * Print child's value
     *
     * Expected output: "Thingy: 101"
     */
    printf("Thingy: %d\n",thingy);
  }

  /*
   * IMPORTANT OBSERVATION:
   * Both processes print different values for the "same" variable!
   * This proves that they have separate memory spaces.
   *
   * Parent prints: 102
   * Child prints: 101
   *
   * Even though they started with the same value (100), modifications
   * in one process don't affect the other.
   */

  return 0;
}

// EXECUTION TRACE:
//
// Time  Process    Memory State         Action
// ----  -------    ------------         ------
// T0    Parent     thingy=100           Initializes thingy
// T1    Parent     thingy=100           Calls fork()
// T2    Parent     thingy=100           pid != 0, takes if branch
//       Child      thingy=100           pid == 0, takes else branch
//                  (SEPARATE COPIES!)
// T3    Parent     thingy=101           Increments thingy
//       Child      thingy=101           Increments thingy
// T4    Parent     thingy=102           Increments thingy again
//       Child      thingy=101           (unchanged)
// T5    Parent     thingy=102           Prints "Thingy: 102"
//       Child      thingy=101           Prints "Thingy: 101"
// T6    Both       -                    Both exit

// EXPECTED OUTPUT (order may vary):
// Thingy: 102
// Thingy: 101
//
// OR:
// Thingy: 101
// Thingy: 102

// MEMORY LAYOUT (conceptual):
//
// BEFORE FORK:
// Parent Process Memory:
// [thingy=100] <- only one copy exists
//
// AFTER FORK:
// Parent Process Memory:          Child Process Memory:
// [thingy=100]                     [thingy=100] (SEPARATE COPY)
//
// AFTER MODIFICATIONS:
// Parent Process Memory:          Child Process Memory:
// [thingy=102]                     [thingy=101]
//
// Same variable name, DIFFERENT values in DIFFERENT memory locations!

// TRY IT:
// Compile: gcc forkVar.c -o forkVar
// Run: ./forkVar
// Observe that parent and child print different values
//
// EXPERIMENT:
// 1. Change the initial value of thingy
// 2. Add more increments in parent or child
// 3. Try with different data types (float, array, struct)
//
// CONTRAST WITH THREADS:
// In the next examples (thread*.c), we'll see threads, which SHARE
// memory. If this were a thread example:
// - Both threads would see the same 'thingy' variable
// - Modifications by one thread would affect the other
// - We'd need synchronization to prevent conflicts
//
// This fundamental difference makes processes safer but less
// convenient for sharing data than threads.
