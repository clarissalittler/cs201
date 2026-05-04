#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// re-visiting fork() now that we know about signals
// recall: after fork() the child gets a *copy* of the parent's signal
// handlers. So if you install a handler before forking, both processes
// share the same handler logic.
//
// the typical pattern for a parent talking to a child:
//   1. parent installs handlers for the signals it wants to receive
//   2. parent forks
//   3. parent and child each know each other's pid (getpid / getppid)
//   4. they kill() each other to communicate
//
// this version is friendly: parent pings child, child replies, that's it.

volatile sig_atomic_t pinged = 0;

void onPing(int sig){
  (void)sig;
  pinged = 1;
  const char* msg = "pong!\n";
  write(1, msg, strlen(msg));
}

int main(){
  struct sigaction sa;
  sa.sa_handler = onPing;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sigaction(SIGUSR1, &sa, NULL);

  pid_t pid = fork();

  if(pid == 0){
    // child: wait for a SIGUSR1 from the parent, then reply with one of our own
    while(!pinged){
      pause();
    }
    kill(getppid(), SIGUSR1);
    return 0;
  }

  // parent: give the child a moment to install its handler, then ping it
  sleep(1);
  printf("parent %d pinging child %d\n", getpid(), pid);
  kill(pid, SIGUSR1);

  // wait for the child's reply
  while(!pinged){
    pause();
  }
  wait(NULL);
  printf("parent done\n");
  return 0;
}
