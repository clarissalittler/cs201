#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mut;
pthread_cond_t enoughInputs;
int numInputs = 0;
int inputArr[2];


void* adder(void* arg){
  pthread_mutex_lock(&mut);
  
  while(numInputs < 2){
    pthread_cond_wait(&enoughInputs,&mut);
  }

  int* num = (int*)arg;
  *num = inputArr[1] + inputArr[0];
  
  pthread_mutex_unlock(&mut);
  
  return NULL;
}

void* reader(void* arg){
  pthread_mutex_lock(&mut);
  while(numInputs < 2){
    printf("Enter a number:\n");
    scanf("%d",&inputArr[numInputs]);
    numInputs++;
  }
  pthread_cond_signal(&enoughInputs);
  pthread_mutex_unlock(&mut);
  return NULL;
}

int main(){
  pthread_mutex_init(&mut,NULL);
  pthread_cond_init(&enoughInputs,NULL);
  pthread_t readThread;
  pthread_t addThread;
  int num = 0;
  pthread_create(&readThread, NULL, reader, NULL);
  pthread_create(&addThread, NULL, adder, &num);

  pthread_join(readThread,NULL);
  pthread_join(addThread, NULL);

  printf("After much labor and effort the numbers added to: %d\n",num);
  
  pthread_mutex_destroy(&mut);
  pthread_cond_destroy(&enoughInputs);
  
  return 0;
}
