#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>


// when you see void*, that's
// polymorphism in C
void* func1(void* arg){
  int sleepAmount = *(int*) arg;
  sleep(sleepAmount);
  printf("[Yawn] I slept for %d seconds\n",sleepAmount);
  return NULL;
}

int main(){
  srand(time(NULL));
  
  pthread_t thread1;
  pthread_t thread2;

  int sleepArg1 = rand()%6 + 1;
  int sleepArg2 = rand()%7 + 2;
  
  pthread_create(&thread1,NULL,func1,&sleepArg1);
  pthread_create(&thread2,NULL,func1,&sleepArg2);

  pthread_join(thread1,NULL);
  pthread_join(thread2,NULL);
  
  return 0;
}
