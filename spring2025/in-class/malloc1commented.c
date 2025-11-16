// PEDAGOGICAL PURPOSE:
// This program introduces malloc, C's dynamic memory allocation function.
// Key learning objectives:
// 1. Understanding malloc - how to allocate memory on the heap at runtime
// 2. The difference between malloc and other allocation methods (like C++ new)
// 3. Type casting with malloc (void* to specific pointer type)
// 4. Using sizeof to calculate the correct allocation size
// 5. Accessing struct members through pointers using the arrow operator (->)
// 6. Understanding that malloc does NOT initialize memory (contains garbage)
//
// KEY CONCEPT:
// malloc is C's way of requesting memory from the heap. Unlike stack allocation
// (which happens automatically for local variables), heap memory persists until
// explicitly freed, and can be allocated in amounts determined at runtime.

#include <stdio.h>
#include <stdlib.h>

// MALLOC FUNDAMENTALS:
// malloc takes a size in bytes as an argument
// and returns a pointer to a region of memory of that size
//
// Key differences from C++ 'new':
// - malloc returns void* (generic pointer) - YOU must cast it
// - malloc does NOT call constructors (C has no constructors)
// - malloc takes SIZE IN BYTES (not object count × size automatically)
// - It's YOUR responsibility to cast to the correct type
// - malloc does NOT initialize the memory (it contains garbage values)

// STRUCT DEFINITION:
// A simple struct named Garbage with two integer fields
// The name "Garbage" is intentional - it reminds us that malloc'd memory
// starts with garbage values (whatever bits happened to be there before)
struct Garbage {
  int n1;   // First integer field
  int n2;   // Second integer field
};

int main(){
  // MALLOC SYNTAX BREAKDOWN:
  // 1. sizeof(struct Garbage) - calculates bytes needed for this struct
  //    On most systems: 2 ints × 4 bytes each = 8 bytes
  // 2. malloc(8) - allocates 8 bytes on the heap
  // 3. Returns void* (generic pointer) pointing to that memory
  // 4. (struct Garbage*) - EXPLICIT cast to the correct pointer type
  //    This tells the compiler: "treat this memory as a struct Garbage"
  // 5. Result stored in pointer variable g
  struct Garbage* g =
    (struct Garbage*) malloc(sizeof(struct Garbage));

  // WHY THE CAST IS NECESSARY:
  // In C, malloc returns void* (pointer to anything)
  // We must cast it to struct Garbage* so we can use it as that type
  // Without the cast, the compiler wouldn't know how to interpret the memory
  // In C++, this cast is required; in C, it's optional but strongly recommended

  // WHY sizeof(struct Garbage) INSTEAD OF A HARDCODED NUMBER:
  // - Portable: works on any system regardless of int size
  // - Maintainable: if we add fields to Garbage, sizeof updates automatically
  // - Safe: prevents allocation size errors
  // WRONG: malloc(8)  // Hardcoded - breaks on systems where int != 4 bytes
  // RIGHT: malloc(sizeof(struct Garbage))  // Always correct

  // CRITICAL WARNING: Memory contains GARBAGE
  // malloc does NOT initialize memory - it contains whatever bits were there
  // before. We MUST assign values before using them!

  // ARROW OPERATOR FOR STRUCT ACCESS:
  // g->n1 means "access the n1 field of the struct that g points to"
  // This is equivalent to (*g).n1, but much more readable
  // We INITIALIZE the fields (overwriting the garbage values)
  g->n1 = 10;  // Assign 10 to the first field
  g->n2 = 20;  // Assign 20 to the second field

  // PRINTING STRUCT MEMBERS:
  // We use -> to access the fields through the pointer
  // %d is the format specifier for integers
  printf("g->n1 is %d\n",g->n1);  // Output: g->n1 is 10
  printf("g->n2 is %d\n",g->n2);  // Output: g->n2 is 20

  // MEMORY LEAK WARNING:
  // This program has a MEMORY LEAK!
  // We allocated memory with malloc but never freed it
  // In a real program, we should add: free(g);
  // We'll see proper cleanup in malloc2.c

  return 0;
}

