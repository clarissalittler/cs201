#include <stdio.h>      // Standard Input/Output library for functions like printf and perror
#include <unistd.h>     // Unix standard library for functions like fork, pipe, read, write, close
#include <string.h>     // String handling library for functions like strlen
#include <sys/types.h>  // Data types used in system calls, like pid_t

int main() {
    int pipefd[2];        // Array to hold the two ends of the pipe: pipefd[0] for reading, pipefd[1] for writing
    pid_t pid;            // Variable to store the process ID returned by fork()
    char writeMsg[] = "Hello from the child process!"; // Message that the child process will send to the parent
    char readMsg[100];    // Buffer to store the message read by the parent process from the pipe

    // Create a pipe
    if (pipe(pipefd) == -1) { // The pipe() function initializes pipefd[0] and pipefd[1]
        perror("Pipe failed"); // If pipe() returns -1, an error occurred. perror() prints the error message
        return 1;             // Exit the program with a status of 1 indicating failure
    }

    // Create a new process by forking
    pid = fork(); // fork() creates a new child process. Both processes continue execution from this point

    if (pid < 0) {
        // Fork failed
        perror("Fork failed"); // If fork() returns a negative value, the creation of a child process failed
        return 1;              // Exit the program with a status of 1 indicating failure
    }
    else if (pid == 0) {
        // This is the child process
        close(pipefd[0]); // Close the read end of the pipe as the child will only write

        // Write the message to the write end of the pipe
        // Note: Corrected variable name from write_msg to writeMsg
        write(pipefd[1], writeMsg, strlen(writeMsg) + 1); 
        // write() sends the contents of writeMsg to pipefd[1]
        // strlen(writeMsg) calculates the length of the message
        // +1 includes the null terminator to properly end the string

        close(pipefd[1]); // Close the write end of the pipe after writing to signal EOF to the parent
    }
    else {
        // This is the parent process
        close(pipefd[1]); // Close the write end of the pipe as the parent will only read

        // Read the message from the read end of the pipe
        ssize_t numBytes = read(pipefd[0], readMsg, sizeof(readMsg)); 
        // read() attempts to read up to sizeof(readMsg) bytes from pipefd[0] into readMsg
        // It returns the number of bytes actually read, which is stored in numBytes

        if (numBytes == -1) {
            perror("Read failed"); // If read() returns -1, an error occurred
            return 1;              // Exit the program with a status of 1 indicating failure
        }

        printf("Parent received: %s\n", readMsg); 
        // Print the message received from the child process

        close(pipefd[0]); // Close the read end of the pipe after reading
    }

    return 0; // Exit the program successfully
}
