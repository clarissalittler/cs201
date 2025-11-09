#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"
#include "stdio.h"
#include "signal.h"
#include "string.h"

enum lilguyState {
  eating,
  sleeping,
  moving,
  vibing
};

struct lilguyData {
  int id;
  enum lilguyState state;
};

sig_atomic_t simulationRunning = 1;

void handler(int signum){
  char msg[] = "Alright everyone clean up and go!\n";
  simulationRunning = 0;
  write(1,msg,strlen(msg));
}

void printState(enum lilguyState s){
  switch(s){
  case eating:
    printf("having a lil snack\n");
    break;
  case sleeping:
    printf("having a lil sleep\n");
    break;
  case vibing:
    printf("having a lil chill\n");
    break;
  case moving:
    printf("having a lil walk\n");
  }
}

void* threadWorker(void* arg){
  struct lilguyData dat = *((struct lilguyData*) arg);
  while(simulationRunning){
    printf("I'm Miss %d and I'm ",dat.id);
    printState(dat.state);
    dat.state = rand()%4;
    sleep(rand()%3+1);
  }

  printf("I'm miss %d and it's time to go!!\n",dat.id);
  return NULL;
}


int main(){

  srand(time(0));

  signal(SIGINT,handler);
  
  pthread_t threads[5];
  struct lilguyData datums[5];
  for(int i=0; i<5; i++){
    datums[i].id = i;
    datums[i].state = rand()%4;
  }
  for(int i=0; i<5; i++){
    pthread_create(&threads[i],NULL,threadWorker,&datums[i]);
  }
  for(int i=0;i<5;i++){
    pthread_join(threads[i],NULL);
  }
  return 0;
}
