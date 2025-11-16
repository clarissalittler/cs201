// PEDAGOGICAL PURPOSE:
// This program demonstrates SHARED MEMORY PRODUCER using POSIX shared memory and semaphores.
// Key learning objectives:
// 1. Understanding shared memory as the fastest IPC mechanism
// 2. Learning shm_open(), ftruncate(), and mmap() for shared memory creation
// 3. Understanding semaphores for synchronization (mutual exclusion)
// 4. Seeing the producer-consumer pattern with shared data
// 5. Learning about the sharedstruct.h header for common definitions
// 6. Understanding why synchronization is critical with shared memory

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>       // For O_CREAT, O_RDWR
#include <sys/mman.h>    // For shm_open(), mmap(), munmap(), shm_unlink()
#include <unistd.h>      // For ftruncate(), sleep()
#include "sharedstruct.h" // Contains: struct sharedData, sharedName, sem_t mutex

int main(){
  // STEP 1: CREATE SHARED MEMORY OBJECT
  //
  // shm_open(name, flags, permissions):
  //   - name: "/shared_data_mem" (must start with /)
  //   - O_CREAT: Create if doesn't exist
  //   - O_RDWR: Read and write access
  //   - 0666: Permissions (rw-rw-rw-)
  //   - Returns file descriptor for shared memory object
  //
  // SHARED MEMORY vs FILES:
  //   - Stored in RAM, not disk
  //   - Much faster than file I/O
  //   - Persists until explicitly removed (shm_unlink)
  //   - Visible in /dev/shm/ on Linux
  int sharedFd = shm_open(sharedName, O_CREAT | O_RDWR, 0666);

  // STEP 2: SET SIZE OF SHARED MEMORY
  //
  // ftruncate(fd, size):
  //   - Sets size of shared memory object
  //   - Must be done before mmap()
  //   - Size = sizeof(struct sharedData)
  //   - Includes space for: mutex semaphore, payload1, payload2
  ftruncate(sharedFd, sizeof(struct sharedData));

  // STEP 3: MAP SHARED MEMORY INTO PROCESS ADDRESS SPACE
  //
  // mmap(NULL, size, protection, flags, fd, offset):
  //   - NULL: Kernel chooses address
  //   - sizeof(struct sharedData): Size to map
  //   - PROT_READ | PROT_WRITE: Can read and write
  //   - MAP_SHARED: Changes visible to other processes
  //   - sharedFd: File descriptor from shm_open()
  //   - 0: Offset (start from beginning)
  //
  // RETURN VALUE:
  //   - Pointer to shared memory region
  //   - Cast to (struct sharedData*)
  //   - Can now access shared->payload1, shared->mutex, etc.
  //
  // WHY mmap()?
  //   - Makes shared memory appear as regular memory
  //   - Can access with normal pointer operations
  //   - No need for special read/write functions
  //   - Direct memory access = FASTEST IPC
  struct sharedData* shared = mmap(NULL, sizeof(struct sharedData),
                                   PROT_READ | PROT_WRITE,
                                   MAP_SHARED, sharedFd, 0);

  // STEP 4: INITIALIZE SEMAPHORE
  //
  // sem_init(&shared->mutex, 1, 1):
  //   - &shared->mutex: Address of semaphore in shared memory
  //   - 1: Shared between processes (not just threads)
  //   - 1: Initial value (binary semaphore, unlocked)
  //
  // BINARY SEMAPHORE:
  //   - Value 1: Unlocked, available
  //   - Value 0: Locked, in use
  //   - Used for mutual exclusion (mutex)
  //
  // WHY SEMAPHORE?
  //   - Shared memory has no built-in synchronization
  //   - Multiple processes can access simultaneously
  //   - Without semaphore: RACE CONDITIONS
  //   - With semaphore: Safe, coordinated access
  sem_init(&shared->mutex, 1, 1);

  // STEP 5: PRODUCE DATA
  // Generate 10 data items and store in shared memory
  for(int i = 0; i < 10; i++){

    // ACQUIRE SEMAPHORE (LOCK):
    // sem_wait(&shared->mutex):
    //   - If semaphore value > 0: Decrement (1 → 0) and proceed
    //   - If semaphore value = 0: Block until it becomes available
    //   - Ensures only ONE process accesses shared data at a time
    //
    // CRITICAL SECTION BEGINS:
    sem_wait(&shared->mutex);

    // WRITE DATA TO SHARED MEMORY:
    // Producer writes two related values
    //   - payload1: i (0, 1, 2, ..., 9)
    //   - payload2: i*i (0, 1, 4, 9, ..., 81)
    //
    // WHY CRITICAL SECTION?
    //   - Writing two values should be atomic
    //   - Without semaphore, consumer might read between writes
    //   - Consumer could see: payload1=5, payload2=16 (inconsistent!)
    //   - With semaphore: Consumer sees consistent pair
    shared->payload1 = i;
    shared->payload2 = i*i;

    printf("Set %d th payload\n", i);

    // RELEASE SEMAPHORE (UNLOCK):
    // sem_post(&shared->mutex):
    //   - Increments semaphore value (0 → 1)
    //   - Signals that resource is available
    //   - Allows consumer to acquire and read data
    //
    // CRITICAL SECTION ENDS:
    sem_post(&shared->mutex);

    // SLEEP FOR DEMONSTRATION:
    // Pause 1 second between produces
    // Allows consumer time to read
    // Makes race conditions more visible if semaphore removed
    sleep(1);
  }

  // CLEANUP: UNMAP SHARED MEMORY
  // Release virtual address mapping
  // Shared memory object still exists for consumer
  munmap(shared, sizeof(struct sharedData));

  // CLEANUP: CLOSE FILE DESCRIPTOR
  // Done with this descriptor
  // Shared memory object still exists
  close(sharedFd);

  // CLEANUP: REMOVE SHARED MEMORY OBJECT
  // shm_unlink(name):
  //   - Marks shared memory for deletion
  //   - Actual deletion when all processes unmap it
  //   - Like rm for files
  //
  // TIMING:
  //   - Producer finishes before consumer (both sleep 10 times)
  //   - Producer unlinks first
  //   - Consumer still has it mapped, can still access
  //   - Final consumer munmap() actually deletes it
  shm_unlink(sharedName);

  return 0;
}

