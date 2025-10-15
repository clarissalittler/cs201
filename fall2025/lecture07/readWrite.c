#include "stdio.h"
#include "unistd.h"

int main(){

  char hiThere[] = "Hey there, goofuses and goofuxen\n";
  
  write(1, hiThere, 33);
  
  return 0;
}
