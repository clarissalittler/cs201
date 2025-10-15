#include "unistd.h"

int main(){
  char buff[30];

  int charsRead = read(0,buff,30);
  write(1,buff,charsRead);
  
  return 0;
}
