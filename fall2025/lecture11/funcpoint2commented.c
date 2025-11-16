// PEDAGOGICAL PURPOSE:
// This program builds on funcpoint1.c by introducing the "reduce" (fold) pattern.
// Key learning objectives:
// 1. Understanding higher-order functions with TWO-parameter function pointers
// 2. Learning the "reduce/fold" pattern - combining array elements with an operation
// 3. Seeing how an accumulator value propagates through iterations
// 4. Contrasting map (transformation) vs reduce (aggregation)
// 5. Understanding the difference between modifying arrays vs computing single values
// 6. Mastering function pointer syntax for multi-parameter functions

#include "stdlib.h"
#include "stdio.h"

// INTENTIONALLY INCORRECT FUNCTION #1:
// Returns 2*a + b instead of a + b
// Used to demonstrate reduce with an incorrect operation
int addWrongly(int a, int b){
  return (2*a + b);
}

// INTENTIONALLY INCORRECT FUNCTION #2:
// Adds 2 instead of 1 (not a true increment)
int incWrongly(int a){
  return (a + 2);
}

// CORRECT ADDITION FUNCTION:
// This function properly adds two numbers
// Used to show the difference between correct and incorrect operations
// When used with reduce, this will correctly sum array elements
int addCorrectly(int a, int b){
  return (a+b);
}

// HIGHER-ORDER FUNCTION: MAP (from previous example)
// Applies a single-argument function to every element of an array
// This modifies the array in-place
//
// Review: int (*func)(int) means:
//   - func is a pointer to a function
//   - That function takes one int
//   - That function returns an int
void map(int arr[], int sz,int (*func)(int)){
  for(int i=0;i<sz;i++){
    arr[i] = func(arr[i]);
  }
}

// HIGHER-ORDER FUNCTION: REDUCE (also called FOLD)
// This is the second major pattern in functional programming!
//
// WHAT IS REDUCE?
// Reduce takes a collection and "reduces" it to a single value
// by repeatedly applying a binary operation
//
// EXAMPLE WITH ADDITION:
// reduce([1,2,3,4], +, 0) = ((((0 + 1) + 2) + 3) + 4) = 10
//
// EXAMPLE WITH MULTIPLICATION:
// reduce([1,2,3,4], *, 1) = ((((1 * 1) * 2) * 3) * 4) = 24
//
// FUNCTION SIGNATURE BREAKDOWN:
// int reduce(int arr[], int sz, int (*func)(int,int), int base)
//   - int arr[]: The array to reduce
//   - int sz: The size of the array
//   - int (*func)(int,int): A FUNCTION POINTER to a TWO-ARGUMENT function!
//   - int base: The initial/base value (also called "seed" or "identity")
//
// UNDERSTANDING THE FUNCTION POINTER:
// int (*func)(int,int) means:
//   - func is a pointer to a function
//   - That function takes TWO int parameters
//   - That function returns an int
//   - This allows us to pass operations like add, multiply, max, min, etc.
//
// THE BASE VALUE:
// - For addition: base = 0 (identity: 0 + x = x)
// - For multiplication: base = 1 (identity: 1 * x = x)
// - For max: base = INT_MIN
// - For min: base = INT_MAX
//
// LEFT FOLD (foldl):
// This implements a LEFT fold, processing from left to right
// Some languages also have RIGHT fold (foldr) which goes right to left
int reduce(int arr[], int sz,
	   int (*func)(int,int),
	   int base){
  // ACCUMULATOR PATTERN:
  // 'acc' holds the running result
  // It starts with the base value
  // Each iteration updates it by combining with the next array element
  int acc = base;

  // ITERATE THROUGH ARRAY:
  for(int i=0;i<sz;i++){
    // THE REDUCTION STEP:
    // acc = func(acc, arr[i])
    //
    // This is where the magic happens!
    // We're calling whatever function was passed to reduce
    // The first argument is the accumulated value so far
    // The second argument is the current array element
    //
    // EXAMPLE TRACE (with addCorrectly and base=0 on [0,1,2,3,4]):
    // i=0: acc = addCorrectly(0, 0) = 0 + 0 = 0
    // i=1: acc = addCorrectly(0, 1) = 0 + 1 = 1
    // i=2: acc = addCorrectly(1, 2) = 1 + 2 = 3
    // i=3: acc = addCorrectly(3, 3) = 3 + 3 = 6
    // i=4: acc = addCorrectly(6, 4) = 6 + 4 = 10
    //
    // EXAMPLE TRACE (with addWrongly and base=0 on [0,1,2,3,4]):
    // i=0: acc = addWrongly(0, 0) = 2*0 + 0 = 0
    // i=1: acc = addWrongly(0, 1) = 2*0 + 1 = 1
    // i=2: acc = addWrongly(1, 2) = 2*1 + 2 = 4
    // i=3: acc = addWrongly(4, 3) = 2*4 + 3 = 11
    // i=4: acc = addWrongly(11, 4) = 2*11 + 4 = 26
    acc = func(acc,arr[i]);
  }

  // RETURN THE FINAL ACCUMULATED VALUE:
  // Unlike map, reduce doesn't modify the array
  // It returns a single value representing the reduction
  return acc;
}



