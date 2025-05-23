/*
 * SOLUTION: Exercise 4 - Parallel Workers
 * 
 * This solution demonstrates managing multiple child processes,
 * each doing different work and exiting with different status codes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int work_times[] = {1, 2, 3};    // Work duration for each worker
    int exit_codes[] = {10, 20, 30}; // Exit codes for each worker
    
    printf("Parent: Starting 3 workers...\n");
    fflush(stdout); // Ensure output is flushed before forking
    
    // Create 3 child processes
    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("Fork failed");
            return EXIT_FAILURE;
        }
        
        if (pid == 0) {
            // Child process
            int worker_id = i + 1;
            
            printf("Worker %d: Starting work (%d second%s)\n", 
                   worker_id, work_times[i], 
                   work_times[i] == 1 ? "" : "s");
            
            // Simulate work
            sleep(work_times[i]);
            
            printf("Worker %d: Work completed\n", worker_id);
            
            // Exit with unique status code
            exit(exit_codes[i]);
        }
    }
    
    // Parent waits for all children to complete
    printf("Parent: Waiting for workers to complete...\n");
    
    for (int i = 0; i < 3; i++) {
        int status;
        pid_t completed_pid = wait(&status);
        
        if (completed_pid > 0) {
            // Check if child exited normally
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                printf("Parent: Worker with PID %d exited with status %d\n",
                       completed_pid, exit_status);
            } else {
                printf("Parent: Worker with PID %d terminated abnormally\n",
                       completed_pid);
            }
        } else {
            perror("Wait failed");
        }
    }
    
    printf("Parent: All workers completed\n");
    return 0;
}

/*
 * ENHANCED VERSION WITH WAITPID():
 * 
 * This version waits for specific children and provides more control:
 */

/*
int main() {
    int work_times[] = {1, 2, 3};
    int exit_codes[] = {10, 20, 30};
    pid_t child_pids[3];
    
    printf("Parent: Starting 3 workers...\n");
    
    // Create children (same as above)
    for (int i = 0; i < 3; i++) {
        // ... (child creation code same as above)
    }
    
    // Wait for specific children using waitpid()
    for (int i = 0; i < 3; i++) {
        int status;
        pid_t result = waitpid(child_pids[i], &status, 0);
        
        if (result == child_pids[i]) {
            if (WIFEXITED(status)) {
                printf("Parent: Worker %d (PID %d) exited with status %d\n",
                       i+1, child_pids[i], WEXITSTATUS(status));
            }
        }
    }
    
    return 0;
}
*/

/*
 * EXPLANATION:
 * 
 * 1. MULTIPLE CHILD CREATION:
 *    - Loop creates 3 children sequentially
 *    - Each child does different work (different sleep times)
 *    - Each child exits with unique status code
 * 
 * 2. EXIT STATUS COMMUNICATION:
 *    - Children call exit() with specific codes
 *    - Parent retrieves codes using WEXITSTATUS() macro
 *    - This is a form of process communication
 * 
 * 3. WAIT() VS WAITPID():
 *    - wait(): Waits for any child to complete
 *    - waitpid(): Can wait for specific child by PID
 *    - Both return child's PID and status information
 * 
 * 4. STATUS MACROS:
 *    - WIFEXITED(status): True if child exited normally
 *    - WEXITSTATUS(status): Extract exit code (0-255)
 *    - WIFSIGNALED(status): True if killed by signal
 *    - WTERMSIG(status): Extract terminating signal
 * 
 * 5. PARALLEL EXECUTION:
 *    - All children run simultaneously
 *    - Worker 1 finishes first (1 second)
 *    - Worker 2 finishes second (2 seconds)
 *    - Worker 3 finishes last (3 seconds)
 * 
 * KEY CONCEPTS:
 * - Managing multiple processes
 * - Process communication via exit status
 * - Non-deterministic completion order
 * - Resource cleanup with wait()
 * 
 * REAL-WORLD APPLICATIONS:
 * - Web servers handling multiple requests
 * - Parallel data processing
 * - Task distribution systems
 * - Build systems (like make -j)
 */