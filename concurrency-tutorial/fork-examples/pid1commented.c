#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    /* 
     * Create a child process and store its PID
     * This is the fundamental way to create a new process in Unix/Linux
     */
    pid_t pid = fork();  // Create a new process

    /* 
     * Error handling for fork()
     * TEACHING POINT: Always check for fork failure
     * Fork can fail if the system is out of resources,
     * has reached its process limit, or has memory issues
     */
    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        return 1;
    }
    else if (pid == 0) {
        /* 
         * Child process code path
         * 
         * TEACHING POINT: Using getpid() to get the process's own PID
         * This demonstrates that the child has its own unique process ID
         * different from its parent
         */
        printf("Hello from the child process! My PID is %d\n", getpid());
    }
    else {
        /* 
         * Parent process code path
         * 
         * TEACHING POINT: The fork() return value gives the parent
         * direct access to its child's PID. This shows the relationship
         * between processes and how the parent can track its children.
         */
        printf("Hello from the parent process! My child's PID is %d\n", pid);
    }

    /* 
     * TEACHING POINT: Code executed by both processes
     * Any code outside the if/else blocks will be executed by both
     * the parent and child processes, demonstrating that they run 
     * independently after the fork.
     * 
     * Note that there's no synchronization here - the parent and child
     * might execute this at different times, causing interleaved output.
     */
    printf("This message is printed by both the parent and the child.\n");

    return 0;
}