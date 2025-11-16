#include <stdio.h> // Includes the standard input/output library, which provides functions like printf for printing to the console.
#include <stdlib.h> // Includes the standard library, which provides functions like malloc for dynamic memory allocation.

int main(){

  // Dynamically allocate memory for an array of 10 integers.
  // `malloc(10 * sizeof(int))` allocates 10 * sizeof(int) bytes of memory on the heap. 
  // `sizeof(int)` gets the size of an integer in bytes (usually 4 bytes). 
  // The cast `(int*)` converts the void pointer returned by malloc to an integer pointer.
  int* arr = (int*) malloc(10 * sizeof(int));

  // Fill the array with squares of numbers from 0 to 9.
  // The loop iterates 10 times, from i = 0 to i = 9.
  for(int i = 0; i < 10; i++){
    // Calculate the square of i and store it in the i-th element of the array.
    // `arr[i]` is equivalent to `*(arr + i)`, both access the i-th element of the array.
    arr[i] = i*i; // *(arr + i) = i * i;
  }

  // Print the contents of the array to the console.
  // The loop iterates 10 times, from i = 0 to i = 9.
  for(int i = 0; i < 10; i++){
    // Print the index and value of each element in the array.
    printf("arr[%d] = %d\n",i,arr[i]);
  }
  
  // Deallocate the memory allocated to the array to prevent memory leaks.
  // `free(arr)` releases the memory pointed to by `arr` back to the heap.
  free(arr);
  
  return 0; // Indicates successful program execution.
}
