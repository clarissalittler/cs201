#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]){

  int* fds = malloc(argc * sizeof(int));
  
  for(int i=0; i < argc; i++){
    fds[i] = open(argv[i],O_RDONLY);
  }

  char readData[10];
  int bytesRead = 0;
  while((bytesRead = read(fds[1],readData,10)) > 0){
    printf("We read in %d out of the file %s\n", bytesRead, argv[1]);
  }
  
  for(int i=0; i< argc; i++){
    close(fds[i]);
  }
  
  return 0;
}
