// PEDAGOGICAL PURPOSE:
// This program demonstrates the fundamental behavior of fork() - process creation.
// Key learning objectives:
// 1. Understanding that fork() creates an exact copy (clone) of the calling process
// 2. Both parent and child processes continue execution AFTER the fork() call
// 3. Process execution is non-deterministic (order is unpredictable)
// 4. Why fork() causes output to appear twice
// 5. Introduction to the concept of concurrent execution

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"     // Contains fork() system call

int main(){

  // This print happens BEFORE the fork
  // Only ONE process exists at this point, so this prints ONCE
  printf("How's it going?\n");

  // FORK SYSTEM CALL:
  // fork() creates a NEW process by cloning the current process
  // After fork() returns, TWO processes exist:
  //   1. Parent process (original) - fork() returns child's PID
  //   2. Child process (new copy) - fork() returns 0
  //
  // IMPORTANT: Both processes continue execution from the NEXT line after fork()
  // They are identical copies (same code, same variables, same position)
  // But they are separate processes with different process IDs
  fork(); // create a new process that's a CLONE of the original

  // This print happens AFTER the fork
  // TWO processes now exist, so this line executes TWICE
  // Both parent and child execute this independently
  //
  // THE MYSTERY: Which prints first? Parent or child?
  // ANSWER: We don't know! The OS scheduler decides.
  // This demonstrates NON-DETERMINISTIC execution
  // Each time you run this program, the order might be different
  printf("Hi there\n");

  // EXPECTED OUTPUT (order may vary):
  // How's it going?
  // Hi there
  // Hi there
  //
  // The first "Hi there" is from one process (parent or child)
  // The second "Hi there" is from the other process (child or parent)

  // COMMENTED OUT CODE:
  // pid_t pid = fork();
  // This line is commented to keep the example simple
  // In the next example, we'll use the return value of fork()
  // to distinguish between parent and child processes

  return 0;
  // Both processes exit normally
  // The child process becomes a zombie until parent collects its status
  // (We'll learn about this in later examples)
}

// EXECUTION TRACE:
//
// Time  Process    Action
// ----  -------    ------
// T0    Parent     Starts, prints "How's it going?"
// T1    Parent     Calls fork()
// T2    Parent     fork() returns (child's PID)
//       Child      fork() returns (0)
// T3    ???        One of them prints "Hi there" (could be either)
// T4    ???        The other one prints "Hi there"
// T5    Both       Both exit

// TRY IT:
// Compile: gcc fork1.c -o fork1
// Run it multiple times: ./fork1
// Notice the output is always:
//   How's it going?
//   Hi there
//   Hi there
// But which process prints first may vary!
