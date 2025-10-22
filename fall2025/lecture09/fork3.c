#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

int main(){
  for(int i=0; i< 4;i++){
    fork();
    printf("I'm Mr. %d here at index %d\n",getpid(),i); 
  }

  return 0;
}
