/*
 * SOLUTION: Exercise 3 - Process Chain
 * 
 * This solution creates a chain of processes and demonstrates process
 * synchronization using wait(). Each process waits for its child to
 * complete before exiting.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int generation = 1; // Start with generation 1
    pid_t pid;
    int status;
    char process_names[] = {'A', 'B', 'C'};
    
    // Print current process information
    printf("Process %c: PID=%d, PPID=%d\n", 
           process_names[generation-1], getpid(), getppid());
    
    // Check if we should create another process
    if (generation < 3) {
        // Fork a child process
        pid = fork();
        
        // Handle fork() error
        if (pid < 0) {
            perror("Fork failed");
            return EXIT_FAILURE;
        }
        
        if (pid == 0) {
            // Child process - increment generation and continue chain
            generation++;
            
            // Recursive approach: restart main logic for next generation
            // Print child process information
            printf("Process %c: PID=%d, PPID=%d\n", 
                   process_names[generation-1], getpid(), getppid());
            
            // Create grandchild if not at max generation
            if (generation < 3) {
                pid_t grandchild_pid = fork();
                
                if (grandchild_pid < 0) {
                    perror("Fork failed for grandchild");
                    return EXIT_FAILURE;
                }
                
                if (grandchild_pid == 0) {
                    // Grandchild process (generation 3)
                    generation++;
                    printf("Process %c: PID=%d, PPID=%d\n", 
                           process_names[generation-1], getpid(), getppid());
                    printf("Process C completed\n");
                    return 0;
                } else {
                    // Child waits for grandchild
                    wait(&status);
                    printf("Process B completed\n");
                    return 0;
                }
            }
        } else {
            // Parent process - wait for child to complete
            wait(&status);
            printf("Process A completed\n");
        }
    } else {
        // This is the leaf process (generation 3)
        printf("Process C completed\n");
    }
    
    return 0;
}

/*
 * ALTERNATIVE ITERATIVE SOLUTION:
 * 
 * A more elegant approach using a loop instead of recursion:
 */

/*
int main() {
    pid_t pid;
    int status;
    char process_names[] = {'A', 'B', 'C'};
    int generation = 1;
    
    while (generation <= 3) {
        printf("Process %c: PID=%d, PPID=%d\n", 
               process_names[generation-1], getpid(), getppid());
        
        if (generation == 3) {
            // Leaf process - no children to create
            printf("Process C completed\n");
            break;
        }
        
        pid = fork();
        
        if (pid < 0) {
            perror("Fork failed");
            return EXIT_FAILURE;
        }
        
        if (pid == 0) {
            // Child process - continue to next generation
            generation++;
        } else {
            // Parent process - wait for child and exit
            wait(&status);
            printf("Process %c completed\n", process_names[generation-1]);
            break;
        }
    }
    
    return 0;
}
*/

/*
 * EXPLANATION:
 * 
 * 1. PROCESS HIERARCHY:
 *    - Process A creates Process B
 *    - Process B creates Process C  
 *    - Forms a linear chain: A → B → C
 * 
 * 2. SYNCHRONIZATION:
 *    - Each parent waits for its child using wait()
 *    - wait() blocks until child terminates
 *    - Ensures orderly completion: C → B → A
 * 
 * 3. WAIT() SYSTEM CALL:
 *    - Suspends parent until any child terminates
 *    - Returns child's PID and exit status
 *    - Prevents zombie processes
 * 
 * 4. EXECUTION FLOW:
 *    a) A creates B, then waits
 *    b) B creates C, then waits  
 *    c) C completes and exits
 *    d) B's wait() returns, B completes
 *    e) A's wait() returns, A completes
 * 
 * KEY CONCEPTS:
 * - Process synchronization
 * - Parent-child relationships
 * - Orderly process termination
 * - Prevention of zombie processes
 * 
 * ZOMBIE PROCESSES:
 * If parents don't wait() for children, the children become zombies
 * after they exit - they remain in the process table until the parent
 * reads their exit status.
 */