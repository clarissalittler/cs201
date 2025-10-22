#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int num = 0;

void* thread_function(void* arg) {
  for(int i=0; i<100; i++){
    int num2 = num; // thread collects state of a variable
    //sleep(rand()%2);
    num2 = num2+1; // does work on it
    num = num2; // stores the update
  }
  printf("Thread executing\n");
  return NULL;
}

int main() {
  srand(time(0));
  pthread_t thread[100];
  for(int i=0;i<100;i++){
    pthread_create(&thread[i], NULL, thread_function, NULL);
  }
  // Wait for thread to complete
  for(int i=0; i<100; i++){
    pthread_join(thread[i], NULL);
  }
  printf("And the total is: %d\n",num);
  
  return 0;
}
