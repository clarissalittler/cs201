#include <stdio.h>  // Include standard input/output library (printf, perror)
#include <unistd.h> // Include POSIX operating system API (fork, pipe, read, write)
#include <string.h> // Include string manipulation functions (strlen)
#include <sys/types.h> // Include system data types (pid_t)

int main() {
    int pipefd[2];        // Declare an array to hold the file descriptors for the pipe
                          // pipefd[0] is for reading, pipefd[1] is for writing

    pid_t pid;            // Declare a variable to store the process ID of the child
    char writeMsg[] = "Hello from the child process!"; // Message to be sent from child
    char readMsg[100];     // Buffer to store the message received by the parent

    // Create a pipe
    if (pipe(pipefd) == -1) { 
        perror("Pipe failed"); // If pipe creation fails, print an error message
        return 1;            // Indicate an error occurred and exit
    }

    // Create a new process
    pid = fork();          // Fork the current process, creating a child process

    if (pid < 0) {         // If fork fails
        perror("Fork failed"); // Print an error message
        return 1;            // Indicate an error occurred and exit
    }
    else if (pid == 0) {   // This is the child process
        close(pipefd[0]); // Close the read end of the pipe in the child process
                         // as it will only write
        write(pipefd[1], writeMsg, strlen(writeMsg) + 1); // Write the message to the pipe
                                                //  + 1 to include the null terminator
        close(pipefd[1]); // Close the write end of the pipe in the child process
                         // as it is done writing
    }
    else {                // This is the parent process
        close(pipefd[1]); // Close the write end of the pipe in the parent process
                         // as it will only read
        read(pipefd[0], readMsg, sizeof(readMsg)); // Read the message from the pipe
        printf("Parent received: %s\n", readMsg); // Print the received message
        close(pipefd[0]); // Close the read end of the pipe in the parent process
                         // as it is done reading
    }

    return 0;            // Exit the program successfully
}
