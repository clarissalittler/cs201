// Change one byte in a file through either a private or shared mapping.
// Usage: ./mmapFile private | shared
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
  
  // Make a file containing "hallo\n" (6 bytes).
  int fd = open("hallo.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
  write(fd, "hallo\n", 6);
  
  char *p = mmap(NULL, 6, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  printf("Before store: mapping[0] = %c\n", p[0]);
  p[0] = 'H'; // you can just set things!
  printf("After store:  mapping[0] = %c\n", p[0]);
  munmap(p, 6);

  // Now ask the file itself what its first byte is.
  char from_file;
  pread(fd, &from_file, 1, 0); // this lets us zip back to the start of the file *and* read in the same call
  printf("After unmap:  file[0] = %c\n", from_file);
  close(fd);
  return 0;
}
