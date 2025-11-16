// PEDAGOGICAL PURPOSE:
// This program demonstrates the fundamental behavior of fork() - the most basic process creation.
// Key learning objectives:
// 1. Understanding that fork() creates an exact copy (clone) of the calling process
// 2. Both parent and child processes continue execution AFTER the fork() call
// 3. Without using fork()'s return value, both processes execute identically
// 4. Process execution is non-deterministic (order is unpredictable)
// 5. Why code after fork() executes twice
// 6. Introduction to the concept of concurrent execution

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Contains the fork() system call
#include <sys/types.h>   // Contains type definitions like pid_t

/*
 * WHAT IS fork()?
 * fork() is a system call that creates a new process by duplicating the calling process.
 * The new process is called the CHILD, the calling process is the PARENT.
 *
 * After fork() returns:
 * - TWO processes exist (parent and child)
 * - Both are IDENTICAL copies (same code, same variables, same execution position)
 * - Both continue execution from the line AFTER fork()
 * - They are separate processes with different process IDs (PIDs)
 */

int main(){

  /*
   * THE SIMPLEST FORK EXAMPLE
   * This demonstrates fork() in its most basic form.
   *
   * TEACHING POINT: We're not capturing fork()'s return value here.
   * This means we have no way to distinguish between parent and child.
   * Both processes will execute the exact same code.
   *
   * In fork1, we call fork() which creates a child process.
   * The child process is an exact copy of the parent process, including
   * memory and execution position.
   *
   * The key teaching point here is that after fork():
   * - We now have TWO processes running
   * - Both will continue from the line after fork()
   * - Both processes will execute the same code
   * - No differentiation between parent and child is made
   */

  // BEFORE FORK:
  // Only ONE process running here
  // Everything before this line executes exactly once

  fork();  // Creates a new process (child) that's a clone of the parent

  // AFTER FORK:
  // Now there are TWO processes running!
  // Both parent and child continue from here

  /*
   * This line will be executed by BOTH the parent and child processes.
   * When you run this program, you'll see this message printed TWICE.
   *
   * IMPORTANT: The order in which parent and child print is UNPREDICTABLE.
   * The operating system's scheduler decides which process runs first.
   * This demonstrates NON-DETERMINISTIC concurrent execution.
   *
   * WHY IS THIS USEFUL?
   * This demonstration shows that forking creates a complete duplicate
   * of the running process. Both continue execution independently
   * from the same point. This is the foundation for:
   * - Creating separate processes for different tasks
   * - Building concurrent applications
   * - Implementing process-based parallelism
   */
  printf("This message should be printed by parent and child\n");

  // Both processes exit normally
  // The child becomes a zombie until parent collects its status
  // (We don't handle that in this simple example)

  return 0;
}

// EXECUTION TRACE:
//
// Time  Process    What's Happening
// ----  -------    ----------------
// T0    Parent     Starts executing main()
// T1    Parent     Calls fork()
// T2    Parent     fork() returns (child's PID to parent)
//       Child      fork() returns (0 to child) - child begins executing
// T3    ???        One process prints message (could be parent OR child)
// T4    ???        Other process prints message (could be child OR parent)
// T5    Both       Both processes exit

// EXPECTED OUTPUT:
// This message should be printed by parent and child
// This message should be printed by parent and child
//
// Notice: The message appears TWICE because TWO processes execute the printf

// TRY IT:
// Compile: gcc fork1.c -o fork1
// Run: ./fork1
// Run it multiple times and observe that you always see the message twice
//
// WHAT'S MISSING?
// This example doesn't distinguish between parent and child.
// See fork2.c for how to use fork()'s return value to differentiate them.
