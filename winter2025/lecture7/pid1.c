#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // need the UNIX standard library
#include <sys/types.h> // the types defined by the operating system
#include <sys/wait.h> // we need the wait command

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
        //getpid() <- returns my PID
        //getppid() <- returns my parents PID
    }
    else {
        // This is the parent process
        printf("Hello from the parent process! My child's PID is %d\n", pid);
    }

    // Both processes reach here and print a message
    printf("This message is printed by both the parent and the child.\n");

    return 0;
}
