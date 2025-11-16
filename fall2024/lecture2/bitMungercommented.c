#include <stdlib.h> // Includes standard library functions, like malloc and free.
#include <stdio.h> // Includes standard input/output functions, like printf and scanf.

// This function toggles the nth bit in a number
void toggle(int* target, int place){
  // We use a pointer to the target variable to modify its value directly within the function.
  // place is the index of the bit we want to toggle, starting from 0 for the least significant bit.
  
  // Create a "mask" with a single 1 bit at the specified place. 
  // The left shift operator (<<) shifts the 1 bit 'place' positions to the left.
  int shifty = 1 << place; 
  // The XOR operator (^) toggles the bit at the specified place.
  // If the bit was 0, it becomes 1, and if it was 1, it becomes 0.
  *target = *target ^ shifty;
}

int isSet(int target, int place){
  // This function checks if the place-th bit is set in the target number.
  
  // Right shift (>>) the target number by 'place' positions to bring the desired bit to the least significant position.
  // Then, AND (&) the result with 1 to isolate the least significant bit.
  // The result will be 1 if the bit was set, and 0 otherwise.
  return ((target >> place) & 1);
}

int main(){
  // Initialize a test number to 0.
  int testNum = 0;
  
  // Toggle bits 30 and 29 of testNum.
  //toggle(&testNum,31); 
  toggle(&testNum,30);
  toggle(&testNum,29);

  // Print the bits set in testNum.
  printf("The bits set in the number %d are:\n",testNum);

  // Iterate through all bits from the most significant bit (31) to the least significant bit (0).
  for(int i=31; i >=0; i--){
    // Print 1 if the bit is set, and 0 if it is not.
    printf("%d",isSet(testNum,i));
  }
  printf("\n");

  /*
  // Uncomment the following code to test the functionality further.
  int num1 = 65;
  int num2 = 0xFFFF;

  // Check if the least significant bit of num1 is set.
  if(isSet(num1,0)){
    printf("Passed test 1\n");
  }
  
  // Toggle the 16th bit of num2.
  toggle(&num2,16);
  
  // Check if the toggle operation increased the value of num2 beyond 0xFFFF.
  if(num2 > 0xFFFF){
    printf("Passed test 2\n");
  }
  */
  
  return 0;
}
