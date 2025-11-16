// PEDAGOGICAL PURPOSE:
// This program demonstrates how to PASS DATA to a thread using the void* argument.
// Key learning objectives:
// 1. Understanding how to pass arguments to threads via void*
// 2. Learning to cast void* to the actual data type inside the thread function
// 3. Understanding why we pass the ADDRESS of a variable (not the value)
// 4. Seeing the complexity of pointer casting with void*
// 5. Understanding the "secret contract" between main and thread function
// 6. Learning the dereference-and-cast pattern: *((int*)arg)
// 7. Building on threads1.c to add data passing capability

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"

// IMPORTANT COMMENT FROM ORIGINAL CODE:
/*
  void and void* mean extremely different things

  void is the type of nothing
    - Functions that don't return anything
    - Functions that take no parameters

  void* is the universal donor of pointer types
    - A generic pointer that can point to ANY type
    - anything can be cast to void*
    - you can, at your own risk, cast void* to anything
    - This is C's mechanism for generic programming
    - But it's type-UNSAFE (compiler can't check if you cast correctly)

  (casting with pointers is *weird*)
    - You're not converting data, you're reinterpreting memory
*/

// WHY void* FOR THREADS (repeated for emphasis):
// void* is the only way to get polymorphism out of C
// the pthread_create function
// has to take the most generic possible
// function type as an argument
// and that's a thing that takes a void*
// and returns a void*
//
// This allows pthread_create to work with ANY data type
// The price: We must manually cast to the correct type

// THREAD FUNCTION WITH ARGUMENT:
// This function demonstrates receiving and using data from the main thread
void* func(void* arg){

  // THE VOID* CASTING DANCE:
  // arg arrives as void* (generic pointer)
  // But we KNOW it's actually an int* (because we passed &arg in main)
  // This is a "secret contract" between main and this function
  //
  // CASTING BREAKDOWN:
  // *((int*)arg)
  //   Step 1: (int*)arg      - Cast void* to int*
  //   Step 2: *((int*)arg)   - Dereference the int* to get the int value
  //
  // WHY SO COMPLEX?
  //   arg is void* - can't dereference void*
  //   (int*)arg is int* - we can dereference int*
  //   *((int*)arg) is int - the actual value
  //
  // ALTERNATIVE (more readable):
  //   int* int_ptr = (int*)arg;    // Cast to int*
  //   int value = *int_ptr;        // Dereference to get value
  //   printf("... %d\n", value);

  // now arg is SECRETLY a int*
  // We cast and dereference in one expression
  printf("Hi I'm a thread and I'm holding: %d\n",
	 *((int*)arg));

  return NULL;
}

int main(){

  // THREAD HANDLE:
  // Same as before - handle for the thread
  pthread_t thread; // the data structure that holds all the information about a thread

  // THE DATA TO PASS:
  // Create a local variable with value 10
  // This will be passed to the thread function
  //
  // IMPORTANT: This is a stack variable in main
  // The thread will receive a POINTER to this variable
  // So the thread will access memory that belongs to main's stack
  // This is safe because main waits (via join) before exiting
  int arg = 10;

  // CREATE THREAD WITH ARGUMENT:
  // pthread_create(&thread, NULL, func, &arg)
  //
  // THE FOURTH PARAMETER: &arg
  // This is the KEY difference from threads1.c!
  //
  // WHY &arg (address) and not just arg?
  //   - pthread_create expects void* (a pointer)
  //   - arg is an int (not a pointer)
  //   - &arg is the address of the int (an int*)
  //   - int* can be implicitly converted to void*
  //
  // THE TYPE CONVERSION:
  //   &arg has type int*
  //   pthread_create expects void*
  //   int* is automatically converted to void*
  //   Inside func, we convert back from void* to int*
  //
  // WHAT GETS PASSED:
  //   The ADDRESS of the variable arg
  //   NOT the value 10 itself
  //   The thread will dereference this address to get 10
  pthread_create(&thread,NULL,func,&arg);

  // COMMENT FROM ORIGINAL CODE (still relevant):
  // the third argument of pthread_create
  // is a function!
  // hence why I was talking about function pointers
  // it's not (entirely) because I'm whacked out with poo-brain

  // WAIT FOR THREAD:
  // Must wait for thread to complete before main exits
  // This is CRITICAL because:
  //   1. The thread is accessing 'arg' on main's stack
  //   2. If main exits, its stack is destroyed
  //   3. The thread would be accessing invalid memory (use-after-free)
  pthread_join(thread,NULL);


  return 0;
}

