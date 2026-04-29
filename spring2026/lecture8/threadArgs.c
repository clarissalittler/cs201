#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// in this program we'll have our threads
// all "say hi" with the "id" we've given them on creation

void* sayHi(void* p){
 
  int id = *((int*)p);  
  printf("Hi my name is: %d\n",id);

  return NULL;
}

int main(){

  pthread_t threads[20];

  int threadArgs[20];
  for(int i=0; i<20; i++){
    threadArgs[i] = i;
  }
  
  for(int i=0; i<20; i++){
    pthread_create(&threads[i],NULL,sayHi,threadArgs+i);
  }

  for(int i=0; i<20; i++){
    pthread_join(threads[i],NULL);
  }
  
  return 0;
}
