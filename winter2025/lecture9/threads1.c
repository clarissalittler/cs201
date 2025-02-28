#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>


// when you see void*, that's
// polymorphism in C
void* func1(void* arg){
  printf("Hi I'm a thread!\n");
  return NULL;
}

int main(){
  pthread_t thread1;
  pthread_t thread2;

  pthread_create(&thread1,NULL,func1,NULL);
  pthread_create(&thread2,NULL,func1,NULL);

  pthread_join(thread1,NULL);
  pthread_join(thread2,NULL);

  return 0;
}
