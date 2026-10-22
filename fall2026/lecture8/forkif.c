#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(){

  pid_t pid = fork();

  if(pid == 0){
    // hrmm need a way to validate that I remembered correctly
    printf("Hello from the child! My pid is: %d and my parent's is %d\n",
	   getpid(),getppid());
  }
  else{
    printf("Hello from the parent! My child's pid is %d\n",pid);
  }
  
  return 0;
}
