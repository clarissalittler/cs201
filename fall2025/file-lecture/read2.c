#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]){

  int* fds = malloc(argc * sizeof(int));
  
  for(int i=0; i < argc; i++){
    fds[i] = open(argv[i],O_RDONLY);
  }

  char readData[1024];
  int bytesRead = read(fds[1],readData,1024);

  printf("We read in %d out of the file %s\n", bytesRead, argv[1]);
  bytesRead = read(fds[1],readData,1024);
  printf("Now we read %d out of the file %s\n", bytesRead, argv[1]);
  
  
  for(int i=0; i< argc; i++){
    close(fds[i]);
  }

  
  return 0;
}
