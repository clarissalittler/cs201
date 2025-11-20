/**
 * ============================================================================
 * Pipes - ROBUST VERSION with Full Error Handling
 * ============================================================================
 *
 * This is the robust version of the pipes example with comprehensive error
 * handling, resource cleanup, and defensive programming practices.
 *
 * Compare with ../05-ipc/01-pipes.c to see the minimal version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define BUFFER_SIZE 100

int main() {
    int pipe_fd[2];
    pid_t child_pid;
    char buffer[BUFFER_SIZE];

    printf("Pipe IPC Demonstration (Robust Version)\n\n");

    /* Create a pipe with error checking */
    if (pipe(pipe_fd) == -1) {
        fprintf(stderr, "ERROR: Failed to create pipe: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("Pipe created successfully.\n");
    printf("Read end (fd=%d), Write end (fd=%d)\n", pipe_fd[0], pipe_fd[1]);

    /* Create a child process with error checking */
    child_pid = fork();

    if (child_pid == -1) {
        fprintf(stderr, "ERROR: Failed to fork: %s\n", strerror(errno));
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return EXIT_FAILURE;
    }

    if (child_pid == 0) {
        /* Child process code */
        printf("Child: Process started (PID: %d)\n", getpid());

        /* Close the write end of the pipe */
        if (close(pipe_fd[1]) == -1) {
            fprintf(stderr, "Child ERROR: Failed to close write end: %s\n",
                    strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("Child: Closed write end of pipe\n");

        /* Read from the pipe with error checking */
        printf("Child: Waiting to read from pipe...\n");

        ssize_t bytes_read = read(pipe_fd[0], buffer, BUFFER_SIZE - 1);

        if (bytes_read == -1) {
            fprintf(stderr, "Child ERROR: Failed to read from pipe: %s\n",
                    strerror(errno));
            close(pipe_fd[0]);
            exit(EXIT_FAILURE);
        }

        if (bytes_read == 0) {
            fprintf(stderr, "Child WARNING: Pipe closed before data received\n");
            close(pipe_fd[0]);
            exit(EXIT_FAILURE);
        }

        /* Null-terminate and print the message */
        buffer[bytes_read] = '\0';
        printf("Child: Received message: \"%s\"\n", buffer);

        /* Read a second message */
        printf("Child: Waiting for another message...\n");
        bytes_read = read(pipe_fd[0], buffer, BUFFER_SIZE - 1);

        if (bytes_read == -1) {
            fprintf(stderr, "Child ERROR: Failed to read second message: %s\n",
                    strerror(errno));
            close(pipe_fd[0]);
            exit(EXIT_FAILURE);
        }

        if (bytes_read == 0) {
            printf("Child: Pipe closed, no more messages\n");
        } else {
            buffer[bytes_read] = '\0';
            printf("Child: Received second message: \"%s\"\n", buffer);
        }

        /* Close the read end */
        if (close(pipe_fd[0]) == -1) {
            fprintf(stderr, "Child ERROR: Failed to close read end: %s\n",
                    strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("Child: Closed read end, exiting.\n");

        exit(EXIT_SUCCESS);
    }
    else {
        /* Parent process code */
        printf("Parent: Child process created with PID: %d\n", child_pid);

        /* Close the read end of the pipe */
        if (close(pipe_fd[0]) == -1) {
            fprintf(stderr, "Parent ERROR: Failed to close read end: %s\n",
                    strerror(errno));
            close(pipe_fd[1]);
            /* Still try to wait for child */
            waitpid(child_pid, NULL, 0);
            return EXIT_FAILURE;
        }
        printf("Parent: Closed read end of pipe\n");

        /* Prepare and send first message */
        const char *message1 = "Hello from parent process!";
        printf("Parent: Sending message: \"%s\"\n", message1);

        ssize_t bytes_written = write(pipe_fd[1], message1, strlen(message1));

        if (bytes_written == -1) {
            fprintf(stderr, "Parent ERROR: Failed to write to pipe: %s\n",
                    strerror(errno));
            close(pipe_fd[1]);
            waitpid(child_pid, NULL, 0);
            return EXIT_FAILURE;
        }

        if ((size_t)bytes_written != strlen(message1)) {
            fprintf(stderr, "Parent WARNING: Partial write (%zd/%zu bytes)\n",
                    bytes_written, strlen(message1));
        }

        /* Small delay to ensure ordering */
        sleep(1);

        /* Send second message */
        const char *message2 = "This is the second message!";
        printf("Parent: Sending second message: \"%s\"\n", message2);

        bytes_written = write(pipe_fd[1], message2, strlen(message2));

        if (bytes_written == -1) {
            fprintf(stderr, "Parent ERROR: Failed to write second message: %s\n",
                    strerror(errno));
            close(pipe_fd[1]);
            waitpid(child_pid, NULL, 0);
            return EXIT_FAILURE;
        }

        /* Close the write end to signal EOF to child */
        if (close(pipe_fd[1]) == -1) {
            fprintf(stderr, "Parent ERROR: Failed to close write end: %s\n",
                    strerror(errno));
            waitpid(child_pid, NULL, 0);
            return EXIT_FAILURE;
        }
        printf("Parent: Closed write end of pipe\n");

        /* Wait for the child to exit */
        printf("Parent: Waiting for child to complete...\n");

        int status;
        if (waitpid(child_pid, &status, 0) == -1) {
            fprintf(stderr, "Parent ERROR: waitpid failed: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        /* Check how child exited */
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("Parent: Child exited with status: %d\n", exit_code);
            if (exit_code != EXIT_SUCCESS) {
                fprintf(stderr, "Parent WARNING: Child exited with error\n");
                return EXIT_FAILURE;
            }
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "Parent ERROR: Child terminated by signal %d\n",
                    WTERMSIG(status));
            return EXIT_FAILURE;
        } else {
            fprintf(stderr, "Parent ERROR: Child terminated abnormally\n");
            return EXIT_FAILURE;
        }

        printf("Parent: Pipe demonstration complete.\n");
    }

    return EXIT_SUCCESS;
}

/**
 * KEY DIFFERENCES FROM MINIMAL VERSION:
 *
 * 1. Error Checking:
 *    - Every system call is checked for errors
 *    - Descriptive error messages with strerror(errno)
 *    - Different error paths for different failures
 *
 * 2. Resource Cleanup:
 *    - Pipes closed even on error paths
 *    - Child process reaped even if parent encounters errors
 *    - No resource leaks
 *
 * 3. Defensive Programming:
 *    - Check for partial writes
 *    - Check for EOF vs error on reads
 *    - Verify child exit status (signal vs normal exit)
 *
 * 4. Better Error Messages:
 *    - Use strerror() for system error descriptions
 *    - Prefix with process role (Parent/Child)
 *    - Severity levels (ERROR/WARNING)
 *
 * 5. Robustness:
 *    - Handles edge cases (pipe closed early, partial writes)
 *    - Graceful degradation on errors
 *    - Clear indication of what went wrong
 *
 * WHEN TO USE THIS VERSION:
 * - Production code
 * - When reliability is critical
 * - When debugging pipe issues
 * - When learning proper error handling
 *
 * WHEN TO USE MINIMAL VERSION:
 * - Quick prototypes
 * - Educational examples focusing on concepts
 * - Code demonstrations in documentation
 * - When error handling would obscure the main point
 */
