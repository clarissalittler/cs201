// PEDAGOGICAL PURPOSE:
// This program demonstrates distinguishing parent from child after fork().
// Key learning objectives:
// 1. Using fork()'s return value to identify parent vs. child
// 2. Understanding that fork() returns DIFFERENT values to each process
// 3. Parent receives the child's PID; child receives 0
// 4. Error checking for fork() failure
// 5. How both processes continue execution after fork()
// 6. Why certain code runs twice (after fork) and certain code runs once (before fork)
// 7. Using perror() for error reporting

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Contains fork(), getpid()
#include <sys/types.h>   // Contains pid_t type definition
#include <sys/wait.h>    // Contains wait() function

int main() {
    // FORK: CREATE A NEW PROCESS
    pid_t pid = fork();  // Create a new process
    //
    // WHAT HAPPENS AT fork()?
    // Before fork(): ONE process exists
    // After fork(): TWO processes exist (parent and child)
    //
    // RETURN VALUES:
    // In parent process: pid = child's PID (positive number, e.g., 5001)
    // In child process:  pid = 0
    // On error:          pid = -1 (fork failed, no child created)
    //
    // WHY DIFFERENT RETURN VALUES?
    // This is how each process knows its role:
    // - Parent needs to know child's PID (for wait, kill, etc.)
    // - Child doesn't need parent's PID (can get it via getppid())
    // - Both check pid to determine what code to execute

    // ERROR HANDLING:
    if (pid < 0) {
        // Fork failed (pid == -1)
        // Possible reasons:
        // - System out of memory
        // - Process limit reached
        // - Other resource constraints

        perror("Fork failed");
        // perror() prints "Fork failed: <error description>"
        // It uses errno to print the actual system error
        // Example: "Fork failed: Resource temporarily unavailable"

        return 1;
        // Exit with error status
        // No child process was created
    }

    // CHILD PROCESS CODE:
    else if (pid == 0) {
        // This code ONLY runs in the child process
        // We know we're the child because fork() returned 0

        // CHILD PRINTS ITS PID:
        printf("Hello from the child process! My PID is %d\n", getpid());
        // getpid() returns this process's PID
        // This will be a different number than the parent's PID
        //
        // NOTE: We could also call getppid() here to show parent's PID:
        // printf("My parent's PID is %d\n", getppid());

        //getpid() <- returns my PID
        //getppid() <- returns my parents PID
        // These comments explain the two key functions for process identity
    }

    // PARENT PROCESS CODE:
    else {
        // This code ONLY runs in the parent process
        // We know we're the parent because fork() returned a positive PID

        // PARENT PRINTS CHILD'S PID:
        printf("Hello from the parent process! My child's PID is %d\n", pid);
        // 'pid' contains the child's PID (the value fork() returned)
        // Parent can use this PID to:
        // - Wait for the child (wait())
        // - Send signals to the child (kill())
        // - Check child's status
    }

    // COMMON CODE - RUNS IN BOTH PROCESSES:
    // Both processes reach here and print a message
    printf("This message is printed by both the parent and the child.\n");
    //
    // WHY PRINTED TWICE?
    // Because TWO processes are running this code:
    // - The parent prints it
    // - The child prints it
    //
    // EXECUTION ORDER:
    // We don't know which prints first! The OS scheduler decides.
    // Sometimes parent prints first, sometimes child prints first.
    // This is NON-DETERMINISTIC execution.

    // BOTH PROCESSES EXIT:
    return 0;
    // When parent exits: Shell shows prompt again
    // When child exits: Parent can collect status (if it calls wait())
    //
    // NOTE: This program does NOT call wait()
    // The child becomes a "zombie" briefly until:
    // - Parent exits (then init adopts and reaps the zombie)
    // - Or parent implicitly waits when exiting
}

