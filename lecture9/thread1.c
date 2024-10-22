#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

void* ourPrinter(void* arg){
  char* msg = (char*) arg;

  printf("Our thread says: %s", msg);

  return 0;
}

int main(){
  pthread_t thread1;
  pthread_t thread2;

  char* msg1 = "Hi there I'm one thread\n";
  char* msg2 = "Hi there I'm a different thread\n";
  
  pthread_create(&thread1,NULL,ourPrinter,msg1);
  pthread_create(&thread2,NULL,ourPrinter,msg2);

  pthread_join(thread1,NULL);
  pthread_join(thread2,NULL);

  return 0;
  
}
