#include <stdio.h> // Includes the standard input/output library, providing functions like printf() for printing to the console.
#include <stdlib.h> // Includes the standard library, providing general utility functions.

int main(){ // The main function, where the program execution begins.
  int arr1[10]; // Declares an integer array named 'arr1' with a size of 10 elements.
  double arr2[10]; // Declares a double-precision floating-point array named 'arr2' with a size of 10 elements.

  // This loop iterates 10 times, filling the arrays with calculated values.
  for(int i = 0; i < 10; i++){
    arr1[i] = i*i; // Assigns the square of the current loop index 'i' to the corresponding element in 'arr1'.
    arr2[i] = i*3.14; // Assigns the product of the current loop index 'i' and 3.14 (an approximation of pi) to the corresponding element in 'arr2'.
  }

  // Prints the value of the 4th element (index 3) in 'arr1' using the array notation.
  printf("arr1[3] = %d\n",arr1[3]); 
  // Prints the value of the 4th element (index 3) in 'arr1' using pointer arithmetic.
  // *(arr1 + 3) is equivalent to arr1[3] because 'arr1' points to the first element of the array and adding 3 to it moves the pointer 3 positions forward, essentially accessing the 4th element.
  printf("arr1[3] = %d\n",*(arr1 + 3));

  // Prints the value of the 4th element (index 3) in 'arr2' using the array notation.
  printf("arr2[3] = %f\n",arr2[3]); 
  // Prints the value of the 4th element (index 3) in 'arr2' using pointer arithmetic.
  // *(arr2 + 3) is equivalent to arr2[3] because 'arr2' points to the first element of the array and adding 3 to it moves the pointer 3 positions forward, essentially accessing the 4th element.
  printf("arr2[3] = %f\n",*(arr2 + 3));

  return 0; // Returns 0 to indicate successful program execution.
}
