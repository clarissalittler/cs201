#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void floatingPointExceptional(int sig){
  char msg[] = "Yeah, don't do that\n";
  write(1, msg,sizeof(msg)-1);
  _exit(0);
}

int main(void){
  signal(SIGFPE, floatingPointExceptional);

  int x =1;
  int y=0;
  int z = x/y;

  return 0;
}
