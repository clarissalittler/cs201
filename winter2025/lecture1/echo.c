#include <stdio.h>

int main(){

  char msg[50];
  
  printf("Type somethin' will ya: ");
  scanf("%s",msg);
  printf("You said: %s\n",msg);
  printf("And msg is actually: %p\n",msg);
  
  return 0;
}
