// we're doing this with syscalls
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]){

  int* fds = malloc(argc * sizeof(int));
  
  for(int i=1; i < argc; i++){
    // note: we deliberately open argv[1] every time, not argv[i]!
    // the point is to see that each open() of the *same* file
    // gets a fresh, increasing file descriptor number
    int fd = open(argv[1], O_RDONLY);
    if(fd < 0){ // this means there was an error
      printf("Something went horribly wrong!\n");
    }
    else{
      printf("We opened the file at fd %d\n",fd);
    }
    fds[i] = fd;
  }

  for(int i=1; i<argc; i++){
    if(fds[i] >= 0){
      close(fds[i]);
    }
  }

  free(fds);
  
  return 0;
}
