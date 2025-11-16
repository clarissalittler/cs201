// PEDAGOGICAL PURPOSE:
// This program demonstrates fundamental differences between C and C++,
// introduces dynamic memory allocation with malloc/free,
// and shows pointer arithmetic basics.
// Key learning objectives:
// 1. C requires explicit #include for standard libraries
// 2. No "using namespace std" in C
// 3. Dynamic memory allocation requires manual management
// 4. Pointer arithmetic is a fundamental C concept
// 5. Array indexing is syntactic sugar for pointer arithmetic

#include "stdio.h"      // For printf function
#include "stdbool.h"    // For bool type (C99 standard)
#include "stdlib.h"     // For malloc and free functions

// This comment highlights a key C vs C++ difference:
// In C++ you would write "using namespace std;", but C has no namespaces
// no using namespace std;

// Struct definition in C - demonstrates basic structure syntax
// In C, you typically need to use "struct Rect" to refer to this type
// (unless you create a typedef)
struct Rect {
  int width;
  int height;
};

int main(){

  // DYNAMIC MEMORY ALLOCATION:
  // malloc allocates memory on the heap and returns a void* pointer
  // In C, we must cast it to the appropriate type (int*)
  // We allocate space for 100 integers
  // sizeof(int) ensures portability across different systems
  int* arr1 = malloc(100*sizeof(int));

  // Loop through all 100 elements
  for(int i=0; i<100; i++){
    // POINTER ARITHMETIC DEMONSTRATION:
    // This line shows pointer arithmetic: (arr1 + i) moves the pointer
    // forward by i * sizeof(int) bytes
    // Dereferencing with * stores i*i at that memory location
    *(arr1 + i) = i*i;

    // ARRAY INDEXING AS SYNTACTIC SUGAR:
    // arr1[i] is exactly equivalent to *(arr1 + i)
    // This demonstrates that array indexing is just pointer arithmetic
    // We use both forms here to show they're interchangeable
    printf("The %d th element is %d\n",i,arr1[i]);
  }

  // MANUAL MEMORY MANAGEMENT:
  // In C, we must explicitly free dynamically allocated memory
  // Failing to do this causes memory leaks
  // This is a key difference from languages with garbage collection
  free(arr1);

  return 0;
}
