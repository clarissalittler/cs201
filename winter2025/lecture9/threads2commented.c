// PEDAGOGICAL PURPOSE:
// This program demonstrates how to capture return values from threads.
// It shows the second parameter of pthread_join and pointer-to-pointer casting.
// Key learning objectives:
// 1. Understanding pthread_join's second parameter (void** retval)
// 2. Learning why double pointers are needed for return values
// 3. Understanding pointer-to-pointer casting with void**
// 4. Seeing the difference between NULL and actual return pointers
// 5. Learning how to retrieve thread function return values
// 6. Understanding why (void**) cast is necessary
// 7. Observing that return values can be pointers (not just NULL)

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

// COMMENT FROM ORIGINAL CODE:
// "when you see void*, that's polymorphism in C"
//
// EXPANDING ON THIS:
// void* is C's generic pointer type
// It's the only way to write functions that work with any pointer type
// pthread_create and pthread_join use void* for maximum flexibility

// THREAD FUNCTION:
// Same as threads1.c, but now we'll capture its return value
//
// WHAT'S DIFFERENT FROM threads1.c?
// Nothing in the function itself!
// The function is identical
// The difference is in HOW we call pthread_join in main()
void* func1(void* arg){

  // PRINT MESSAGE:
  // Each thread prints this
  printf("Hi I'm a thread!\n");

  // RETURN NULL:
  // Returns NULL (a void* value)
  // NULL is a special pointer value meaning "no address"
  //
  // COMMENT FROM ORIGINAL CODE:
  // "HEY BOZO, look up why no explicit return gives a pointer of 0x11"
  //
  // WHAT DOES THIS MEAN?
  // If you OMIT the return statement in a non-void function:
  //   - C allows it (lenient language)
  //   - Return value is UNDEFINED
  //   - Might return whatever was in the register (garbage)
  //   - Common garbage value: 0x11 (happens to be in register)
  //
  // WHY 0x11 specifically?
  //   - Not guaranteed, just coincidence
  //   - Depends on compiler, optimization, register state
  //   - Could be any value
  //
  // LESSON: Always explicitly return!
  // Even if returning NULL, be explicit
  return NULL;
}

int main(){

  // THREAD HANDLES:
  // Same as before - handles to identify threads
  pthread_t thread1;
  pthread_t thread2;

  // RETURN VALUE POINTERS:
  // These will receive the return values from the threads
  //
  // TYPE: int*
  // Why int* if func1 returns void*?
  //   - func1 returns void* (generic pointer)
  //   - We're INTERPRETING it as int*
  //   - void* can be cast to any pointer type
  //
  // IMPORTANT: Type mismatch is intentional here!
  // func1 returns NULL (which is void*)
  // We're storing it as int*
  // NULL is valid for ALL pointer types
  // So int* ret1 = (int*)NULL is fine
  int* ret1;
  int* ret2;

  // CREATE THREADS:
  // Same as threads1.c
  // Both threads execute func1
  pthread_create(&thread1,NULL,func1,NULL);
  pthread_create(&thread2,NULL,func1,NULL);

  // JOIN THREAD1 AND CAPTURE RETURN VALUE:
  // THIS IS THE KEY DIFFERENCE FROM threads1.c!
  //
  // FUNCTION SIGNATURE:
  // int pthread_join(pthread_t thread, void **retval)
  //
  // SECOND PARAMETER: void **retval
  // This is a POINTER TO POINTER to void
  // Why double pointer?
  //
  // EXPLANATION:
  // - Thread function returns void* (a pointer)
  // - pthread_join needs to GIVE YOU that pointer
  // - In C, to modify a variable, you pass its ADDRESS
  // - To modify a POINTER, you pass a pointer to pointer
  //
  // ANALOGY:
  // To modify an int, you pass int*
  // To modify a pointer (int*), you pass int**
  // To modify a void*, you pass void**
  //
  // WHAT HAPPENS:
  // 1. thread1's func1 returns NULL
  // 2. pthread_join receives NULL
  // 3. pthread_join stores NULL at address &ret1
  // 4. ret1 now contains NULL
  //
  // THE CAST: (void**)&ret1
  // &ret1 has type int** (pointer to int-pointer)
  // pthread_join expects void** (pointer to void-pointer)
  // Must cast: (void**) to match expected type
  //
  // WHY THE CAST IS SAFE:
  // All pointer types have the same size
  // Casting between pointer types is safe
  // This is how C does generic programming
  pthread_join(thread1,(void**)&ret1);

  // JOIN THREAD2:
  // Same as above, but capturing thread2's return value
  pthread_join(thread2,(void**)&ret2);

  // PRINT THE RETURN VALUES:
  // %p is the format specifier for printing pointers
  // Prints in hexadecimal (usually)
  //
  // EXPECTED OUTPUT:
  // The value of ret1 is: (nil)
  // The value of ret2 is: (nil)
  //
  // (nil) is how printf displays NULL pointers
  // Some systems show 0x0 instead
  //
  // WHY NULL?
  // Because func1 returns NULL
  // If func1 returned a different pointer, we'd see that address
  printf("The value of ret1 is: %p\n",ret1);
  printf("The value of ret2 is: %p\n",ret2);

  return 0;
}

// EXPECTED OUTPUT:
// Hi I'm a thread!
// Hi I'm a thread!
// The value of ret1 is: (nil)
// The value of ret2 is: (nil)
//
// Note: (nil) means NULL
// Some systems display: 0x0 or (null)

