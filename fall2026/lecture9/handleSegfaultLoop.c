#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void segfun(int sig){
  const char msg[] = "Stop that!\n";
  write(1, msg, sizeof(msg)-1);
}

int main(){
  int* boop = NULL;
  signal(SIGSEGV, segfun);
  *boop = 50;

  return 0;
}
