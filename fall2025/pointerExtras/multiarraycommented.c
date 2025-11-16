// PEDAGOGICAL PURPOSE:
// This program demonstrates dynamic allocation of 2D arrays using pointer-to-pointer approach.
// Key learning objectives:
// 1. Understanding pointer-to-pointer (int**) for representing 2D arrays
// 2. Learning the two-step malloc process for 2D dynamic arrays
// 3. Understanding that arr[i][j] is syntactic sugar for *(*(arr + i) + j)
// 4. Seeing how array notation works with dynamically allocated memory
// 5. Understanding memory layout of pointer-based 2D arrays
// 6. Recognizing that this creates an "array of pointers to arrays"
// 7. Learning the relationship between bracket notation and pointer dereferencing

#include "stdio.h"
#include "stdlib.h"

// IMPORTANT CONCEPTUAL NOTE FROM ORIGINAL:
// This demonstrates the pointers-to-pointers way of creating
// dynamically allocated multi-dimensional arrays
// This is ONE approach - there are others (like allocating a single
// contiguous block), but this method is more flexible

int main(){

  // STEP 1: ALLOCATE THE "OUTER" ARRAY (ARRAY OF POINTERS)
  //
  // We're creating a 10x10 2D array
  //
  // int** arr means: arr is a pointer to a pointer to int
  // Breaking it down:
  //   - int** arr is a pointer that will hold the address of the first row pointer
  //   - We allocate space for 10 pointers (int*)
  //   - Each pointer will eventually point to a row of integers
  //
  // malloc(10*sizeof(int*)) allocates space for 10 pointers
  // Each pointer is 8 bytes on 64-bit systems
  // So this allocates 10 * 8 = 80 bytes
  //
  // MEMORY LAYOUT AFTER THIS LINE:
  // arr -> [ptr0][ptr1][ptr2][ptr3][ptr4][ptr5][ptr6][ptr7][ptr8][ptr9]
  //        (each is an int*, currently uninitialized/garbage)
  //
  // WHY THIS DESIGN?
  // - Allows non-contiguous rows (flexibility)
  // - Each row can potentially be a different size (not used here, but possible)
  // - Mirrors the syntax of 2D arrays (arr[i][j])
  int** arr = malloc(10*sizeof(int*));

  // STEP 2: ALLOCATE EACH ROW
  //
  // For each of the 10 row pointers, allocate space for 10 integers
  for(int i = 0; i<10; i++){

    // CRITICAL UNDERSTANDING: arr[i] IS REALLY *(arr + i)
    // This comment from the original code is KEY to understanding pointers!
    //
    // arr[i] means: "dereference the pointer that's i positions from arr"
    // Since arr is int**, arr[i] gives us an int* (a pointer to int)
    // We're setting that pointer to point to a newly allocated array of 10 ints
    //
    // EQUIVALENT EXPRESSIONS:
    // arr[i] = malloc(10*sizeof(int));     // What we write
    // *(arr + i) = malloc(10*sizeof(int)); // What it really means
    //
    // malloc(10*sizeof(int)) allocates space for 10 integers
    // 10 * 4 bytes = 40 bytes per row
    //
    // MEMORY LAYOUT AFTER FIRST ITERATION (i=0):
    // arr -> [ptr0] -> [int][int][int][int][int][int][int][int][int][int]
    //        [ptr1][ptr2][ptr3][ptr4][ptr5][ptr6][ptr7][ptr8][ptr9]
    //
    // After all iterations, we have:
    // arr -> [ptr0] -> [row 0: 10 ints]
    //        [ptr1] -> [row 1: 10 ints]
    //        [ptr2] -> [row 2: 10 ints]
    //        ...
    //        [ptr9] -> [row 9: 10 ints]
    arr[i] = malloc(10*sizeof(int));
  }

  // STEP 3: POPULATE THE 2D ARRAY
  //
  // Now we fill each position with a value
  // Using nested loops to traverse rows and columns
  for(int i=0; i < 10; i++){      // i is the row index (0-9)
    for(int j=0; j < 10; j++){    // j is the column index (0-9)

      // DOUBLE BRACKET NOTATION: arr[i][j]
      // This is syntactic sugar that hides pointer arithmetic!
      //
      // WHAT IT REALLY MEANS:
      // arr[i][j] is equivalent to *(*(arr + i) + j)
      //
      // Let's break it down step by step:
      // 1. arr is a pointer to pointer (int**)
      // 2. arr + i gives us the address of the i-th row pointer
      // 3. *(arr + i) dereferences to get the i-th row pointer (int*)
      // 4. (*(arr + i)) + j adds j to that row pointer
      // 5. *((*(arr + i)) + j) dereferences to get the actual int
      //
      // SIMPLIFIED:
      // arr[i] gets us the pointer to row i
      // arr[i][j] gets us the j-th element in row i
      //
      // THE VALUE WE'RE STORING:
      // 100*(i+1) + j creates unique values for visualization
      // Row 0: 100, 101, 102, ..., 109
      // Row 1: 200, 201, 202, ..., 209
      // Row 2: 300, 301, 302, ..., 309
      // etc.
      arr[i][j] = 100*(i+1) + j;
    }
  }

  // STEP 4: DISPLAY THE 2D ARRAY
  //
  // Print out the array to verify it was created and populated correctly
  for(int i=0; i < 10; i++){
    for(int j=0; j < 10; j++){
      // %d prints each integer value
      // The space after %d separates values horizontally
      printf("%d ", arr[i][j]);
    }
    // After each row, print a newline to move to the next row
    printf("\n");
  }

  // MEMORY LEAK WARNING!
  // This program does NOT free the allocated memory
  // In a proper program, you would need to:
  // 1. Free each row: for(int i=0; i<10; i++) free(arr[i]);
  // 2. Free the array of pointers: free(arr);
  // The order matters - free rows first, then the array of pointers!

  return 0;
}

