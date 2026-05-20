// anonymous pipe: a pair of file descriptors with the read end at fds[0]
// and the write end at fds[1]. After fork() both processes have both
// ends, so we close the one we aren't using on each side.
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(){
  int fds[2];
  pipe(fds);

  pid_t pid = fork();
  if(pid == 0){
    // child reads
    close(fds[1]);
    char buf[128];
    ssize_t n = read(fds[0], buf, sizeof(buf) - 1);
    buf[n] = 0;
    printf("child read: %s", buf);
    close(fds[0]);
  } else {
    // parent writes
    close(fds[0]);
    const char* msg = "hello from the parent, down a pipe\n";
    write(fds[1], msg, strlen(msg));
    close(fds[1]); // closing the write end is what gives the child its EOF
    wait(NULL);
  }
  return 0;
}