// CONCEPTUAL EXPLANATION:
//
// MALLOC vs STACK ALLOCATION:
//
// STACK ALLOCATION (automatic):
// struct Garbage g;        // Allocated on stack
// g.n1 = 10;              // Use dot operator (g is a variable, not pointer)
// // Memory automatically freed when function returns
//
// HEAP ALLOCATION (dynamic with malloc):
// struct Garbage* g = malloc(sizeof(struct Garbage));  // Allocated on heap
// g->n1 = 10;             // Use arrow operator (g is a pointer)
// free(g);                // Must manually free when done
//
// WHEN TO USE HEAP ALLOCATION:
// 1. Need memory to outlive the function that created it
// 2. Size determined at runtime (can't know at compile time)
// 3. Very large data structures (stack is limited, typically 1-8MB)
// 4. Need to return data from a function (can't return address of local variable)
//
// MEMORY LAYOUT:
//
// Stack:                  Heap:
// --------                --------
// main's frame            [8 bytes allocated]
//   g (pointer)    --->   [n1: 10  ] (4 bytes)
//   (8 bytes)             [n2: 20  ] (4 bytes)
//
// The pointer g is stored on the stack (automatic storage)
// The actual struct Garbage is stored on the heap (dynamic storage)
//
// MALLOC RETURN VALUE:
// malloc returns NULL if allocation fails (out of memory)
// Robust programs should check for this:
//
// struct Garbage* g = malloc(sizeof(struct Garbage));
// if (g == NULL) {
//   fprintf(stderr, "Memory allocation failed!\n");
//   exit(1);
// }
// // Safe to use g here
//
// TYPE CASTING EXPLAINED:
// void* is a "generic pointer" - it can point to any type
// (struct Garbage*) converts this generic pointer to a specific type
// This tells the compiler: "interpret this memory as struct Garbage"
//
// The cast enables:
// - Member access with ->
// - Type checking by compiler
// - Pointer arithmetic if needed
//
// COMMON MISTAKES:
//
// MISTAKE 1: Not allocating enough memory
// struct Garbage* g = malloc(4);  // WRONG! Only allocates 4 bytes, need 8
// g->n2 = 20;                     // Writes beyond allocated memory! CRASH!
//
// MISTAKE 2: Using uninitialized memory
// struct Garbage* g = malloc(sizeof(struct Garbage));
// printf("%d\n", g->n1);  // WRONG! n1 contains garbage, not 0
//
// MISTAKE 3: Forgetting to allocate
// struct Garbage* g;      // Uninitialized pointer - points to garbage address
// g->n1 = 10;            // CRASH! g doesn't point to valid memory
//
// MISTAKE 4: Using after free
// free(g);
// g->n1 = 10;            // UNDEFINED BEHAVIOR! Memory no longer ours
//
// MISTAKE 5: Memory leak (our current program!)
// struct Garbage* g = malloc(sizeof(struct Garbage));
// // ... use g ...
// // Never call free(g) - memory leaked!
//
// THE ARROW OPERATOR IN DETAIL:
// When you have a pointer to a struct, you have two syntax options:
//
// Option 1: Dereference then access (clunky)
// (*g).n1 = 10;          // Parentheses required! . has higher precedence than *
//
// Option 2: Arrow operator (clean)
// g->n1 = 10;            // Much more readable
//
// Why -> exists:
// Without parentheses: *g.n1 means *(g.n1) - tries to access n1 first, then dereference
// With parentheses: (*g).n1 - dereferences g first, then accesses n1
// The -> operator does both operations in the correct order, cleanly
//
// sizeof OPERATOR:
// sizeof is evaluated at COMPILE TIME (not runtime)
// It returns the size in bytes as a size_t (unsigned integer type)
//
// sizeof(struct Garbage)  // Size of the entire struct (8 bytes)
// sizeof(int)            // Size of an int (typically 4 bytes)
// sizeof(g)              // Size of the pointer itself (8 bytes on 64-bit systems)
// sizeof(*g)             // Size of what g points to (8 bytes, same as struct Garbage)
//
// WHY WE NEED malloc IN C:
// Unlike some languages, C doesn't have automatic garbage collection
// The programmer has complete control over memory allocation and deallocation
// This gives:
// - Maximum performance (no garbage collector overhead)
// - Predictable behavior (deterministic memory management)
// - Complete control (allocate exactly what you need, when you need it)
// - Responsibility (you must manually free what you allocate)
