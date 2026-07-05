#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

int main(){
  int* counter = mmap(NULL, sizeof(int),
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  *counter = 0;

  pid_t pid = fork();
  if(pid == 0){
    for(int i = 0; i < 100000; i++){
      (*counter)++;
    }
    return 0;
  } else {
    for(int i = 0; i < 100000; i++){
      (*counter)++;
    }
    wait(NULL);
    printf("final counter (expected 200000): %d\n", *counter);
  }

  munmap(counter, sizeof(int));
  return 0;
}
