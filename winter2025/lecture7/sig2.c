#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

volatile sig_atomic_t num = 3;

void handler(int sig){
  if(num > 0){
    printf("Caught that sigint!\n");
    printf("Explosion in...%d\n",num);
    num--;
  }
  else{
    printf("Boom!\n");
    exit(0);
  }
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
