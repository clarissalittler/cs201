#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(){
  char fname[] = "Agenda.org";
  int fd = open(fname,O_RDONLY);

  printf("stdin: %d\n",STDIN_FILENO);
  printf("stdout: %d\n",STDOUT_FILENO);
  printf("stderr: %d\n",STDERR_FILENO);
  
  if(fd >= 0){
    printf("Hey we opened the file %s and its file descriptor is: %d\n",fname,fd);
  }
  else {
    printf("We couldn't open the file\n");
  }
  
  
  close(fd);
  
  return 0;
}
