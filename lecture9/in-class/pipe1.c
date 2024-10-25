#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

// We want a program that will fork
// a process, read a number from stdin
// via scanf and then if the scanf was
// successful, send that number to the
// parent

int main(){

  int pipeFileDesc[2]; // this is going to hold
  // the file descriptors for the pipe
  int returned;
  int output;

  pipe(pipeFileDesc);
  // after this point pipeFileDesc[0] should be the read end
  // pipeFileDesc[1] should be write end
  
  pid_t pid = fork();

  if( pid == 0){
    // in the child process
    int blah;
    int results;
    printf("Say somethin', will ya: ");
    results = scanf("%d",&blah);
    if(results < 1){
      //scanf failed
      return 1;
    }
    else{
      //scanf succeded
      char buf[10];
      sprintf(buf,"%d",blah);
      write(pipeFileDesc[1],buf,strlen(buf)+1);
      close(pipeFileDesc[1]);
      close(pipeFileDesc[0]);
      return 0;
    }
  }
  else {
    // in the parent process
    wait(&returned);
    if(WEXITSTATUS(returned) == 1){
      printf("I guess there was nothing to read\n");
      close(pipeFileDesc[1]);
      close(pipeFileDesc[0]);
    }
    else{
      char outBuf[10];
      read(pipeFileDesc[0],outBuf,10);

      printf("I got the number: %d\n",atoi(outBuf));
    }
  }

  return 0;
}
