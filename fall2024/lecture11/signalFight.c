#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <time.h>

volatile int hp = 50;
volatile bool stillFighting = true;
volatile bool won = true;

void hitHandler(int sig){
  if(hp > 0){
    int d = rand()%5+1;
    printf("I, # %d, have been hit! I took %d damage!\n",getpid(),d);
    hp = hp - d;
    if(hp <= 0){
      stillFighting = false;
      won = false;
    }
  }
}

void endHandler(int sig){
  stillFighting = false;
}

void fightLoop(int enemy){
  while(stillFighting){
    printf("I, mr. %d, have %d hp left\n",getpid(),hp);
    kill(enemy,SIGUSR1);
    sleep(rand()%4+1);
  }
  kill(enemy,SIGUSR2);
  if(!won){
    printf("I, mr. %d, just died!\n",getpid());
  }
}

int main(){
  
  signal(SIGUSR1,hitHandler);
  signal(SIGUSR2,endHandler);
  
  int pid = fork();

  srand(time(0));
  
  if(pid == 0){
    srand(rand());
    fightLoop(getppid());
  }
  else {
    fightLoop(pid);
    wait(0);
  }
  
  return 0;
}
