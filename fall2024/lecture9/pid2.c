#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){

  pid_t pid = fork();

  if(pid < 0){
    perror("Fork failed");
    return 1;
  }
  else if(pid == 0){
    printf("This is the child process.\n");
  }  
  else{
    wait(0);
    printf("This is the parent process.\n");
  }
  printf("This message is printed by both the parent and the child\n");
  return 0;
}
