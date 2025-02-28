#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>


// when you see void*, that's
// polymorphism in C
void* func1(void* arg){
  int* sleepPointer = malloc(sizeof(int));

  *sleepPointer = rand()%5+1;
  sleep(*sleepPointer);
  return sleepPointer;
}

int main(){
  srand(time(NULL));
  
  pthread_t thread1;
  pthread_t thread2;

  int* ret1;
  int* ret2;
  
  pthread_create(&thread1,NULL,func1,NULL);
  pthread_create(&thread2,NULL,func1,NULL);

  pthread_join(thread1,(void**)&ret1);
  pthread_join(thread2,(void**)&ret2);

  printf("The value of ret1 is: %d\n",*ret1);
  printf("The value of ret2 is: %d\n",*ret2);
  
  return 0;
}
