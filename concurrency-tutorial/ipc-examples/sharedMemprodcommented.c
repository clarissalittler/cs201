#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include "sharedstruct.h"

int main(){
  /*
   * Create and open a shared memory object
   * 
   * TEACHING POINT: shm_open() creates/opens a POSIX shared memory object:
   * - O_CREAT: Create the object if it doesn't exist
   * - O_RDWR: Open for both reading and writing
   * - 0666: Set permissions (read/write for all users)
   * 
   * This returns a file descriptor that can be used with mmap() to map
   * the shared memory into the process's address space.
   * 
   * As the producer, this process is responsible for creating the
   * shared memory object and setting its size.
   */
  int sharedFd = shm_open(sharedName, O_CREAT | O_RDWR, 0666);
  
  /*
   * Set the size of the shared memory object
   * 
   * TEACHING POINT: ftruncate() sets the size of the shared memory object.
   * This must be done before mapping it with mmap(). Without this step,
   * the shared memory would have size 0 and mmap() would fail.
   * 
   * The size is set to exactly fit our shared data structure.
   */
  ftruncate(sharedFd, sizeof(struct sharedData));
  
  /*
   * Map the shared memory object into the process's address space
   * 
   * TEACHING POINT: mmap() creates a mapping between the process's
   * address space and the shared memory object:
   * - NULL: Let the kernel choose where to place the mapping
   * - sizeof(struct sharedData): Size of the mapping
   * - PROT_READ | PROT_WRITE: Allow reading and writing
   * - MAP_SHARED: Updates to the mapping are visible to other processes
   * 
   * This gives us a pointer to the shared memory, which we can use as if
   * it were a normal structure in our process's memory.
   */
  struct sharedData* shared = mmap(NULL, sizeof(struct sharedData), PROT_READ | PROT_WRITE,
                                   MAP_SHARED, sharedFd, 0);
  
  /*
   * Initialize the semaphore in the shared structure
   * 
   * TEACHING POINT: sem_init() initializes the semaphore:
   * - &shared->mutex: Pointer to the semaphore
   * - 1: Set to 1 to indicate it's shared between processes
   * - 1: Initial value (1 = unlocked, 0 = locked)
   * 
   * This semaphore protects access to the shared data, ensuring that
   * the producer and consumer don't try to access it simultaneously.
   */
  sem_init(&shared->mutex, 1, 1);
  
  /*
   * Producer loop: generate and write data to shared memory
   * 
   * TEACHING POINT: This loop demonstrates the producer's role in
   * the producer-consumer pattern:
   * 1. Acquire exclusive access via semaphore
   * 2. Write data to the shared memory
   * 3. Release the semaphore
   * 4. Wait briefly before the next iteration
   */
  for(int i = 0; i < 10; i++){
    /*
     * Wait on the semaphore to get exclusive access
     */
    sem_wait(&shared->mutex);
    
    /*
     * Update the shared data
     * 
     * TEACHING POINT: This updates the two payload fields.
     * Since we're protected by the semaphore, we can make
     * multiple updates and they'll be seen as atomic by the consumer.
     */
    shared->payload1 = i;
    shared->payload2 = i*i;
    printf("Set %d th payload\n",i);
    
    /*
     * Release the semaphore
     */
    sem_post(&shared->mutex);
    
    /*
     * Delay before the next update
     * 
     * TEACHING POINT: This slows down the producer to make the
     * example easier to follow. In a real application, the producer
     * might generate data at irregular intervals.
     */
    sleep(1);
  }

  /*
   * Clean up resources
   * 
   * TEACHING POINT: Proper cleanup is essential in IPC:
   * 1. Unmap the shared memory from the address space
   * 2. Close the file descriptor
   * 3. Delete the shared memory object
   * 
   * As the producer (creator) of the shared memory, this process is
   * responsible for unlinking it when it's no longer needed.
   */
  munmap(shared, sizeof(struct sharedData));
  close(sharedFd);
  shm_unlink(sharedName);
  return 0;
}