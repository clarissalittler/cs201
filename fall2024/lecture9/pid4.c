#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){

  pid_t pid = fork();
  int returned;

  
  if(pid < 0){
    perror("Fork failed");
    return 1;
  }
  else if(pid == 0){
    int blah;
    printf("Say somethin', will ya: ");
    returned = scanf("%d",&blah);
    if(returned < 1){
      return 1;
    }
    else{
      return 0;
    }
  }
  else{
    wait(&returned);
  }

  returned = (returned >> 8) && 0xFF;
  if(returned  == 1){
    printf("They massacred my boy!\n");
  }
  else{
    printf("Everything's great, isn't it?\n");
  }
  return 0;
}
