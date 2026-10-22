#define _POSIX_C_SOURCE 200809L
#include <signal.h>  /* sigaction, sigprocmask, sigsuspend */
#include <stdio.h>   /* printf, perror, fflush */
#include <unistd.h>  /* getpid */

static volatile sig_atomic_t received = 0;

static void handle_signal(int signal_number) {
    (void)signal_number;
    received = 1;
}

int main(void) {
    sigset_t blocked, previous;
    sigemptyset(&blocked);
    sigaddset(&blocked, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &blocked, &previous) == -1) {
        perror("sigprocmask");
        return 1;
    }
    struct sigaction action = {0};
    action.sa_handler = handle_signal;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("sigaction");
        return 1;
    }
    sigset_t waiting = previous;
    sigdelset(&waiting, SIGUSR1);
    printf("Send: kill -USR1 %ld\n", (long)getpid());
    fflush(stdout);
    while (!received) {
        /* Temporarily unblock SIGUSR1 and wait atomically. Returning
           restores the blocked mask before we inspect the flag again. */
        sigsuspend(&waiting);
    }
    if (sigprocmask(SIG_SETMASK, &previous, NULL) == -1) {
        perror("sigprocmask restore");
        return 1;
    }
    puts("Main noticed the flag; the handler did no printing.");
    return 0;
}
