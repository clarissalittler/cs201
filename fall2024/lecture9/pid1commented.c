#include <stdio.h>      // Standard input/output library for functions like printf
#include <stdlib.h>     // Standard library for functions like exit
#include <unistd.h>     // POSIX API for functions like fork and getpid
#include <sys/types.h>  // Defines data types used in system calls
#include <sys/wait.h>   // For waiting on process state changes, like wait

int main() {
    // Create a new process by duplicating the current process
    pid_t pid = fork();  // fork() returns:
                          // - Negative value if the creation of a child process was unsuccessful
                          // - Zero to the newly created child process
                          // - Positive value (child's PID) to the parent process

    if (pid < 0) {
        // This block executes if fork() failed
        perror("Fork failed");  // Print an error message to stderr
        return 1;               // Exit the program with a non-zero status indicating failure
    }
    else if (pid == 0) {
        // This block executes only in the child process
        printf("Hello from the child process! My PID is %d\n", getpid());
        // getpid() retrieves the Process ID (PID) of the calling process (child)
    }
    else {
        // This block executes only in the parent process
        printf("Hello from the parent process! My child's PID is %d\n", pid);
        // 'pid' contains the PID of the child process returned by fork()
    }

    // This statement is executed by both the parent and the child processes
    printf("This message is printed by both the parent and the child.\n");

    return 0;  // Exit the program successfully
}
