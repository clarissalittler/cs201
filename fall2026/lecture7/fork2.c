#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// technically I should include a thing called wait()
// but I'm not for illustrative purposes just yet
int main(){
  int num = 5;
  fork();
  num++;
  printf("My number is: %d\n",num);
  return 0;
}
