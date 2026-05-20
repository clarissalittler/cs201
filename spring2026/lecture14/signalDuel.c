// quick refresher: parent and child duel each other with SIGUSR1
// each one has 10 hp, takes 1 damage per hit, prints its remaining hp
// after every round
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

volatile sig_atomic_t hp        = 10;
volatile sig_atomic_t fighting  = 1;

void onHit(int sig){
  hp--;
  if(hp <= 0) fighting = 0;
}

void endGame(int sig){
  fighting = 0;
  char msg[] = "Oh, I guess I won?\n";
  write(1,msg,sizeof(msg));
}

int main(){
  // both processes inherit the handler since we install it BEFORE fork()
  signal(SIGUSR1, onHit);
  signal(SIGUSR2, endGame);

  pid_t pid    = fork();
  pid_t enemy  = (pid == 0) ? getppid() : pid;
  srand(time(0) ^ pid);
  while(fighting){
    kill(enemy, SIGUSR1);
    int left = rand()%3+1;
    while((left = sleep(left)) > 0);
    if(fighting){
      printf("pid %d: hp = %d\n", getpid(), hp);
    }
  }

  if(hp <= 0){
    printf("pid %d: I died!\n", getpid());
    kill(enemy, SIGUSR2);
  }
  
  if(pid > 0) wait(NULL);
  return 0;
}
