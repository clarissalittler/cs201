/**
 * ============================================================================
 * Named Pipes (FIFOs) - Communication Between Unrelated Processes
 * ============================================================================
 * 
 * This example demonstrates using named pipes (FIFOs) for inter-process 
 * communication between unrelated processes. Named pipes appear as files in 
 * the filesystem but act as pipes.
 * 
 * Key concepts demonstrated:
 * - Creating named pipes with mkfifo()
 * - Opening named pipes for reading and writing
 * - Communicating between unrelated processes
 * - Cleanup of named pipes
 * 
 * This file contains both reader and writer functionality that can be selected
 * at runtime to demonstrate communication between separate processes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define FIFO_PATH "/tmp/example_fifo"
#define BUFFER_SIZE 100

/* Function prototypes */
void run_writer();
void run_reader();
void usage(const char* program_name);

int main(int argc, char* argv[]) {
    /* Check command-line arguments */
    if (argc != 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    printf("Named Pipes (FIFO) IPC Demonstration\n\n");
    
    /* Run in reader or writer mode based on command-line argument */
    if (strcmp(argv[1], "reader") == 0) {
        run_reader();
    } else if (strcmp(argv[1], "writer") == 0) {
        run_writer();
    } else {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

/**
 * Run in writer mode: Create the named pipe and write messages to it.
 */
void run_writer() {
    int fd;
    char buffer[BUFFER_SIZE];
    
    printf("Writer: Starting up...\n");
    
    /* 
     * Create the named pipe (FIFO)
     * - Path: Where to create the FIFO in the filesystem
     * - Mode: Permissions (0666 = read/write for all users)
     * 
     * If the FIFO already exists, this will fail with EEXIST,
     * which we can safely ignore.
     */
    if (mkfifo(FIFO_PATH, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
    
    printf("Writer: FIFO created at %s\n", FIFO_PATH);
    printf("Writer: Opening FIFO for writing...\n");
    
    /* 
     * Open the FIFO for writing
     * Using O_WRONLY | O_NONBLOCK to avoid blocking indefinitely if no reader.
     * We'll check for ENXIO error and handle it by waiting for a reader.
     */
    fd = open(FIFO_PATH, O_WRONLY | O_NONBLOCK);
    
    if (fd == -1 && errno == ENXIO) {
        printf("Writer: No reader available yet. Waiting for reader...\n");
        
        /* 
         * No reader available yet. Re-open in blocking mode, which will
         * wait until a reader opens the FIFO.
         */
        fd = open(FIFO_PATH, O_WRONLY);
    }
    
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    printf("Writer: FIFO opened successfully.\n");
    
    /* Send multiple messages to demonstrate continuous communication */
    for (int i = 1; i <= 5; i++) {
        /* Prepare a message to send */
        snprintf(buffer, BUFFER_SIZE, "Message #%d from writer process (PID: %d)", 
                 i, getpid());
        
        printf("Writer: Sending message: \"%s\"\n", buffer);
        
        /* Write the message to the FIFO */
        ssize_t bytes_written = write(fd, buffer, strlen(buffer) + 1);
        
        if (bytes_written == -1) {
            perror("write");
            close(fd);
            exit(EXIT_FAILURE);
        }
        
        /* Pause between messages to make the demo easier to follow */
        sleep(1);
    }
    
    /* Close the FIFO when we're done */
    close(fd);
    printf("Writer: Closed FIFO. Exiting.\n");
    
    /* 
     * Note: We don't unlink (delete) the FIFO here to allow the reader
     * to access it even if it starts after the writer exits.
     */
}

/**
 * Run in reader mode: Open the named pipe and read messages from it.
 */
void run_reader() {
    int fd;
    char buffer[BUFFER_SIZE];
    
    printf("Reader: Starting up...\n");
    printf("Reader: Waiting for FIFO at %s...\n", FIFO_PATH);
    
    /* 
     * Open the FIFO for reading
     * This will block until a writer opens the FIFO
     */
    fd = open(FIFO_PATH, O_RDONLY);
    
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    printf("Reader: FIFO opened successfully.\n");
    printf("Reader: Waiting for messages...\n");
    
    /*
     * Read messages in a loop
     * In a real application, you might break this loop based on
     * a special "end" message or signal.
     */
    int message_count = 0;
    ssize_t bytes_read;
    
    /* Read messages until EOF (writer closes FIFO) or error */
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';  /* Null-terminate the string */
        message_count++;
        
        printf("Reader: Received message %d: \"%s\"\n", message_count, buffer);
    }
    
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    /* We reached EOF (writer closed the FIFO) */
    printf("Reader: Writer closed the FIFO. Received %d messages in total.\n", 
           message_count);
    
    /* Close the FIFO */
    close(fd);
    
    /*
     * Clean up: remove the FIFO from the filesystem
     * This is typically done by the last process to use the FIFO
     */
    if (unlink(FIFO_PATH) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }
    
    printf("Reader: Cleaned up FIFO. Exiting.\n");
}

/**
 * Display usage information for the program.
 */
void usage(const char* program_name) {
    fprintf(stderr, "Usage: %s <mode>\n", program_name);
    fprintf(stderr, "  mode: 'reader' or 'writer'\n");
    fprintf(stderr, "\nTo demonstrate FIFO communication:\n");
    fprintf(stderr, "  1. Run in writer mode in one terminal: %s writer\n", program_name);
    fprintf(stderr, "  2. Run in reader mode in another terminal: %s reader\n", program_name);
}

/**
 * EXPLANATION:
 * 
 * Named Pipes (FIFOs) - Inter-Process Communication:
 * 
 * 1. What Are Named Pipes?
 *    - Special files in the filesystem that behave like pipes
 *    - Allow communication between unrelated processes
 *    - Persist in the filesystem until explicitly removed
 *    - Unidirectional: data flows in only one direction
 *    - Identified by a pathname, visible with 'ls'
 * 
 * 2. Named Pipes vs. Anonymous Pipes:
 *    - Named pipes appear in the filesystem (anonymous pipes don't)
 *    - Named pipes can be used by unrelated processes
 *    - Named pipes persist until explicitly removed
 *    - Named pipes can be opened and closed multiple times
 *    - Basic operation is similar (read/write semantics)
 * 
 * 3. Creating and Using Named Pipes:
 *    - mkfifo(): Creates a new named pipe
 *    - open(): Opens a named pipe for reading or writing
 *    - read()/write(): Read from or write to the pipe
 *    - close(): Close the pipe file descriptor
 *    - unlink(): Remove the pipe from the filesystem
 * 
 * 4. Key Behaviors:
 *    - Opening for reading blocks until a writer opens it
 *    - Opening for writing blocks until a reader opens it
 *    - When all writers close, readers receive EOF (read returns 0)
 *    - Writing to a pipe with no readers raises the SIGPIPE signal
 *    - Data is transferred between processes in kernel memory
 * 
 * 5. Handling Blocking Behavior:
 *    - Use O_NONBLOCK flag with open() to avoid blocking
 *    - Check for ENXIO error when opening for writing in non-blocking mode
 *    - Use select()/poll()/epoll() for monitoring multiple pipes
 * 
 * 6. Common Use Cases:
 *    - Client-server communication
 *    - Command pipelines with multiple instances of a program
 *    - Logger/monitoring services
 *    - Simple request-response patterns
 * 
 * 7. Limitations:
 *    - Unidirectional: Need two FIFOs for bidirectional communication
 *    - No random access: Only sequential access to data
 *    - Limited buffer size: Large transfers may block
 *    - No built-in message boundaries: Need to implement your own protocol
 * 
 * 8. Running This Example:
 *    - Execute in writer mode in one terminal
 *    - Execute in reader mode in another terminal
 *    - Observe messages being passed between processes
 *    - Named pipe persists until the reader removes it with unlink()
 */