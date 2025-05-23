#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

/*
 * Thread function that returns a dynamically allocated result
 * 
 * TEACHING POINT: This demonstrates a common pattern for thread functions
 * that need to return data to the parent thread:
 * 1. Allocate memory on the heap (not stack)
 * 2. Store results in the allocated memory
 * 3. Return a pointer to the allocated memory
 */
void* weirdFunction(void* arg){
  /*
   * Allocate memory on the heap to store the result
   * 
   * TEACHING POINT: The memory MUST be allocated with malloc (heap)
   * not as a local variable (stack). Stack variables would be invalid
   * after the thread terminates, but heap allocations persist.
   */
  int* sleepPointer = malloc(sizeof(int));

  /*
   * Generate a random number between 0 and 4 and store it
   * in the allocated memory
   */
  *sleepPointer = rand() % 5;
  
  /*
   * Sleep for the generated number of seconds
   * This simulates the thread doing some variable-length work
   */
  sleep(*sleepPointer);

  /*
   * Return the pointer to the allocated memory
   * 
   * TEACHING POINT: The pointer will be passed back to the parent thread
   * through the pthread_join call. The memory it points to remains valid
   * because it was allocated on the heap, not the stack.
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
   * Variables to store the thread return values
   * 
   * TEACHING POINT: These void pointers will hold the addresses returned
   * by the thread functions. We use void* because we don't know the
   * specific type the thread will return (could be any pointer type).
   */
  void* res1;
  void* res2;

  /*
   * Create two threads
   */
  pthread_create(&thread1,NULL,weirdFunction,NULL);
  pthread_create(&thread2,NULL,weirdFunction,NULL);

  /*
   * TEACHING POINT: These comments explain how void** works in pthread_join
   * and why we pass &res1 instead of just res1.
   * 
   * void** is a pointer to a void pointer, allowing pthread_join to
   * modify the void* variable in the caller's scope.
   * 
   * The comments show that if we created a void** array, accessing
   * elements would follow normal pointer arithmetic (move by sizeof(void*)).
   */
  // void** resref = &res1;
  // resref[0] -> res1
  // resref[1] -> whatever is 8 bytes after res1 in memory
  
  
  /*
   * Wait for the first thread to complete and get its return value
   * 
   * TEACHING POINT: The second argument to pthread_join is a pointer to
   * a void pointer (void**). When the thread terminates, the value it
   * returned is stored in the location pointed to by this argument.
   */
  pthread_join(thread1,&res1);
  
  /*
   * Wait for the second thread to complete and get its return value
   */
  pthread_join(thread2,&res2);
  
  /*
   * TEACHING POINT: These comments explain how pthread_join stores
   * the thread's return value in the provided pointer.
   * 
   * Inside pthread_join, it gets the return value from the thread,
   * then stores it in the location pointed to by the void** parameter.
   */
  /*
    inside pthread_join(void** ref)
     void* temp = op(arg);
     *ref = temp;
   */

  /*
   * Print the values returned by the threads
   * 
   * TEACHING POINT: To access the integers returned by the threads,
   * we must cast the void pointers to the correct type (int*) and
   * then dereference them to get the actual integer values.
   */
  printf("Thread 1 did a sleep for %d seconds\n",*(int*)res1);
  printf("Thread 2 did a sleep for %d seconds\n",*(int*)res2);

  /*
   * TEACHING POINT: This program has a memory leak!
   * We allocated memory in the thread functions with malloc(),
   * but never freed it. We should have:
   *   free(res1);
   *   free(res2);
   * before returning.
   */
  return 0;
  
}