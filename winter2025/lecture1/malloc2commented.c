// PEDAGOGICAL PURPOSE:
// This program demonstrates allocating a large array dynamically with malloc,
// showing that the heap can accommodate much larger data than the stack.
// Key learning objectives:
// 1. Allocating arrays dynamically with malloc
// 2. Understanding heap vs stack memory limits
// 3. Using sizeof for calculating allocation sizes
// 4. Type casting with malloc (array pointers)
// 5. The importance of free to prevent memory leaks
// 6. How malloc enables runtime-sized allocations

#include <stdio.h>
#include <stdlib.h>      // For malloc() and free()

int main(){

  // LARGE DYNAMIC ARRAY ALLOCATION:
  // This allocates space for 10 million integers on the HEAP
  //
  // BREAKDOWN OF THE ALLOCATION:
  // - 10000000 elements
  // - sizeof(int) bytes per element (typically 4 bytes)
  // - Total: 10,000,000 × 4 = 40,000,000 bytes = 40 MB
  //
  // WHY THIS WORKS:
  // The heap is much larger than the stack
  // Stack: typically 1-8 MB (varies by system)
  // Heap: limited by available RAM and system limits (gigabytes)
  int* arr = malloc(10000000*sizeof(int));

  // WHAT IF WE TRIED THIS ON THE STACK?
  // int arr[10000000];  // STACK OVERFLOW! Would likely crash
  // The stack is too small for such large allocations
  // This is why malloc exists - for large or dynamic allocations

  // MALLOC DETAILS:
  // - malloc returns void* (generic pointer)
  // - In C, void* automatically converts to int*
  // - malloc allocates contiguous memory (all in one block)
  // - Memory is UNINITIALIZED (contains garbage values)
  // - Returns NULL if allocation fails (out of memory)

  // MEMORY REPRESENTATION:
  // After successful allocation, memory looks like:
  //
  // Heap (simplified):
  //   Address          Content
  //   0x12340000       arr[0] (4 bytes, uninitialized)
  //   0x12340004       arr[1] (4 bytes, uninitialized)
  //   0x12340008       arr[2] (4 bytes, uninitialized)
  //   ...
  //   0x14A31BFC       arr[9999999] (4 bytes, uninitialized)
  //
  // Stack:
  //   arr: 0x12340000 (pointer variable, stores address of first element)

  // THE COMMENT EXPLAINS THE PURPOSE:
  // "Creates an array that's 10000000 elements wide"
  // This is exactly what we've done with malloc
  // Unlike stack arrays, the size could even be determined at runtime:
  // int n;
  // scanf("%d", &n);
  // int* arr = malloc(n * sizeof(int));  // Size not known at compile time!

  // MEMORY CLEANUP:
  // free() releases the allocated memory back to the system
  // CRITICAL: Without this, we have a 40 MB memory leak!
  //
  // What happens when we call free:
  // 1. The memory becomes available for future allocations
  // 2. The pointer arr still holds the address (now invalid)
  // 3. Accessing arr after free is undefined behavior
  free(arr);

  // GOOD PRACTICE (not shown here):
  // arr = NULL;  // Set to NULL after freeing
  // This makes bugs more obvious (dereferencing NULL crashes immediately)

  return 0;
}

// COMPREHENSIVE EXPLANATION:
//
// HEAP vs STACK:
//
// STACK:
// - Automatic storage duration
// - Limited size (typically 1-8 MB, set at program start)
// - Fast allocation (just moving stack pointer)
// - Automatic deallocation (when function returns)
// - Memory is contiguous and organized in frames
// - Grows downward (on most systems)
// - Used for local variables, function parameters
//
// HEAP:
// - Dynamic storage duration
// - Much larger (gigabytes, limited by RAM/swap)
// - Slower allocation (must find suitable free block)
// - Manual deallocation (must call free)
// - Memory can be fragmented
// - No specific growth direction
// - Used for large data, data that outlives function, runtime-sized data

// CALCULATING ALLOCATION SIZE:
//
// sizeof(int) * count is the standard pattern
// WHY multiply by sizeof(int)?
// - We need to know BYTES to allocate
// - Each int is sizeof(int) bytes (typically 4)
// - For 10,000,000 ints: 10,000,000 × 4 = 40,000,000 bytes
//
// PORTABILITY:
// Using sizeof(int) makes code portable
// On some systems, int might be 2 bytes
// On others, 4 bytes or even 8 bytes
// sizeof ensures we always allocate the right amount

// WHAT IF MALLOC FAILS?
//
// malloc returns NULL when it can't allocate memory
// Causes:
// - Out of memory (RAM exhausted)
// - System limits (ulimit on Unix)
// - Memory fragmentation (no contiguous block large enough)
//
// PROPER ERROR HANDLING (not shown in this simple example):
// int* arr = malloc(10000000 * sizeof(int));
// if (arr == NULL) {
//   fprintf(stderr, "Memory allocation failed!\n");
//   return 1;  // Exit with error code
// }
// // Now safe to use arr
// free(arr);

