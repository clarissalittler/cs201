// PEDAGOGICAL PURPOSE:
// This program demonstrates dynamic allocation of arrays using malloc.
// Key learning objectives:
// 1. Allocating arrays dynamically on the heap
// 2. Using malloc to create arrays of structs
// 3. Array notation with malloc'd memory (pointer is treated as array)
// 4. The relationship between pointers and arrays in C
// 5. Proper memory management - freeing dynamically allocated arrays
// 6. Calculating allocation size for multiple elements
//
// KEY CONCEPT:
// malloc can allocate space for multiple elements (arrays) by multiplying
// the size of one element by the number of elements needed. The returned
// pointer can then be used with array notation (brackets) just like a
// regular array.

#include <stdio.h>
#include <stdlib.h>

// MALLOC FUNDAMENTALS:
// malloc takes a size in bytes as an argument
// and returns a pointer to a region of memory of that size
//
// For ARRAYS, we allocate: number_of_elements × size_of_one_element
//
// Key differences from C++ 'new':
// - malloc returns void* (generic pointer) - YOU must cast it
// - malloc does NOT call constructors (C has no constructors)
// - malloc takes SIZE IN BYTES (not just element count)
// - It's YOUR responsibility to cast to the correct type
// - malloc does NOT initialize the memory (it contains garbage values)

// STRUCT DEFINITION:
// A simple struct named Garbage with two integer fields
// The name "Garbage" reminds us that malloc'd memory contains
// garbage values until we explicitly initialize them
struct Garbage {
  int n1;   // First integer field
  int n2;   // Second integer field
};

int main(){
  // DYNAMIC ARRAY ALLOCATION:
  // This line allocates space for 10 struct Garbage elements
  // BREAKDOWN:
  // 1. sizeof(struct Garbage) = 8 bytes (on most systems)
  // 2. 10 × 8 = 80 bytes total
  // 3. malloc(80) allocates 80 contiguous bytes on the heap
  // 4. Returns void* pointing to the start of this memory block
  // 5. (struct Garbage*) casts to the appropriate pointer type
  // 6. Result: g points to the first element of an array of 10 struct Garbage
  struct Garbage* g =
    (struct Garbage*) malloc(10 * sizeof(struct Garbage)); // make an array of 10 struct Garbage

  // CRITICAL INSIGHT: POINTERS AND ARRAYS ARE INTERCHANGEABLE
  // After this malloc, g can be used exactly like an array:
  // - g[0] accesses the first element
  // - g[1] accesses the second element
  // - g[9] accesses the tenth element
  //
  // In C, array notation is just pointer arithmetic:
  // g[i] is equivalent to *(g + i)

  // WHY THIS ALLOCATION SIZE?
  // 10 × sizeof(struct Garbage) ensures we allocate exactly enough space
  // for 10 complete struct Garbage objects
  // Each element needs 8 bytes, so total is 80 bytes
  //
  // WRONG: malloc(10)  // Only 10 bytes - not enough for 10 structs!
  // RIGHT: malloc(10 * sizeof(struct Garbage))  // 80 bytes - correct!

  // ARRAY NOTATION WITH DYNAMICALLY ALLOCATED MEMORY:
  // Even though g is a pointer, we can use array notation with it
  // g[0] accesses the first element of our dynamically allocated array
  // The dot operator (.) is used because g[0] gives us the struct itself,
  // not a pointer to it
  g[0].n1 = 10;  // Set first field of first element to 10
  g[0].n2 = 20;  // Set second field of first element to 20

  // UNDERSTANDING THE SYNTAX:
  // g is a pointer to struct Garbage
  // g[0] is equivalent to *(g + 0), which dereferences to get the actual struct
  // Since g[0] IS the struct (not a pointer to it), we use dot operator (.)
  // Compare: g->n1 would access the first field of the struct that g points to
  //          g[0].n1 accesses the first field of the first element in the array

  // PRINTING ARRAY ELEMENTS:
  // We use array notation with dot operator to access struct members
  printf("g[0].n1 is %d\n",g[0].n1);  // Output: g[0].n1 is 10
  printf("g[0].n2 is %d\n",g[0].n2);  // Output: g[0].n2 is 20

  // MEMORY CLEANUP:
  // free() returns the allocated memory back to the operating system
  // IMPORTANT: free() works for both single elements and arrays
  // - If you allocated with malloc(size), free it with free(pointer)
  // - It doesn't matter if size was for 1 element or 100 elements
  // - free() uses metadata stored by malloc to know how much to free
  free(g);

  // After free():
  // - g becomes a "dangling pointer" (points to memory we no longer own)
  // - Using g after this point is UNDEFINED BEHAVIOR
  // - Good practice: set g = NULL after free() to catch errors

  return 0;
}

