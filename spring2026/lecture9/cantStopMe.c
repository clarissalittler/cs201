#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

// "killing doesn't always make something dead"
// here we install a handler for SIGINT (the Ctrl-C signal)
// instead of dying when the user hits Ctrl-C, the program prints a sass message
// and keeps running
//
// the only signals you cannot intercept this way are SIGKILL and SIGSTOP
// -- those are guaranteed to work, which is why "kill -9" is the nuclear option

void ouch(int sig){
  (void)sig;
  // printf is technically not async-signal-safe so we use write() instead
  // we'll explain *why* when we get to the "modern way" of writing handlers
  const char* msg = "\nOuch! That tickles. I'm not going anywhere.\n";
  write(1, msg, strlen(msg));
}

int main(){
  signal(SIGINT, ouch);  // the "old way" of installing a handler

  while(1){
    printf("I'm Mr. %d and I cannot be stopped!\n", getpid());
    sleep(1);
  }
  return 0;
}