// MEMORY INITIALIZATION:
//
// malloc does NOT initialize memory
// The array contains whatever garbage was in that memory before
//
// If you need initialized memory:
// 1. Use calloc (allocates and zeros):
//    int* arr = calloc(10000000, sizeof(int));
//    // All elements are 0
//
// 2. Manually initialize with memset:
//    int* arr = malloc(10000000 * sizeof(int));
//    memset(arr, 0, 10000000 * sizeof(int));
//    // All elements are 0
//
// 3. Loop through and initialize:
//    for (int i = 0; i < 10000000; i++) {
//      arr[i] = 0;  // or any initial value
//    }

// USING THE ALLOCATED ARRAY:
//
// Once allocated, you can use arr like any array:
//
// arr[0] = 42;              // Set first element
// arr[9999999] = 100;       // Set last element
// int x = arr[5000000];     // Read middle element
//
// for (int i = 0; i < 10000000; i++) {
//   arr[i] = i * i;         // Initialize with squares
// }
//
// int sum = 0;
// for (int i = 0; i < 10000000; i++) {
//   sum += arr[i];          // Sum all elements
// }

// POINTER ARITHMETIC:
//
// arr is a pointer to the first element
// arr + i points to the i-th element
// *(arr + i) accesses the i-th element
// arr[i] is syntactic sugar for *(arr + i)
//
// These are equivalent:
//   arr[0]     ≡  *(arr + 0)  ≡  *arr
//   arr[5]     ≡  *(arr + 5)
//   arr[9999999] ≡  *(arr + 9999999)

// MEMORY LEAKS:
//
// If we forget to call free:
// int* arr = malloc(10000000 * sizeof(int));
// // ... use arr ...
// return 0;  // LEAK! 40 MB not freed
//
// The memory remains allocated until the program exits
// For short programs, OS reclaims it at exit
// For long-running programs (servers, daemons), leaks accumulate
//
// Finding leaks:
// $ valgrind --leak-check=full ./malloc2
// (Shows all memory leaks with stack traces)

// COMMON MISTAKES:
//
// 1. WRONG SIZEOF:
//    int* arr = malloc(10000000 * sizeof(arr));  // WRONG! sizeof(pointer)
//    CORRECT:
//    int* arr = malloc(10000000 * sizeof(int));
//    OR:
//    int* arr = malloc(10000000 * sizeof(*arr));  // sizeof what arr points to
//
// 2. ARITHMETIC OVERFLOW:
//    size_t size = 10000000 * sizeof(int);  // Could overflow on 32-bit!
//    SAFER:
//    size_t size = (size_t)10000000 * sizeof(int);
//
// 3. FORGETTING TO FREE:
//    int* arr = malloc(10000000 * sizeof(int));
//    return 0;  // LEAK!
//
// 4. USING AFTER FREE:
//    int* arr = malloc(10000000 * sizeof(int));
//    free(arr);
//    arr[0] = 42;  // UNDEFINED BEHAVIOR!
//
// 5. DOUBLE FREE:
//    free(arr);
//    free(arr);  // UNDEFINED BEHAVIOR!

// COMPARING ALLOCATION METHODS:
//
// STACK ARRAY (compile-time size):
//   int arr[100];
//   - Fast allocation
//   - Automatic cleanup
//   - Size must be constant
//   - Limited by stack size
//
// MALLOC (runtime size):
//   int* arr = malloc(n * sizeof(int));
//   - Slower allocation
//   - Manual cleanup required
//   - Size can be variable
//   - Limited by heap size (much larger)
//
// VARIABLE LENGTH ARRAY / VLA (C99, runtime size on stack):
//   int n = 100;
//   int arr[n];
//   - Automatic cleanup
//   - Runtime size
//   - Still limited by stack size
//   - Not supported in all C++ compilers
//   - Not recommended for large sizes

// SYSTEM LIMITS:
//
// Check memory limits on Unix:
// $ ulimit -a
// Shows various limits including max memory size
//
// This program allocates 40 MB
// On most modern systems, this is trivial
// But on embedded systems or with strict limits, it might fail

// TO COMPILE AND RUN:
// gcc malloc2.c -o malloc2
// ./malloc2
// (No output, but successfully allocates and frees 40 MB)
//
// TO CHECK MEMORY USAGE:
// On Linux, use time and /usr/bin/time:
// /usr/bin/time -v ./malloc2
// Shows "Maximum resident set size" (peak memory usage)
//
// TO CHECK FOR LEAKS:
// valgrind ./malloc2
// Should report: "All heap blocks were freed -- no leaks are possible"
