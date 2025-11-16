// PEDAGOGICAL PURPOSE:
// This program demonstrates SHARED MEMORY CONSUMER - reads data produced by sharedMemprod.c
// Key learning objectives:
// 1. Understanding consumer side of shared memory IPC
// 2. Learning that consumer opens existing shared memory (no O_CREAT)
// 3. Seeing synchronized reading with semaphores
// 4. Understanding cleanup responsibilities (who unlinks?)
// 5. Demonstrating the producer-consumer synchronization pattern

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "sharedstruct.h"

int main(){
  // STEP 1: OPEN EXISTING SHARED MEMORY
  //
  // shm_open(name, O_RDWR, 0666):
  //   - NO O_CREAT flag: Expecting it to already exist
  //   - Producer must have already created it
  //   - If doesn't exist: shm_open() fails
  //   - O_RDWR: Read and write access (though we only read)
  //
  // PRODUCER vs CONSUMER:
  //   - Producer: O_CREAT | O_RDWR (creates)
  //   - Consumer: O_RDWR (opens existing)
  int sharedFd = shm_open(sharedName, O_RDWR, 0666);

  // STEP 2: MAP SHARED MEMORY
  // Same mmap() call as producer
  // Maps the SAME physical memory into this process
  //
  // SHARED MEMORY MAGIC:
  //   - Producer and consumer have different address spaces
  //   - But shared points to SAME physical RAM
  //   - Changes by producer immediately visible to consumer
  //   - No copying, no kernel involvement
  struct sharedData* shared = mmap(NULL, sizeof(struct sharedData),
                                   PROT_READ | PROT_WRITE,
                                   MAP_SHARED, sharedFd, 0);

  // STEP 3: CONSUME DATA
  // Read 10 data items produced by producer
  for(int i = 0; i < 10; i++){

    // ACQUIRE SEMAPHORE:
    // Wait for exclusive access to shared memory
    // If producer is writing, we wait
    // If available, we lock and proceed
    sem_wait(&shared->mutex);

    // READ DATA FROM SHARED MEMORY:
    // Access payload1 and payload2
    // Semaphore ensures we read consistent pair
    // Producer might be updating next pair, but we can't see it yet
    printf("Payload received: %d,%d\n", shared->payload1, shared->payload2);

    // RELEASE SEMAPHORE:
    // Allow producer to write next item
    sem_post(&shared->mutex);

    // SLEEP:
    // Match producer's timing
    // In real application, might wait on condition variable instead
    sleep(1);
  }

  // CLEANUP: UNMAP
  munmap(shared, sizeof(struct sharedData));

  // CLEANUP: CLOSE FD
  close(sharedFd);

  // CLEANUP: UNLINK
  //
  // IMPORTANT DECISION: Who unlinks?
  //   - This consumer unlinks
  //   - Producer also unlinks
  //   - Both calling shm_unlink() is OK
  //   - First unlink marks for deletion
  //   - Second unlink might fail (already unlinked)
  //   - Actual deletion happens when all processes munmap()
  //
  // ALTERNATIVE:
  //   - Only producer unlinks
  //   - Consumer doesn't call shm_unlink()
  //   - Clearer responsibility
  shm_unlink(sharedName);

  return 0;
}

// EXPECTED OUTPUT:
// Payload received: 0,0
// Payload received: 1,1
// Payload received: 2,4
// Payload received: 3,9
// ...
// Payload received: 9,81

// SYNCHRONIZATION GUARANTEES:
//
// WITH SEMAPHORES:
//   - Consumer always sees consistent pair (i, i*i)
//   - Never sees partial update
//   - Producer and consumer take turns
//
// WITHOUT SEMAPHORES (if we remove sem_wait/sem_post):
//   - Race conditions possible
//   - Might see: payload1=5, payload2=16 (should be 25)
//   - Undefined behavior - timing dependent

// TIMING DIAGRAM:
//
// Time  Producer                Consumer
// ----  --------                --------
// 0     lock, write (0,0), unlock
// 0.5                           lock, read (0,0), unlock
// 1     lock, write (1,1), unlock
// 1.5                           lock, read (1,1), unlock
// 2     lock, write (2,4), unlock
// 2.5                           lock, read (2,4), unlock
// ...

// TRY IT:
// 1. Run producer and consumer together
// 2. See synchronized output
// 3. EXPERIMENT: Start consumer before producer (fails - no shared memory yet)
// 4. EXPERIMENT: Run two consumers simultaneously
// 5. CHALLENGE: Modify to use ring buffer for multiple items