// EXPECTED OUTPUT:
// Set 0 th payload
// (1 second pause)
// Set 1 th payload
// (1 second pause)
// ...
// Set 9 th payload

// SHARED MEMORY ADVANTAGES:
//
// 1. FASTEST IPC:
//   - No copying data between processes
//   - Direct memory access
//   - No kernel involvement for access (only for setup)
//
// 2. LARGE DATA:
//   - Efficient for sharing large data structures
//   - Arrays, images, databases
//
// 3. BIDIRECTIONAL:
//   - Both processes can read and write
//   - No separate channels needed

// SHARED MEMORY CHALLENGES:
//
// 1. SYNCHRONIZATION REQUIRED:
//   - No built-in locking
//   - Must use semaphores, mutexes, or other mechanisms
//   - Easy to create race conditions
//
// 2. MORE COMPLEX:
//   - More steps than pipes or FIFOs
//   - Need to manage lifecycle (create, resize, map, unmap, unlink)
//
// 3. NO AUTOMATIC CLEANUP:
//   - If process crashes, shared memory remains
//   - Check /dev/shm/ for orphaned objects
//   - Need manual shm_unlink()

// SEMAPHORE PATTERN:
//
// sem_wait(&mutex);    // Lock
// // Critical section: access shared data
// shared->data = value;
// sem_post(&mutex);    // Unlock

// TRY IT:
// 1. Compile producer: gcc sharedMemprod.c -o producer -lrt -lpthread
//    (-lrt for shm_open, -lpthread for sem_init)
// 2. Compile consumer: gcc sharedMemcons.c -o consumer -lrt -lpthread
// 3. Run producer: ./producer &
// 4. Run consumer: ./consumer
// 5. See synchronized data transfer
// 6. EXPERIMENT: Comment out sem_wait/sem_post
//    See race conditions (inconsistent data)
// 7. EXPERIMENT: Check /dev/shm/ while running
//    See shared memory object
// 8. CHALLENGE: Add third process that also reads
// 9. CHALLENGE: Use condition variables instead of sleep()
