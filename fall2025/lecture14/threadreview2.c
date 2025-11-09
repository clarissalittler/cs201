#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

char* msgs[] = {"Hi there ","how are you? ","I hope this message came through"};

int counter = 0;

pthread_mutex_t mut;

void* threadWorker(void* arg){
  sleep(rand()%3+1);

  int temp;
  
  pthread_mutex_lock(&mut);
  // now we're in the critical section
  temp = counter;
  counter++;
  pthread_mutex_unlock(&mut);
  printf(msgs[temp]);
  
  return NULL;
}

int main(){

  srand(time(NULL));
  
  pthread_t thread1;
  pthread_t thread2;
  pthread_t thread3;

  pthread_mutex_init(&mut,NULL);
  pthread_create(&thread1,NULL,threadWorker,NULL);
  pthread_create(&thread2,NULL,threadWorker,NULL);
  pthread_create(&thread3,NULL,threadWorker,NULL);

  pthread_join(thread1,NULL);
  pthread_join(thread2,NULL);
  pthread_join(thread3,NULL);

  printf("\n\n");
  
  return 0;
}
