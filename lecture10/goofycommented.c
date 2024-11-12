#include <stdio.h> // This line includes the standard input/output library, which provides functions like printf for printing to the console.

// This function takes two integers as input and calculates a result based on them.
int goofyfunction(int a, int b){
  // It calculates the product of the two integers (a * b) and then adds the first integer (a) to the result.
  return (a * b + a); 
}

// This is the main function, where the program starts execution.
int main(){
  // This line declares a character pointer named msg and initializes it with a string literal.
  char* msg = "This is silly\n"; // The string literal is a sequence of characters enclosed in double quotes, and \n is a newline character.

  // These lines declare two integer variables and assign them values.
  int num1 = 5;
  int num2 = 6;

  // This line calls the goofyfunction with num1 and num2 as arguments and stores the returned value in num3.
  int num3 = goofyfunction(num1,num2);

  // This line prints the string stored in msg and the value of num3 to the console.
  printf("%s %d",msg,num3); // %s is a format specifier for strings, and %d is a format specifier for integers.

  // This line returns 0, indicating that the program executed successfully.
  return 0;
}
