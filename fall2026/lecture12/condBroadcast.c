#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


int raceStarted = 0;
// it's the macros I mentioned last time
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
  pthread_t threads[10];
  int ids[10];

  for(int i = 0; i < 10; i++){
    ids[i] = i;   // this trick again
    pthread_create(&threads[i], NULL, runner, &ids[i]);
  }

  printf("On your marks... get set...\n");
  sleep(2);

  pthread_mutex_lock(&mut);
  raceStarted = 1;
  pthread_cond_broadcast(&gun);   // wake EVERYONE (cf. signal: just one)
  pthread_mutex_unlock(&mut);

  for(int i = 0; i < 10; i++){
    pthread_join(threads[i], NULL);
  }
  printf("everyone's running\n");
  return 0;
}
