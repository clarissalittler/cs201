/*
 * Exercise 4: Parallel Workers
 * Difficulty: 🟡 Intermediate
 * 
 * OBJECTIVE:
 * Create a program that spawns 3 child processes. Each child should "work" 
 * for a different amount of time, then exit with a unique status code. 
 * The parent should wait for all children and report their exit statuses.
 * 
 * REQUIREMENTS:
 * 1. Create exactly 3 child processes
 * 2. Each child should:
 *    - Print a start message with its worker ID (1, 2, or 3)
 *    - Sleep for a different duration (1, 2, 3 seconds respectively)
 *    - Print a completion message
 *    - Exit with a unique status code (10, 20, 30 respectively)
 * 3. The parent should:
 *    - Wait for all children to complete
 *    - Print the exit status of each child as they finish
 *    - Print a final summary
 * 
 * EXPECTED OUTPUT (order of completion may vary):
 * Parent: Starting 3 workers...
 * Worker 1: Starting work (1 second)
 * Worker 2: Starting work (2 seconds)  
 * Worker 3: Starting work (3 seconds)
 * Worker 1: Work completed
 * Parent: Worker with PID 1234 exited with status 10
 * Worker 2: Work completed
 * Parent: Worker with PID 1235 exited with status 20
 * Worker 3: Work completed
 * Parent: Worker with PID 1236 exited with status 30
 * Parent: All workers completed
 * 
 * HINTS:
 * - Use a loop to create multiple children
 * - Use WEXITSTATUS() macro to extract exit status from wait()
 * - Children should exit() with their status code
 * - Parent needs to wait() three times
 * 
 * LEARNING GOALS:
 * - Managing multiple child processes
 * - Understanding exit status communication
 * - Process synchronization patterns
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    // TODO: Define arrays for work duration and exit status
    // int work_times[] = {1, 2, 3};
    // int exit_codes[] = {10, 20, 30};
    
    printf("Parent: Starting 3 workers...\n");
    
    // TODO: Create 3 child processes using a loop
    // for (int i = 0; i < 3; i++) {
    
        // TODO: Fork a child process
        
        // TODO: Handle fork() error
        
        // TODO: In child process:
        // - Print start message with worker ID (i+1)
        // - Sleep for work_times[i] seconds
        // - Print completion message
        // - Exit with exit_codes[i]
    // }
    
    // TODO: Parent waits for all children
    // for (int i = 0; i < 3; i++) {
        // TODO: Wait for any child to complete
        // TODO: Extract and print the exit status
        // TODO: Print which PID completed with what status
    // }
    
    printf("Parent: All workers completed\n");
    return 0;
}