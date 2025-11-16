#include <stdio.h> // This line includes the standard input/output library, which provides functions for interacting with the user, like printing to the console (printf).

int main(){ // The main function is the entry point of the program. Execution begins here.

  float test = 0; // Declare a floating-point variable named 'test' and initialize it to 0. This variable will store the result of our calculation.

  for(int i = 0; i < 10; i++){ // This loop iterates 10 times (from i=0 to i=9). It's a common way to repeat a block of code.
    test = test + 0.01; // In each iteration, the value 0.01 is added to the 'test' variable.
  }

  // After the loop, the variable 'test' should theoretically be 0.1 (0.01 added 10 times). 
  // However, this is where the problem arises.

  if(test == 0.1){ // This conditional statement checks if the value of 'test' is exactly equal to 0.1.
    printf("Everything's good\n"); // If the condition is true (test is exactly 0.1), this message is printed.
  }
  else{ // This block executes if the condition in the 'if' statement is false.
    printf("Whoops\n"); // This message is printed if the 'test' value is not exactly 0.1.
    printf("Test is actually: %f\n",test); // This line prints the actual value stored in the 'test' variable, showing the difference from the expected 0.1.
  }
  
  return 0; // This line returns 0 to the operating system, indicating that the program executed successfully.
}
