// PEDAGOGICAL PURPOSE:
// This program demonstrates parent-child synchronization with wait().
// Key learning objectives:
// 1. Using wait(NULL) to synchronize parent and child
// 2. Ensuring deterministic execution order (child always before parent)
// 3. Understanding that wait() blocks the parent until child exits
// 4. Preventing zombie processes
// 5. Why wait(NULL) ignores the exit status
// 6. Controlled message ordering in parent-child programs

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Contains fork(), getpid()
#include <sys/types.h>   // Contains pid_t type definition
#include <sys/wait.h>    // Contains wait() function

int main(){

  // CREATE CHILD PROCESS:
  pid_t pid = fork();
  // After fork(): TWO processes exist
  // Parent gets child's PID, child gets 0

  // ERROR HANDLING:
  if(pid < 0){
    // Fork failed
    perror("Fork failed");
    return 1;
  }

  // CHILD PROCESS CODE:
  else if(pid == 0){
    // This code runs ONLY in the child process

    // CHILD PRINTS MESSAGE:
    printf("This is the child process.\n");
    // This will ALWAYS print before the parent's message
    // Why? Because the parent calls wait() and blocks

    // CHILD EXITS:
    // When we return, the child process terminates
    // The exit status (0) becomes available to the parent
  }

  // PARENT PROCESS CODE:
  else{
    // This code runs ONLY in the parent process

    // WAIT FOR CHILD TO FINISH:
    wait(NULL);
    // wait(NULL) does several things:
    // 1. BLOCKS until one child process terminates
    // 2. Collects the child's exit status (but we ignore it with NULL)
    // 3. Prevents the child from becoming a zombie
    // 4. Returns the PID of the child that exited
    //
    // WHY wait(NULL)?
    // We pass NULL because we don't care about the exit status
    // If we wanted the status, we'd pass: wait(&status)
    //
    // WHAT HAPPENS DURING wait()?
    // - Parent is BLOCKED (not running)
    // - Child continues to execute
    // - When child exits, parent UNBLOCKS and continues
    //
    // RESULT: Child's printf() always happens before parent's printf()

    // PARENT PRINTS MESSAGE (AFTER CHILD EXITS):
    printf("This is the parent process.\n");
    // This ALWAYS prints AFTER the child's message
    // Guaranteed by wait() blocking until child exits
  }

  // COMMON CODE - RUNS IN BOTH:
  printf("This message is printed by both the parent and the child\n");
  // Both processes reach this line and print
  //
  // EXPECTED OUTPUT ORDER:
  // 1. Child prints "This is the child process."
  // 2. Child prints "This message is printed by both..."
  // 3. Child exits
  // 4. Parent unblocks from wait()
  // 5. Parent prints "This is the parent process."
  // 6. Parent prints "This message is printed by both..."
  // 7. Parent exits
  //
  // Notice: Child's messages (1,2) come before parent's messages (5,6)
  // because wait() ensures the child finishes first.

  return 0;
}

// EXECUTION TRACE:
//
// Time    Process    Action                              State
// ----    -------    ------                              -----
// T0      Parent     main() starts
// T1      Parent     pid = fork()
// T2      Parent     fork() returns child's PID          Parent: pid = 5001
//         Child      fork() returns 0                    Child:  pid = 0
// T3      Parent     pid > 0, enter else block
//         Child      pid == 0, enter else if block
// T4      Parent     wait(NULL) - BLOCKS                 Parent: WAITING
//         Child      printf("This is the child...")
// T5      Child      Output: "This is the child process."
// T6      Child      Fall through to common code
// T7      Child      printf("This message...")
// T8      Child      Output: "This message is printed by both..."
// T9      Child      return 0 - CHILD EXITS
// T10     Parent     wait() returns - UNBLOCKED          Parent: RUNNING
// T11     Parent     printf("This is the parent...")
// T12     Parent     Output: "This is the parent process."
// T13     Parent     Fall through to common code
// T14     Parent     printf("This message...")
// T15     Parent     Output: "This message is printed by both..."
// T16     Parent     return 0 - PARENT EXITS

// EXPECTED OUTPUT (always in this order):
// This is the child process.
// This message is printed by both the parent and the child
// This is the parent process.
// This message is printed by both the parent and the child

// CONTRAST WITH pid1.c:
//
// pid1.c (without wait):
// - Output order is NON-DETERMINISTIC
// - Parent and child race to print
// - Sometimes parent prints first, sometimes child
// - Child might become a zombie temporarily
//
// pid2.c (with wait):
// - Output order is DETERMINISTIC
// - Child ALWAYS prints before parent
// - Parent waits for child to complete
// - No zombie process (parent collects status immediately)

