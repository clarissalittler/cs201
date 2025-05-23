#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
  int thingy = 100;
  pid_t pid = fork();
  if(pid != 0){
    thingy++;
    thingy++;
    printf("Thingy: %d\n",thingy);
  }
  else{
    thingy++;
    printf("Thingy: %d\n",thingy);
  }
  return 0;
}
