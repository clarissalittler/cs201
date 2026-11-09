// Everything is a file, part 5: a pipe is a file with no name
//
// pipe() gives us two descriptors. Bytes written into fds[1] come out
// of fds[0]. We use the plain old read and write on them -- no new API.
//
// This is exactly what the shell builds when you type   ls | wc -l
//
// try:
//   ./fdPipe
//   then comment out the close(fds[1]) in the parent and run it again.
//   Why does it hang?

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void){
  int fds[2];
  if(pipe(fds) < 0){
    perror("pipe");
    return 1;
  }

  pid_t pid = fork();
  if(pid == 0){
    // child: the writer
    close(fds[0]);               // child doesn't read

    char* lines[] = { "first message\n", "second message\n", "third message\n" };
    for(int i = 0; i < 3; i++){
      write(fds[1], lines[i], strlen(lines[i]));
      sleep(1);
    }

    close(fds[1]);               // closing the last write end = EOF for the reader
    return 0;
  }

  // parent: the reader
  close(fds[1]);                 // parent doesn't write -- important! (see above)

  char buf[100];
  ssize_t n;
  while((n = read(fds[0], buf, sizeof(buf))) > 0){
    printf("parent read %zd bytes: ", n);
    fflush(stdout);
    write(1, buf, n);
  }
  printf("read returned %zd: the pipe is at EOF\n", n);

  close(fds[0]);
  wait(NULL);
  return 0;
}
