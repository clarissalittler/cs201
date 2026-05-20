#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>

struct shared {
  sem_t lock;
  int   counter;
};

int main(){
  struct shared* s = mmap(NULL, sizeof(struct shared),
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  sem_init(&s->lock, 1, 1); // pshared = 1, initial value = 1
  s->counter = 0;

  pid_t pid = fork();
  if(pid == 0){
    for(int i = 0; i < 100000; i++){
      sem_wait(&s->lock);
      s->counter++;
      sem_post(&s->lock);
    }
    return 0;
  } else {
    for(int i = 0; i < 100000; i++){
      sem_wait(&s->lock);
      s->counter++;
      sem_post(&s->lock);
    }
    wait(NULL);
    printf("final counter (should be 200000): %d\n", s->counter);
  }

  sem_destroy(&s->lock);
  munmap(s, sizeof(struct shared));
  return 0;
}
