#include <stdio.h>

int main(){
  char msg[256];
  printf("Enter a message to echo (no longer than 256 characters): ");
  scanf("%s",msg);
  printf("You said: %s\n",msg);
  return 0;
}
