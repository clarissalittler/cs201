#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "signal.h"

void uninterruptable(int signum){
  write(1,"\nOuch!\n",6);
}

int main(){

  signal(SIGINT, uninterruptable); //install the signal handler
  
  while(1){
    printf("I'm Mr. %d and I cannot be stopped!\n\n",getpid());
    sleep(1);
  }

  return 0;
}
