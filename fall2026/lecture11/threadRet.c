#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// let's show how return values work with threads

void* threader(void* p){

  int* n = malloc(sizeof(int));
  // int n = ... return &n
  *n = rand()%10;
  
  return n;
}

int main(){

  srand(time(NULL));
  
  pthread_t thread;

  pthread_create(&thread,NULL,threader,NULL);

  int* retVal;
  
  pthread_join(thread, (void**)&retVal);

  printf("The value returned was: %d\n",*retVal);
  free(retVal);
  retVal = NULL;
  
  return 0;
}
