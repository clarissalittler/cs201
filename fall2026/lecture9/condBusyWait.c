#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// the problem condition variables solve, part 1: the BAD version
//
// a worker thread needs to wait until the boss says go. all we have
// so far is a mutex, so the best we can do is... check. and check.
// and check. run this and watch `top` in another terminal: the worker
// pins an entire CPU core doing absolutely nothing useful.
//
// this is called busy-waiting or spinning, and for waits longer than
// a few microseconds it's a waste of electricity

int ready = 0;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void* worker(void* p){
  (void)p;
  while(1){
    pthread_mutex_lock(&mut);
    if(ready){
      pthread_mutex_unlock(&mut);
      break;
    }
    pthread_mutex_unlock(&mut);
    // ...and immediately lock again to peek again. forever.
  }
  printf("worker: finally! got the go-ahead\n");
  return NULL;
}

int main(){
  pthread_t t;
  pthread_create(&t, NULL, worker, NULL);

  printf("boss: making the worker wait (watch your CPU meter)...\n");
  sleep(5);

  pthread_mutex_lock(&mut);
  ready = 1;
  pthread_mutex_unlock(&mut);

  pthread_join(t, NULL);
  return 0;
}
