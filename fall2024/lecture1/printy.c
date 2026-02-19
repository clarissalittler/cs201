#include <stdio.h>

int main(){
  int num = 10;
  char* str1 = "here's a string";
  char* str2 = "oh no here's another";
  
  // printf("Here's two numbers: %d%f\n", num, 2.9);
  // printf("%s%s\n",str1,str2);

  printf("The string %s starts at %p\n",str1,(void*)str1);
  printf("Here's num too: %d and another string: %s\n", num, str2);
  return 0;
}
