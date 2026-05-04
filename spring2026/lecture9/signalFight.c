#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

// two processes attacking each other with signals
// SIGUSR1 = "I just hit you, take damage"
// SIGUSR2 = "I just died, you can stop fighting now"
//
// each process keeps its own hp, picks a random damage value when hit,
// and kills its opponent (with SIGUSR1) on each turn until somebody dies.
// when hp drops to 0, the dying process sends SIGUSR2 to its opponent
// and exits cleanly.
//
// this is the canonical "signals as IPC" demo:
// no shared memory, no pipes, just process IDs and kill().
//
// subtlety #1: we can't use printf in a signal handler -- it's not
//   async-signal-safe. We use sprintf+write, which is a common workaround
//   (sprintf isn't strictly safe either, but for these toy examples it's fine,
//    and the strings are simple enough that it works in practice).
// subtlety #2: rand() in a child after fork() will give the same sequence
//   as the parent unless we re-seed it -- so we XOR with getpid()
// subtlety #3: sleep() can be interrupted by an incoming signal,
//   so the loop sleep(rand()%4+1) might come back early. fine for us;
//   we just want some jitter between turns.

#define STDOUT 1

volatile sig_atomic_t hp = 50;
volatile sig_atomic_t stillFighting = 1;
volatile sig_atomic_t won = 1;

void onHit(int sig){
  (void)sig;
  if(hp > 0){
    int dmg = rand() % 5 + 1;
    char buf[80];
    int n = sprintf(buf, "I, mr %d, took %d damage!\n", getpid(), dmg);
    write(STDOUT, buf, n);
    hp -= dmg;
    if(hp <= 0){
      stillFighting = 0;
      won = 0;
    }
  }
}

void onEnd(int sig){
  (void)sig;
  stillFighting = 0;
}

void fightLoop(int enemy){
  while(stillFighting){
    char buf[80];
    int n = sprintf(buf, "I, mr %d, have %d hp left\n", getpid(), hp);
    write(STDOUT, buf, n);
    kill(enemy, SIGUSR1);
    sleep(rand()%4 + 1);
  }
  // opponent should know we're done
  kill(enemy, SIGUSR2);
  if(!won){
    char buf[80];
    int n = sprintf(buf, "I, mr %d, just died!\n", getpid());
    write(STDOUT, buf, n);
  }
}

int main(){
  struct sigaction hit;
  hit.sa_handler = onHit;
  sigemptyset(&hit.sa_mask);
  hit.sa_flags = SA_RESTART;
  sigaction(SIGUSR1, &hit, NULL);

  struct sigaction end;
  end.sa_handler = onEnd;
  sigemptyset(&end.sa_mask);
  end.sa_flags = SA_RESTART;
  sigaction(SIGUSR2, &end, NULL);

  pid_t pid = fork();

  // re-seed AFTER the fork so the two processes don't roll identical damage
  srand(time(0) ^ getpid());

  if(pid == 0){
    // child: opponent is the parent
    fightLoop(getppid());
  } else {
    // parent: opponent is the child
    fightLoop(pid);
    wait(NULL);
  }
  return 0;
}
