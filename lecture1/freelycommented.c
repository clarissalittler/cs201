#include <stdio.h> // Include the standard input/output library for functions like printf
#include <stdlib.h> // Include the standard library for functions like malloc and free

int main(){
  int* intPoint; // Declare a pointer variable called 'intPoint' to store the address of an integer
  intPoint = (int*) malloc(sizeof(int)); // Allocate memory for an integer using malloc.
                                        // 'sizeof(int)' determines the size of an integer in bytes.
                                        // The result of malloc is a void pointer, which is cast to an int pointer.
  
  *intPoint = 10; // Dereference the pointer 'intPoint' (access the memory location it points to) 
                  // and store the value 10 in it.
  
  printf("the integer is: %d\n",*intPoint); // Print the value stored at the memory location pointed to by 'intPoint'.

  free(intPoint); // Release the allocated memory back to the system using free.
  intPoint = NULL; // Set the pointer to NULL to indicate that it no longer points to valid memory.

  printf("What is NULL: %p\n",NULL); // Print the address of NULL, which is often 0.
                                   // The %p format specifier is used to print pointer addresses.
  
  return 0; // Return 0 to indicate successful program execution.
}
