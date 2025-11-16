#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

void alarm_handler(int sig){
  printf("The bells have been rung!\n");
}

int main(){
  printf("This program will end once the bells have been rung\n");
  signal(SIGALRM, alarm_handler);
  alarm(5);
  printf("We slumber\n");
  pause();
  printf("We have awoken!\n");
  return 0;
}
