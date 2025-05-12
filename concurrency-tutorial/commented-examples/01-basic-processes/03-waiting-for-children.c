/**
 * ============================================================================
 * Process Termination and Waiting for Child Processes
 * ============================================================================
 * 
 * This example demonstrates how to properly wait for child processes to
 * terminate and retrieve their exit status. This is a fundamental aspect
 * of process management and is important for preventing zombie processes.
 * 
 * We'll cover:
 * - How to use wait() and waitpid()
 * - How to retrieve and interpret a child's exit status
 * - How to prevent zombie processes
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* 
 * Function prototype for our example child function.
 * This will be run by each child process.
 */
void child_function(int child_num, int exit_code);

int main() {
    pid_t pid1, pid2, pid3;
    int status;
    
    printf("Parent process (PID: %d) starting...\n", getpid());
    
    /* Create first child process */
    pid1 = fork();
    
    if (pid1 < 0) {
        perror("First fork failed");
        return EXIT_FAILURE;
    }
    else if (pid1 == 0) {
        /* First child: exit with status 0 (success) */
        child_function(1, 0);
        /* Note: child_function calls exit(), so we never reach here */
    }
    
    /* Create second child process (parent continues here) */
    pid2 = fork();
    
    if (pid2 < 0) {
        perror("Second fork failed");
        return EXIT_FAILURE;
    }
    else if (pid2 == 0) {
        /* Second child: exit with status 1 (indicating an error) */
        child_function(2, 1);
    }
    
    /* Create third child process (parent continues here) */
    pid3 = fork();
    
    if (pid3 < 0) {
        perror("Third fork failed");
        return EXIT_FAILURE;
    }
    else if (pid3 == 0) {
        /* 
         * Third child: exit with status 42 
         * (custom code that could mean something specific to our application)
         */
        child_function(3, 42);
    }
    
    /* Parent code continues here after creating all children */
    printf("\nParent: All children created.\n");
    printf("Parent: Waiting for children to terminate...\n\n");
    
    /* ================== WAITING FOR CHILDREN ================== */
    
    /* Method 1: Using wait() - Waits for ANY child to terminate */
    pid_t terminated_pid = wait(&status);
    printf("Parent: Child with PID %d terminated.\n", terminated_pid);
    
    /* 
     * Checking exit status:
     * WIFEXITED(status) - Returns true if the child terminated normally
     * WEXITSTATUS(status) - Returns the exit code (0-255) if child exited normally
     */
    if (WIFEXITED(status)) {
        printf("  - Child exited normally with status: %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        /* WIFSIGNALED and WTERMSIG check if the child was terminated by a signal */
        printf("  - Child terminated by signal: %d\n", WTERMSIG(status));
    }
    
    /* Method 2: Using waitpid() - Waits for a SPECIFIC child */
    printf("\nParent: Waiting specifically for child with PID %d...\n", pid2);
    
    /* 
     * waitpid() parameters:
     * - PID to wait for (or -1 for any child, like wait())
     * - Pointer to status variable
     * - Options (0 for default behavior, see below for others)
     */
    waitpid(pid2, &status, 0);
    
    if (WIFEXITED(status)) {
        printf("  - Child %d exited normally with status: %d\n", pid2, WEXITSTATUS(status));
    }
    
    /* Method 3: Non-blocking wait using WNOHANG option */
    printf("\nParent: Checking if child %d has terminated (non-blocking)...\n", pid3);
    
    /* 
     * WNOHANG option makes waitpid() non-blocking:
     * - Returns 0 if the child is still running
     * - Returns the child's PID if it has terminated
     */
    pid_t result;
    do {
        result = waitpid(pid3, &status, WNOHANG);
        
        if (result == 0) {
            printf("  - Child %d is still running. Waiting a bit...\n", pid3);
            sleep(1); /* Wait a second before checking again */
        }
    } while (result == 0);
    
    if (WIFEXITED(status)) {
        printf("  - Child %d exited normally with status: %d\n", pid3, WEXITSTATUS(status));
    }
    
    printf("\nParent: All children have been waited for. Exiting.\n");
    
    return EXIT_SUCCESS;
}

/* 
 * Implementation of the child function.
 * Each child will execute this code.
 */
void child_function(int child_num, int exit_code) {
    printf("Child %d (PID: %d): Starting...\n", child_num, getpid());
    
    /* Sleep for a random time between 1-3 seconds */
    int sleep_time = 1 + (child_num % 3);
    printf("Child %d: Working for %d seconds...\n", child_num, sleep_time);
    sleep(sleep_time);
    
    printf("Child %d: Finished work, exiting with code %d.\n", child_num, exit_code);
    
    /* 
     * exit() immediately terminates the process with the given status code.
     * The status code can be retrieved by the parent process.
     */
    exit(exit_code);
}

/**
 * EXPLANATION:
 * 
 * Waiting for Child Processes:
 * 
 * 1. Why Wait?
 *    - To prevent zombie processes (terminated but not reaped by parent)
 *    - To retrieve the child's exit status
 *    - To ensure proper synchronization between parent and child
 * 
 * 2. wait() vs waitpid():
 *    - wait(&status): Blocks until ANY child terminates
 *    - waitpid(pid, &status, options): More control over which process to wait for
 * 
 * 3. waitpid() Options:
 *    - 0: Default behavior (blocks until the specified child terminates)
 *    - WNOHANG: Non-blocking; returns immediately if no child has exited
 *    - WUNTRACED: Also return if a child has stopped (but not traced via ptrace)
 *    - WCONTINUED: Also return if a stopped child has been resumed
 * 
 * 4. Checking Exit Status:
 *    - WIFEXITED(status): True if the child terminated normally (via exit())
 *    - WEXITSTATUS(status): The exit status (0-255) of the child
 *    - WIFSIGNALED(status): True if the child was terminated by a signal
 *    - WTERMSIG(status): The signal number that caused the child to terminate
 * 
 * 5. Process Termination Methods:
 *    - return (from main): Equivalent to exit() with the return value
 *    - exit(): Normal termination with status code
 *    - _exit(): Immediate termination (doesn't flush stdio buffers)
 *    - Signals (e.g., SIGTERM, SIGKILL): Abnormal termination
 */