#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

int echoFile(char* fName, char* outFile){
  int fd = open(fName, O_RDONLY);
  if(fd == -1){
    printf("Something went wrong opening the file!\n");
    return 1;
  }

  int outFd = STDOUT_FILENO;

  if(outFile != NULL){
    outFd = open(outFile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
  }
  
  char buffer[1024];
  ssize_t bytes_read;
  
  bytes_read = read(fd, buffer, sizeof(buffer));

  if(bytes_read < 0){
    printf("Something went wrong reading the file!\n");
    return 1;
  }
  
  while (bytes_read > 0) {
    write(outFd, buffer, bytes_read);
    bytes_read = read(fd, buffer, sizeof(buffer));
  }

  if(outFile != NULL){
    close(outFd);
  }
  
  close(fd);
  return 0;
}

int main(int argc, char* argv[]) {

  // let's assume that if a flag is given it should be
  // -o and thus be the 1th argument and the 2th argument for the filename
  if(strcmp(argv[1],"-o") == 0){
      for(int i = 3; i < argc; i++){
        echoFile(argv[i],argv[2]);
      }
  }
  else {
      for(int i = 1; i < argc; i++){
        echoFile(argv[i],NULL);
      }
  }

  
  return 0;
}