// UTILITY FUNCTION: PRINT ARRAY
// Same as before - prints array in [elem1,elem2,elem3,] format
void printArray(int arr[], int sz){
  printf("[");
  for(int i=0;i<sz;i++){
    printf("%d,",arr[i]);
  }
  printf("]\n");
}

int main(){

  // CREATE AND INITIALIZE ARRAY:
  int dummy[5] = {0,1,2,3,4};

  // DEMONSTRATE REDUCE:
  // reduce(dummy, 5, addWrongly, 0)
  //
  // This calls reduce with:
  // - array: dummy = [0,1,2,3,4]
  // - size: 5
  // - function: addWrongly (which does 2*a + b)
  // - base: 0
  //
  // STEP-BY-STEP EXECUTION:
  // acc starts at 0
  // acc = addWrongly(0, 0) = 2*0 + 0 = 0
  // acc = addWrongly(0, 1) = 2*0 + 1 = 1
  // acc = addWrongly(1, 2) = 2*1 + 2 = 4
  // acc = addWrongly(4, 3) = 2*4 + 3 = 11
  // acc = addWrongly(11, 4) = 2*11 + 4 = 26
  // Final result: 26
  //
  // NOTE: If we used addCorrectly instead, result would be 0+1+2+3+4 = 10
  printf("dummy wrongly added together is: %d\n",reduce(dummy,5,addWrongly,0));

  // DEMONSTRATE MAP:
  // Show the original array
  printArray(dummy,5);

  // Apply incWrongly to each element (adds 2 to each)
  // [0,1,2,3,4] becomes [2,3,4,5,6]
  map(dummy,5,incWrongly);

  // Show the transformed array
  printArray(dummy,5);

  return 0;
}

// EXPECTED OUTPUT:
// dummy wrongly added together is: 26
// [0,1,2,3,4,]
// [2,3,4,5,6,]

// KEY DIFFERENCES: MAP vs REDUCE
//
// MAP:
// - Input: Array of N elements
// - Output: Array of N elements (modified in place)
// - Function type: int (*)(int) - one parameter
// - Purpose: Transform each element
// - Example: Double every number, increment every number
//
// REDUCE:
// - Input: Array of N elements
// - Output: Single value
// - Function type: int (*)(int,int) - two parameters
// - Purpose: Combine/aggregate elements
// - Example: Sum, product, maximum, minimum
//
// Both are HIGHER-ORDER FUNCTIONS because they take functions as arguments

// WHY REDUCE/FOLD MATTERS:
//
// 1. MATHEMATICAL OPERATIONS:
//    Sum, product, average all use the reduce pattern
//
// 2. FINDING EXTREMES:
//    Maximum, minimum can be implemented as reduce
//    reduce(arr, sz, max, INT_MIN)
//
// 3. DATA AGGREGATION:
//    Combining results from parallel computations
//    MapReduce paradigm (Google's distributed computing model)
//
// 4. BUILDING COMPLEX RESULTS:
//    Reduce can build strings, construct data structures, etc.
//
// 5. FUNCTIONAL PROGRAMMING:
//    Map and Reduce are two of the most fundamental patterns
//    Along with Filter, they form the core of functional programming

// FUNCTION POINTER COMPLEXITY:
// Compare these signatures:
//   int (*func)(int)        - pointer to function taking int, returning int
//   int (*func)(int,int)    - pointer to function taking two ints, returning int
//   void (*func)(void)      - pointer to function taking nothing, returning nothing
//   int* (*func)(char*)     - pointer to function taking char*, returning int*
//   int (*func)(int (*)(int)) - pointer to function taking function pointer!

// TRY IT:
// 1. Compile: gcc funcpoint2.c -o funcpoint2
// 2. Run: ./funcpoint2
// 3. EXPERIMENT: Change addWrongly to addCorrectly in the reduce call
//    You should get 10 instead of 26
// 4. EXPERIMENT: Write a multiply function and use it with reduce
//    What should the base value be? (Hint: 1)
// 5. EXPERIMENT: Write a max function and use it with reduce to find the maximum
//    What should the base value be? (Hint: INT_MIN from limits.h)
// 6. CHALLENGE: Can you implement a reduce that goes RIGHT to LEFT instead?
