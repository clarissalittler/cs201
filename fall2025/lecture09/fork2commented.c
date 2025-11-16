// PEDAGOGICAL PURPOSE:
// This program demonstrates how to distinguish between parent and child processes
// after a fork(), and shows that forked processes have SEPARATE memory spaces.
// Key learning objectives:
// 1. Using fork()'s return value to differentiate parent from child
// 2. Understanding that fork() creates SEPARATE memory spaces (copy-on-write)
// 3. Each process has its own copy of variables after fork()
// 4. Process IDs (PIDs) identify different processes
// 5. Parent and child can execute different code paths
//
// KEY CONCEPTS:
// - fork() returns DIFFERENT values to parent and child
// - Memory is COPIED (each process gets its own num1)
// - Changes to variables in one process DON'T affect the other
// - This is the foundation for process-based concurrency

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"     // Contains fork() and getpid() system calls

int main(){

  // BEFORE FORK:
  // Only ONE process exists at this point
  // There is only ONE num1 variable in memory
  int num1 = 10; // here there's only one num1

  // FORK SYSTEM CALL:
  // This is where the magic happens!
  // fork() creates a NEW process that's a CLONE of the original
  //
  // CRITICAL: fork() returns TWICE (once in each process)
  // - In the PARENT process: fork() returns the child's PID (a positive number)
  // - In the CHILD process:  fork() returns 0
  //
  // WHY different return values?
  // This is HOW we distinguish between parent and child!
  // Without different returns, both would execute the same code
  pid_t pid = fork(); // create a new process that's a CLONE of the original

  // WHAT GETS CLONED?
  // fork() clones *EVERYTHING* about the process:
  // - Memory space (all variables, including num1)
  // - Stack (local variables, function call history)
  // - Heap (dynamically allocated memory)
  // - Register state (CPU registers)
  // - Program counter (position in code)
  // - File descriptors (open files)
  // - Everything that makes a process a process!
  //
  // BUT: They are SEPARATE processes with SEPARATE memory
  // Changes in one process do NOT affect the other

  // AFTER FORK:
  // Now TWO processes exist, each with its own copy of EVERYTHING
  // Both processes continue from HERE (right after the fork)
  //
  // MEMORY SEPARATION:
  // - Parent has its own num1 (still = 10)
  // - Child has its own num1 (also = 10, but in DIFFERENT memory)
  // - Parent has pid = child's PID (e.g., 12345)
  // - Child has pid = 0

  // DISTINGUISHING PARENT FROM CHILD:
  // We use the return value stored in 'pid'
  // If pid != 0, we're in the parent (pid contains child's PID)
  // If pid == 0, we're in the child
  if(pid != 0){
    // PARENT PROCESS CODE:
    // This code ONLY runs in the parent process
    // because only the parent has pid != 0

    num1++;  // Increment parent's copy of num1
             // Parent's num1: 10 -> 11
             // Child's num1: still 10 (UNAFFECTED!)
             // WHY? Separate memory spaces!

    printf("I'm the parent. Here's my number  %d\n",num1);
    // Output: "I'm the parent. Here's my number  11"

    // WHY two spaces after "number"? Typo in original code!
    // Notice: We could access the child's PID using 'pid' variable
    // (though this code doesn't print it)
  }
  else{
    // CHILD PROCESS CODE:
    // This code ONLY runs in the child process
    // because only the child has pid == 0

    num1--;  // Decrement child's copy of num1
             // Child's num1: 10 -> 9
             // Parent's num1: still 11 (UNAFFECTED!)
             // WHY? Separate memory spaces!

    printf("I'm the child. Here's my number %d\n",num1);
    // Output: "I'm the child. Here's my number 9"

    // Note: The child cannot easily access parent's PID from here
    // (would need getppid() system call)
  }

  // BOTH PROCESSES EXECUTE THIS:
  // After the if-else, both processes continue here
  // This line executes TWICE (once in parent, once in child)
  printf("And we both say hi\n");
  // This will print twice (order is non-deterministic!)

  // PROCESS-BASED CONCURRENCY:
  // fork() is the earlier (traditional) concurrency primitive
  // Before threads existed, fork() was THE way to do concurrent programming
  // WHY use fork() instead of threads?
  // - Complete isolation (no shared memory issues)
  // - More robust (one process crash doesn't kill the other)
  // - Security (processes can have different permissions)
  // BUT: More expensive (copying memory, context switches)

  return 0;
  // Both processes exit here
  // The child becomes a zombie until the parent waits for it
  // (We don't call wait() here, so child becomes a zombie temporarily)
}