// EXPECTED OUTPUT:
// 100 101 102 103 104 105 106 107 108 109
// 200 201 202 203 204 205 206 207 208 209
// 300 301 302 303 304 305 306 307 308 309
// 400 401 402 403 404 405 406 407 408 409
// 500 501 502 503 504 505 506 507 508 509
// 600 601 602 603 604 605 606 607 608 609
// 700 701 702 703 704 705 706 707 708 709
// 800 801 802 803 804 805 806 807 808 809
// 900 901 902 903 904 905 906 907 908 909
// 1000 1001 1002 1003 1004 1005 1006 1007 1008 1009

// CONCEPTUAL UNDERSTANDING:
//
// WHY USE int** FOR 2D ARRAYS?
// The int** approach creates a "ragged array" structure where:
// - Each row can be allocated separately
// - Rows don't have to be contiguous in memory
// - Each row can theoretically be a different size
//
// VISUALIZATION OF MEMORY:
//
// Stack/Local Variables:
// arr: [address of outer array]
//
// Heap Memory:
// Outer array (10 pointers):
// [0x2000][0x2100][0x2200][0x2300][0x2400][0x2500][0x2600][0x2700][0x2800][0x2900]
//    ↓       ↓       ↓       ↓       ↓       ↓       ↓       ↓       ↓       ↓
// Row 0   Row 1   Row 2   Row 3   Row 4   Row 5   Row 6   Row 7   Row 8   Row 9
// [10     [10     [10     [10     [10     [10     [10     [10     [10     [10
//  ints]   ints]   ints]   ints]   ints]   ints]   ints]   ints]   ints]   ints]
//
// Note: Rows are not necessarily contiguous! They could be anywhere in the heap.
//
// ALTERNATIVE APPROACH (not shown here):
// You could allocate one contiguous block:
//   int* flat = malloc(10 * 10 * sizeof(int));
//   // Access element [i][j] as: flat[i * 10 + j]
// This uses less memory (no extra pointers) but requires manual index calculation
//
// COMPARISON TO STATIC 2D ARRAYS:
// Static:  int arr[10][10];        // All on stack, fixed size, contiguous
// Dynamic: int** arr = ...;        // On heap, flexible size, non-contiguous rows
//
// THE BRACKET NOTATION EQUIVALENCE:
// arr[i][j]  ≡  *(*(arr + i) + j)  ≡  *(arr[i] + j)
//
// Step by step for arr[3][5]:
// 1. arr + 3 = address of the 3rd row pointer
// 2. *(arr + 3) = the 3rd row pointer itself (int*)
// 3. (*(arr + 3)) + 5 = address of the 5th element in that row
// 4. *((*(arr + 3)) + 5) = the value at position [3][5]
//
// WHY THIS MATTERS:
// 1. Dynamic sizing - can create arrays of any size at runtime
// 2. Heap allocation - not limited by stack size
// 3. Persistence - can return from functions (unlike stack arrays)
// 4. Understanding the relationship between pointers and arrays
// 5. Foundation for understanding pointer-to-pointer in other contexts
//
// COMMON MISTAKES:
// 1. Forgetting the second malloc (allocating rows)
// 2. Not freeing memory (memory leak)
// 3. Freeing in wrong order (free outer array before rows = memory leak)
// 4. Confusing int** with a true 2D array (memory layout is different!)
// 5. Accessing out of bounds (no automatic bounds checking)
//
// TRY IT:
// 1. Compile: gcc multiarray.c -o multiarray
// 2. Run: ./multiarray
// 3. Try changing the array size (make it 5x5 or 15x15)
// 4. Add proper free() calls to prevent memory leaks
// 5. Use valgrind to check for memory leaks: valgrind ./multiarray
// 6. Experiment with making a "ragged array" where rows have different sizes
