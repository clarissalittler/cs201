#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

volatile sig_atomic_t hp = 50;
volatile sig_atomic_t stillFighting = 1;
volatile sig_atomic_t won = 1;

#define STDOUT 1

void hitHandler(int sig){
  if(hp > 0){
    int d = rand()%5+1;
    char str[50];
    sprintf(str,"I, # %d, have been hit! I took %d damage!\n",getpid(),d);
    write(STDOUT,str,strlen(str));
    //printf("I, # %d, have been hit! I took %d damage!\n",getpid(),d);
    hp = hp - d;
    if(hp <= 0){
      stillFighting = 0;
      won = 0;
    }
  }
}

void endHandler(int sig){
  stillFighting = 0;
}

void fightLoop(int enemy){
  while(stillFighting){
    char str[50];
    sprintf(str,"I, mr. %d, have %d hp left\n",getpid(),hp);
    write(STDOUT,str,strlen(str));
    kill(enemy,SIGUSR1);
    int rest;
    do {
      rest = sleep(rand()%4+1);
    }while(rest > 0);
  }
  kill(enemy,SIGUSR2);
  if(!won){
    char str[50];
    sprintf(str,"I, mr. %d, just died!\n",getpid());
    write(STDOUT,str,strlen(str));
  }
}

int main(){
  /*
  struct sigaction sig;
  sig.sa_handler = hitHandler;
  sig.sa_flags = SA_RESTART;
  sigemptyset(&sig.sa_mask);
  sigaction(SIGUSR1, &sig, NULL);
  //
  */
  signal(SIGUSR1,hitHandler);
  signal(SIGUSR2,endHandler);
  
  int pid = fork();

  srand(time(0) ^ getpid());
  
  if(pid == 0){
    fightLoop(getppid());
  }
  else {
    fightLoop(pid);
    wait(0);
  }
  
  return 0;
}
