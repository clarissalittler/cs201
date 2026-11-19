#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

// this is the one where we modify our operation so it turns a file into nothing but screaming
int main(int argc, char* argv[]){
  if(argc < 2){
    printf("usage: %s <file>\n", argv[0]);
    return 1;
  }

  int fd = open(argv[1], O_RDWR);

  struct stat st;
  fstat(fd, &st);

 
  char* contents = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  // We replace every non-newline character with A
  for(int i=0; i < st.st_size; i++){
    if(contents[i] != '\n'){
      contents[i] = 'A';
    }
  }
  
  printf("file is %ld bytes\n", (long)st.st_size);
  printf("first byte: '%c'\n", contents[0]);
  printf("last byte:  '%c'\n", contents[st.st_size - 1]);
  printf("dumping the whole thing as if it were a buffer:\n");
  write(1, contents, st.st_size);
  
  munmap(contents, st.st_size);
  close(fd);
  return 0;
}
