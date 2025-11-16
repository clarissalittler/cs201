#include <stdlib.h> // Includes standard library functions like malloc, free, etc.
#include <stdio.h> // Includes standard input/output functions like printf, scanf, etc.

// Function to toggle a specific bit in an integer.
void toggle(int* target, int place){
  // Create a bitmask with a 1 at the specified 'place' position.
  int shifty = 1 << place; 
  // XOR the target integer with the bitmask. This flips the bit at the 'place' position.
  *target = *target ^ shifty; 
}

// Function to check if a specific bit in an integer is set (1).
int isSet(int target, int place){
  // Right shift the target integer by 'place' positions.
  // This moves the bit at 'place' to the least significant bit (LSB).
  // Then, perform a bitwise AND with 1.
  // If the LSB is 1 (indicating the bit was set), it returns 1; otherwise, it returns 0.
  return ((target >> place) & 1); 
}

// Function to print the binary representation of an integer.
void printBits(int target){
  // Iterate through all the bits (32 bits for a standard int).
  for(int i=31; i >=0; i--){
    // Use the isSet function to check if the current bit is set (1).
    printf("%d",isSet(target,i));
  }
}

int main(){

  // Initialize a test integer with value 0.
  int testNum = 0; 
  // Toggle the most significant bit (MSB) using the toggle function.
  toggle(&testNum,31); 
  // Uncomment these lines to toggle other bits.
  //toggle(&testNum,30);
  //toggle(&testNum,29);

  // Print a message and the decimal value of testNum.
  printf("The bits set in the number %d are:\n",testNum);
  // Print the binary representation of testNum using the printBits function.
  printBits(testNum); 
  printf("\n");
  
  return 0; // Exit the program successfully.
}
