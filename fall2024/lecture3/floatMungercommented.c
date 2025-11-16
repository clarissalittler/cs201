#include <stdlib.h> // Includes standard library functions, such as malloc() and free()
#include <stdio.h>  // Includes standard input/output functions, such as printf() and scanf()

// This function toggles the bit at the specified position in a given integer.
void toggle(int* target, int place) {
  // Creates a bit mask with a 1 at the specified position and 0s elsewhere.
  int shifty = 1 << place; 
  // Performs a bitwise XOR operation between the target integer and the bit mask. 
  // This effectively toggles the bit at the specified position.
  *target = *target ^ shifty;
}

// This function checks if the bit at the specified position in a given integer is set.
int isSet(int* target, int place) {
  // Shifts the target integer to the right by the specified position.
  // This brings the bit at the specified position to the least significant bit (LSB) position.
  // Then, performs a bitwise AND operation with 1.
  // If the bit at the specified position was set, the result will be 1, otherwise it will be 0.
  return ((*target >> place) & 1); 
}

// This function prints the binary representation of a given integer.
void printBits(int* target) {
  // Iterates through all 32 bits of the integer, from the most significant bit to the least significant bit.
  for(int i=31; i >=0; i--){
    // Calls the isSet() function to check if the bit at the current position is set.
    // Prints 1 if the bit is set, and 0 if it is not set.
    printf("%d",isSet(target,i));
  }
}

int main(){
  /*
  float testNum = 0.3333;
  // The following lines demonstrate how to toggle specific bits within a floating-point number.
  // The code is commented out for clarity, as it focuses on the main functionality in the next block.
  // However, you can uncomment these lines to experiment with bit manipulation on floating-point numbers.
  toggle((int*)&testNum,12);
  toggle((int*)&testNum,30);
  toggle((int*)&testNum,24);*/

  // Initializes two floating-point variables: targetNum and testNum.
  float targetNum = 0.1;
  float testNum = 0;

  // This loop demonstrates the addition of 0.01 to testNum ten times.
  // It's worth noting that floating-point arithmetic can lead to imprecision.
  // This means that the result of adding 0.01 ten times might not be exactly 0.1 due to rounding errors.
  for(int i =0; i < 10; i++){
    testNum = testNum+0.01;
  }
  /*
  // These lines demonstrate the toggling of specific bits within testNum.
  // Again, this block is commented out to focus on the core functionality, but you can uncomment it to experiment.
  toggle((int*)&testNum,31);
  toggle((int*)&testNum,30);
  toggle((int*)&testNum,29);
  toggle((int*)&testNum,28);
  toggle((int*)&testNum,27);
  toggle((int*)&testNum,26);
  toggle((int*)&testNum,25);
  toggle((int*)&testNum,24);
  toggle((int*)&testNum,23);

  toggle((int*)&testNum,0);
  */

  // Prints the binary representation of targetNum.
  printf("The bits set in the number %.12f are:\n",targetNum);
  printBits((int*) &targetNum); // Note: This casts the address of targetNum to an integer pointer for bitwise operations.
  printf("\n");

  // Prints the binary representation of testNum.
  printf("The bits set in the number %.12f are:\n",testNum);
  printBits((int*) &testNum); 
  printf("\n");

  
  return 0;
}
