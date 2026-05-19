// we're doing this with syscalls
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]){

  // open the "first" argument (i.e. argv[1])
  // as a file and then close it

  if(argc < 2){
    // no file, no point
    return 1;
  }
  
  // open returns
  int fd = open(argv[1], O_RDONLY);
  if(fd < 0){ // this means there was an error
    printf("Something went horribly wrong!\n");
  }
  else{
    printf("We opened the file at fd %d\n",fd);
    close(fd);
  }
  return 0;
}
