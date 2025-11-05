#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "unistd.h"
#include "string.h"
#include "sys/types.h"

volatile sig_atomic_t state = 0;

void sigh_andler(int signum){
  if(signum == SIGUSR1){
    state +=1;
    char msg[] = "I'm gonna leave soon\n"; 
    write(1,msg,strlen(msg));
  }
  else if(signum == SIGUSR2){
    state -= 1;
    char msg2[] = "Actually I'm vibing\n";
    write(1,msg2,strlen(msg2));
  }
}

int main(){

  signal(SIGUSR1,sigh_andler);
  signal(SIGUSR2,sigh_andler);
  
  while(state < 5){
    printf("I'm Mr. %d and I'm just minding my own business...\n",getpid());
    sleep(1);
  }
  
  return 0;
}