// CONCEPTUAL EXPLANATION:
//
// DYNAMIC ARRAY ALLOCATION FORMULA:
// pointer = (type*) malloc(number_of_elements × sizeof(type));
//
// Examples:
// int* arr = malloc(100 * sizeof(int));              // 100 integers
// struct Garbage* g = malloc(10 * sizeof(struct Garbage));  // 10 structs
// char* str = malloc(50 * sizeof(char));             // 50 characters (string)
//
// MEMORY LAYOUT:
//
// Stack:                  Heap (80 bytes allocated):
// --------                --------------------------------
// main's frame            [g[0].n1: 10 ] (4 bytes)
//   g (pointer)    --->   [g[0].n2: 20 ] (4 bytes)
//   (8 bytes)             [g[1].n1: ??] (4 bytes) } garbage
//                         [g[1].n2: ??] (4 bytes) }
//                         [g[2].n1: ??] (4 bytes) }
//                         [g[2].n2: ??] (4 bytes) } garbage
//                         ...
//                         [g[9].n1: ??] (4 bytes) }
//                         [g[9].n2: ??] (4 bytes) } garbage
//
// Only g[0] is initialized; g[1] through g[9] contain garbage values
//
// POINTER ARITHMETIC EXPLAINED:
// When you have: struct Garbage* g
// And you do: g[i]
// The compiler translates this to: *(g + i)
//
// How pointer arithmetic works:
// - g points to address, say, 0x1000
// - g + 1 doesn't mean 0x1001 !
// - g + 1 means 0x1000 + sizeof(struct Garbage) = 0x1008
// - g + 2 means 0x1000 + 2×sizeof(struct Garbage) = 0x1010
//
// This is why g[i] gives you the i-th element correctly!
//
// ARRAY ACCESS EQUIVALENCES:
// Given: struct Garbage* g = malloc(10 * sizeof(struct Garbage));
//
// These are all equivalent ways to access the first element's n1:
// g[0].n1          // Array notation with dot operator (clearest)
// (g + 0)->n1      // Pointer arithmetic with arrow operator
// g->n1            // Arrow operator on base pointer (g + 0 is just g)
// (*g).n1          // Dereference pointer, then access member
//
// For the second element:
// g[1].n1          // Array notation (clearest)
// (g + 1)->n1      // Pointer arithmetic with arrow operator
// (*(g + 1)).n1    // Dereference (g+1), then access member
//
// MALLOC vs STACK ARRAY COMPARISON:
//
// STACK ARRAY (automatic, fixed size):
// struct Garbage arr[10];     // Size must be known at compile time
// arr[0].n1 = 10;            // Use directly, no malloc needed
// // Automatically freed when function returns
// // Cannot return this array from function (it will be destroyed)
// // Limited by stack size (typically 1-8MB total)
//
// HEAP ARRAY (dynamic, flexible size):
// int count = get_user_input();  // Size can be determined at runtime
// struct Garbage* g = malloc(count * sizeof(struct Garbage));
// g[0].n1 = 10;              // Use exactly like stack array
// free(g);                   // Must manually free
// // Can return g from function (heap memory persists)
// // Limited by available RAM (much larger than stack)
//
// FREE() INTERNALS:
// How does free() know how much memory to free?
// - malloc stores metadata just before the allocated block
// - This metadata includes the size of the allocation
// - When you call free(g), it reads this metadata
// - It then frees the entire block (whether it's 8 bytes or 80 bytes)
//
// Simplified memory layout with metadata:
// [metadata: size=80] [your 80 bytes of data...]
// ^                   ^
// hidden from you     g points here
//
// COMMON MISTAKES:
//
// MISTAKE 1: Freeing individual elements (WRONG with malloc)
// for(int i = 0; i < 10; i++) {
//   free(&g[i]);  // WRONG! Only free the pointer you got from malloc
// }
// // CORRECT:
// free(g);  // Free the entire array at once
//
// MISTAKE 2: Allocating too little memory
// struct Garbage* g = malloc(10);  // Only 10 bytes, need 80!
// g[1].n1 = 5;  // Writes beyond allocated memory - UNDEFINED BEHAVIOR!
//
// MISTAKE 3: Off-by-one array access
// struct Garbage* g = malloc(10 * sizeof(struct Garbage));
// g[10].n1 = 5;  // WRONG! Valid indices are 0-9, not 0-10
//
// MISTAKE 4: Using after free
// free(g);
// g[0].n1 = 5;  // UNDEFINED BEHAVIOR! Memory no longer belongs to us
//
// MISTAKE 5: Double free
// free(g);
// free(g);  // CRASH! Can't free the same memory twice
//
// MISTAKE 6: Forgetting to free (memory leak)
// struct Garbage* g = malloc(10 * sizeof(struct Garbage));
// // ... use g ...
// // Never call free(g) - 80 bytes leaked!
//
// DOT vs ARROW OPERATOR RULES:
// Use . (dot) when you have the struct itself:
//   - Stack variable: struct Garbage g; g.n1 = 10;
//   - Array element: g[0].n1 = 10; (g[0] is the struct itself)
//   - Dereferenced pointer: (*ptr).n1 = 10;
//
// Use -> (arrow) when you have a pointer to the struct:
//   - Pointer variable: struct Garbage* g; g->n1 = 10;
//   - Malloc'd single struct: g->n1 = 10;
//   - Pointer arithmetic: (g+1)->n1 = 10;
//
// WHEN TO USE DYNAMIC ARRAYS:
// 1. Size not known until runtime (user input, file size, etc.)
// 2. Need very large arrays (stack is limited)
// 3. Need to return array from function
// 4. Need array to persist after function returns
// 5. Need to resize array later (allocate larger, copy, free old)
//
// BEST PRACTICES:
// 1. Always check malloc return value for NULL
// 2. Always free what you malloc (prevents memory leaks)
// 3. Set pointer to NULL after freeing (catches use-after-free bugs)
// 4. Use sizeof(type) instead of hardcoded sizes (portable code)
// 5. Comment why you're allocating a specific amount
//
// Example of robust allocation:
// struct Garbage* g = malloc(10 * sizeof(struct Garbage));
// if (g == NULL) {
//   fprintf(stderr, "Failed to allocate memory\n");
//   exit(1);
// }
// // Use g...
// free(g);
// g = NULL;  // Prevent accidental use after free
