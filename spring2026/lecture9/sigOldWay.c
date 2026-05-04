#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

// the "old way" of installing a signal handler: signal()
// the API is dead simple --
//   signal(SIGUSR1, myHandler);
// and now whenever this process gets a SIGUSR1, myHandler runs.
//
// problems with this API:
//   1. on some old systems the handler gets reset to default after the first
//      signal, so you have to re-install it every time you handle one
//   2. there's no way to say which signals should be blocked WHILE the handler runs
//      so a second SIGUSR1 mid-handler can re-enter you
//   3. there's no way to ask for SA_RESTART (auto-restart syscalls interrupted
//      by the signal), so a syscall like read() in main() may abort with EINTR
//      every time a signal arrives
//
// it's fine for quick demos; the modern way (next file) is what real code uses

volatile sig_atomic_t hits = 0;

void handler(int sig){
  (void)sig;
  hits++;
  // again -- printf is not async-signal-safe, so write() it is
  const char* msg = "got SIGUSR1\n";
  write(1, msg, strlen(msg));
}

int main(){
  signal(SIGUSR1, handler);

  printf("I'm Mr. %d -- send me SIGUSR1 with: kill -SIGUSR1 %d\n",
         getpid(), getpid());

  while(hits < 5){
    pause(); // sleep until any signal arrives
  }

  printf("got hit %d times -- exiting\n", hits);
  return 0;
}
