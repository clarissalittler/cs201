#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

void* ourPrinter(void* arg){
  char* msg = (char*) arg;

  printf("Our thread says: %s", msg);

  int sleepTime = (rand() % 5) + 1; 

  sleep(sleepTime);

  int* sleepPointer = malloc(sizeof(int));
  *sleepPointer = sleepTime;
  
  return sleepPointer;
}

int main(){

  srand(time(0));
  pthread_t thread1;
  pthread_t thread2;

  char* msg1 = "Hi there I'm one thread\n";
  char* msg2 = "Hi there I'm a different thread\n";
  
  pthread_create(&thread1,NULL,ourPrinter,msg1);
  pthread_create(&thread2,NULL,ourPrinter,msg2);

  void* res1;
  void* res2;
  
  pthread_join(thread1,&res1);
  pthread_join(thread2,&res2);

  printf("Thread 1 ended up waiting for %d seconds\n",*(int*)res1);
  printf("Thread 2 ended up waiting for %d seconds\n",*(int*)res2);

  return 0;
  
}
