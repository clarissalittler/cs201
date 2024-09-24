#include <stdio.h>

int main(){
  char msg[256];
  printf("Say something, will ya: ");
  scanf("%s", msg);
  printf("You said: %s\n", msg);

  return 0;
}
