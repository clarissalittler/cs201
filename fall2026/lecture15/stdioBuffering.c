// The FILE API, part 7: buffering -- when do the bytes actually leave?
//
// printf doesn't call write every time. It puts bytes in a buffer
// inside our process and calls write later:
//   - stdout to a terminal:   line buffered (write at every '\n')
//   - stdout to a file/pipe:  fully buffered (write when the buffer fills,
//                             on fflush, or when the program exits)
//   - stderr:                 unbuffered (write right away)
// write() has no buffer in our program at all. It goes straight to the kernel.
//
// try:
//   ./stdioBuffering                  (A B C D, in order)
//   ./stdioBuffering | cat            (what order now?!)
//   ./stdioBuffering flush | cat      (fflush after every printf)

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[]){
  int flush = (argc >= 2 && strcmp(argv[1], "flush") == 0);

  printf("A: printf\n");
  if(flush) fflush(stdout);     // push the buffer out with write() now

  write(1, "B: write\n", 9);

  printf("C: printf\n");
  if(flush) fflush(stdout);

  write(1, "D: write\n", 9);

  // when main returns, the C library flushes stdout one last time.
  // (if we crashed or called _exit() here, unflushed text would be lost!)
  return 0;
}
