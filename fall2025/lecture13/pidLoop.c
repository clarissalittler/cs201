#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

int main(){

 pid_t myPID = getpid(); // gets your own process id
 while(1){
  printf("I'm mr. %d\n",myPID);
  sleep(1);  
 }

 return 0;
}
