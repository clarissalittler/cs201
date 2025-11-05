#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "unistd.h"
#include "string.h"

void sigh_andler(int signum){
  if(signum == SIGUSR1){
    char msg[] = "I guess we're done?\n"; 
    write(1,msg,strlen(msg));
    exit(0); // oh yeah we hadn't seen this yet, huh
  }
  else if(signum == SIGUSR2){
    char msg2[] = "Did you say something?\n";
    write(1,msg2,strlen(msg2));
  }
}

int main(){

  signal(SIGUSR1,sigh_andler);
  signal(SIGUSR2,sigh_andler);
  
  while(1){
    printf("I'm Mr. %d and I'm just minding my own business...\n",getpid());
    sleep(1);
  }
  
  return 0;
}