// EXECUTION TRACE:
//
// Time    Process    Action                              Output
// ----    -------    ------                              ------
// T0      Parent     main() starts, only ONE process exists
// T1      Parent     pid = fork()
// T2      Parent     fork() returns child's PID (e.g., 5001)
//         Child      fork() returns 0
// T3      Parent     pid (5001) not < 0, not == 0, must be > 0
//         Child      pid (0) not < 0, is == 0
// T4      Parent     Enter else block
//         Child      Enter else if (pid == 0) block
// T5      Child      printf("Hello from child...")       "Hello from child! My PID is 5002"
// T6      Parent     printf("Hello from parent...")      "Hello from parent! My child's PID is 5002"
// T7      ???        One prints the common message       "This message is printed..."
// T8      ???        Other prints the common message     "This message is printed..."
// T9      Both       return 0, both processes exit

// TYPICAL OUTPUT (order may vary):
// Hello from the child process! My PID is 12346
// Hello from the parent process! My child's PID is 12346
// This message is printed by both the parent and the child.
// This message is printed by both the parent and the child.
//
// OR:
// Hello from the parent process! My child's PID is 12346
// Hello from the child process! My PID is 12346
// This message is printed by both the parent and the child.
// This message is printed by both the parent and the child.
//
// OR even interleaved:
// Hello from the parent process! My child's PID is 12346
// This message is printed by both the parent and the child.
// Hello from the child process! My PID is 12346
// This message is printed by both the parent and the child.

// CONCEPTUAL EXPLANATION:
//
// FORK'S DUAL RETURN VALUE:
// fork() is unique because it "returns twice":
// - Once in the parent (returns child's PID)
// - Once in the child (returns 0)
// This is possible because after fork(), TWO processes exist,
// each continuing from the same point but with different return values.
//
// WHY 0 FOR CHILD?
// - 0 is never a valid PID (reserved for kernel)
// - Easy to test: if (pid == 0) → I'm the child
// - Parent gets the actually useful information (child's PID)
//
// PROCESS IDENTITY SUMMARY:
// From parent's perspective:
// - I called fork() and got back a PID
// - That PID is my child's process ID
// - I can use it to interact with my child
//
// From child's perspective:
// - I called fork() and got back 0
// - I'm a new process, a copy of my parent
// - I can use getppid() if I need parent's PID
//
// MEMORY AFTER FORK:
// The child is a COPY of the parent:
// - All variables are copied (including 'pid')
// - All code is shared (copy-on-write)
// - All file descriptors are duplicated
// - Memory is duplicated (but efficiently with copy-on-write)
// But 'pid' has different VALUES in each process!

// COMMON QUESTIONS:
//
// Q: How can fork() return twice?
// A: It doesn't really. It returns once in each process.
//    From the parent's perspective, it returns once.
//    From the child's perspective, it returns once.
//    But since two processes exist, there are two returns total.
//
// Q: Why does the child get 0 instead of its own PID?
// A: The child can always get its PID with getpid().
//    But the parent can't easily get the child's PID without fork()'s return value.
//    So fork() gives the parent the useful information.
//
// Q: What if fork() is called in a loop?
// A: You get exponential growth! 1st fork: 2 processes,
//    2nd fork: 4 processes (both processes fork),
//    3rd fork: 8 processes, etc.
//
// Q: Why do both processes print the last message?
// A: Because both processes execute all code after fork().
//    Code before fork() runs once; code after fork() runs twice.
//
// Q: Which process prints first?
// A: Undefined! The OS scheduler decides. This is non-determinism.

// COMPARISON WITH OTHER EXAMPLES:
//
// This is simpler than pid2.c which adds wait().
// This is more realistic than fork1.c which doesn't use pid.
// This shows the basic pattern for distinguishing parent and child.

// TRY IT:
// Compile: gcc pid1.c -o pid1
// Run: ./pid1
//
// Expected output (PIDs will vary):
// Hello from the child process! My PID is 12346
// Hello from the parent process! My child's PID is 12346
// This message is printed by both the parent and the child.
// This message is printed by both the parent and the child.
//
// Experiments:
// 1. Run multiple times and observe:
//    - PIDs change each time
//    - Order of output may vary
//
// 2. Add sleep to control ordering:
//    In parent: sleep(1); before printf
//    Now child always prints first
//
// 3. Print both PIDs in child:
//    printf("Child PID: %d, Parent PID: %d\n", getpid(), getppid());
//
// 4. Add more forks and see what happens:
//    fork(); fork();
//    Now you get 4 processes!
