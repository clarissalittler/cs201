// Everything is a file, part 2: one copy loop, many kinds of "file"
//
// This is the same open/read/write/close loop from lecture 13.
// The point is how many different things it works on without
// changing a single line.
//
// try:
//   ./fdCat sample.txt
//   ./fdCat /proc/cpuinfo           (not on any disk -- the kernel generates it as you read)
//   ./fdCat /proc/self/status       ("self" means whichever process opened it: us!)
//   ./fdCat /dev/null               (always empty: read returns 0 right away)
//   echo piped in | ./fdCat /dev/stdin
//   ./fdCat /etc                    (a directory: open works, but read fails)
//   ./fdCat nope.txt                (doesn't exist: open fails)

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

// write can write fewer bytes than we asked for, so keep going
// until all n bytes are out
int writeAll(int fd, const char* buf, ssize_t n){
  while(n > 0){
    ssize_t w = write(fd, buf, n);
    if(w < 0) return -1;
    buf += w;
    n   -= w;
  }
  return 0;
}

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("usage: %s <file>\n", argv[0]);
    return 1;
  }

  int fd = open(argv[1], O_RDONLY);
  if(fd < 0){
    perror("open");     // prints our message plus *why* it failed
    return 1;
  }

  char buf[4096];
  ssize_t n;
  while((n = read(fd, buf, sizeof(buf))) > 0){
    if(writeAll(1, buf, n) < 0){
      perror("write");
      break;
    }
  }
  if(n < 0){
    perror("read");
  }

  close(fd);
  return 0;
}
