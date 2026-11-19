#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(){
  int p2c[2]; // this runs from the parent -> child
  int c2p[2]; // this runs from the child -> parent
  int ready = 1;
  pipe(p2c);
  pipe(c2p);

  pid_t pid = fork();
  if(pid == 0){
    // child reads in a loop until the parent closes the write end
    close(p2c[1]);
    close(c2p[0]);
    char buf[128];
    ssize_t n;
    while((n = read(p2c[0], buf, sizeof(buf) - 1)) > 0){
      buf[n] = 0;
      printf("child echo: %s\n", buf);
      write(c2p[1], &ready, sizeof(int));
    }
    close(p2c[0]);
    close(c2p[1]);
  } else {
    close(p2c[0]);
    close(c2p[1]);
    char msg[100];
    while(1){
      printf("Mr. %d: What am I saying to the child? (type quit to stop): ", getpid());
      scanf("%s", msg);
      if(strcmp(msg, "quit") == 0) break;
      write(p2c[1], msg, strlen(msg));
      read(c2p[0],&ready,sizeof(int));
    }
    close(p2c[1]);
    close(c2p[0]);
    wait(NULL);
  }
  return 0;
}
