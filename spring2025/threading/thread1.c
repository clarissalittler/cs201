#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

// note that our thread function
// is always going to have this type
// it takes in a void*
// and returns a void*
// why?
// void* is kind of the wildcard pointer
// remember how we can cast pointers to basically
// anything?
// well that means a void* pointer is kind of the
// most generic pointer
// C lets you implicitly cast to void*
void* ourPrinter(void* arg){
  char* msg = (char*) arg;

  printf("Our thread says: %s", msg);

  // return 0; <--- ookay, but a little naughty
  // 0 IS NULL but it's visually unhelpful
  return NULL;
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
