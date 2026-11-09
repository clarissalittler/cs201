// Everything is a file, part 6: how the shell does   ls -l > listing.txt
//
// dup2(oldfd, newfd) makes newfd refer to the same open file as oldfd.
// If we point fd 1 at a file, then anything that writes to fd 1 --
// printf, puts, or a whole other program after exec -- writes to the file.
// The program being run never finds out.
//
// try:
//   ./fdRedirect
//   cat listing.txt

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(void){
  pid_t pid = fork();

  if(pid == 0){
    // child: open the file, then make it our stdout
    int fd = open("listing.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd < 0){
      perror("open");
      return 1;
    }
    dup2(fd, 1);    // fd 1 now refers to listing.txt
    close(fd);      // we don't need the extra copy anymore

    printf("this printf line is going into the file too\n");
    fflush(stdout); // (we'll see why this is needed in stdioBuffering.c)

    execlp("ls", "ls", "-l", (char*)NULL);
    perror("execlp");   // only runs if exec failed
    return 1;
  }

  wait(NULL);
  printf("parent: child finished. Its stdout was listing.txt; ours is still the terminal.\n");
  printf("parent: try   cat listing.txt\n");
  return 0;
}
