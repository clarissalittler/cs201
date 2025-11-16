// PEDAGOGICAL PURPOSE:
// This program demonstrates dynamic allocation of structs and the arrow operator.
// Key learning objectives:
// 1. Using malloc to allocate memory for struct types
// 2. Understanding the arrow operator (->) for pointer-to-struct member access
// 3. The arrow operator is syntactic sugar for pointer dereferencing
// 4. Proper memory management: every malloc needs a corresponding free
// 5. Using sizeof with struct types ensures correct allocation size
// 6. Accessing struct members through pointers

#include "stdio.h"
#include "stdlib.h"

// STRUCT DEFINITION:
// A Rect (Rectangle) has two integer fields: width and height
// This is a simple composite data type grouping related data together
struct Rect {
  int width;
  int height;
};

int main(){

  // DYNAMIC STRUCT ALLOCATION:
  // malloc allocates memory for ONE struct Rect on the heap
  // sizeof(struct Rect) calculates the exact bytes needed for this struct
  // (In this case: 2 ints × 4 bytes each = 8 bytes on most systems)
  // malloc returns void*, which we implicitly cast to struct Rect*
  struct Rect* r = malloc(sizeof(struct Rect));

  // IMPORTANT: After malloc, the memory contains garbage values
  // We must initialize the fields before using them

  // ARROW OPERATOR (->) SYNTAX:
  // The arrow operator accesses members of a struct through a POINTER
  // r->width is equivalent to (*r).width
  // This is a convenience feature to make code more readable
  r->width = 10; // (*r).width

  // Why -> exists:
  // Writing (*r).width requires parentheses (because . has higher precedence than *)
  // The -> operator is cleaner and more intuitive for pointer access
  r->height = 20;

  // ACCESSING STRUCT MEMBERS:
  // We use -> to access both width and height, then multiply them
  // This calculates the area of the rectangle
  printf("%d\n", r->width * r->height);

  // Expected output: 10 × 20 = 200

  // MEMORY CLEANUP:
  // free() returns the allocated memory back to the system
  // After free(), the pointer r becomes a "dangling pointer"
  // Using r after free() is undefined behavior (don't do it!)
  free(r);

  return 0;
}

// CONCEPTUAL EXPLANATION:
//
// ARROW OPERATOR vs DOT OPERATOR:
// . (dot) is used when you have a struct VARIABLE
// -> (arrow) is used when you have a POINTER to a struct
//
// Examples:
// struct Rect myRect;       // myRect is a struct variable
// myRect.width = 10;        // Use dot operator
//
// struct Rect* r;           // r is a pointer to a struct
// r->width = 10;            // Use arrow operator
// (*r).width = 10;          // Equivalent, but uglier
//
// WHY THE PARENTHESES ARE NEEDED in (*r).width:
// Without parentheses, *r.width would mean *(r.width) (wrong!)
// The . operator has higher precedence than the * operator
// So we need (*r).width to dereference first, then access the member
// The -> operator solves this by combining dereference and member access
//
// MEMORY LAYOUT:
// When we allocate a struct Rect, memory looks like:
//
// Address    Content
// --------   -------
// 0x1000     width (4 bytes)
// 0x1004     height (4 bytes)
//
// The pointer r holds the address 0x1000 (the start of the struct)
// r->width accesses the memory at 0x1000
// r->height accesses the memory at 0x1004
//
// WHY MALLOC FOR STRUCTS?
// 1. Dynamic lifetime - the struct can outlive the function that created it
// 2. Size - large structs are better allocated on heap (stack is limited)
// 3. Flexibility - size can be determined at runtime
// 4. Return from function - you can return a malloc'd struct pointer from a function
//
// COMMON MISTAKE:
// struct Rect* r;           // Uninitialized pointer - DANGER!
// r->width = 10;            // CRASH! r doesn't point to valid memory
//
// CORRECT:
// struct Rect* r = malloc(sizeof(struct Rect));  // r now points to valid memory
// r->width = 10;            // Safe!
//
// ANOTHER COMMON MISTAKE:
// free(r);
// r->width = 10;            // Use after free - UNDEFINED BEHAVIOR!
//
// Setting r = NULL after free is a good practice:
// free(r);
// r = NULL;                 // Now accessing r will crash immediately (easier to debug)
