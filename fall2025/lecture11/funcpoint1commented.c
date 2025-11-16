// PEDAGOGICAL PURPOSE:
// This program introduces function pointers and higher-order functions in C.
// Key learning objectives:
// 1. Understanding that functions have addresses in memory (can be treated as values)
// 2. Learning the syntax for declaring function pointers: returnType (*name)(paramTypes)
// 3. Implementing the "map" pattern - applying a function to every array element
// 4. Understanding how to pass functions as arguments to other functions
// 5. Seeing practical applications of function pointers for code reusability
// 6. Building foundational knowledge for callbacks and generic programming in C

#include "stdlib.h"
#include "stdio.h"

// INTENTIONALLY INCORRECT FUNCTION #1:
// This function is named "addWrongly" to demonstrate a point in teaching
// It multiplies 'a' by 2 and adds 'b' instead of just adding them
// This is used to show how different functions can be passed as arguments
int addWrongly(int a, int b){
  return (2*a + b);
}

// INTENTIONALLY INCORRECT FUNCTION #2:
// This function adds 2 to the input instead of adding 1 (incrementing)
// The "wrong" behavior is intentional for pedagogical purposes
// It demonstrates that we can pass ANY function matching the signature
//
// WHAT THIS FUNCTION DOES:
// Takes an integer, adds 2 to it, returns the result
int incWrongly(int a){
  return (a + 2);
}

// HIGHER-ORDER FUNCTION: MAP
// This is the heart of the example - a function that takes another function as an argument!
//
// WHAT IS MAP?
// Map is a common pattern in programming that applies a transformation
// to every element of a collection. It's like saying:
// "For each item in this array, do this operation"
//
// FUNCTION SIGNATURE BREAKDOWN:
// void map(int arr[], int sz, int (*func)(int))
//   - int arr[]: The array we want to modify
//   - int sz: The size of the array
//   - int (*func)(int): A FUNCTION POINTER!
//
// UNDERSTANDING THE FUNCTION POINTER SYNTAX:
// int (*func)(int) means:
//   - func is a POINTER to a function
//   - That function takes one int parameter
//   - That function returns an int
//   - The parentheses around *func are CRITICAL!
//     Without them: int *func(int) would mean "function returning int*"
//     With them: int (*func)(int) means "pointer to function returning int"
//
// EXAMPLE USAGE:
// map([1,2,3,4], 4, incWrongly) transforms the array to [3,4,5,6]
// Because incWrongly adds 2 to each element:
//   1 + 2 = 3
//   2 + 2 = 4
//   3 + 2 = 5
//   4 + 2 = 6
void map(int arr[], int sz,int (*func)(int)){
  // LOOP THROUGH ARRAY:
  // For each position in the array...
  for(int i=0;i<sz;i++){
    // FUNCTION POINTER CALL:
    // func(arr[i]) calls whatever function was passed to map
    // This is the MAGIC of function pointers!
    // If we passed incWrongly, this calls incWrongly(arr[i])
    // If we passed a different function, it would call that instead
    //
    // IN-PLACE MODIFICATION:
    // We store the result back into the same array position
    // This modifies the original array (not making a copy)
    arr[i] = func(arr[i]);
  }
}

// UTILITY FUNCTION: PRINT ARRAY
// Helper function to visualize array contents
// Prints array in format: [elem1,elem2,elem3,]
//
// WHY THE TRAILING COMMA?
// It's a simple implementation that doesn't check if we're at the last element
// In production code, you'd typically avoid the trailing comma
void printArray(int arr[], int sz){
  printf("[");
  for(int i=0;i<sz;i++){
    printf("%d,",arr[i]);
  }
  printf("]\n");
}

int main(){

  // CREATE AND INITIALIZE ARRAY:
  // int dummy[5] = {0,1,2,3,4} creates an array with 5 elements
  // initialized to the values 0, 1, 2, 3, 4
  int dummy[5] = {0,1,2,3,4};

  // BEFORE MAP:
  // Print the array before transformation
  // Expected output: [0,1,2,3,4,]
  printArray(dummy,5);

  // THE MAGIC HAPPENS HERE:
  // map(dummy, 5, incWrongly) does the following:
  // 1. Takes the dummy array
  // 2. For each element, calls incWrongly on it
  // 3. Stores the result back in the array
  //
  // NOTICE: We're passing the FUNCTION NAME without parentheses!
  // incWrongly (not incWrongly())
  // This passes the ADDRESS of the function, not the result of calling it
  //
  // WHAT HAPPENS:
  // dummy[0] = incWrongly(0) = 0 + 2 = 2
  // dummy[1] = incWrongly(1) = 1 + 2 = 3
  // dummy[2] = incWrongly(2) = 2 + 2 = 4
  // dummy[3] = incWrongly(3) = 3 + 2 = 5
  // dummy[4] = incWrongly(4) = 4 + 2 = 6
  map(dummy,5,incWrongly);

  // AFTER MAP:
  // Print the array after transformation
  // Expected output: [2,3,4,5,6,]
  printArray(dummy,5);

  return 0;
}

// EXPECTED OUTPUT:
// [0,1,2,3,4,]
// [2,3,4,5,6,]

// WHY FUNCTION POINTERS MATTER:
// 1. CODE REUSABILITY:
//    We can write ONE map function that works with ANY transformation
//    We don't need mapWithIncrement, mapWithDouble, mapWithSquare, etc.
//    We just need ONE map function and pass different functions to it!
//
// 2. CALLBACKS:
//    This is the foundation for callback functions used in:
//    - Event handlers
//    - Signal handlers
//    - qsort() - C's built-in sorting function takes a comparison function
//    - Thread creation (pthread_create takes a function pointer)
//
// 3. POLYMORPHISM IN C:
//    Since C doesn't have classes/inheritance, function pointers provide
//    a way to write generic, reusable code
//
// 4. FUNCTIONAL PROGRAMMING:
//    Map is a fundamental concept from functional programming
//    Modern languages have this built-in, but C requires manual implementation

// COMMON PITFALLS:
// 1. Forgetting parentheses around *func in the declaration
//    Wrong: int *func(int)  - function returning int*
//    Right: int (*func)(int) - pointer to function returning int
//
// 2. Calling the function instead of passing it
//    Wrong: map(dummy, 5, incWrongly())  - tries to pass return value
//    Right: map(dummy, 5, incWrongly)    - passes function address
//
// 3. Type mismatches - the function signature must EXACTLY match
//    If map expects int (*)(int), you can't pass int (*)(void)

// TRY IT:
// 1. Compile: gcc funcpoint1.c -o funcpoint1
// 2. Run: ./funcpoint1
// 3. EXPERIMENT: Write your own transformation function (like int double(int))
//    and pass it to map instead of incWrongly
// 4. EXPERIMENT: Can you write a different map that takes int (*)(int, int)?
//    What would you need to change?
