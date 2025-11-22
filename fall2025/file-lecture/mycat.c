#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]){
  int numFiles = argc - 1;
  
  int* fds = malloc(numFiles * sizeof(int));
  
  for(int i=0; i < numFiles; i++){
    fds[i] = open(argv[i+1],O_RDONLY);
  }

  for(int i=0; i < numFiles; i++){  
  
    char readData[1024];
    int bytesRead = 0;
    while((bytesRead = read(fds[i],readData,1024)) > 0){
      write(STDOUT_FILENO,readData,bytesRead);
    }
  }
  for(int i=0; i< argc; i++){
    close(fds[i]);
  }
  
  return 0;
  
}
