/*
 * SOLUTION: Exercise 1 - First Fork
 * 
 * This solution demonstrates the basic use of fork() to create a child process.
 * Both parent and child print their process IDs and parent process IDs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid;
    
    printf("Before fork: About to create child process\n");
    fflush(stdout); // Ensure output is flushed before fork
    
    // Fork to create a child process
    pid = fork();
    
    // Check for fork() failure
    if (pid < 0) {
        perror("Fork failed");
        return EXIT_FAILURE;
    }
    
    // Check if this is the child process
    if (pid == 0) {
        // This is the child process
        printf("Child process: PID=%d, PPID=%d\n", getpid(), getppid());
        
        // Optional: Add small delay to observe scheduling
        usleep(100000); // 100ms delay
        printf("Child: Exiting\n");
    } else {
        // This is the parent process
        printf("Parent process: PID=%d, PPID=%d, Child PID=%d\n", 
               getpid(), getppid(), pid);
        
        // Optional: Add small delay to observe scheduling  
        usleep(50000); // 50ms delay
        printf("Parent: Exiting\n");
    }
    
    return 0;
}

/*
 * EXPLANATION:
 * 
 * 1. fork() creates an exact copy of the calling process
 * 2. After fork(), both processes continue from the same point
 * 3. fork() returns:
 *    - 0 to the child process
 *    - Child's PID to the parent process  
 *    - -1 if fork() failed
 * 
 * 4. getpid() returns the current process's PID
 * 5. getppid() returns the parent process's PID
 * 
 * KEY OBSERVATIONS:
 * - The child's PPID equals the parent's PID
 * - Both processes run independently
 * - The order of output may vary due to process scheduling
 * - Each process has its own memory space after fork()
 * 
 * COMMON MISTAKES:
 * - Forgetting to check fork() return value for errors
 * - Not understanding that both processes execute the code after fork()
 * - Assuming a particular execution order
 */