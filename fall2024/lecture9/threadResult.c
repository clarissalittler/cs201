#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

void* weirdFunction(void* arg){

  int* sleepPointer = malloc(sizeof(int));

  *sleepPointer = rand()%5+1;

  sleep(*sleepPointer);
  
  return sleepPointer;
}

int main(){

  srand(time(0));
  
  pthread_t thread1;
  pthread_t thread2;

  void* res1;
  void* res2;

  pthread_create(&thread1,NULL,weirdFunction,NULL);
  pthread_create(&thread2,NULL,weirdFunction,NULL);

  pthread_join(thread1,&res1);
  pthread_join(thread2,&res2);

  printf("Thread 1 did a sleep for %d seconds\n",*(int*)res1);
  printf("Thread 2 did a sleep for %d seconds\n",*(int*)res2);
  return 0;
}
