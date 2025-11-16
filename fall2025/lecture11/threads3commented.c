// PEDAGOGICAL PURPOSE:
// This program demonstrates how to GET RESULTS back from a thread using the return value.
// Key learning objectives:
// 1. Understanding how threads return values via void*
// 2. Learning how pthread_join retrieves thread return values
// 3. Understanding why pthread_join takes void** (pointer to pointer)
// 4. Seeing how to allocate return values on the heap (not stack)
// 5. Understanding memory management with thread return values
// 6. Learning the complete data flow: main → thread → thread back to main
// 7. Understanding why we need malloc for return values
// 8. Practicing double pointer (void**) manipulation

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"

// COMMENT FROM ORIGINAL CODE:
// how do I get results *out* of a thread?
//
// ANSWER: Through the return value!
// Threads return void*, and pthread_join captures that return value

// THREAD FUNCTION WITH INPUT AND OUTPUT:
// This function demonstrates:
// 1. Receiving data from main (via arg parameter)
// 2. Processing that data
// 3. Returning a result to main (via return value)
void* func(void* arg){

  // CONVERT THE INPUT ARGUMENT:
  // arg is void* but we know it's actually int*
  // We dereference and store in a local variable for clarity
  //
  // WHY STORE IN LOCAL VARIABLE?
  // It makes the code more readable
  // We could write *((int*)arg) everywhere, but convArg is cleaner
  int convArg = *((int*)arg);

  // ORIGINAL VERSION (commented for teaching):
  // This version directly uses the casted argument in printf
  // Both versions work, but convArg is more readable
  // now arg is SECRETLY a int*
  printf("Hi I'm a thread and I'm holding: %d\n",
	 *((int*)arg));

  // ALLOCATE MEMORY FOR THE RETURN VALUE:
  // This is CRITICAL!
  //
  // WHY MALLOC?
  // We need to return a value to main
  // But local variables are destroyed when the function returns
  // Solution: Allocate on the HEAP, which persists after function return
  //
  // WHAT IF WE DIDN'T USE MALLOC?
  //   int local_ret = 2 * convArg;
  //   return &local_ret;  // WRONG! DANGER!
  // This would return a pointer to stack memory that's about to be destroyed
  // When main tries to read it, it's UNDEFINED BEHAVIOR (crash, garbage, etc.)
  //
  // HEAP ALLOCATION:
  // malloc returns a pointer to heap memory
  // This memory persists until explicitly freed
  // The thread allocates it, but MAIN must free it
  int* ret = malloc(sizeof(int));

  // COMPUTE THE RESULT:
  // Double the input value and store in the heap-allocated memory
  // convArg = 10, so *ret = 20
  *ret = 2*convArg;

  // RETURN THE RESULT:
  // We return the POINTER to the heap-allocated int
  // Type: int* → void* (implicit conversion)
  //
  // This pointer will be received by pthread_join in main
  return ret;
}

int main(){

  // THREAD HANDLE:
  pthread_t thread; // the data structure that holds all the information about a thread

  // INPUT FOR THE THREAD:
  int arg = 10;

  // VARIABLE TO RECEIVE THE RESULT:
  // This will hold the return value from the thread
  // Type: int* (pointer to the result)
  //
  // WHY int* AND NOT int?
  // The thread returns void*, which we'll cast to int*
  // The actual int value is on the heap, pointed to by this pointer
  int* res;

  // CREATE THE THREAD WITH AN ARGUMENT:
  // Pass &arg (address of the int 10) to the thread
  pthread_create(&thread,NULL,func,&arg);

  // COMMENT FROM ORIGINAL CODE:
  // the third argument of pthread_create
  // is a function!
  // hence why I was talking about function pointers
  // it's not (entirely) because I'm whacked out with poo-brain

  // WAIT FOR THREAD AND GET RETURN VALUE:
  // pthread_join now has TWO purposes:
  // 1. Wait for thread to complete (like before)
  // 2. Retrieve the thread's return value
  //
  // FUNCTION SIGNATURE:
  // int pthread_join(pthread_t thread, void **retval)
  //
  // SECOND PARAMETER: void **retval
  // This is a POINTER TO POINTER!
  //
  // WHY void** AND NOT void*?
  // pthread_join needs to MODIFY a pointer (res)
  // To modify a pointer, we need a pointer TO that pointer
  //
  // TYPE CHAIN:
  // res has type int*
  // &res has type int** (pointer to int*)
  // (void**)&res has type void** (cast for pthread_join)
  //
  // WHAT HAPPENS:
  // 1. Thread returns void* (actually an int*)
  // 2. pthread_join receives this void*
  // 3. pthread_join stores it in the location pointed to by (void**)&res
  // 4. After join, res points to the heap-allocated int
  //
  // ALTERNATIVE (more explicit):
  //   void* temp;
  //   pthread_join(thread, &temp);
  //   res = (int*)temp;

  // join takes the thread and a void**
  // why **?
  // Because pthread_join needs to modify a POINTER
  // It needs to store a pointer value in res
  // To modify a variable, you need its address
  // res is a pointer (int*), so &res is a pointer-to-pointer (int**)
  pthread_join(thread,(void**)&res);

  // PRINT THE RESULT:
  // res now points to the heap-allocated int
  // *res dereferences to get the value (20)
  //
  // DATA FLOW SUMMARY:
  // 1. main passes 10 to thread
  // 2. thread computes 2*10 = 20
  // 3. thread allocates int on heap, stores 20
  // 4. thread returns pointer to that int
  // 5. pthread_join captures that pointer in res
  // 6. main dereferences res to get 20
  printf("The return value from the thread is: %d\n",*res);

  // FREE THE MEMORY:
  // CRITICAL: We must free the memory allocated by the thread!
  //
  // WHO ALLOCATES? Thread (with malloc)
  // WHO FREES? Main (with free)
  //
  // This is a common pattern:
  // The producer (thread) allocates
  // The consumer (main) frees
  //
  // MEMORY LEAK WARNING:
  // If we forget this free(), the memory is leaked!
  // The program will lose 4 bytes (size of int)
  // Not a big deal for this small program
  // But in a long-running program with many threads, leaks accumulate!
  free(res);

  return 0;
}

