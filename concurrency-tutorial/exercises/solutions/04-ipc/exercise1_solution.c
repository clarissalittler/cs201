/*
 * Exercise 1 Solution: Parent-Child Pipe Communication
 *
 * This program demonstrates bidirectional communication between parent
 * and child processes using two anonymous pipes.
 *
 * Key concepts:
 * - Creating pipes with pipe()
 * - Forking processes
 * - Closing unused pipe ends to prevent deadlocks
 * - Bidirectional communication
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    // Create two pipes for bidirectional communication
    // pipe_p2c: parent writes to [1], child reads from [0]
    // pipe_c2p: child writes to [1], parent reads from [0]
    int pipe_p2c[2];  // Parent to child
    int pipe_c2p[2];  // Child to parent

    // Create the first pipe (parent to child)
    if (pipe(pipe_p2c) == -1) {
        perror("pipe_p2c failed");
        return 1;
    }

    // Create the second pipe (child to parent)
    if (pipe(pipe_c2p) == -1) {
        perror("pipe_c2p failed");
        close(pipe_p2c[0]);
        close(pipe_p2c[1]);
        return 1;
    }

    // Fork a child process
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("fork failed");
        close(pipe_p2c[0]);
        close(pipe_p2c[1]);
        close(pipe_c2p[0]);
        close(pipe_c2p[1]);
        return 1;
    }
    else if (pid == 0) {
        // CHILD PROCESS

        // Close unused ends of pipes
        // Child reads from parent, so close parent->child write end
        close(pipe_p2c[1]);
        // Child writes to parent, so close child->parent read end
        close(pipe_c2p[0]);

        int number;
        printf("Child (PID %d): Ready to receive numbers\n", getpid());

        // Read numbers from parent, calculate squares, send back
        for (int i = 0; i < 5; i++) {
            // Read a number from parent
            if (read(pipe_p2c[0], &number, sizeof(int)) <= 0) {
                fprintf(stderr, "Child: Failed to read from parent\n");
                break;
            }

            printf("Child: Received %d from parent\n", number);

            // Calculate the square
            int square = number * number;

            // Send the square back to parent
            if (write(pipe_c2p[1], &square, sizeof(int)) <= 0) {
                fprintf(stderr, "Child: Failed to write to parent\n");
                break;
            }

            printf("Child: Sent %d^2 = %d to parent\n", number, square);
        }

        // Close remaining pipe ends
        close(pipe_p2c[0]);
        close(pipe_c2p[1]);

        printf("Child: Finished, exiting\n");
        exit(0);
    }
    else {
        // PARENT PROCESS

        // Close unused ends of pipes
        // Parent writes to child, so close parent->child read end
        close(pipe_p2c[0]);
        // Parent reads from child, so close child->parent write end
        close(pipe_c2p[1]);

        printf("Parent (PID %d): Starting communication with child (PID %d)\n",
               getpid(), pid);

        // Send numbers 1-5 to child and receive squared results
        for (int i = 1; i <= 5; i++) {
            // Send number to child
            if (write(pipe_p2c[1], &i, sizeof(int)) <= 0) {
                fprintf(stderr, "Parent: Failed to write to child\n");
                break;
            }

            printf("Parent: Sent %d to child\n", i);

            // Read squared result from child
            int result;
            if (read(pipe_c2p[0], &result, sizeof(int)) <= 0) {
                fprintf(stderr, "Parent: Failed to read from child\n");
                break;
            }

            printf("Parent: Received result: %d^2 = %d\n", i, result);
        }

        // Close remaining pipe ends
        close(pipe_p2c[1]);
        close(pipe_c2p[0]);

        // Wait for child to finish
        int status;
        waitpid(pid, &status, 0);
        printf("Parent: Child exited with status %d\n", WEXITSTATUS(status));
    }

    return 0;
}
