#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

/*
 * This function will be executed by each thread
 * 
 * TEACHING POINT: Thread functions must have this specific signature:
 * void* function_name(void* arg)
 *
 * - They take a single void pointer parameter (which can point to any data)
 * - They return a void pointer (which can point to any result)
 * 
 * This allows threads to receive arbitrary input and return arbitrary output.
 */
void* ourPrinter(void* arg){
  /* 
   * We cast the void pointer to the expected type (char*)
   * This demonstrates a common pattern for thread functions:
   * 1. Cast the void* argument to the actual type
   * 2. Use the typed pointer
   */
  char* msg = (char*) arg;

  /* 
   * Print the message that was passed to the thread
   * Since both threads execute this same function with different
   * arguments, each thread will print a different message.
   */
  printf("Our thread says: %s", msg);

  /* 
   * Thread functions must return something, even if it's just NULL
   * Here we're returning 0 cast to a void pointer, which is 
   * equivalent to returning NULL.
   */
  return 0;
}

int main(){
  /* 
   * Define variables to hold thread identifiers
   * Each pthread_t uniquely identifies a thread in the system
   */
  pthread_t thread1;
  pthread_t thread2;

  /* 
   * Define messages to pass to each thread
   * These will be passed as arguments to the thread function
   * 
   * TEACHING POINT: The messages are allocated in the main thread's
   * stack memory, but they're accessible to the child threads because
   * all threads share the same address space.
   */
  char* msg1 = "Hi there I'm one thread\n";
  char* msg2 = "Hi there I'm a different thread\n";
  
  /* 
   * Create the first thread
   * Arguments:
   * 1. Pointer to pthread_t to store thread ID
   * 2. Thread attributes (NULL means use defaults)
   * 3. Function pointer to the code the thread will execute
   * 4. Argument to pass to the thread function
   * 
   * TEACHING POINT: pthread_create starts a new thread immediately.
   * The thread begins executing the ourPrinter function with msg1 as its argument.
   */
  pthread_create(&thread1,NULL,ourPrinter,msg1);
  
  /* 
   * Create the second thread
   * This thread will also execute the ourPrinter function, but with msg2 as its argument
   */
  pthread_create(&thread2,NULL,ourPrinter,msg2);

  /* 
   * Wait for the first thread to complete
   * 
   * TEACHING POINT: pthread_join blocks the calling thread until
   * the specified thread terminates. This demonstrates a synchronization
   * point between the main thread and the child threads.
   * 
   * Arguments:
   * 1. The thread ID to wait for
   * 2. Pointer to store the thread's return value (NULL means we don't care)
   */
  pthread_join(thread1,NULL);
  
  /* 
   * Wait for the second thread to complete
   */
  pthread_join(thread2,NULL);

  return 0;
  
}