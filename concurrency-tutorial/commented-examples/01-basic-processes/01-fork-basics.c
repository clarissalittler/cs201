/**
 * ============================================================================
 * Process Creation Basics - Understanding fork()
 * ============================================================================
 * 
 * This example demonstrates the most basic usage of the fork() system call.
 * The fork() function creates a new process by duplicating the calling process.
 * The new process is called the child, while the original is the parent.
 * 
 * Key points about fork():
 * - After fork(), both processes continue execution from the same point
 * - The processes run independently from each other
 * - The child is an exact duplicate of the parent (with a few exceptions)
 * - fork() returns different values to parent and child processes
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  /* For fork() and process-related functions */
#include <sys/types.h>  /* For pid_t type */

int main() {
    /* 
     * The process ID (PID) is a unique identifier for a process.
     * We'll use this to store what fork() returns.
     */
    pid_t pid;
    
    printf("Before fork: I am process %d\n", getpid());
    
    /* 
     * Call fork() to create a new process.
     * 
     * IMPORTANT: After this line, we have TWO processes running the same code!
     * Both processes continue from the next line, but with different return values.
     */
    pid = fork();
    
    /* 
     * Check the return value to see if we're in the parent or child process:
     * - If fork() returns -1: Error occurred, no child was created
     * - If fork() returns 0: We are in the child process
     * - If fork() returns a positive number: We are in the parent process,
     *   and the value is the child's PID
     */
    if (pid < 0) {
        /* Fork failed - this is rare but should be handled */
        perror("Fork failed");
        return EXIT_FAILURE;
    } 
    else if (pid == 0) {
        /* 
         * We're in the child process 
         * getpid() returns the current process ID
         * getppid() returns the parent process ID
         */
        printf("CHILD: I am the child process (PID: %d)\n", getpid());
        printf("CHILD: My parent is (PPID: %d)\n", getppid());
    } 
    else {
        /* 
         * We're in the parent process
         * pid holds the child's PID
         */
        printf("PARENT: I am the parent process (PID: %d)\n", getpid());
        printf("PARENT: My child is (PID: %d)\n", pid);
    }
    
    /* 
     * Both parent and child will execute this code.
     * This demonstrates that both processes continue from the fork point.
     */
    printf("Process %d is exiting.\n", getpid());
    
    return EXIT_SUCCESS;
}

/**
 * EXPLANATION:
 *
 * The key concept to understand is that fork() creates a completely new process
 * that is initially identical to the parent process. However, after fork(),
 * they are separate processes with their own memory, file descriptors, etc.
 *
 * The most common usage pattern is:
 * 1. Call fork() to create a child process
 * 2. Use the return value to determine if we're in the parent or child
 * 3. Each process performs its specific tasks
 *
 * Common gotchas:
 * - Variables set before the fork() are copied to both processes
 * - Changing a variable in one process does NOT affect the other process
 * - Both processes compete for CPU time; there's no guarantee about which runs first
 * - If the parent exits before the child, the child becomes an "orphan" and is
 *   adopted by the init process (PID 1)
 * - If the child exits but the parent doesn't call wait(), the child becomes a
 *   "zombie" process
 */

/**
 * ============================================================================
 * EXPECTED OUTPUT AND TESTING
 * ============================================================================
 *
 * TO COMPILE AND RUN:
 * gcc 01-fork-basics.c -o 01-fork-basics
 * ./01-fork-basics
 *
 * EXPECTED OUTPUT (PIDs will vary):
 * Before fork: I am process 12345
 * CHILD: I am the child process (PID: 12346)
 * CHILD: My parent is (PPID: 12345)
 * PARENT: I am the parent process (PID: 12345)
 * PARENT: My child is (PID: 12346)
 * Process 12346 is exiting.
 * Process 12345 is exiting.
 *
 * NOTE: The order of CHILD and PARENT messages may vary!
 * Due to process scheduling, you might see:
 *
 * POSSIBLE VARIATION 1 (child first):
 * Before fork: I am process 12345
 * CHILD: I am the child process (PID: 12346)
 * CHILD: My parent is (PPID: 12345)
 * Process 12346 is exiting.
 * PARENT: I am the parent process (PID: 12345)
 * PARENT: My child is (PID: 12346)
 * Process 12345 is exiting.
 *
 * POSSIBLE VARIATION 2 (parent first):
 * Before fork: I am process 12345
 * PARENT: I am the parent process (PID: 12345)
 * PARENT: My child is (PID: 12346)
 * CHILD: I am the child process (PID: 12346)
 * CHILD: My parent is (PPID: 12345)
 * Process 12345 is exiting.
 * Process 12346 is exiting.
 *
 * KEY OBSERVATIONS:
 * 1. "Before fork" message appears ONCE (only original process)
 * 2. CHILD and PARENT messages appear ONCE each (two processes)
 * 3. "is exiting" appears TWICE (both processes exit)
 * 4. Child PID = Parent PID + 1 (usually, but not guaranteed)
 * 5. Child's PPID = Parent's PID (always)
 *
 * VERIFICATION:
 * Run the program multiple times:
 * $ ./01-fork-basics
 * $ ./01-fork-basics
 * $ ./01-fork-basics
 *
 * Notice:
 * - PIDs change each run
 * - Message order may vary
 * - But structure is always consistent
 *
 * TESTING WITH ps:
 * In one terminal, modify the program to sleep:
 * (Add: sleep(10); after fork())
 *
 * While running, in another terminal:
 * $ ps -ef | grep 01-fork-basics
 * Shows both parent and child processes running!
 */