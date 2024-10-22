#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

int main() {
    int pipefd[2];        // pipefd[0] is for reading, pipefd[1] is for writing
    pid_t pid;
    char write_msg[] = "Hello from the child process!";
    char read_msg[100];

    // Create a pipe
    if (pipe(pipefd) == -1) {
        perror("Pipe failed");
        return 1;
    }

    // Create a new process
    pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        return 1;
    }
    else if (pid == 0) {
        // This is the child process
        close(pipefd[0]); // Close the read end of the pipe
        write(pipefd[1], write_msg, strlen(write_msg) + 1); // Send message to parent
        close(pipefd[1]); // Close the write end of the pipe after writing
    }
    else {
        // This is the parent process
        close(pipefd[1]); // Close the write end of the pipe
        read(pipefd[0], read_msg, sizeof(read_msg)); // Read the message from the child
        printf("Parent received: %s\n", read_msg);
        close(pipefd[0]); // Close the read end of the pipe after reading
    }

    return 0;
}
