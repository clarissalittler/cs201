#include <stdio.h>  // Include the standard input/output library for functions like printf.
#include <stdlib.h> // Include the standard library for functions like perror.
#include <unistd.h> // Include the POSIX operating system API for functions like fork and getpid.
#include <sys/types.h> // Include the system types library for the pid_t data type.
#include <sys/wait.h> // Include the wait library for functions like wait.

int main() {
    pid_t pid = fork(); // Create a new process. fork() returns the process ID (PID) of the child process in the parent process, and 0 in the child process.

    if (pid < 0) { // Check if fork() failed.
        // Fork failed
        perror("Fork failed"); // Print an error message to stderr.
        return 1; // Return 1 to indicate an error occurred.
    }
    else if (pid == 0) { // This is the child process, as fork() returns 0 for the child.
        // This is the child process
        printf("Hello from the child process! My PID is %d\n", getpid()); // Print a message indicating the child process and its PID.
        // getpid() returns the process ID of the current process.
    }
    else { // This is the parent process, as fork() returns a positive value for the parent.
        // This is the parent process
        printf("Hello from the parent process! My child's PID is %d\n", pid); // Print a message indicating the parent process and the PID of the child process.
    }

    // Both processes reach here and print a message
    printf("This message is printed by both the parent and the child.\n"); // Print a message that is printed by both the parent and the child processes.

    return 0; // Return 0 to indicate the program executed successfully.
}
