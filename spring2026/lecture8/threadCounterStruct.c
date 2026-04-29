#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

struct ThreadState {
  pthread_mutex_t mut;
  int counter;
};

// need to change this so we're passing in a pointer to a struct
// rather than passing in only the pointer to the counter
void* threadFun(void* p){
  struct ThreadState* arg = (struct ThreadState*)p;
  pthread_mutex_lock(&arg->mut); // the first person to hit this will run this function successfully and move on
  // everyone else waits on this line of code
  // UNTIL an unlock has occurred
  int temp = arg->counter;
  // the pattern of a lot of threading work
  // is that you take some data, then you operate on it
  // for awhile
  // then you put it back
  // the problem is that someone else may have put back
  // their work while you were doing yours
  for(int i=0; i<100; i++){
    temp++;
    usleep(100);
  }
  
  // now we've done our work and we need to store
  // it back into memory
  arg->counter = temp;
  pthread_mutex_unlock(&arg->mut);
  return NULL;
}

int main(){

  struct ThreadState s;
  s.counter = 0;
  
  pthread_mutex_init(&s.mut, NULL);
  
  int counter = 0;
  
  // first we make the structure to hold
  // the thread's information
  pthread_t thread[100];
  // next, we need to launch the thread by passing this structure (by reference) to pthread_create
  for(int i=0; i<100; i++){
    pthread_create(&thread[i],NULL,threadFun,&s);
  }

  for(int i=0; i<100; i++){
    pthread_join(thread[i],NULL);
  }
  
  printf("Counter is: %d\n",s.counter);

  pthread_mutex_destroy(&s.mut);
  
  return 0;
}
