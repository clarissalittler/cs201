#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

void* weirdFunction(void* arg){

  int* sleepPointer = malloc(sizeof(int));

  *sleepPointer = rand() % 5;
  
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

  // void** resref = &res1;
  // resref[0] -> res1
  // resref[1] -> whatever is 8 bytes after res1 in memory
  
  
  pthread_join(thread1,&res1);
  pthread_join(thread2,&res2);
  /*
    inside pthread_join(void** ref)
     void* temp = op(arg);
     *ref = temp;
   */

  printf("Thread 1 did a sleep for %d seconds\n",*(int*)res1);
  printf("Thread 2 did a sleep for %d seconds\n",*(int*)res2);

  return 0;
  
}
