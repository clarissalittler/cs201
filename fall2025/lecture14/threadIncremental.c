#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

pthread_mutex_t mut;

int income = 0;
int gen1 = 0;
int gen2 = 0;
int gen3 = 0;

char menuString[] = "Press 1 to tick up your income\nPress 2 to buy a first-level generator (cost 10)\nPress 3 to buy a second-level generator (cost 1000)\nPress 4 to buy a third-level generator (cost 100000)\n";


void printMenu(){
  printf("Money %d\nGen1: %d\nGen2: %d\nGen3: %d\n",
	 income,gen1,gen2,gen3);
  printf(menuString);
}

// now the heartbeat is a threadworker
void* heartbeat(void* arg){
  while(1){
    pthread_mutex_lock(&mut);
    printMenu();
    gen2 += gen3;
    gen1 += gen2;
    income += gen1;
    pthread_mutex_unlock(&mut);
    sleep(1);
  }
    
  return NULL;
}

int main(){

  pthread_t pulse;
  pthread_mutex_init(&mut,NULL);
  pthread_create(&pulse,NULL,heartbeat,NULL);

  
  while(1){
    int choice;
    scanf("%d",&choice);
    
    switch(choice){
      
    case 1:
      pthread_mutex_lock(&mut);
      income++;
      pthread_mutex_unlock(&mut);

      break;
    case 2:
      pthread_mutex_lock(&mut);
      if(income >= 10){
	income = income -10;
	gen1++;
      }
      pthread_mutex_unlock(&mut);
      break;
    case 3:
      pthread_mutex_lock(&mut);
      if(income >= 1000){
	income = income - 1000;
	gen2++;
      }
      pthread_mutex_unlock(&mut);

      break;
    case 4:
      pthread_mutex_lock(&mut);
      if(income >= 100000){
	income -= 100000;
	gen3++;
      }
      pthread_mutex_unlock(&mut);

    default:
      break;
    }
  }

  pthread_join(pulse,NULL);
  pthread_mutex_destroy(&mut);
  
  return 0;
}
