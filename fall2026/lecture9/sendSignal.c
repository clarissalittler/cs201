#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

// the "kill" command is just a thin wrapper around the kill() syscall
// from inside C, you call kill(pid, sig) directly
// this little program is a hand-rolled "kill" command:
//   ./sendSignal 12345 2
// will send signal #2 (SIGINT) to pid 12345
// useful for demoing: run cantStopMe.c in one terminal, sendSignal it from another

int main(int argc, char** argv){
  if(argc != 3){
    fprintf(stderr, "usage: %s <pid> <sig>\n", argv[0]);
    return 1;
  }

  int pid = atoi(argv[1]);
  int sig = atoi(argv[2]);

  if(kill(pid, sig) == -1){
    perror("kill");
    return 1;
  }

  printf("delivered signal %d to pid %d\n", sig, pid);
  return 0;
}
