#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "sys/types.h"
#include "signal.h"
#include "string.h"

volatile sig_atomic_t income = 0;
volatile sig_atomic_t gen1 = 0;
volatile sig_atomic_t gen2 = 0;
volatile sig_atomic_t gen3 = 0;

char menuString[] = "Press 1 to tick up your income\nPress 2 to buy a first-level generator (cost 10)\nPress 3 to buy a second-level generator (cost 1000)\nPress 4 to buy a third-level generator (cost 100000)\n";


void printMenu(){
  char resources[100];
  sprintf(resources,"Money %d\nGen1: %d\nGen2: %d\nGen3: %d\n",income,gen1,gen2,gen3);
  write(STDOUT_FILENO,resources,strlen(resources));
  write(STDOUT_FILENO,menuString,strlen(menuString));
}


void heartbeat(int signumb){
  printMenu();
  gen2 += gen3;
  gen1 += gen2;
  income += gen1;
}

    
int main(){


  pid_t pinger = fork();

  if(pinger == 0){
    while(1){
      pid_t parent = getppid();
      sleep(1);
      kill(parent,SIGUSR1);
    }
  }

  signal(SIGUSR1,heartbeat);
  
  printMenu();
  while(1){
    int choice;
    scanf("%d",&choice);
    switch(choice){
    case 1:
      income++;
      break;
    case 2:
      if(income >= 10){
	income = income -10;
	gen1++;
      }
      break;
    case 3:
      if(income >= 1000){
	income = income - 1000;
	gen2++;
      }
      break;
    case 4:
      if(income >= 100000){
	income -= 100000;
	gen3++;
      }
    }
    
  }

  
  
  return 0;
}
