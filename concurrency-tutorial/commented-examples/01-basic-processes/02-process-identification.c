/**
 * ============================================================================
 * Process Identification and Parent-Child Relationships
 * ============================================================================
 * 
 * This example demonstrates how to work with process IDs (PIDs) and shows
 * how to identify parent-child relationships between processes.
 * 
 * Every process in a Unix-like system has:
 * - A unique Process ID (PID)
 * - A Parent Process ID (PPID)
 * 
 * These identifiers help track process relationships and manage processes
 * within the system.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>  /* For wait() function */

int main() {
    pid_t pid;
    
    /* Print the PID of the original process before forking */
    printf("Original process PID: %d\n", getpid());
    
    /* Create a child process */
    pid = fork();
    
    if (pid < 0) {
        /* Error handling */
        perror("Fork failed");
        return EXIT_FAILURE;
    }
    else if (pid == 0) {
        /* Child process code */
        
        /* 
         * getpid() - Get the current process ID
         * getppid() - Get the parent process ID
         */
        printf("\nCHILD INFO:\n");
        printf("  I am the child process\n");
        printf("  My PID: %d\n", getpid());
        printf("  My parent's PID (PPID): %d\n", getppid());
        
        /* 
         * Sleep to demonstrate what happens when a parent terminates
         * before its child (orphan process)
         */
        printf("  Child sleeping for 2 seconds...\n");
        sleep(2);
        
        /* 
         * After sleeping, check if parent is still around.
         * If parent terminated, the PPID will have changed to the
         * init process (usually PID 1) or another "adoption" process.
         */
        printf("\nCHILD AFTER SLEEP:\n");
        printf("  My PID (still): %d\n", getpid());
        printf("  My parent's PID now: %d\n", getppid());
        
        if (getppid() != pid) {
            printf("  I've been orphaned! My original parent (PID %d) is gone.\n", pid);
            printf("  I've been adopted by another process.\n");
        }
        
        return EXIT_SUCCESS;
    }
    else {
        /* Parent process code */
        
        printf("\nPARENT INFO:\n");
        printf("  I am the parent process\n");
        printf("  My PID: %d\n", getpid());
        printf("  My child's PID: %d\n", pid);
        
        /* 
         * Option 1: Wait for child to finish
         * Uncomment this to see normal parent-child termination
         */
        /*
        printf("  Parent waiting for child to finish...\n");
        wait(NULL);
        printf("  Parent: Child has terminated.\n");
        */
        
        /* 
         * Option 2: Exit immediately to create an orphaned process
         * Comment this out to wait for the child instead
         */
        printf("  Parent exiting immediately without waiting for child.\n");
        printf("  This will cause the child to become an orphan.\n");
        
        return EXIT_SUCCESS;
    }
}

/**
 * EXPLANATION:
 * 
 * Process Relationships:
 * 
 * 1. Normal Process Termination
 *    - If the parent calls wait(), it will block until the child terminates
 *    - The parent can retrieve the child's exit status
 *    - This prevents zombie processes
 * 
 * 2. Orphan Processes
 *    - If a parent terminates before its child, the child becomes an "orphan"
 *    - Orphaned processes are adopted by the init process (PID 1) or another system process
 *    - The orphaned process's PPID changes to its new parent
 *    - The system ensures orphans are properly managed
 * 
 * 3. Zombie Processes
 *    - A child that has terminated but whose parent hasn't called wait()
 *    - The process entry remains in the process table
 *    - Only the exit status and minimal information is kept
 *    - Too many zombies can exhaust the process table
 * 
 * Process Management Functions:
 * - getpid(): Get current process ID
 * - getppid(): Get parent process ID
 * - wait()/waitpid(): Wait for child processes to terminate
 */