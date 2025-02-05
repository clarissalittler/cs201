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
    printf("Here's the int returned by childReturn: %d\n",(childReturn >> 8) & 255);
    return 0;
  }

  
  return 0;
}
