// The FILE API, part 8: a FILE* is a file descriptor plus a buffer
//
//   fileno(f)          give me the descriptor inside this FILE*
//   fdopen(fd, mode)   wrap a descriptor I already have in a FILE*
//
// So the FILE API isn't a separate world: underneath every FILE*
// there's an int, and every fgets/fprintf eventually becomes a read/write.
//
// try:
//   ./stdioFileno
//   cat wrapped.txt

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
  printf("fileno(stdin)  = %d\n", fileno(stdin));
  printf("fileno(stdout) = %d\n", fileno(stdout));
  printf("fileno(stderr) = %d\n", fileno(stderr));

  FILE* f = fopen("sample.txt", "r");
  if(f == NULL){
    perror("sample.txt");
    return 1;
  }
  printf("fopen'd sample.txt, its fd is %d\n", fileno(f));
  fclose(f);

  // go the other way: get an fd with the system call...
  int fd = open("wrapped.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if(fd < 0){
    perror("open");
    return 1;
  }
  printf("open'd wrapped.txt as fd %d\n", fd);

  // ...then wrap it so we can use fprintf on it
  FILE* w = fdopen(fd, "w");
  fprintf(w, "this line was written with fprintf on fd %d\n", fd);
  fprintf(w, "%d squared is %d\n", 12, 12 * 12);

  fclose(w);   // closes the FILE* *and* the fd -- don't close(fd) again!
  return 0;
}
