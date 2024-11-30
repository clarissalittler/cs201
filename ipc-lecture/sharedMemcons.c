#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "sharedstruct.h"

int main(){
  int sharedFd = shm_open(sharedName, O_RDWR, 0666);
  struct sharedData* shared = mmap(NULL, sizeof(struct sharedData), PROT_READ | PROT_WRITE,
                                   MAP_SHARED, sharedFd, 0);
  for(int i = 0; i < 10; i++){
    sem_wait(&shared->mutex);
    printf("Payload received: %d,%d\n",shared->payload1,shared->payload2);
    sem_post(&shared->mutex);
    sleep(1);
  }

  munmap(shared, sizeof(struct sharedData));
  close(sharedFd);
  shm_unlink(sharedName);
  return 0;
}
