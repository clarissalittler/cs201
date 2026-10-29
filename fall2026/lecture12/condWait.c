#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int ready = 0;
pthread_mutex_t mut  = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;

void* worker(void* p){
  (void)p;
  pthread_mutex_lock(&mut);

  while(!ready){
    pthread_cond_wait(&cond, &mut);
  }
  pthread_mutex_unlock(&mut);

  printf("worker: finally! got the go-ahead\n");
  return NULL;
}

int main(){
  pthread_t t;
  pthread_create(&t, NULL, worker, NULL);

  printf("boss here at %d we're making the worker wait (CPU meter stays quiet this time)...\n",getpid());
  sleep(10);

  pthread_mutex_lock(&mut);
  ready = 1;                    // change the condition...
  pthread_cond_signal(&cond);   // ...then wake up one waiter
  pthread_mutex_unlock(&mut);

  pthread_join(t, NULL);
  return 0;
}
