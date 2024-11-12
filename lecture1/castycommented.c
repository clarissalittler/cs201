#include <stdio.h>  // Includes the standard input/output library for functions like printf.
#include <stdlib.h> // Includes the standard library for general purpose functions.

int main() { // The main function, where the program execution begins.

  int ourInt = -10; // Declares an integer variable named 'ourInt' and initializes it with the value -10.
  unsigned int otherInt = (unsigned int)ourInt; // Declares an unsigned integer variable named 'otherInt' and initializes it by casting 'ourInt' (which is a signed integer) to an unsigned integer. 

  // This line performs a type cast, which converts the value stored in 'ourInt' from a signed integer to an unsigned integer.
  // The cast is necessary because the assignment would otherwise result in a compiler warning.

  printf("As an int %d as an unsigned %u\n", ourInt, otherInt); 
  // Prints the values of 'ourInt' and 'otherInt' to the console.
  // %d is a format specifier for integers and %u is a format specifier for unsigned integers.
  
  return 0; // Returns 0 to indicate successful program execution.
}
