#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handler(int sig){
  printf("Mwhahaha\n");
}

int main(){

  signal(SIGTSTP,handler);
  
  while(1){
    printf("Here I am!: %d\n",getpid());
    sleep(1);
  }

  return 0;
}
