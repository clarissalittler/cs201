#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// signal vs. broadcast: a starting line for a footrace
//
// ten runner threads all wait on the same condition. when the race
// starts, pthread_cond_signal would wake exactly ONE of them (and the
// other nine would sleep forever, since nobody signals again).
// pthread_cond_broadcast wakes ALL the waiters, they each re-acquire
// the mutex one at a time, re-check the condition, and go.
//
// try it: change broadcast to signal below and watch the program hang
// with nine runners still asleep at the line. (ctrl-C to escape.)
//
// rule of thumb: signal when any one waiter can consume the event
// (like one item added to a queue), broadcast when the WORLD changed
// and every waiter needs to re-evaluate (like "the race has started"
// or "the queue is being shut down")

#define RUNNERS 10

int raceStarted = 0;
pthread_mutex_t mut  = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  gun  = PTHREAD_COND_INITIALIZER;

void* runner(void* arg){
  int id = *(int*)arg;

  pthread_mutex_lock(&mut);
  while(!raceStarted){
    pthread_cond_wait(&gun, &mut);
  }
  pthread_mutex_unlock(&mut);

  printf("runner %d is off!\n", id);
  return NULL;
}

int main(){
  pthread_t threads[RUNNERS];
  int ids[RUNNERS];

  for(int i = 0; i < RUNNERS; i++){
    ids[i] = i;   // one slot per thread: no &i loop-variable bug here!
    pthread_create(&threads[i], NULL, runner, &ids[i]);
  }

  printf("On your marks... get set...\n");
  sleep(2);

  pthread_mutex_lock(&mut);
  raceStarted = 1;
  pthread_cond_broadcast(&gun);   // wake EVERYONE (cf. signal: just one)
  pthread_mutex_unlock(&mut);

  for(int i = 0; i < RUNNERS; i++){
    pthread_join(threads[i], NULL);
  }
  printf("everyone's running\n");
  return 0;
}
