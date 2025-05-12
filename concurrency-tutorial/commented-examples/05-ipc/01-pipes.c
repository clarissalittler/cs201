/**
 * ============================================================================
 * Pipes - Unidirectional Communication Between Related Processes
 * ============================================================================
 * 
 * This example demonstrates using pipes for inter-process communication (IPC).
 * Pipes provide a simple way for related processes (typically parent-child) to
 * communicate via a unidirectional data channel.
 * 
 * Key concepts demonstrated:
 * - Creating a pipe with the pipe() system call
 * - Fork-and-pipe pattern for parent-child communication
 * - Reading from and writing to pipes
 * - Proper pipe management and cleanup
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

int main() {
    int pipe_fd[2];    /* Array to hold the file descriptors for the pipe */
    pid_t child_pid;   /* Process ID of the child */
    char buffer[BUFFER_SIZE];  /* Buffer for data transfer */
    
    printf("Pipe IPC Demonstration\n\n");
    
    /*
     * Create a pipe
     * pipe_fd[0] is the read end of the pipe
     * pipe_fd[1] is the write end of the pipe
     */
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }
    
    printf("Pipe created successfully.\n");
    printf("Read end (fd=%d), Write end (fd=%d)\n", pipe_fd[0], pipe_fd[1]);
    
    /* Create a child process */
    child_pid = fork();
    
    if (child_pid == -1) {
        /* Fork failed */
        perror("fork");
        return EXIT_FAILURE;
    }
    
    if (child_pid == 0) {
        /* 
         * Child process code
         * 
         * In this example, the child will:
         * 1. Close the write end of the pipe (we'll only read)
         * 2. Read data from the pipe
         * 3. Process the data (in this case, just print it)
         */
        
        printf("Child: Process started (PID: %d)\n", getpid());
        
        /* 
         * Close the write end of the pipe
         * This is important for proper pipe operation and prevents
         * the child from reading its own writes.
         */
        close(pipe_fd[1]);
        printf("Child: Closed write end of pipe\n");
        
        /* Read from the pipe */
        printf("Child: Waiting to read from pipe...\n");
        
        /*
         * read() will block until data is available or the write end
         * of the pipe is closed by all processes.
         */
        ssize_t bytes_read = read(pipe_fd[0], buffer, BUFFER_SIZE - 1);
        
        if (bytes_read == -1) {
            perror("read");
            close(pipe_fd[0]);
            return EXIT_FAILURE;
        }
        
        /* Null-terminate the buffer and print the message */
        buffer[bytes_read] = '\0';
        printf("Child: Received message: \"%s\"\n", buffer);
        
        /* Read a second message to demonstrate multiple reads */
        printf("Child: Waiting for another message...\n");
        bytes_read = read(pipe_fd[0], buffer, BUFFER_SIZE - 1);
        
        if (bytes_read == -1) {
            perror("read");
            close(pipe_fd[0]);
            return EXIT_FAILURE;
        }
        buffer[bytes_read] = '\0';
        printf("Child: Received second message: \"%s\"\n", buffer);
        
        /* Close the read end when we're done */
        close(pipe_fd[0]);
        printf("Child: Closed read end, exiting.\n");
        
        return EXIT_SUCCESS;
    }
    else {
        /* 
         * Parent process code
         * 
         * In this example, the parent will:
         * 1. Close the read end of the pipe (we'll only write)
         * 2. Write data to the pipe
         * 3. Wait for the child process to complete
         */
        
        printf("Parent: Child process created with PID: %d\n", child_pid);
        
        /* Close the read end of the pipe */
        close(pipe_fd[0]);
        printf("Parent: Closed read end of pipe\n");
        
        /* Prepare the message to send */
        const char *message1 = "Hello from parent process!";
        printf("Parent: Sending message: \"%s\"\n", message1);
        
        /* 
         * Write the message to the pipe
         * write() may block if the pipe buffer is full
         */
        ssize_t bytes_written = write(pipe_fd[1], message1, strlen(message1));
        
        if (bytes_written == -1) {
            perror("write");
            close(pipe_fd[1]);
            return EXIT_FAILURE;
        }
        
        /* Give the child some time to process the first message */
        sleep(1);
        
        /* Send a second message */
        const char *message2 = "This is the second message!";
        printf("Parent: Sending second message: \"%s\"\n", message2);
        
        bytes_written = write(pipe_fd[1], message2, strlen(message2));
        
        if (bytes_written == -1) {
            perror("write");
            close(pipe_fd[1]);
            return EXIT_FAILURE;
        }
        
        /* 
         * Close the write end of the pipe when we're done
         * This is important to signal the read end that no more data is coming
         */
        close(pipe_fd[1]);
        printf("Parent: Closed write end of pipe\n");
        
        /* Wait for the child to exit */
        printf("Parent: Waiting for child to complete...\n");
        
        int status;
        waitpid(child_pid, &status, 0);
        
        if (WIFEXITED(status)) {
            printf("Parent: Child exited with status: %d\n", WEXITSTATUS(status));
        } else {
            printf("Parent: Child terminated abnormally\n");
        }
        
        printf("Parent: Pipe demonstration complete.\n");
    }
    
    return EXIT_SUCCESS;
}

/**
 * EXPLANATION:
 * 
 * Pipes - Unidirectional IPC:
 * 
 * 1. What Are Pipes?
 *    - Unidirectional communication channels
 *    - Provide a way for processes to exchange data
 *    - Primarily used between related processes (e.g., parent-child)
 *    - Created by the pipe() system call
 *    - One of the oldest and simplest IPC mechanisms in Unix
 * 
 * 2. Pipe Characteristics:
 *    - Two file descriptors: one for reading, one for writing
 *    - Data is read in the same order it was written (FIFO)
 *    - Buffered by the operating system (typically 4-64 KB)
 *    - Unidirectional: data flows in only one direction
 *    - Anonymous: only accessible to the creating process and its children
 *    - Temporary: exists only as long as at least one process has it open
 * 
 * 3. Common Pipe Operations:
 *    - pipe(): Create a new pipe
 *    - read(): Read data from the pipe (blocks if pipe is empty)
 *    - write(): Write data to the pipe (blocks if pipe is full)
 *    - close(): Close a pipe file descriptor
 * 
 * 4. Pipe Usage Pattern:
 *    - Create a pipe before forking
 *    - After fork, close unused ends in each process:
 *      * Reader closes the write end
 *      * Writer closes the read end
 *    - Write to/read from the appropriate end
 *    - Close all pipe ends when done
 * 
 * 5. Pipe Behavior:
 *    - read() blocks when the pipe is empty until data is available
 *    - read() returns 0 when all write ends are closed (EOF)
 *    - write() blocks when the pipe buffer is full
 *    - write() fails with SIGPIPE if all read ends are closed
 * 
 * 6. Common Use Cases:
 *    - Parent-child communication
 *    - Shell pipelines (cmd1 | cmd2)
 *    - Simple producer-consumer patterns
 *    - Capture command output (popen())
 * 
 * 7. Limitations:
 *    - Unidirectional: For bidirectional communication, need two pipes
 *    - Related processes only: Can't be used between unrelated processes
 *    - No random access: Only sequential access to data
 *    - No persistent storage: Data is lost when all processes close the pipe
 *    - Limited buffer size: Large transfers may block
 * 
 * 8. Variants and Alternatives:
 *    - Named pipes (FIFOs): Visible in the filesystem, usable by unrelated processes
 *    - socketpair(): Creates a bidirectional pipe-like channel
 *    - POSIX message queues: More structured message-based communication
 *    - Shared memory: Higher performance for large data transfers
 */