#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int counter = 0;

pthread_mutex_t lockLevel1;
pthread_mutex_t lockLevel2;

void* inc1(void* arg){
  pthread_mutex_lock(&lockLevel1);
  printf("Inc1 has acquired lock 1\n");
  sleep(1);
  pthread_mutex_lock(&lockLevel2);
  printf("Inc1 has acquired lock 2\n");
  counter++;
  pthread_mutex_unlock(&lockLevel2);
  printf("Inc1 has released lock 2\n");
  pthread_mutex_unlock(&lockLevel1);
  printf("Inc1 has released lock 1\n");
  return NULL;
}

void* inc2(void* arg){
  pthread_mutex_lock(&lockLevel2);
  printf("Inc2 has acquired lock 2\n");
  sleep(1);
  pthread_mutex_lock(&lockLevel1);
  printf("Inc2 has acquired lock 1\n");
  counter++;
  pthread_mutex_unlock(&lockLevel1);
  printf("Inc2 has released lock 1\n");
  pthread_mutex_unlock(&lockLevel2);
  printf("Inc2 has acquired lock 2\n");
  return NULL;
}

int main(){
  pthread_t t1, t2;
  pthread_mutex_init(&lockLevel1,NULL);
  pthread_mutex_init(&lockLevel2,NULL);

  pthread_create(&t1,NULL,inc1,NULL);
  pthread_create(&t2,NULL,inc2,NULL);

  pthread_join(t1,NULL);
  pthread_join(t2,NULL);

  pthread_mutex_destroy(&lockLevel1);
  pthread_mutex_destroy(&lockLevel2);

  printf("And the counter is: %d\n",counter);

  return 0;
}
