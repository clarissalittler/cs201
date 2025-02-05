#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

/*
  This program will fork and then have the parent send SIGUSR1 to the child and the child will send SIGUSR2 to the parent and we'll handlers for each
 */

volatile sig_atomic_t num = 0;

void handler1(int sig){
  // this will be for the child receiving sigusr1
  printf("I've been tagged by my parent %d times\n",num);
  num++;
}

void handler2(int sig){
  printf("I've been tagged by my child %d times\n",num);
  num++;
}

int main(){

  signal(SIGUSR1,handler1);
  signal(SIGUSR2,handler2);
  int pid = fork();
  
  if(pid < 0){
    return 1;
  }
  else if(pid == 0){
    // child logic
    while(num < 5){
      kill(getppid(),SIGUSR2);
      sleep(1);
    }
    return 0;
  }
  else{
    // parent logic
    while(num < 5){
      kill(pid,SIGUSR1);
      sleep(1);
    }
    kill(pid,SIGTERM);
    wait(0);
  }
  
  return 0;
}
