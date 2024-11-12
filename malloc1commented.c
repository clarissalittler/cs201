#include <stdlib.h> // Includes the standard library functions, including malloc and free
#include <stdio.h> // Includes the standard input/output functions, including printf and scanf

int main(){

  int* arr; // Declares a pointer to an integer, which will store the address of the dynamically allocated array

  int size; // Declares an integer variable to store the size of the array entered by the user

  printf("Enter the size of the array to allocate: "); // Prompts the user to enter the size of the array
  scanf("%d",&size); // Reads the integer entered by the user and stores it in the 'size' variable

  arr = (int*) malloc(size * sizeof(int)); // Dynamically allocates memory for an array of integers. 
                                         // 'malloc' allocates a block of memory of the specified size (size * sizeof(int)). 
                                         // The result, which is a memory address, is then cast to an integer pointer and assigned to 'arr'.
                                         // 'sizeof(int)' returns the size of an integer in bytes, ensuring that the correct amount of memory is allocated.

  for(int i=0; i < size; i++){
    arr[i] = i*i; // Iterates through the allocated array and assigns the square of the index (i*i) to each element.
  }

  for(int i=0; i < size; i++){
    printf("arr[%d] = %d\n",i,arr[i]); // Iterates through the array and prints the index (i) and the corresponding value (arr[i]) to the console.
  }

  free(arr); // Deallocates the memory previously allocated by malloc. This is crucial to prevent memory leaks.
  arr = NULL; // Sets the pointer 'arr' to NULL after freeing the memory. This helps prevent accidental use of the freed memory.

  return 0; // Indicates that the program executed successfully.
}
