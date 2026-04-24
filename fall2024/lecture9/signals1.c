#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

volatile sig_atomic_t counter = 0;

void sigint_handler(int sig){
  (void)sig;
  if(counter < 3){
    counter++;
  }
}

int main(){
  sig_atomic_t reported = 0;

  printf("We have a ctrl-c handler here!\n");
  signal(SIGINT, sigint_handler);
  while(true){
    while(reported < counter){
      reported++;
      printf("Caught a sigint: Press ctrl-c %d more times to exit\n",3-reported);
    }
    if(counter >= 3){
      printf("I've been banished!\n");
      break;
    }
    printf("Boop boop\n");
    sleep(1);
  }
  
  return 0;
}
