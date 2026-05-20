#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("usage: %s <file>\n", argv[0]);
    return 1;
  }

  int fd = open(argv[1], O_RDONLY);

  struct stat st;
  fstat(fd, &st);

  // PROT_READ: read-only memory; MAP_PRIVATE: changes (if any) stay local
  char* contents = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

  printf("file is %ld bytes\n", (long)st.st_size);
  printf("first byte: '%c'\n", contents[0]);
  printf("last byte:  '%c'\n", contents[st.st_size - 1]);
  printf("dumping the whole thing as if it were a buffer:\n");
  write(1, contents, st.st_size);

  munmap(contents, st.st_size);
  close(fd);
  return 0;
}
