#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(){
  int fds[2];
  pipe(fds);

  pid_t pid = fork();
  if(pid == 0){
    // child reads in a loop until the parent closes the write end
    close(fds[1]);
    char buf[128];
    ssize_t n;
    while((n = read(fds[0], buf, sizeof(buf) - 1)) > 0){
      buf[n] = 0;
      printf("\nchild echo: %s\n", buf);
    }
    close(fds[0]);
  } else {
    close(fds[0]);
    char msg[100];
    while(1){
      printf("What am I saying to the child? (type quit to stop): ");
      scanf("%s", msg);
      if(strcmp(msg, "quit") == 0) break;
      write(fds[1], msg, strlen(msg));
    }
    close(fds[1]);
    wait(NULL);
  }
  return 0;
}
