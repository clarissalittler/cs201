#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

int main(){

  sigset_t blocks, prevs;
  sigemptyset(&blocks);
  sigaddset(&blocks,SIGINT);
  sigprocmask(SIG_BLOCK,&blocks,&prevs);
  
  while(true){
    printf("I bet you can't KILL me, mr. %u\n",getpid());
    sleep(1);
  }
  
  return 0;
}
