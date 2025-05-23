#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

/*
 * Structure containing both a resource and its protecting mutex
 * 
 * TEACHING POINT: This demonstrates an object-oriented approach to
 * thread synchronization: encapsulating both the shared resource and
 * its synchronization primitive in a single data structure. This is
 * a common pattern in multi-threaded programming, especially when
 * there are multiple independent resources to protect.
 */
struct threadData {
  long resource;
  pthread_mutex_t mut;
};

/*
 * Thread function that safely increments a resource counter
 * 
 * TEACHING POINT: This demonstrates how to access an encapsulated
 * resource in a thread-safe manner. The function receives a pointer
 * to the thread data structure, which contains both the resource and
 * its protecting mutex.
 */
void* inc(void* arg){
  /*
   * Cast the argument to the appropriate type
   * This is a common pattern when passing data to threads
   */
  struct threadData* d = (struct threadData*) arg;
  
  /*
   * Perform 100,000 thread-safe increments
   */
  for(int i = 0; i < 100000; i++){
    /*
     * Lock the mutex specific to this resource
     * 
     * TEACHING POINT: Notice how we're accessing the mutex that's part
     * of the same structure as the resource it protects. This ensures
     * the correct mutex is always used with its corresponding resource.
     */
    pthread_mutex_lock(&d->mut);
    
    /*
     * Critical section - increment the resource
     */
    d->resource++;
    
    /*
     * Release the mutex
     */
    pthread_mutex_unlock(&d->mut);
  }
  return NULL;
}

int main(){
  /*
   * Array to hold thread identifiers for 1000 threads
   */
  pthread_t threads[1000];

  /*
   * Create two separate threadData structures
   * 
   * TEACHING POINT: This demonstrates how to manage multiple independent
   * resources that need thread-safe access. Each resource has its own mutex,
   * allowing operations on different resources to proceed in parallel.
   */
  struct threadData ev;
  struct threadData odd;
  
  /*
   * Initialize the resources to 0
   */
  ev.resource = 0;
  odd.resource = 0;
  
  /*
   * Initialize the mutexes
   * 
   * TEACHING POINT: Each resource has its own mutex, demonstrating
   * fine-grained locking. This allows operations on different resources
   * to proceed in parallel, improving concurrency compared to using
   * a single mutex for all resources.
   */
  pthread_mutex_init(&ev.mut, NULL);
  pthread_mutex_init(&odd.mut, NULL);
  
  /*
   * Create 1000 threads, alternating between operating on even and odd resources
   * 
   * TEACHING POINT: This demonstrates how to distribute work across different
   * resources based on thread ID. Even-numbered threads increment the ev resource,
   * while odd-numbered threads increment the odd resource.
   */
  for (int i = 0; i < 1000; i++) {
    if(i % 2 == 0){
      /*
       * Even-numbered threads work on the 'ev' resource
       * We pass a pointer to the threadData structure as the thread argument
       */
      pthread_create(&threads[i], NULL, inc, &ev);
    }
    else{
      /*
       * Odd-numbered threads work on the 'odd' resource
       */
      pthread_create(&threads[i], NULL, inc, &odd);
    }
  }
  
  /*
   * Wait for all threads to complete
   */
  for (int i = 0; i < 1000; i++) {
    pthread_join(threads[i], NULL);
  }
  
  /*
   * Clean up the mutexes
   */
  pthread_mutex_destroy(&ev.mut);
  pthread_mutex_destroy(&odd.mut);

  /*
   * Print the final counter values
   * 
   * TEACHING POINT: With proper synchronization, each resource should have
   * been incremented 500 times with 100,000 increments each, for a total of
   * 50,000,000 per resource. The separation of resources allows for parallel
   * execution across the two groups of threads, potentially improving performance.
   */
  printf("Final even counter value: %ld (Expected: %d)\n", ev.resource, 500 * 100000);
  printf("Final odd counter value: %ld (Expected: %d)\n", odd.resource, 500 * 100000);
  pthread_exit(NULL);  
}