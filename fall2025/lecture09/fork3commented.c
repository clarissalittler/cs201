// PEDAGOGICAL PURPOSE:
// This program demonstrates EXPONENTIAL PROCESS CREATION through fork() in a loop.
// This is a CRITICAL example showing how processes multiply rapidly.
// Key learning objectives:
// 1. Understanding that EACH process created by fork() can also fork()
// 2. Process creation grows EXPONENTIALLY (2^n processes after n iterations)
// 3. Non-deterministic execution order with many concurrent processes
// 4. How getpid() identifies individual processes
// 5. Why fork bombs can crash systems (if done carelessly)
//
// WARNING: This creates 16 processes total - safe for demonstration
// NEVER increase the loop limit significantly without understanding the impact!
//
// KEY CONCEPTS:
// - Exponential growth: 1 -> 2 -> 4 -> 8 -> 16 processes
// - Process tree structure
// - Concurrent execution at scale
// - System resource management

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"     // Contains fork() and getpid()

int main(){
  // THE EXPONENTIAL FORK LOOP:
  // This innocent-looking loop creates MANY processes
  // Let's trace exactly what happens...
  //
  // ITERATION 0: 1 process calls fork() -> creates 2 processes total
  // ITERATION 1: 2 processes call fork() -> creates 4 processes total
  // ITERATION 2: 4 processes call fork() -> creates 8 processes total
  // ITERATION 3: 8 processes call fork() -> creates 16 processes total
  //
  // FORMULA: After n iterations, there are 2^n processes
  // After 4 iterations: 2^4 = 16 processes!

  for(int i=0; i< 4;i++){
    // CRITICAL POINT:
    // Every process (parent AND children) executes this loop!
    // Children created in iteration 0 will also execute iterations 1, 2, 3
    // Children created in iteration 1 will execute iterations 2, 3
    // And so on...

    fork();
    // After each fork():
    // - Original process continues (parent)
    // - New process starts from RIGHT HERE (child)
    // - Both continue through the loop
    // - Both will fork again in the next iteration!

    // THIS PRINTS FROM EVERY PROCESS:
    // After all iterations, this line executes 30 times total!
    // WHY 30? See detailed calculation below
    printf("I'm Mr. %d here at index %d\n",getpid(),i);
    //      Process ID: unique identifier for each process
    //      i: which iteration this print occurs in
  }

  return 0;
  // All 16 processes will eventually exit
}

// DETAILED EXECUTION TRACE:
//
// Let's call the original process P0
// Processes created in iteration i are labeled with iteration number
//
// ITERATION i=0:
// ============
// BEFORE fork(): 1 process exists (P0)
// P0 calls fork() -> creates P0-child-0
// AFTER fork(): 2 processes exist
//
// BOTH processes print:
//   P0:          "I'm Mr. 1000 here at index 0"  (example PID)
//   P0-child-0:  "I'm Mr. 1001 here at index 0"
//
// TOTAL PRINTS SO FAR: 2
// PROCESSES ALIVE: 2
//
// ITERATION i=1:
// ============
// BEFORE fork(): 2 processes exist (P0 and P0-child-0)
// BOTH processes call fork():
//   P0 creates P0-child-1
//   P0-child-0 creates P0-child-0-child-1
// AFTER fork(): 4 processes exist
//
// ALL 4 processes print:
//   P0:                    "I'm Mr. 1000 here at index 1"
//   P0-child-0:            "I'm Mr. 1001 here at index 1"
//   P0-child-1:            "I'm Mr. 1002 here at index 1"
//   P0-child-0-child-1:    "I'm Mr. 1003 here at index 1"
//
// PRINTS THIS ITERATION: 4
// TOTAL PRINTS SO FAR: 2 + 4 = 6
// PROCESSES ALIVE: 4
//
// ITERATION i=2:
// ============
// BEFORE fork(): 4 processes exist
// EACH of the 4 processes calls fork():
//   P0 creates P0-child-2
//   P0-child-0 creates P0-child-0-child-2
//   P0-child-1 creates P0-child-1-child-2
//   P0-child-0-child-1 creates P0-child-0-child-1-child-2
// AFTER fork(): 8 processes exist
//
// ALL 8 processes print at index 2
// PRINTS THIS ITERATION: 8
// TOTAL PRINTS SO FAR: 2 + 4 + 8 = 14
// PROCESSES ALIVE: 8
//
// ITERATION i=3:
// ============
// BEFORE fork(): 8 processes exist
// EACH of the 8 processes calls fork() -> 8 new children
// AFTER fork(): 16 processes exist
//
// ALL 16 processes print at index 3
// PRINTS THIS ITERATION: 16
// TOTAL PRINTS SO FAR: 2 + 4 + 8 + 16 = 30
// PROCESSES ALIVE: 16
//
// LOOP ENDS:
// =========
// All 16 processes exit from main()
//
// TOTAL OUTPUT LINES: 30
// FINAL PROCESS COUNT: 16

// MATHEMATICAL ANALYSIS:
//
// After iteration i, there are 2^(i+1) processes
// Each iteration i has 2^(i+1) print statements
//
// Total prints = 2^1 + 2^2 + 2^3 + 2^4
//              = 2 + 4 + 8 + 16
//              = 30
//
// General formula for n iterations:
// Total prints = 2^1 + 2^2 + ... + 2^n = 2^(n+1) - 2
// For n=4: 2^5 - 2 = 32 - 2 = 30 ✓
//
// Final process count = 2^n
// For n=4: 2^4 = 16 ✓

