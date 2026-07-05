#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){

  printf("We haven't forked yet!\n");
  pid_t pid = fork();
  if(pid != 0){
    printf("I'm the parent!\n");
  }
  else{
    printf("I'm the child!\n");
  }
  
  printf("The value of pid is: %d\n",pid);
  printf("My name is: %d\n",getpid());
  
  return 0;
}
