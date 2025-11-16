#include <stdio.h>  // Include the standard input/output library for functions like printf
#include <stdlib.h> // Include the standard library for general-purpose functions

int main() {  // Main function - the entry point of the program

  char c;  // Declare a variable 'c' of type char (character)
  int i;  // Declare a variable 'i' of type int (integer)
  float f;  // Declare a variable 'f' of type float (single-precision floating-point number)
  double nd;  // Declare a variable 'nd' of type double (double-precision floating-point number)
  int* p1;  // Declare a variable 'p1' of type int* (pointer to an integer)
  double** p2;  // Declare a variable 'p2' of type double** (pointer to a pointer to a double)

  // Print the size of each data type using the sizeof operator
  printf("The size of a char is: %u\n", sizeof(c)); // Output: The size of a char in bytes
  printf("The size of an int is: %u\n", sizeof(i)); // Output: The size of an int in bytes
  printf("The size of a float is: %u\n", sizeof(f)); // Output: The size of a float in bytes
  printf("The size of a double is: %u\n", sizeof(nd)); // Output: The size of a double in bytes
  printf("The size of an int* is: %u\n", sizeof(p1)); // Output: The size of a pointer to an int in bytes
  printf("The size of a double** is: %u\n", sizeof(p2)); // Output: The size of a pointer to a pointer to a double in bytes

  return 0; // Return 0 to indicate successful program execution
}
