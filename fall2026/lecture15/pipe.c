#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(){

  int fd[2]; // we need two file-descriptors, one for the read end and one for the write end respectively
  
  pipe(fd); // okay we set up the pipe, but here's the weird thing: a pipe between two processes needs to flow noly one directino

  printf("I have several file handles open: check them at ls -l /proc/%d/fd/\n",getpid());
  char buffy[10];
  scanf("%s",buffy);
  
  pid_t pid = fork(); // we fork it up
  
  if(pid == 0){
    close(fd[1]); // in this program the child reads so we have to close the write end
    char buffer[100];
    ssize_t n = read(fd[0], buffer, sizeof(buffer) -1);
    buffer[n] = '\0';
    printf("The child heard: %s\n", buffer);
    close(fd[0]);
    _exit(0);
  }
  else{
    close(fd[0]);
    printf("I have several file handles open: check them at ls -l /proc/%d/fd/\n",getpid());
    printf("and my kid has some open: check them at ls -l /proc/%d/fd/\n",pid);
    char buffy[10];
    scanf("%s",buffy);
    

    char msg[] = "Hey there kiddo!";
    write(fd[1], msg, strlen(msg));
    close(fd[1]);
    wait(NULL);
  }
  
  return 0;
}