// PROCESS TREE VISUALIZATION:
//
// After all forks, the process tree looks like:
//
//                          P0 (original)
//                     /     |      \       \
//          (i=0)--> child0  |       |    (i=1)-->child1
//                 /    \    |       |           /     \
//    (i=1)--> child0  child0|       |  (i=2)->child1 child1
//            /  \    /   \  |       |        /   \   /    \
//           ... ... ... ... |       |       ... ... ... ...
//                           |       |
//                     (i=2)->child2 |
//                          /    \   |
//                         ...   ... |
//                                   |
//                            (i=3)->child3
//                                 /   \
//                                ... ...
//
// (Simplified - actual tree has 16 leaf nodes)

// NON-DETERMINISTIC ORDERING:
//
// The OUTPUT ORDER is completely unpredictable!
// Example possible output:
//
// I'm Mr. 1000 here at index 0
// I'm Mr. 1001 here at index 0
// I'm Mr. 1000 here at index 1
// I'm Mr. 1002 here at index 1
// I'm Mr. 1001 here at index 1
// I'm Mr. 1003 here at index 1
// ... (26 more lines in random order)
//
// WHY random? The OS scheduler decides which process runs when
// With 16 concurrent processes, there are MANY possible orderings!
// Number of possible orderings > 30! (factorial) - astronomical!

// WHY IS THIS DANGEROUS?
//
// Imagine changing the loop to i < 10:
// - After 10 iterations: 2^10 = 1,024 processes
// - Total prints: 2^11 - 2 = 2,046 prints
// - Still manageable, but getting expensive
//
// Now imagine i < 20:
// - After 20 iterations: 2^20 = 1,048,576 processes (1 million!)
// - Total prints: 2^21 - 2 = 2,097,150 prints
// - This is a FORK BOMB - can crash the system!
//
// FORK BOMB:
// A program that creates processes exponentially until system resources
// (memory, process table entries) are exhausted, causing system crash
//
// PROTECTION:
// Modern systems have limits (ulimit) to prevent fork bombs
// But still: BE CAREFUL with fork() in loops!

// EDUCATIONAL VALUE:
//
// This example teaches:
// 1. Exponential growth can happen quickly with seemingly simple code
// 2. Every forked process is a fully independent process that can fork
// 3. Process creation has real costs (memory, CPU, process table entries)
// 4. Concurrent processes execute in non-deterministic order
// 5. System resources are finite and must be managed carefully

// COMPARISON WITH fork2.c:
//
// fork2.c: Single fork() -> 2 processes total
// fork3.c: 4 forks in loop -> 16 processes total
//
// The difference is EXPONENTIAL growth vs LINEAR growth

// OBSERVING THE PROCESSES:
//
// While this program runs, you can observe it:
// 1. Run in background: ./fork3 &
// 2. Quickly run: ps aux | grep fork3
// 3. You'll see multiple fork3 processes (might be too fast to catch!)
//
// Or add sleep() to slow it down:
//   for(int i=0; i< 4; i++){
//     fork();
//     printf("I'm Mr. %d here at index %d\n",getpid(),i);
//     sleep(1);  // Wait 1 second
//   }
// Now you can see all 16 processes in another terminal!

// UNDERSTANDING getpid():
//
// getpid() returns the Process ID (PID) of the calling process
// Each process has a UNIQUE PID assigned by the OS
// PIDs are typically small integers (1000-32767 on many systems)
//
// In the output, you'll see 16 different PIDs
// Usually sequential or close together (child PIDs near parent PID)
//
// Example output snippet:
// I'm Mr. 5000 here at index 0  <- Original process
// I'm Mr. 5001 here at index 0  <- First child (PID = parent PID + 1)
// I'm Mr. 5000 here at index 1  <- Original continues
// I'm Mr. 5002 here at index 1  <- New child
// ...

// MEMORY IMPLICATIONS:
//
// Each fork() creates a COPY of the process memory
// Modern systems use COPY-ON-WRITE optimization:
// - Memory isn't actually copied until one process writes to it
// - Read-only pages (like code) are shared
// - But each process still needs its own page table, stack, etc.
//
// With 16 processes, if each needs 10MB:
// - Theoretical total: 160MB
// - Actual (with COW): Much less, maybe 20-30MB
// - Still significant!

// CONTROL FLOW CONFUSION:
//
// Students often think "the loop runs 4 times, so 4 processes"
// NO! The loop BODY runs many times, from many processes
//
// MENTAL MODEL:
// - Think of fork() as "clone yourself and both continue"
// - In a loop, each clone also continues the loop
// - Each clone can clone again!
// - Result: exponential explosion

// PRACTICAL USES:
//
// While this example is primarily educational, exponential forking
// has real uses:
// - Parallel tree processing (each node forks for children)
// - Distributed computation bootstrapping
// - Testing system limits
// - Demonstrating process concepts
//
// But usually, you'd use fork() more carefully, often with:
// - Conditional forking (not every process forks)
// - Depth limits (stop after certain generations)
// - wait() calls (parent waits for children to prevent zombies)

// TRY IT:
// Compile: gcc fork3.c -o fork3
// Run: ./fork3
// Run multiple times - notice the order changes!
//
// EXPERIMENTS:
// 1. Change loop limit to 3 - how many prints? (14)
// 2. Change loop limit to 5 - how many prints? (62) - careful!
// 3. Add sleep(1) - watch processes in another terminal
// 4. Print more info: printf("Parent of %d is %d at index %d\n", getpid(), getppid(), i);
// 5. Add a wait() call - see zombie discussion in advanced examples

// NEXT STEPS:
// - thread1.c: Introduction to threads (lighter weight than processes)
// - thread2.c: Why threads need synchronization (shared memory!)
