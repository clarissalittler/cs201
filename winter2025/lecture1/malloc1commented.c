// PEDAGOGICAL PURPOSE:
// This program demonstrates dynamic memory allocation with malloc, structs,
// pointer dereferencing, and the arrow operator for struct member access.
// Key learning objectives:
// 1. Using malloc() to allocate memory on the heap
// 2. Understanding struct definition and usage
// 3. Pointer-to-struct member access with (*ptr).member syntax
// 4. The arrow operator (->) as syntactic sugar for pointer dereferencing
// 5. The sizeof operator with structs
// 6. Memory management: malloc must be paired with free
// 7. Setting pointers to NULL after freeing (good practice)

#include <stdio.h>
#include <stdlib.h>      // For malloc() and free()

// STRUCT DEFINITION:
// A struct (structure) is a composite data type
// It groups related data members together
// Each instance of Point has its own x and y values
struct Point{
  int x;     // First member: 4 bytes (on most systems)
  int y;     // Second member: 4 bytes (on most systems)
};
// Total size: typically 8 bytes (but could have padding)
// Padding may be added for alignment depending on architecture

int main(){

  // SIZE INSPECTION:
  // sizeof operator returns the size in bytes
  // %lu is the format specifier for "long unsigned" (size_t type)
  // This demonstrates that sizeof works at compile time
  printf("The size of Point is: %lu\n",sizeof(struct Point));
  // Expected output: 8 (on most systems)
  // Note: C requires "struct Point" not just "Point" (unless using typedef)

  // DYNAMIC MEMORY ALLOCATION:
  // malloc (memory allocate) allocates memory on the HEAP
  //
  // WHY HEAP vs STACK?
  // - Stack: automatic storage, limited size, local to function
  // - Heap: manual management, larger, persists until freed
  //
  // MALLOC SYNTAX:
  // malloc(size_in_bytes) returns void*
  // sizeof(struct Point) ensures we allocate the correct amount
  // malloc returns pointer to the allocated memory
  // If malloc fails (out of memory), it returns NULL
  struct Point* pointy = malloc(sizeof(struct Point));

  // IMPORTANT: Memory from malloc is UNINITIALIZED
  // It contains whatever garbage values were there before
  // Always initialize before reading!

  // POINTER DEREFERENCING WITH DOT OPERATOR:
  // (*pointy).x means:
  //   1. Dereference pointy to get the struct
  //   2. Access the x member of that struct
  //
  // WHY PARENTHESES?
  // Without them: *pointy.x would be parsed as *(pointy.x)
  // The dot operator (.) has higher precedence than dereference (*)
  // So we need (*pointy).x to dereference first
  (*pointy).x = 10;
  (*pointy).y = 20;

  // PRINT USING DEREFERENCE SYNTAX:
  printf("The field x is %d and y is %d\n",(*pointy).x,(*pointy).y);
  // Output: The field x is 10 and y is 20

  // ARROW OPERATOR (->):
  // The arrow operator is syntactic sugar for pointer dereferencing
  // pointy->x is EXACTLY equivalent to (*pointy).x
  // It's more readable and commonly used with pointers to structs
  //
  // WHY -> EXISTS:
  // (*pointy).x is awkward and requires parentheses
  // pointy->x is cleaner and more intuitive
  // Both do the same thing: access member through pointer
  pointy->x = 50;
  pointy->y = 40;

  // PRINT USING ARROW SYNTAX:
  printf("The field x is %d and y is %d\n",pointy->x,pointy->y);
  // Output: The field x is 50 and y is 40

  // MEMORY CLEANUP:
  // free() returns allocated memory back to the system
  // CRITICAL: Every malloc should have a corresponding free
  // Failing to free causes MEMORY LEAKS
  //
  // What free does:
  // - Marks the memory as available for future allocations
  // - Does NOT clear the memory or set the pointer to NULL
  // - After free, pointy is a "dangling pointer" (points to freed memory)
  free(pointy);

  // SETTING POINTER TO NULL:
  // Good practice: set pointer to NULL after freeing
  // Benefits:
  // - Dereferencing NULL crashes immediately (easier to debug)
  // - Calling free(NULL) is safe (does nothing)
  // - Prevents use-after-free bugs from being silent
  pointy = 0;     // 0 and NULL are equivalent for pointers
                  // In modern C, NULL is preferred for clarity

  return 0;
}

// COMPREHENSIVE EXPLANATION:
//
// MEMORY LAYOUT:
// When we allocate a Point struct, memory looks like:
//
// Heap memory (example addresses):
//   Address    Content
//   --------   -------
//   0x1000     x value (4 bytes)
//   0x1004     y value (4 bytes)
//
// Stack memory:
//   pointy: 0x1000 (stores the address of the heap memory)
//
// After pointy->x = 50:
//   0x1000: 50 (x field)
//   0x1004: 40 (y field)