// EXPECTED OUTPUT:
// Hi I'm a thread and I'm holding: 10

// THE DATA FLOW:
//
// Step 1: main creates int arg = 10
//   Memory: Stack of main
//   Address: (let's say 0x7fff1234)
//   Value: 10
//
// Step 2: main calls pthread_create(..., &arg)
//   Passes: Address 0x7fff1234 (as void*)
//
// Step 3: Thread receives argument
//   Receives: void* pointing to 0x7fff1234
//
// Step 4: Thread casts and dereferences
//   Cast: void* → int*
//   Dereference: Read value from 0x7fff1234
//   Result: 10

// CASTING PATTERNS:
//
// PATTERN 1: All-in-one (used in this program)
//   *((int*)arg)
//   Pros: Compact
//   Cons: Hard to read
//
// PATTERN 2: Step-by-step
//   int* ptr = (int*)arg;
//   int value = *ptr;
//   Pros: Clear and readable
//   Cons: Extra variable
//
// PATTERN 3: Access through cast pointer
//   ((int*)arg)[0]
//   Pros: Can be used for arrays
//   Cons: Obscure notation

// COMMON MISTAKES:
//
// MISTAKE 1: Passing value instead of address
//   pthread_create(&thread, NULL, func, arg);  // WRONG!
//   This tries to pass the VALUE 10 as a pointer
//   Compiler warning: "passing argument 4 of 'pthread_create' makes pointer from integer"
//
// MISTAKE 2: Wrong cast in thread function
//   char c = *((char*)arg);  // WRONG TYPE!
//   This would reinterpret the int as a char
//   Would get unexpected/garbage values
//
// MISTAKE 3: Forgetting to dereference
//   printf("%d\n", (int*)arg);  // WRONG!
//   This prints the POINTER (address), not the VALUE
//   Would print something like: 140734799825972
//
// MISTAKE 4: Using argument after main exits
//   If you DON'T join the thread before main returns,
//   the thread might access destroyed stack memory
//   This is undefined behavior (crash, corruption, or "works" by luck)

// THE void* TYPE SYSTEM HOLE:
//
// C's type system is WEAK for void*
// The compiler allows ANY pointer → void* → ANY pointer
// This means you can cast incorrectly and the compiler won't stop you!
//
// EXAMPLE OF TYPE ERROR:
//   int x = 10;
//   void* ptr = &x;
//   double* dptr = (double*)ptr;  // WRONG but compiles!
//   double d = *dptr;              // Undefined behavior!
//
// This is why void* is powerful but DANGEROUS
// The programmer must ensure type safety manually

// MEMORY SAFETY CONCERN:
//
// In this program, we pass &arg where arg is on main's stack
//
// SAFE because:
//   - main waits with pthread_join()
//   - main's stack is valid until pthread_join() returns
//   - Thread finishes before main's stack is destroyed
//
// WOULD BE UNSAFE if:
//   - main returned before thread finished
//   - arg was a local variable in a function that returned
//   - Example:
//       void create_thread() {
//         int local_var = 10;
//         pthread_create(&thread, NULL, func, &local_var);
//         // DON'T JOIN HERE
//       }  // local_var is destroyed when function returns!
//       // Thread now has a dangling pointer!

// ALTERNATIVE: HEAP ALLOCATION
//
// For long-lived threads, allocate argument on heap:
//   int* arg = malloc(sizeof(int));
//   *arg = 10;
//   pthread_create(&thread, NULL, func, arg);
//
// Inside thread:
//   int value = *((int*)arg);
//   free(arg);  // Thread is responsible for freeing!

// TRY IT:
// 1. Compile: gcc threads2.c -o threads2 -pthread
// 2. Run: ./threads2
// 3. Verify output is: Hi I'm a thread and I'm holding: 10
// 4. EXPERIMENT: Change arg to different values (42, -5, 0)
// 5. EXPERIMENT: Pass a different type:
//    double d = 3.14;
//    pthread_create(&thread, NULL, func, &d);
//    Inside func: *((double*)arg)
// 6. EXPERIMENT: What happens if you print arg without casting?
//    printf("%p\n", arg);  // Prints the address
// 7. EXPERIMENT: What happens with wrong cast?
//    printf("%c\n", *((char*)arg));  // Reinterprets as char
// 8. CHALLENGE: Pass a string (char*) to the thread
//    Hint: char* str = "Hello"; pthread_create(..., str);
//    Inside thread: char* message = (char*)arg;
