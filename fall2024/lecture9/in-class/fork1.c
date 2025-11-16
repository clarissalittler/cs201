#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/*
  Write a simple program that will fork to make a new process
 */

int main(){

  fork();

  printf("This message should be printed by parent and child\n");
  
  return 0;
}
