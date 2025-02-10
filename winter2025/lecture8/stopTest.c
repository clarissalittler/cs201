#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


int main(){

  sigset_t blocks;
  sigset_t prevs; // this is the type for the signal blocking flags
  sigemptyset(&blocks); // set the blocks to the empty set
  sigaddset(&blocks,SIGSTOP); // add sigstop to the blocked signals
  sigprocmask(SIG_BLOCK,&blocks,&prevs); // the last is an optional argument to save what the previous state of the blocked signals datatype was
  
  while(1){
    printf("Here I am!: %d\n",getpid());
    sleep(1);
  }

  return 0;
}
