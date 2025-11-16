#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>

struct guardCounter {
  int counter;
  sem_t semaphore;
};

int main(){

  srand(time(0));
  
  struct guardCounter* ourCounter = mmap(NULL, sizeof(struct guardCounter),
                                         PROT_READ | PROT_WRITE,
                                         MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  sem_init(&ourCounter->semaphore,1,1); // the semaphore, 1 for being shared between processes, 1 for starting value
  ourCounter->counter =0;

  pid_t pid = fork();

  for(int i =0;i < 5;i++){
    sem_wait(&ourCounter->semaphore);
    
    int temp = ourCounter->counter;
    sleep(rand()%3);
    ourCounter->counter = temp+1;
    sem_post(&ourCounter->semaphore);
  }

  if(pid != 0){
    wait(NULL);
    printf("Survey says! %d\n",ourCounter->counter);
    sem_destroy(&ourCounter->semaphore);
    munmap(ourCounter, sizeof(struct guardCounter));    
  }
  
  return 0;
}
