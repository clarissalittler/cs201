// PEDAGOGICAL PURPOSE:
// This simple program demonstrates the getpid() system call and process identity.
// Key learning objectives:
// 1. Understanding that every process has a unique Process ID (PID)
// 2. Using getpid() to query your own process ID
// 3. Understanding that PID remains constant throughout process lifetime
// 4. Preparing for signal demonstrations (you need to know PID to send signals)
// 5. Observing that a process can query its own identity

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"     // Contains getpid() and sleep()

int main(){

  // GET PROCESS ID:
  // getpid() is a system call that returns the calling process's PID
  //
  // WHY STORE IT?
  // - The PID never changes during process execution
  // - Storing it once is more efficient than calling getpid() repeatedly
  // - Makes the printf() cleaner (no nested function call)
  //
  // pid_t TYPE:
  // - Special type for process IDs (defined in sys/types.h)
  // - Usually implemented as an integer
  // - Using pid_t makes code portable across systems
  pid_t myPID = getpid(); // gets your own process id

  // INFINITE LOOP:
  // This process will run forever (until killed)
  while(1){
    // PRINT PROCESS IDENTITY:
    // Shows this process's PID repeatedly
    // This is useful when you want to send signals to this process
    // You can note the PID and use it with the kill command
    printf("I'm mr. %d\n",myPID);

    // SLEEP FOR 1 SECOND:
    // sleep() suspends execution for specified seconds
    // Without this, the loop would print millions of times per second
    // The sleep makes the output readable and reduces CPU usage
    sleep(1);
  }

  // UNREACHABLE CODE:
  // We never exit the infinite loop, so return is never reached
  // To stop this program, you must:
  // - Press Ctrl+C (sends SIGINT)
  // - Use kill command from another terminal
  // - Close the terminal window
  return 0;
}

// EXECUTION TRACE:
//
// Time    Action                          Output
// ----    ------                          ------
// T0      main() starts
// T1      getpid() called, returns 12345
// T2      myPID = 12345
// T3      Enter while loop
// T4      printf()                        "I'm mr. 12345"
// T5      sleep(1) - process suspends
// T6      Wake up after 1 second
// T7      Loop back to printf()
// T8      printf()                        "I'm mr. 12345"
// T9      sleep(1) - process suspends
// ...     Pattern repeats forever         "I'm mr. 12345" every second

// CONCEPTUAL EXPLANATION:
//
// WHAT IS A PROCESS ID (PID)?
// - A unique integer identifier for each running process
// - Assigned by the operating system when process is created
// - Guaranteed to be unique among currently running processes
// - PIDs can be reused after a process terminates
// - PID 1 is special - it's the init process (first process at boot)
//
// WHY DO WE NEED PIDs?
// PIDs serve several purposes:
// 1. Uniquely identify processes for management
// 2. Target for signals (kill command needs a PID)
// 3. Parent-child relationships (parent knows child's PID)
// 4. Resource accounting and monitoring
// 5. Security and permissions (who owns this process?)
//
// TYPICAL PID RANGE:
// - PIDs are positive integers
// - Usually start from 1 (init) and count up
// - Maximum PID varies by system (often 32768 or higher)
// - When max is reached, PIDs wrap around (skipping currently used ones)
//
// SYSTEM CALLS FOR PROCESS IDENTITY:
// - getpid()  - Get my own PID
// - getppid() - Get my parent's PID
// - getuid()  - Get my user ID
// - getgid()  - Get my group ID
//
// USING THIS WITH SIGNALS:
// This program is designed to be a target for signal practice:
// 1. Run this program: ./pidLoop
// 2. Note the PID it prints (e.g., 12345)
// 3. From another terminal, send signals:
//    kill -SIGUSR1 12345
//    kill -SIGUSR2 12345
//    kill -SIGTERM 12345
// 4. Watch how the process responds (or doesn't)
//
// By default, SIGUSR1 and SIGUSR2 terminate the process
// Later programs will install handlers to respond differently

// PARENT VS CHILD PID:
// When a process forks:
// - Parent gets child's PID from fork() return value
// - Child can find parent's PID using getppid()
// - Child gets its own PID using getpid()
//
// Example:
//   pid_t pid = fork();
//   if(pid == 0){
//     // Child process
//     printf("I am %d, my parent is %d\n", getpid(), getppid());
//   } else {
//     // Parent process
//     printf("I am %d, my child is %d\n", getpid(), pid);
//   }

// COMPARISON WITH OTHER IDENTIFIERS:
//
// THREAD ID (TID):
// - Each thread within a process has a TID
// - For single-threaded programs, TID == PID
// - Get with gettid() on Linux
//
// PROCESS GROUP ID (PGID):
// - Group of related processes (e.g., pipeline)
// - Used for job control (Ctrl+C kills whole group)
// - Get with getpgid()
//
// SESSION ID (SID):
// - Collection of process groups
// - Usually one per terminal or login session
// - Get with getsid()

// WHY THIS PROGRAM EXISTS:
// This is a simple "target" program for signal experiments
// It's easier to send signals to a long-running process
//
// Typical usage in lecture:
// Terminal 1: ./pidLoop
//             I'm mr. 12345
//             I'm mr. 12345
//             ...
//
// Terminal 2: kill -SIGUSR1 12345
//             [Process terminates]
//
// Terminal 1: [process exits]
//
// This demonstrates that signals can terminate processes

// COMMON MISTAKES:
//
// MISTAKE 1: Calling getpid() in the loop
// while(1){
//   printf("I'm mr. %d\n", getpid());  // Wasteful!
// }
// This works, but calls getpid() unnecessarily every iteration
// The PID never changes, so call it once and store it
//
// MISTAKE 2: Wrong format specifier
// printf("I'm mr. %s\n", myPID);  // WRONG! PID is integer, not string
// Use %d for pid_t (or %ld on some systems for safety)
//
// MISTAKE 3: Forgetting sleep()
// while(1){
//   printf("I'm mr. %d\n", myPID);
//   // No sleep - will print millions of times per second!
// }
// This will flood your terminal and consume 100% CPU

// TRY IT:
// Compile: gcc pidLoop.c -o pidLoop
// Run: ./pidLoop
//
// You'll see output like:
//   I'm mr. 12345
//   I'm mr. 12345
//   I'm mr. 12345
//   ...
//
// Try these experiments:
// 1. Note the PID number
// 2. Open another terminal
// 3. Run: ps aux | grep pidLoop
//    You'll see the same PID
// 4. Run: kill <PID>
//    The program terminates
// 5. Run ./pidLoop again
//    Notice the PID is different (new process, new PID)
