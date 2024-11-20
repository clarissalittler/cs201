#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

int echoFile(char* fName, int showSize){
  int fd = open(fName, O_RDONLY);
  if(fd == -1){
    printf("Something went wrong opening the file!\n");
    return 1;
  }

  if(showSize){
    struct stat fileStat;
    lstat(fName, &fileStat);
    printf("The file %s is %ld bytes long, here's it's deal:\n",fName, (long) fileStat.st_size);
  }
  
  char buffer[1024];
  ssize_t bytes_read;
  
  bytes_read = read(fd, buffer, sizeof(buffer));

  if(bytes_read < 0){
    printf("Something went wrong reading the file!\n");
    return 1;
  }
  
  while (bytes_read > 0) {
    write(STDOUT_FILENO, buffer, bytes_read);
    bytes_read = read(fd, buffer, sizeof(buffer));
  }
  
  close(fd);
  return 0;
}

int main(int argc, char* argv[]) {

  // let's assume that if a flag is given it should be
  // -s and thus be the 1th argument
  if(strcmp(argv[1],"-s") == 0){
      for(int i = 2; i < argc; i++){
        echoFile(argv[i],1);
      }
  }
  else {
      for(int i = 1; i < argc; i++){
        echoFile(argv[i],0);
      }
  }

  
  return 0;
}
