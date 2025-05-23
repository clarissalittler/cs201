#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include "sharedstruct.h"

int main(){
  /*
   * Open the existing shared memory object
   * 
   * TEACHING POINT: Unlike the producer, the consumer doesn't create
   * the shared memory (no O_CREAT flag). It assumes the producer has
   * already created and initialized the shared memory object.
   * 
   * This is a common pattern in IPC: one process is responsible for
   * creating and initializing shared resources, and others simply use them.
   */
  int sharedFd = shm_open(sharedName, O_RDWR, 0666);
  
  /*
   * Map the shared memory object into the process's address space
   * 
   * TEACHING POINT: The mapping parameters must match those used by
   * the producer, especially the size and protection flags. This gives
   * the consumer the same view of the shared memory as the producer.
   * 
   * Note that the consumer doesn't need to call ftruncate() since
   * the producer has already set the size of the shared memory object.
   */
  struct sharedData* shared = mmap(NULL, sizeof(struct sharedData), PROT_READ | PROT_WRITE,
                                   MAP_SHARED, sharedFd, 0);
  
  /*
   * Consumer loop: read data from shared memory
   * 
   * TEACHING POINT: This loop demonstrates the consumer's role in
   * the producer-consumer pattern:
   * 1. Acquire exclusive access via semaphore
   * 2. Read data from the shared memory
   * 3. Release the semaphore
   * 4. Wait briefly before the next iteration
   */
  for(int i = 0; i < 10; i++){
    /*
     * Wait on the semaphore to get exclusive access
     * 
     * TEACHING POINT: This ensures that the consumer doesn't read
     * the data while the producer is updating it, which could lead
     * to inconsistent values (e.g., reading payload1 after it's updated
     * but payload2 before it's updated).
     */
    sem_wait(&shared->mutex);
    
    /*
     * Read and display the shared data
     * 
     * TEACHING POINT: The consumer simply reads the payload values
     * set by the producer. Thanks to the semaphore, it's guaranteed
     * to see consistent updates.
     */
    printf("Payload received: %d,%d\n",shared->payload1,shared->payload2);
    
    /*
     * Release the semaphore
     */
    sem_post(&shared->mutex);
    
    /*
     * Delay before the next read
     * 
     * TEACHING POINT: This simulates the consumer processing the data
     * before checking for more. In a real application, the consumer might
     * process data at its own pace, independent of the producer.
     */
    sleep(1);
  }

  /*
   * Clean up resources
   * 
   * TEACHING POINT: The consumer also needs to clean up its resources:
   * 1. Unmap the shared memory from its address space
   * 2. Close the file descriptor
   * 3. Delete the shared memory object
   * 
   * Note that both the producer and consumer call shm_unlink().
   * This is actually a potential issue - whichever process calls it first
   * will remove the object, potentially before the other process is done with it.
   * In a real application, you'd need more sophisticated cleanup coordination.
   */
  munmap(shared, sizeof(struct sharedData));
  close(sharedFd);
  shm_unlink(sharedName);
  return 0;
}