// STRUCT MEMBER ACCESS SYNTAX COMPARISON:
//
// When you have a struct VARIABLE:
//   struct Point p;
//   p.x = 10;        // Use dot operator
//   p.y = 20;        // Direct member access
//
// When you have a POINTER to struct:
//   struct Point* ptr;
//   (*ptr).x = 10;   // Dereference then access (verbose)
//   ptr->x = 10;     // Arrow operator (preferred)
//
// Both pointer syntaxes are equivalent:
//   (*ptr).x  ≡  ptr->x
//   (*ptr).y  ≡  ptr->y

// WHY MALLOC RETURNS void*:
//
// void* is a "generic pointer" - points to any type
// In C, void* automatically converts to any pointer type
// So this is valid:
//   struct Point* pointy = malloc(sizeof(struct Point));
//
// In C++ you must cast:
//   struct Point* pointy = (struct Point*)malloc(sizeof(struct Point));
//
// But in C, the cast is optional (though some style guides require it)

// COMMON MALLOC MISTAKES:
//
// 1. FORGETTING TO CHECK FOR NULL:
//    struct Point* p = malloc(sizeof(struct Point));
//    p->x = 10;  // CRASH if malloc failed!
//
//    BETTER:
//    struct Point* p = malloc(sizeof(struct Point));
//    if (p == NULL) {
//      fprintf(stderr, "malloc failed\n");
//      return 1;
//    }
//    p->x = 10;  // Safe
//
// 2. WRONG SIZE:
//    struct Point* p = malloc(sizeof(p));  // WRONG! sizeof(pointer), not sizeof(struct)
//    CORRECT:
//    struct Point* p = malloc(sizeof(struct Point));
//    OR:
//    struct Point* p = malloc(sizeof(*p));  // sizeof what p points to
//
// 3. MEMORY LEAK:
//    struct Point* p = malloc(sizeof(struct Point));
//    p = malloc(sizeof(struct Point));  // LEAK! Lost first allocation
//
// 4. USE AFTER FREE:
//    free(p);
//    p->x = 10;  // UNDEFINED BEHAVIOR! Memory is freed
//
// 5. DOUBLE FREE:
//    free(p);
//    free(p);  // UNDEFINED BEHAVIOR! Freeing same memory twice
//
// 6. FREEING STACK MEMORY:
//    struct Point p;
//    free(&p);  // UNDEFINED BEHAVIOR! Can't free stack memory

// THE sizeof OPERATOR:
//
// sizeof is a compile-time operator (not a function!)
// It returns the size in bytes of a type or expression
//
// Examples:
//   sizeof(int)              → 4 (on most modern systems)
//   sizeof(struct Point)     → 8 (2 ints × 4 bytes each)
//   sizeof(char)             → 1 (always)
//   sizeof(pointy)           → 8 (size of pointer on 64-bit system)
//   sizeof(*pointy)          → 8 (size of what pointy points to)
//
// Return type: size_t (unsigned integer type)
// Print with: %zu (C99) or %lu (older, but works)

// STRUCT PADDING AND ALIGNMENT:
//
// Compilers may add padding for alignment:
//
// struct Example {
//   char c;    // 1 byte
//   // 3 bytes padding (on most systems)
//   int x;     // 4 bytes
// };
// sizeof(struct Example) = 8, not 5!
//
// Why padding?
// - CPUs are faster when data is aligned to natural boundaries
// - ints should start at addresses divisible by 4
// - Accessing unaligned data may be slower or cause errors
//
// Our Point struct has no padding:
// struct Point {
//   int x;     // 4 bytes at offset 0
//   int y;     // 4 bytes at offset 4
// };
// Both are naturally aligned, so no padding needed

// WHEN TO USE MALLOC:
//
// Use malloc when:
// 1. Size isn't known at compile time
// 2. Data must outlive the current function
// 3. Data is too large for the stack (stack is limited, heap is much larger)
// 4. You need to return a pointer to data from a function
//
// Use stack allocation when:
// 1. Size is small and known at compile time
// 2. Data only needed within current function
// 3. Automatic cleanup is desired (no need to free)

// NULL vs 0 vs nullptr:
//
// In C:
//   pointy = 0;        // Valid, 0 converts to null pointer
//   pointy = NULL;     // Preferred, NULL is defined as 0 or (void*)0
//   pointy = nullptr;  // NOT VALID in C (C++ only)
//
// NULL is typically defined as:
//   #define NULL ((void*)0)  or  #define NULL 0

// TO COMPILE AND RUN:
// gcc malloc1.c -o malloc1
// ./malloc1
//
// Expected output:
// The size of Point is: 8
// The field x is 10 and y is 20
// The field x is 50 and y is 40
//
// TO CHECK FOR MEMORY LEAKS (if valgrind is installed):
// valgrind ./malloc1
// Should report: "All heap blocks were freed -- no leaks are possible"
