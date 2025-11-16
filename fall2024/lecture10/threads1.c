#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

void* myFun(void* arg){
  
  int temp = rand()%3+1;
  sleep(temp);
  
  printf("I slept for %d seconds\n",temp);

  return NULL;
}

int main(){
  srand(time(0));
  
  pthread_t thread1;
  pthread_t thread2;

  pthread_create(&thread1,NULL,myFun,NULL);
  // void pthread_create (blah,blah,(void*)(*threadFunction)(void*),void*)
  pthread_create(&thread2,NULL,myFun,NULL);

  pthread_join(thread1,NULL); // pthread_t,void**
  pthread_join(thread2,NULL); // 
  
  return 0;
}