// POINTER TO POINTER EXPLANATION:
//
// WHY DO WE NEED DOUBLE POINTER?
//
// Consider this simpler example:
//
// void modify_int(int* p) {
//   *p = 42;  // Modifies the int that p points to
// }
//
// void modify_pointer(int** pp) {
//   *pp = some_address;  // Modifies the POINTER that pp points to
// }
//
// int main() {
//   int x = 10;
//   int* ptr;
//
//   modify_int(&x);      // Pass address of x (x becomes 42)
//   modify_pointer(&ptr); // Pass address of ptr (ptr gets new address)
// }
//
// APPLYING TO pthread_join:
//
// Thread returns void* (a pointer)
// pthread_join needs to modify your void* variable
// So it needs void** (address of your pointer variable)

// WHAT IF WE USED WRONG TYPES?
//
// ATTEMPT 1: Pass ret1 directly (wrong!)
//   pthread_join(thread1, ret1);
//   ERROR: ret1 is int*, but need void**
//   Won't compile
//
// ATTEMPT 2: Pass &ret1 without cast (wrong!)
//   pthread_join(thread1, &ret1);
//   ERROR: &ret1 is int**, but need void**
//   Won't compile (or gives warning)
//
// ATTEMPT 3: Pass (void*)&ret1 (wrong!)
//   pthread_join(thread1, (void*)&ret1);
//   ERROR: (void*)&ret1 is void*, but need void**
//   Wrong number of indirection levels
//
// CORRECT: Pass (void**)&ret1
//   pthread_join(thread1, (void**)&ret1);
//   Correct type!
//   Compiles and works

// WHAT ABOUT THE "0x11" COMMENT?
//
// From the original code:
// "HEY BOZO, look up why no explicit return gives a pointer of 0x11"
//
// EXPERIMENT:
// Change func1 to:
//   void* func1(void* arg) {
//     printf("Hi I'm a thread!\n");
//     // NO RETURN STATEMENT
//   }
//
// Then compile and run.
//
// POSSIBLE RESULTS:
// The value of ret1 is: 0x11
// The value of ret2 is: 0x11
//
// WHY 0x11?
// - Without explicit return, return value is undefined
// - Function returns whatever was in the return register
// - Happens to be 0x11 in many cases
// - NOT GUARANTEED - could be anything
// - 0x11 might be a common pattern from compiler/system internals
//
// LESSON:
// ALWAYS USE EXPLICIT RETURN!
// Even returning NULL is better than undefined behavior
// Undefined behavior can cause crashes, bugs, security issues

// KEY CONCEPTS:
//
// 1. THREAD RETURN VALUES:
//    Threads can return data via their return value
//    Return type must be void* (generic pointer)
//    Can return any pointer (int*, char*, struct*, etc.)
//    Just cast to void* when returning
//
// 2. DOUBLE POINTERS:
//    To modify a variable, pass its address
//    To modify a pointer, pass address of pointer (double pointer)
//    pthread_join needs to modify your pointer variable
//    So it needs pointer-to-pointer
//
// 3. VOID** PARAMETER:
//    pthread_join takes void** as second parameter
//    This is "address of a pointer to void"
//    Allows pthread_join to store thread's return value
//    If you don't care, pass NULL instead
//
// 4. TYPE CASTING:
//    Must cast &ret1 to (void**) to match pthread_join
//    All pointer types are same size, so casting is safe
//    This is how C achieves generic programming
//
// 5. NULL AS RETURN VALUE:
//    NULL is valid for any pointer type
//    Can return NULL to indicate "no data"
//    Different from returning meaningful pointer

// COMPARISON: With and Without Return Value
//
// WITHOUT capturing return value (threads1.c):
//   pthread_join(thread1, NULL);
//   Don't care about return value
//   Simpler
//
// WITH capturing return value (this file):
//   int* ret1;
//   pthread_join(thread1, (void**)&ret1);
//   Get the return value
//   More complex but more powerful

// MEMORY DIAGRAM:
//
// Before pthread_join:
//   ret1: [undefined garbage]
//   ret2: [undefined garbage]
//
// After pthread_create/pthread_join:
//   thread1 executes: func1() returns NULL
//   pthread_join stores: NULL into ret1
//
//   thread2 executes: func1() returns NULL
//   pthread_join stores: NULL into ret2
//
// Result:
//   ret1: NULL (0x0)
//   ret2: NULL (0x0)

// WHY THIS PATTERN IS USEFUL:
//
// In later examples (threads3.c), threads return REAL data:
//   - Pointers to malloc'd memory
//   - Computation results
//   - Status codes
//
// This example shows the MECHANISM
// Later examples show PRACTICAL uses

// COMPILE AND RUN:
// gcc threads2.c -o threads2 -pthread
// ./threads2
//
// EXPECTED OUTPUT:
// Hi I'm a thread!
// Hi I'm a thread!
// The value of ret1 is: (nil)
// The value of ret2 is: (nil)

// TRY IT:
// 1. Compile and run as-is
//    See that ret1 and ret2 are NULL
//
// 2. Remove the return NULL; line from func1
//    Recompile and run
//    What do you see? (Possibly 0x11, possibly other garbage)
//    This demonstrates undefined behavior
//
// 3. Change func1 to return a different value:
//    return (void*)0x42;
//    See that ret1 and ret2 become 0x42
//
// 4. Try forgetting the cast:
//    pthread_join(thread1, &ret1);
//    What error do you get?
//
// 5. Try wrong cast:
//    pthread_join(thread1, (void*)&ret1);
//    Compile and run - what happens? (Likely crash or garbage)

// NEXT STEPS:
// threads3.c shows how to return REAL data (malloc'd integers)
// threads4.c shows how to PASS data to threads
// Both build on the concepts learned here
