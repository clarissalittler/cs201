#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

/*
 * Thread function that returns a dynamically allocated result
 * 
 * TEACHING POINT: This function demonstrates how threads can return values
 * back to the thread that created them. It shows a common pattern:
 * 1. Allocate memory for the result (must be heap memory, not stack!)
 * 2. Compute the result and store it in the allocated memory
 * 3. Return a pointer to the allocated memory
 */
void* weirdFunction(void* arg){
  /* 
   * Allocate memory on the heap to hold the result
   * 
   * TEACHING POINT: This memory MUST be allocated with malloc (heap)
   * and not as a local variable (stack). Stack variables are deallocated
   * when the function returns, but we need this memory to persist after
   * the thread terminates so the main thread can access it.
   */
  int* sleepPointer = malloc(sizeof(int));

  /* 
   * Generate a random value between 1 and 5
   * This will be both the sleep duration and our "result"
   */
  *sleepPointer = rand()%5+1;

  /* 
   * Sleep for the random duration
   * This simulates the thread doing some variable-length work
   */
  sleep(*sleepPointer);
  
  /* 
   * Return the pointer to the allocated memory
   * 
   * TEACHING POINT: The memory is allocated on the heap,
   * so it remains valid even after this function returns.
   * The calling thread is responsible for freeing this memory
   * when it's no longer needed (though this example doesn't do that).
   */
  return sleepPointer;
}

int main(){
  /* 
   * Initialize random number generator
   */
  srand(time(0));
  
  /* 
   * Define variables to hold thread identifiers
   */
  pthread_t thread1;
  pthread_t thread2;

  /* 
   * Variables to hold thread return values
   * 
   * TEACHING POINT: These variables will receive pointers to
   * the memory allocated by each thread. They're declared as void*
   * because pthread_join accepts a void** parameter.
   */
  void* res1;
  void* res2;

  /* 
   * Create two threads
   */
  pthread_create(&thread1,NULL,weirdFunction,NULL);
  pthread_create(&thread2,NULL,weirdFunction,NULL);

  /* 
   * Wait for threads to complete and capture their return values
   * 
   * TEACHING POINT: The second argument to pthread_join is a pointer
   * to a void pointer (void**). When the thread terminates, the value
   * it returned (via return or pthread_exit) is stored in the location
   * pointed to by this argument. This is how threads communicate results
   * back to the thread that created them.
   */
  pthread_join(thread1,&res1);
  pthread_join(thread2,&res2);

  /* 
   * Access and print the results
   * 
   * TEACHING POINT: We must cast the void* back to the appropriate
   * type (int*) before dereferencing it to access the actual value.
   * This demonstrates the full cycle of thread return values:
   * 1. Thread allocates and populates memory
   * 2. Thread returns pointer to that memory
   * 3. Main thread receives the pointer via pthread_join
   * 4. Main thread casts and dereferences to access the value
   */
  printf("Thread 1 did a sleep for %d seconds\n",*(int*)res1);
  printf("Thread 2 did a sleep for %d seconds\n",*(int*)res2);
  
  /* 
   * MISSING TEACHING POINT: This program has a memory leak!
   * The memory allocated by the threads is never freed.
   * It should have these lines before returning:
   *   free(res1);
   *   free(res2);
   */
  return 0;
}