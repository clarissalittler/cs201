#include <stdio.h>
#include <stdlib.h>

int main(){
  char test = -1;

  if(test > 0){
    printf("Well, char is unsigned!\n");
  }
  else{
    printf("char is signed, I guess\n");
  }
  
  return 0;
}
