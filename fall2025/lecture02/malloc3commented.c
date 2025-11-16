// PEDAGOGICAL PURPOSE:
// This program demonstrates sizeof with structs containing arrays.
// Key learning objectives:
// 1. Understanding that arrays inside structs are stored INLINE (not as pointers)
// 2. sizeof calculates the total memory footprint of a struct
// 3. Structs containing large arrays can be very large themselves
// 4. The difference between sizeof(struct) and sizeof(pointer to struct)
// 5. Why dynamically allocating large structs is often necessary
//
// KEY CONCEPT:
// When an array is a member of a struct, the ENTIRE array is stored within the struct.
// This is different from having a pointer to an array in the struct.

#include "stdio.h"
#include "stdlib.h"

// STRUCT DEFINITION:
// Goofus contains an array of 100 integers INLINE
// This means the struct itself is large: 100 ints × 4 bytes = 400 bytes
// The name "Goofus" is whimsical - perhaps suggesting this is not always
// the best design (large structs are expensive to copy)
struct Goofus {
  int arr[100];  // Array stored DIRECTLY in the struct, not as a pointer
};

int main(){

  // DYNAMIC ALLOCATION OF LARGE STRUCT:
  // We allocate memory for ONE Goofus struct on the heap
  // sizeof(struct Goofus) returns the total size needed for the entire struct
  // including all 100 integers in its array
  struct Goofus* g = malloc(sizeof(struct Goofus));

  // WHY ALLOCATE ON HEAP?
  // This struct is 400 bytes (quite large)
  // Stack space is limited (typically a few MB)
  // Large structs should be allocated on the heap to avoid stack overflow
  // Also, heap allocation allows the struct to outlive the function

  // SIZEOF DEMONSTRATION:
  // sizeof(struct Goofus) calculates the total memory footprint
  // Expected output on most systems: 400 bytes (100 integers × 4 bytes each)
  // %ld is the correct format specifier for size_t (returned by sizeof)
  printf("A goofus is %ld bytes big\n",sizeof(struct Goofus));

  // MEMORY CLEANUP:
  // Free the allocated memory
  // This returns 400 bytes back to the system
  free(g);

  // NOTE: We allocated the struct but never actually used it!
  // This program's purpose is purely educational - to demonstrate sizeof

  return 0;
}

// CONCEPTUAL EXPLANATION:
//
// ARRAYS IN STRUCTS vs ARRAYS AS POINTERS:
//
// INLINE ARRAY (what we have here):
// struct Goofus {
//   int arr[100];      // 400 bytes stored INSIDE the struct
// };
// sizeof(struct Goofus) = 400 bytes
// The array is PART OF the struct
//
// POINTER TO ARRAY (alternative design):
// struct Gallant {
//   int* arr;          // Only 8 bytes (size of pointer on 64-bit system)
// };
// sizeof(struct Gallant) = 8 bytes
// The array is SEPARATE from the struct (must allocate separately)
//
// MEMORY LAYOUT:
//
// For struct Goofus* g = malloc(sizeof(struct Goofus)):
//
// Heap:
// Address    Content
// --------   -------
// 0x1000     arr[0]
// 0x1004     arr[1]
// 0x1008     arr[2]
// ...
// 0x118C     arr[99]
//
// All 100 integers are stored consecutively in one memory block
//
// IMPLICATIONS:
//
// 1. COPYING IS EXPENSIVE:
//    struct Goofus g2 = *g;  // Copies all 400 bytes!
//    This is why we often pass pointers to large structs
//
// 2. MEMORY IS CONTIGUOUS:
//    All array elements are guaranteed to be adjacent in memory
//    This is cache-friendly (good for performance)
//
// 3. NO SEPARATE ALLOCATION NEEDED:
//    The array is automatically allocated when the struct is allocated
//    We don't need a separate malloc for arr
//
// 4. FIXED SIZE:
//    The array size is fixed at compile time (100 elements)
//    Cannot grow or shrink at runtime
//
// WHEN TO USE INLINE ARRAYS:
// - When the size is known and fixed
// - When you want automatic allocation/deallocation with the struct
// - When you want guaranteed contiguous memory
// - When the array is reasonably small
//
// WHEN TO USE POINTER TO ARRAY:
// - When the size is dynamic (determined at runtime)
// - When the array might be very large
// - When you want to share the array between multiple structs
// - When you want the struct itself to be small (for copying)
//
// COMMON MISTAKE:
// Students sometimes expect:
//   sizeof(struct Goofus) to return 8 (size of a pointer)
// But it actually returns 400 because the array is stored INLINE.
//
// COMPARISON:
// printf("Size of the struct: %ld\n", sizeof(struct Goofus));  // 400
// printf("Size of pointer to struct: %ld\n", sizeof(g));       // 8 (on 64-bit)
// printf("Size of the array: %ld\n", sizeof(g->arr));          // 400
// printf("Size of one element: %ld\n", sizeof(g->arr[0]));     // 4
