#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void handler(int sig){
  printf("Caught that sigint!\n");
  exit(0);
}

int main(){

  printf("We have a ctrl-c handler here! I.E. SIGINT (%d)\n",SIGINT);
  signal(SIGINT, handler);
  while(1){
    printf("Beep boop\n");
    sleep(1);
  }

  return 0;
}
