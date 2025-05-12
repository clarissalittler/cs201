#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){

  pid_t pid = fork();
  int returned;

  if(pid == 0){
    // in the child process
    int blah;
    int results;
    printf("Say somethin', will ya: ");
    results = scanf("%d",&blah);
    if(results < 1){
      return 1;
    }
    else {
      return 0;
    }
  }
  else {
    // in the parent process
    wait(&returned);
  }
  printf("This was returned: %d\n",WEXITSTATUS(returned) );
  
  return 0;
}
