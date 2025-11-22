#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char* argv[]){

  printf("stdin: %d\n",STDIN_FILENO);
  printf("stdout: %d\n",STDOUT_FILENO);
  printf("stderr: %d\n",STDERR_FILENO);

  int* fds = malloc(argc * sizeof(int));
  
  for(int i=0; i < argc; i++){
    fds[i] = open(argv[i],O_RDONLY);
    if(fds[i] >= 0){
      printf("Hey we opened the file %s and its file descriptor is: %d\n",argv[i],fds[i]);
    }
    else {
      printf("We couldn't open the file\n");
    }
  }
  for(int i=0; i< argc; i++){
    close(fds[i]);
  }
    
  return 0;
}
