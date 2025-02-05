#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){

  pid_t pid = fork();
  int childReturn;
  if(pid < 0){
    perror("Fork failed");
    return 1;
  }
  else if(pid ==0){
    printf("Hey, give me a number, will ya?\n");
    int num;
    int success = scanf("%d",&num);
    if(success){
      return 0;
    }
    else{
      return 1;
    }
  }
  else{
    wait(&childReturn);
    // WEXITSTATUS(int) => (int >> 8) & 255
    if(WEXITSTATUS(childReturn) == 0){
      // everything was cool
      printf("Thanks for being nice to my child!\n");
    }
    else{
      printf("They've massacred my boy!\n");
    }
    return 0;
  }

  
  return 0;
}
