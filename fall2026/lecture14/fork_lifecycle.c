#define _POSIX_C_SOURCE 200809L
#include <errno.h>    /* errno, EINTR */
#include <stdio.h>    /* printf, fflush, perror */
#include <sys/wait.h> /* waitpid and status macros */
#include <unistd.h>   /* fork, execl, _exit */

int main(int argc, char **argv) {
    (void)argv;
    int value = 10;
    printf("Before fork: value=%d\n", value);
    if (fflush(stdout) == EOF) {
        perror("fflush");
        return 1;
    }
    pid_t child = fork();
    if (child == -1) {
        perror("fork");
        return 1;
    }
    if (child == 0) {
        value = 99;
        printf("Child copy: value=%d\n", value);
        if (fflush(stdout) == EOF) {
            _exit(1);
        }
        /* Any argument requests the deliberately failing exec branch. */
        const char *program = argc > 1 ? "/no-such-cs201-program" : "/bin/echo";
        execl(program, program, "The child now runs another program", (char *)NULL);
        perror("execl"); /* successful exec never returns */
        _exit(127);
    }
    int status;
    while (waitpid(child, &status, 0) == -1) {
        if (errno != EINTR) {
            perror("waitpid");
            return 1;
        }
    }
    printf("Parent copy after wait: value=%d\n", value);
    if (WIFEXITED(status)) {
        printf("Child exit status=%d\n", WEXITSTATUS(status));
        return WEXITSTATUS(status) == 0 ? 0 : 1;
    }
    if (WIFSIGNALED(status)) {
        printf("Child terminated by signal %d\n", WTERMSIG(status));
    }
    return 1;
}
