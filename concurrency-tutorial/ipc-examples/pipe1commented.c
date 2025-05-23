#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

int main() {
    /*
     * File descriptors for the pipe
     * 
     * TEACHING POINT: In a pipe, we need two file descriptors:
     * - pipefd[0] is for reading from the pipe
     * - pipefd[1] is for writing to the pipe
     * 
     * This is a unidirectional communication channel - data flows
     * in one direction only, from the write end to the read end.
     */
    int pipefd[2];        // pipefd[0] is for reading, pipefd[1] is for writing
    
    /*
     * Process ID for tracking parent and child
     */
    pid_t pid;
    
    /*
     * Message buffers for communication
     */
    char write_msg[] = "Hello from the child process!";
    char read_msg[100];

    /*
     * Create a pipe
     * 
     * TEACHING POINT: The pipe() system call creates a unidirectional
     * communication channel. If successful, it returns 0 and populates
     * the pipefd array with file descriptors. If it fails, it returns -1.
     * 
     * This must be called BEFORE fork() so that both parent and child
     * processes have access to the pipe's file descriptors.
     */
    if (pipe(pipefd) == -1) {
        perror("Pipe failed");
        return 1;
    }

    /*
     * Create a new process
     * 
     * TEACHING POINT: After fork(), both parent and child have copies
     * of the pipe file descriptors. This allows them to communicate
     * through the pipe even though they're separate processes.
     */
    pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        return 1;
    }
    else if (pid == 0) {
        /*
         * Child process code path
         * 
         * TEACHING POINT: In this example, the child is the sender and
         * parent is the receiver. The child closes the read end since
         * it doesn't need it, then writes to the write end.
         */
        
        /*
         * Close the read end of the pipe
         * 
         * TEACHING POINT: It's important to close unused pipe ends to:
         * 1. Free up file descriptors
         * 2. Allow proper EOF detection - if all write ends are closed,
         *    read() will return 0 to indicate EOF
         */
        close(pipefd[0]); // Close the read end of the pipe
        
        /*
         * Send a message to the parent
         * 
         * TEACHING POINT: write() sends data through the pipe to the reader.
         * We include the null terminator (+1) to ensure it's a proper string.
         */
        write(pipefd[1], write_msg, strlen(write_msg) + 1); // Send message to parent
        
        /*
         * Close the write end after we're done
         * 
         * TEACHING POINT: This is crucial! If the write end isn't closed,
         * the parent's read() call would block indefinitely waiting for more
         * data instead of getting EOF.
         */
        close(pipefd[1]); // Close the write end of the pipe after writing
    }
    else {
        /*
         * Parent process code path
         * 
         * TEACHING POINT: The parent is the receiver in this example.
         * It closes the write end since it doesn't need it, then reads
         * from the read end.
         */
        
        /*
         * Close the write end of the pipe
         */
        close(pipefd[1]); // Close the write end of the pipe
        
        /*
         * Read the message from the child
         * 
         * TEACHING POINT: read() blocks until either:
         * 1. Data is available to read from the pipe
         * 2. All write ends of the pipe are closed (EOF)
         * 
         * Since pipes are byte streams with no message boundaries,
         * the parent could read() in a loop until EOF if it needed
         * to handle variable-sized messages.
         */
        read(pipefd[0], read_msg, sizeof(read_msg)); // Read the message from the child
        
        /*
         * Print the received message
         */
        printf("Parent received: %s\n", read_msg);
        
        /*
         * Close the read end after we're done
         */
        close(pipefd[0]); // Close the read end of the pipe after reading
    }

    return 0;
}