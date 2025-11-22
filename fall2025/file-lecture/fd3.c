#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
  printf("%d arguments were passed in\n",argc);
  for(int i=0; i<argc; i++){
    printf("The %dth argument is: %s\n",i,argv[i]);
  }
  
  
  return 0;
}
