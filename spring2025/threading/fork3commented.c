// PEDAGOGICAL PURPOSE:
// This program demonstrates proper cleanup of child processes using wait().
// Key learning objectives:
// 1. Understanding the zombie process problem
// 2. How wait() prevents zombie processes
// 3. Why parents should wait for their children
// 4. Process synchronization - parent blocks until child finishes
// 5. The "anti-zombie ward" pattern
// 6. Ensuring deterministic output order through synchronization

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Contains fork()
#include <sys/types.h>   // Contains pid_t
#include <sys/wait.h>    // Contains wait() - NEW!

/*
 * THE ZOMBIE PROCESS PROBLEM:
 *
 * When a process exits, it doesn't disappear immediately.
 * It becomes a ZOMBIE - a "dead" process that still has an entry
 * in the process table.
 *
 * WHY DO ZOMBIES EXIST?
 * The OS keeps the zombie around so the parent can retrieve the
 * child's exit status. Once the parent reads this status (via wait()),
 * the zombie can be fully removed.
 *
 * WHAT HAPPENS IF WE DON'T WAIT?
 * - Zombie processes accumulate in the process table
 * - They consume system resources (memory for process table entries)
 * - With enough zombies, you can't create new processes!
 *
 * SOLUTION: Use wait() to collect child exit status and clean up zombies.
 */

int main(){

  /*
   * Create a child process
   * - Parent gets child's PID
   * - Child gets 0
   */
  pid_t pid = fork(); // the parent forks, gets the pid in this variable

  if(pid != 0){
    /*
     * PARENT PROCESS
     *
     * The parent calls wait() BEFORE doing anything else.
     * This is the "anti-zombie ward" - it prevents zombie processes.
     */

    // anti-zombie ward
    /*
     * wait() does THREE important things:
     *
     * 1. BLOCKS the parent until the child terminates
     *    - Parent stops executing and waits
     *    - When child exits, parent resumes
     *
     * 2. COLLECTS the child's exit status
     *    - We pass NULL because we don't care about the status here
     *    - See fork4.c for how to retrieve and use the exit status
     *
     * 3. CLEANS UP the zombie
     *    - Removes child's entry from process table
     *    - Frees associated system resources
     *
     * TEACHING POINT: wait(NULL) means "wait for any child to exit,
     * but I don't care about the exit status"
     *
     * WHY WAIT FIRST?
     * By waiting before printing, we ensure deterministic order:
     * child always prints first, then parent prints.
     */
    wait(NULL);

    /*
     * This prints AFTER the child has finished
     *
     * Because we called wait() first, we KNOW the child has already
     * printed its message by the time we get here.
     *
     * RESULT: Output order is now predictable!
     */
    printf("I'm the parent!\n");
  }
  else{
    /*
     * CHILD PROCESS
     *
     * The child executes independently and prints its message.
     * When it reaches the end of main and returns, it will:
     * 1. Become a zombie
     * 2. Signal the parent that it's done
     * 3. The parent's wait() will collect its status
     * 4. The zombie will be cleaned up
     */
    printf("I'm the child!\n");
  }

  /*
   * Both processes execute this line
   *
   * TEACHING POINT: Notice the output order is now deterministic:
   * 1. Child prints "I'm the child!"
   * 2. Child exits (becomes zombie)
   * 3. Parent's wait() returns
   * 4. Parent prints "I'm the parent!"
   * 5. Parent prints this message
   * 6. Child prints this message... WAIT, NO!
   *
   * Actually, the child already exited! So only the parent prints this.
   * No wait... the child hasn't hit return yet, so it DOES execute this line!
   */
  // this is the first line the child sees
  printf("This message should be printed by parent and child\n");

  return 0;
  // Child: exits here, becomes zombie, then immediately cleaned up by parent's wait()
  // Parent: exits here, program terminates
}

// EXECUTION TRACE:
//
// Time  Process    Action
// ----  -------    ------
// T0    Parent     Calls fork()
// T1    Parent     pid != 0, enters if block
//       Child      pid == 0, enters else block
// T2    Parent     Calls wait(), BLOCKS (stops executing)
//       Child      Prints "I'm the child!"
// T3    Child      Prints "This message should be..."
// T4    Child      Returns from main, exits (becomes zombie)
// T5    Parent     wait() returns (child zombie cleaned up)
// T6    Parent     Prints "I'm the parent!"
// T7    Parent     Prints "This message should be..."
// T8    Parent     Exits

// EXPECTED OUTPUT (always in this order):
// I'm the child!
// This message should be printed by parent and child
// I'm the parent!
// This message should be printed by parent and child

// TRY IT:
// Compile: gcc fork3.c -o fork3
// Run: ./fork3
// Run it multiple times - notice the output order is ALWAYS the same!
// This is different from fork2.c where the order was unpredictable.
//
// WHY IS ORDER PREDICTABLE NOW?
// Because wait() synchronizes the processes:
// - Child runs to completion
// - Parent waits for child
// - Parent continues only after child is done
//
// NEXT STEPS:
// We've learned to prevent zombies with wait(NULL).
// But what if we want to retrieve the child's exit status?
// See fork4.c to learn how to capture and use the exit status!
