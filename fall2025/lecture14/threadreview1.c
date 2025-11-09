#include "pthread.h"
#include "unistd.h"
#include "stdlib.h"
#include "stdio.h"

// let's make a simple program that spawns threads
// that each print a message and are done

// the type of a thread worker is always of the form
// void* name(void* arg) this looks dumb, but it's the only
// way to get polymorphism
// (meaning that it can take and return all possible types)

void* threadWorker(void* arg){

  char* msg = (char*)arg; // gotta cast 'em all

  usleep(rand()%15000); // usleep mu-sleep 
  
  // unlike with signals when we HAD to use the write syscall
  // printf and scanf are generally thread safe
  printf(msg);
  
  return NULL; // you can sorta not do this, relying on an implicit return 0; (because 0 is also the null pointer)
}

// REMINDER go show how to re-use the thread datastructure later
int main(){

  srand(time(NULL));
    
  pthread_t thread1;
  pthread_t thread2;
  pthread_t thread3;

  char msg1[] = "Hi there\n";
  char msg2[] = " how are you?\n";
  char msg3[] = " I hope this message came through\n";
  
  // pthread_create
  // pthread_t*, the actual thread
  // configuration for the thread (advanced, usually NULL)
  // worker function pointer
  pthread_create(&thread1,NULL,threadWorker,msg1);
  pthread_create(&thread2,NULL,threadWorker,msg2);
  pthread_create(&thread3,NULL,threadWorker,msg3);
 
  pthread_join(thread1,NULL);
  pthread_join(thread2,NULL);
  pthread_join(thread3,NULL);

  pthread_create(&thread1,NULL,threadWorker,msg1);
  pthread_join(thread1,NULL);
  
  printf("\n\n");
  
  return 0;
}
