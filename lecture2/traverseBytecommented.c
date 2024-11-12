#include <stdio.h> // include the standard input/output library for functions like printf
#include <stdlib.h> // include the standard library for functions like malloc and exit

/* 
  This program demonstrates how to traverse an integer byte by byte.
  It does this by casting a pointer to the integer to a one-byte wide pointer type.
  This allows us to access individual bytes of the integer, which is useful for low-level operations or understanding how integers are stored in memory.
*/

int main(){

  int ourNum = 0x12345678; // initialize an integer variable with a hexadecimal value

  //printf("ourNum = %d\n",ourNum); // uncomment to print the integer in decimal format
  //printf("ourNum = %u\n",ourNum); // uncomment to print the integer in unsigned decimal format

  unsigned char* pointy = (unsigned char*) &ourNum; // create a pointer to the first byte of ourNum by casting the address of ourNum to a pointer to unsigned char (one byte wide).
  //printf("The first byte of our number is: %x\n", pointy[3]); // uncomment to print the hexadecimal value of the first byte, which is actually the fourth byte in memory due to little-endian ordering

  //the bytes in memory order of ourNum
  for(int i=0; i < sizeof(typeof(ourNum)); i++){ // iterate through each byte of ourNum. sizeof(typeof(ourNum)) gets the size of the integer type, which is 4 bytes on most systems.
    printf("byte #%d = %x\n",i,pointy[i]); // print the byte number and its hexadecimal value. pointy[i] accesses the i-th byte of the integer.
  }
  
  return 0; // return 0 to indicate successful program execution
}