// CONCEPTUAL EXPLANATION:
//
// WHAT IS wait()?
// wait() is a system call that:
// 1. Suspends the calling process (parent)
// 2. Waits for a child process to change state (usually to exit)
// 3. Collects the child's exit status
// 4. Cleans up the child's process table entry
//
// WHY USE wait()?
// Without wait():
// - Child exits but remains as a "zombie"
// - Zombie sits in process table until parent waits or exits
// - Too many zombies can exhaust process table
//
// With wait():
// - Parent immediately collects child's status when child exits
// - Child is completely cleaned up
// - No zombie accumulation
//
// wait(NULL) vs wait(&status):
// - wait(NULL): Ignore the exit status, just wait for termination
// - wait(&status): Save the exit status in 'status' variable
//
// BLOCKING BEHAVIOR:
// When parent calls wait():
// 1. If child already exited: wait() returns immediately
// 2. If child still running: parent blocks until child exits
// 3. If no children exist: wait() returns -1 with errno = ECHILD
//
// ZOMBIE PROCESS PREVENTION:
// When a process exits:
// - Kernel keeps minimal info in process table (PID, exit status)
// - Process is "zombie" until parent calls wait()
// - wait() reads this info and allows kernel to fully clean up
// - This is called "reaping" the zombie

// WHY wait(NULL)?
//
// We use wait(NULL) instead of wait(&status) because:
// 1. We don't care about the child's exit status
// 2. Child always exits with status 0 (success)
// 3. We only care about SYNCHRONIZATION
// 4. NULL is simpler when status isn't needed
//
// If we wanted to check the status:
// int status;
// wait(&status);
// if (WIFEXITED(status)) {
//     printf("Child exited with code %d\n", WEXITSTATUS(status));
// }

// PROCESS LIFECYCLE:
//
// 1. RUNNING - Process is executing
// 2. SLEEPING - Process is waiting for event (like I/O)
// 3. STOPPED - Process is paused (via SIGSTOP)
// 4. ZOMBIE - Process exited but parent hasn't wait()'ed
// 5. TERMINATED - Process fully cleaned up (after wait())
//
// This program demonstrates:
// Child: RUNNING → ZOMBIE (briefly) → TERMINATED (after parent's wait)
// Parent: RUNNING → SLEEPING (in wait) → RUNNING → TERMINATED

// COMMON QUESTIONS:
//
// Q: What if child exits before parent calls wait()?
// A: Child becomes a zombie. When parent calls wait(),
//    it immediately collects the status and zombie is cleaned up.
//
// Q: What if parent exits before calling wait()?
// A: Child (zombie) is adopted by init (PID 1).
//    Init periodically calls wait() to clean up orphaned zombies.
//
// Q: Can wait() wait for a specific child?
// A: Yes! Use waitpid(pid, &status, options) to wait for specific child.
//    wait(NULL) is equivalent to waitpid(-1, NULL, 0).
//
// Q: What if we have multiple children?
// A: wait() returns when ANY child exits.
//    To wait for all children, call wait() in a loop.
//
// Q: Does wait() return the child's PID?
// A: Yes! wait() returns the PID of the child that exited.
//    We're ignoring it here, but you could: pid_t child_pid = wait(NULL);

// TYPICAL USAGE PATTERN:
//
// pid_t pid = fork();
// if (pid == 0) {
//     // Child does work
//     // ...
//     exit(0);  // or return 0
// } else {
//     // Parent waits for child
//     wait(NULL);  // Block until child finishes
//     // Continue after child completes
// }

// TRY IT:
// Compile: gcc pid2.c -o pid2
// Run: ./pid2
//
// Output (always in this order):
// This is the child process.
// This message is printed by both the parent and the child
// This is the parent process.
// This message is printed by both the parent and the child
//
// Experiments:
// 1. Run multiple times - output order is ALWAYS the same
//    (Unlike pid1.c which has non-deterministic ordering)
//
// 2. Add sleep to visualize wait():
//    In child, before printf: sleep(3);
//    Now you can see parent waits 3 seconds for child
//
// 3. Remove wait(NULL) and compare with pid1.c behavior
//
// 4. Change wait(NULL) to:
//    int status;
//    pid_t child_pid = wait(&status);
//    printf("Child %d exited with status %d\n", child_pid, WEXITSTATUS(status));
//
// 5. Add multiple children:
//    fork(); fork();
//    Now 4 processes exist! Parent would need multiple wait() calls.
