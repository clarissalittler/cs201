#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include "sharedstruct.h"

int main(){
  int sharedFd = shm_open(sharedName, O_CREAT | O_RDWR, 0666);
  ftruncate(sharedFd, sizeof(struct sharedData));
  struct sharedData* shared = mmap(NULL, sizeof(struct sharedData), PROT_READ | PROT_WRITE,
                                   MAP_SHARED, sharedFd, 0);
  sem_init(&shared->mutex, 1, 1);
  for(int i = 0; i < 10; i++){
    sem_wait(&shared->mutex);
    shared->payload1 = i;
    shared->payload2 = i*i;
    printf("Set %d th payload\n",i);
    sem_post(&shared->mutex);
    sleep(1);
  }

  munmap(shared, sizeof(struct sharedData));
  close(sharedFd);
  // Note: Consumer will unlink the shared memory
  return 0;
}