// EXECUTION TRACE:
//
// Time  Process    pid variable    num1    Action
// ----  -------    ------------    ----    ------
// T0    Parent          -          10      Starts, declares num1=10
// T1    Parent          -          10      Calls fork()
// T2    Parent      (child PID)    10      fork() returns child's PID
//       Child           0          10      fork() returns 0 (child gets copy of num1=10)
// T3    Parent      (child PID)    11      Increments num1, prints "I'm the parent...11"
//       Child           0           9      Decrements num1, prints "I'm the child...9"
// T4    ???             -           -      One prints "And we both say hi"
// T5    ???             -           -      Other prints "And we both say hi"
// T6    Both exit

// EXPECTED OUTPUT (order may vary):
// I'm the parent. Here's my number  11
// I'm the child. Here's my number 9
// And we both say hi
// And we both say hi
//
// OR (if child runs first):
// I'm the child. Here's my number 9
// I'm the parent. Here's my number  11
// And we both say hi
// And we both say hi
//
// The order is NON-DETERMINISTIC!
// The OS scheduler decides which process runs when

// MEMORY DIAGRAM AFTER FORK:
//
// PARENT PROCESS MEMORY:              CHILD PROCESS MEMORY:
// +------------------+                +------------------+
// | Code Segment     |                | Code Segment     |  (same code)
// | (same program)   |                | (same program)   |
// +------------------+                +------------------+
// | Data Segment     |                | Data Segment     |
// | pid = 12346      |                | pid = 0          |  (DIFFERENT!)
// | num1 = 11        |                | num1 = 9         |  (DIFFERENT!)
// +------------------+                +------------------+
// | Stack            |                | Stack            |
// | (separate)       |                | (separate)       |
// +------------------+                +------------------+
// | Heap             |                | Heap             |
// | (separate)       |                | (separate)       |
// +------------------+                +------------------+
//
// CRITICAL INSIGHT: These are DIFFERENT memory spaces!
// They cannot directly affect each other's variables
// (unlike threads, which share memory)

// WHY IS THIS IMPORTANT?
//
// 1. ISOLATION: Bugs in child can't corrupt parent's memory
//    Example: Child crashes -> parent keeps running
//
// 2. SECURITY: Child can drop privileges, run untrusted code
//    Example: Web server forks to handle requests safely
//
// 3. SIMPLICITY: No race conditions on variables (unlike threads)
//    Example: No need for locks/mutexes on num1
//
// 4. HISTORICAL: Unix was built on this model
//    Every program starts with fork() from shell

// COMPARISON WITH THREADS (preview):
//
// FORK (processes):
// - Separate memory spaces
// - More expensive (copy memory)
// - Complete isolation
// - No shared state (good and bad)
//
// THREADS (coming in thread1.c, thread2.c):
// - Shared memory space
// - Cheaper (no memory copy)
// - Shared state
// - Need synchronization (locks)

// COMMON MISTAKES:
//
// 1. Forgetting that BOTH processes continue after fork():
//    fork();
//    printf("Hi\n");  // This prints TWICE!
//
// 2. Thinking variables are shared:
//    fork();
//    num++;  // Each process modifies its OWN copy
//
// 3. Not checking fork() return value:
//    fork();
//    // How do we know if we're parent or child?
//
// 4. Expecting deterministic order:
//    // Which process prints first? UNKNOWN!

// TRY IT:
// Compile: gcc fork2.c -o fork2
// Run: ./fork2
// Run it multiple times - notice the order might change!
//
// EXPERIMENTS:
// 1. Add more variables - see that they're all copied
// 2. Print the actual PID values: printf("My PID: %d\n", getpid());
// 3. Add a sleep() in one process - see how it affects ordering
// 4. Don't use the if-else - see what happens!

// NEXT STEPS:
// - fork3.c: What happens when we fork() in a loop?
// - thread1.c: Introduction to threads (shared memory)
// - thread2.c: Why shared memory needs synchronization
