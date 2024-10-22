#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();  // Create a new process

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        return 1;
    }
    else if (pid == 0) {
        // This is the child process
        printf("Hello from the child process! My PID is %d\n", getpid());
    }
    else {
        // This is the parent process
        printf("Hello from the parent process! My child's PID is %d\n", pid);
    }

    // Both processes reach here and print a message
    printf("This message is printed by both the parent and the child.\n");

    return 0;
}
