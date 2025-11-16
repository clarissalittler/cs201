#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

volatile int counter = 0;

void sigint_handler(int sig){
  counter++;
  printf("Caught a sigint: Press ctrl-c %d more times to exit\n",3-counter);
  if(counter >=3){
    printf("I've been banished!\n");
    exit(0);
  }
}

int main(){

  printf("We have a ctrl-c handler here!\n");
  signal(SIGINT, sigint_handler);
  while(true){
    printf("Boop boop\n");
    sleep(1);
  }
  
  return 0;
}
