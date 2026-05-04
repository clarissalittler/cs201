#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

// the "modern way" of installing a handler: sigaction()
// it's wordier than signal() but you actually get to control the behavior
//
// the struct sigaction has three pieces we care about:
//   sa_handler    the function to run, just like signal()
//   sa_mask       a set of signals to block WHILE this handler is running
//                 (so we don't get re-entered mid-handler)
//   sa_flags      modifiers; the most common is SA_RESTART, which asks the
//                 kernel to auto-resume any syscalls interrupted by this signal
//
// also: this is where we usually remind everyone that signal handlers run
// in a kind of "borrowed" context -- you can't safely call most of libc from
// inside one. The list of *async-signal-safe* functions is short:
// write(), _exit(), kill(), the simple getpid()/getppid()-style queries, and
// not much else. printf() is NOT on that list, so we use write() and
// pre-formatted strings.

volatile sig_atomic_t hits = 0;

void handler(int sig){
  (void)sig;
  hits++;
  const char* msg = "got SIGUSR1 (modern handler)\n";
  write(1, msg, strlen(msg));
}

int main(){
  struct sigaction sa;
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);          // don't block any extra signals
  sa.sa_flags   = SA_RESTART;        // auto-restart interrupted syscalls

  if(sigaction(SIGUSR1, &sa, NULL) == -1){
    perror("sigaction");
    return 1;
  }

  printf("I'm Mr. %d -- send me SIGUSR1 with: kill -SIGUSR1 %d\n",
         getpid(), getpid());

  while(hits < 5){
    pause();
  }

  printf("got hit %d times -- exiting\n", hits);
  return 0;
}
