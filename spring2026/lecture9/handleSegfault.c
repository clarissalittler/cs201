#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void segfun(int sig){
  const char msg[] = "This segmented too close to the sun\n";
  write(1, msg, sizeof(msg)-1);
  _exit(1);
}

int main(){

  int* boop = NULL;

  signal(SIGSEGV,segfun);
  
  *boop = 50;

  return 0;
}