// EXPECTED OUTPUT:
// Hi I'm a thread and I'm holding: 10
// The return value from the thread is: 20

// COMPLETE DATA FLOW DIAGRAM:
//
// MAIN THREAD                          WORKER THREAD
// -----------                          -------------
// arg = 10 (stack)
//   |
//   v (pass address)
// pthread_create(..., &arg)    --->    receives void* arg
//                                      |
// pthread_join(..., &res)              | casts to int*, dereferences: 10
// (WAITING...)                         |
//                                      | allocates: int* ret = malloc(...)
//                                      |
//                                      | computes: *ret = 20
//                                      |
//                                      v (return pointer)
//                              <---    return ret
// receives ret in res
//   |
//   v (dereference)
// *res = 20
//   |
//   v (print)
// prints: 20
//   |
//   v (cleanup)
// free(res)

// UNDERSTANDING void**:
//
// pthread_join needs to MODIFY res
// res is a pointer (int*)
// To modify a variable, you need its ADDRESS
// &res is the address of res
// &res has type int**
// Cast to void** for pthread_join
//
// ANALOGY:
// If you want a function to modify an int:
//   void modify_int(int* ptr) { *ptr = 42; }
//   int x;
//   modify_int(&x);
//
// If you want a function to modify a pointer:
//   void modify_ptr(int** ptr_ptr) { *ptr_ptr = malloc(...); }
//   int* p;
//   modify_ptr(&p);
//
// pthread_join modifies a pointer, so it needs void**

// WHY HEAP ALLOCATION IS NECESSARY:
//
// WRONG APPROACH (stack allocation):
//   void* func(void* arg) {
//     int local_result = 20;
//     return &local_result;  // DANGER! UNDEFINED BEHAVIOR!
//   }
//
// PROBLEM:
// local_result is on the thread's stack
// When func returns, the stack is destroyed/reused
// The pointer points to invalid memory
// When main tries to read *res, it gets garbage or crashes
//
// RIGHT APPROACH (heap allocation):
//   void* func(void* arg) {
//     int* result = malloc(sizeof(int));
//     *result = 20;
//     return result;  // Safe! Heap persists after return
//   }
//
// HEAP MEMORY:
// Allocated with malloc
// Persists until explicitly freed
// Can be passed between functions/threads safely

// MEMORY MANAGEMENT RESPONSIBILITY:
//
// RULE: Whoever receives the return value must free it
//
// IN THIS PROGRAM:
// - Thread allocates with malloc
// - Main receives via pthread_join
// - Main must call free
//
// MEMORY LEAK EXAMPLE:
//   pthread_join(thread, (void**)&res);
//   printf("%d\n", *res);
//   // Forgot to free(res)!  MEMORY LEAK!
//   return 0;

// PATTERN VARIATIONS:
//
// VARIATION 1: Return a struct
//   struct Result* r = malloc(sizeof(struct Result));
//   r->status = 0;
//   r->value = 42;
//   return r;
//
// VARIATION 2: Return an array
//   int* array = malloc(100 * sizeof(int));
//   // fill array...
//   return array;
//
// VARIATION 3: Return a string
//   char* str = malloc(100);
//   strcpy(str, "Hello from thread");
//   return str;

// COMMON MISTAKES:
//
// MISTAKE 1: Returning address of stack variable
//   int result = 20;
//   return &result;  // WRONG! Undefined behavior!
//
// MISTAKE 2: Forgetting to free
//   pthread_join(thread, (void**)&res);
//   // use res...
//   // forgot free(res)!  MEMORY LEAK!
//
// MISTAKE 3: Wrong type for res
//   int res;  // Should be int*, not int!
//   pthread_join(thread, (void**)&res);  // Type mismatch!
//
// MISTAKE 4: Dereferencing before join
//   pthread_create(&thread, NULL, func, &arg);
//   printf("%d\n", *res);  // WRONG! res not set yet!
//   pthread_join(thread, (void**)&res);
//
// MISTAKE 5: Joining twice
//   pthread_join(thread, (void**)&res);
//   pthread_join(thread, (void**)&res);  // WRONG! Undefined behavior!

// COMPARISON WITH OTHER LANGUAGES:
//
// JAVA:
//   class MyThread extends Thread {
//     private int result;
//     public void run() { result = 20; }
//     public int getResult() { return result; }
//   }
//
// PYTHON:
//   import threading
//   def func():
//     return 20
//   thread = threading.Thread(target=func)
//   result = thread.join()  # But Python threads don't return values this way!
//
// C is more manual but gives more control

// TRY IT:
// 1. Compile: gcc threads3.c -o threads3 -pthread
// 2. Run: ./threads3
// 3. Verify output shows input 10 and output 20
// 4. EXPERIMENT: Change the computation (*ret = convArg * 3)
// 5. EXPERIMENT: Comment out free(res) and run with valgrind
//    valgrind ./threads3
//    You'll see a memory leak!
// 6. EXPERIMENT: Try returning a stack variable (WRONG):
//    int local = 20;
//    return &local;
//    Compile with -Wall and see the warning!
// 7. EXPERIMENT: Return NULL and see what happens
//    pthread_join will set res to NULL
//    *res would crash!
// 8. CHALLENGE: Modify to create 5 threads, each returning different values
//    Store results in an array and sum them
