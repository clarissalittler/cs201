// PEDAGOGICAL PURPOSE:
// This program demonstrates how to distinguish between parent and child processes using fork()'s return value.
// Key learning objectives:
// 1. Understanding fork()'s return value: returns PID to parent, 0 to child
// 2. Using the return value to create different execution paths for parent vs child
// 3. The pid_t data type for storing process IDs
// 4. How parent and child can execute different code despite being clones
// 5. Basic process identification and control flow based on process role

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Contains the fork() system call
#include <sys/types.h>   // Contains pid_t type definition

/*
 * UNDERSTANDING fork()'s RETURN VALUE:
 *
 * fork() returns DIFFERENT values to parent and child:
 * - In the PARENT process: returns the child's PID (a positive number)
 * - In the CHILD process: returns 0
 * - On error: returns -1 (no child created)
 *
 * WHY DOES fork() RETURN DIFFERENT VALUES?
 * This is how we can write code that does different things in parent vs child.
 * It's the ONLY way to distinguish which process you're in after fork().
 *
 * TEACHING POINT: This is a clever design. Both processes execute the same
 * code, but by checking the return value, they can take different branches.
 */

int main(){

  /*
   * Store fork()'s return value in a variable
   *
   * TEACHING POINT: pid_t is a typedef for process ID (usually an int)
   *
   * After this line executes:
   * - In the parent: pid contains the child's process ID (positive number)
   * - In the child: pid contains 0
   *
   * WHY SAVE THE RETURN VALUE?
   * The parent needs the child's PID to:
   * 1. Wait for the child to finish (wait/waitpid)
   * 2. Send signals to the child
   * 3. Track which child is which if multiple children exist
   */
  pid_t pid = fork(); // the parent forks, gets the pid in this variable

  /*
   * Use the return value to distinguish parent from child
   *
   * TEACHING POINT: This if-else creates DIFFERENT execution paths
   *
   * The condition (pid != 0) means:
   * - TRUE in the parent (because pid = child's PID, which is non-zero)
   * - FALSE in the child (because pid = 0)
   */
  if(pid != 0){
    /*
     * PARENT PROCESS EXECUTES THIS BLOCK
     *
     * We know we're in the parent because pid is non-zero
     * (it contains the child's PID)
     */
    printf("I'm the parent!\n");
  }
  else{
    /*
     * CHILD PROCESS EXECUTES THIS BLOCK
     *
     * We know we're in the child because pid is 0
     *
     * TEACHING POINT: Even though the child is a copy of the parent,
     * the VALUE of the pid variable is different in child vs parent.
     * This is because fork() returned different values to each process.
     */
    printf("I'm the child!\n");
  }

  /*
   * This line executes in BOTH processes
   *
   * After the if-else, both processes continue executing the same code.
   * This message will print TWICE (once by parent, once by child).
   *
   * TEACHING POINT: Process control flow:
   * - Parent: executes "I'm the parent!" then this message
   * - Child: executes "I'm the child!" then this message
   */
  // this is the first line the child sees
  printf("This message should be printed by parent and child\n");

  return 0;
  // Both processes exit
  // Child becomes a zombie until parent reaps it (we'll fix this in fork3)
}

// EXECUTION TRACE:
//
// Time  Process    Variable    Action
// ----  -------    --------    ------
// T0    Parent     -           Calls fork()
// T1    Parent     pid=1234    fork() returns child's PID (e.g., 1234)
//       Child      pid=0       fork() returns 0
// T2    Parent     pid!=0      Takes if branch, prints "I'm the parent!"
//       Child      pid==0      Takes else branch, prints "I'm the child!"
// T3    Both       -           Both print "This message should be..."
// T4    Both       -           Both exit

// EXPECTED OUTPUT (order may vary):
// I'm the parent!
// This message should be printed by parent and child
// I'm the child!
// This message should be printed by parent and child
//
// OR:
// I'm the child!
// I'm the parent!
// This message should be printed by parent and child
// This message should be printed by parent and child
//
// NOTE: The exact order depends on OS scheduling

// TRY IT:
// Compile: gcc fork2.c -o fork2
// Run: ./fork2
// Run it multiple times - notice the order of output may change!
//
// NEXT STEPS:
// This example shows how to differentiate parent and child.
// But there's a problem: the child becomes a zombie after it exits.
// See fork3.c to learn how to properly clean up child processes with wait().
