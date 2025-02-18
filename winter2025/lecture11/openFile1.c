#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>

int echoFile(char* fName, bool showSize){
  int fd = open(fName, O_RDONLY);

  char buffer[1024] = {0};
  ssize_t bytesRead = 0;

  if(showSize){
    struct stat fileStat;
    lstat(fName, &fileStat);
    printf("The file %s is %ld bytes long and here it is:\n", fName, (long) fileStat.st_size);
  }
  
  bytesRead = read(fd, buffer, sizeof(buffer));

  if(bytesRead < 0){
    printf("Something went wrong (and you should feel bad)\n");
    return 1;
  }

  while(bytesRead > 0){
    write(STDOUT_FILENO, buffer, bytesRead);
    bytesRead = read(fd, buffer, sizeof(buffer));
  }

  close(fd);


}

int main(int argc, char* argv[]){
  // change this so that if you pass in a -s argument
  // then it also prints the size using lstat

  if(strcmp(argv[1],"-s") == 0){
    echoFile(argv[2],true);
  }
  else{
    echoFile(argv[1],false);
  }
  
  return 0;
}
