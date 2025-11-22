/*
 * Exercise 4 Solution: Shared Memory - Producer
 *
 * This producer creates shared memory, initializes it, and writes data.
 * It uses semaphores to ensure synchronized access with the consumer.
 *
 * Key concepts:
 * - Creating shared memory with shm_open()
 * - Setting size with ftruncate()
 * - Mapping memory with mmap()
 * - Initializing semaphores with sem_init()
 * - Synchronization with sem_wait() and sem_post()
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include "exercise4_shared_solution.h"

#define NUM_ITEMS 10

int main() {
    int shm_fd;
    struct shared_data* shared;

    printf("Producer: Starting up...\n");

    // Create or open the shared memory object
    // O_CREAT: Create if it doesn't exist
    // O_RDWR: Open for reading and writing
    // 0666: Permissions (read/write for owner, group, others)
    shm_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    printf("Producer: Shared memory created/opened\n");

    // Set the size of the shared memory object
    // Without this, the memory object has size 0
    if (ftruncate(shm_fd, sizeof(struct shared_data)) == -1) {
        perror("ftruncate");
        close(shm_fd);
        shm_unlink(SHARED_MEM_NAME);
        return 1;
    }

    // Map the shared memory into this process's address space
    // NULL: Let system choose the address
    // PROT_READ | PROT_WRITE: Allow reading and writing
    // MAP_SHARED: Changes are visible to other processes
    shared = mmap(NULL, sizeof(struct shared_data),
                  PROT_READ | PROT_WRITE,
                  MAP_SHARED, shm_fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        shm_unlink(SHARED_MEM_NAME);
        return 1;
    }

    printf("Producer: Shared memory mapped\n");

    // Initialize the semaphore
    // &shared->mutex: Pointer to the semaphore
    // 1: Shared between processes (not just threads)
    // 1: Initial value (unlocked, available)
    if (sem_init(&shared->mutex, 1, 1) == -1) {
        perror("sem_init");
        munmap(shared, sizeof(struct shared_data));
        close(shm_fd);
        shm_unlink(SHARED_MEM_NAME);
        return 1;
    }

    // Initialize shared data
    shared->is_ready = 0;
    shared->sequence = 0;
    shared->value = 0;

    printf("Producer: Starting to produce data...\n");
    printf("========================================\n");

    // Produce data
    for (int i = 0; i < NUM_ITEMS; i++) {
        // Wait (lock) the semaphore
        // This ensures exclusive access to shared memory
        sem_wait(&shared->mutex);

        // Critical section: modify shared data
        shared->sequence = i + 1;
        shared->value = (i + 1) * 10;  // Simple data: 10, 20, 30, ...
        shared->is_ready = 1;

        printf("Producer: Produced item #%d, value = %d\n",
               shared->sequence, shared->value);

        // Signal (unlock) the semaphore
        // This allows consumer to access shared memory
        sem_post(&shared->mutex);

        // Sleep briefly to simulate work and allow consumer to run
        sleep(1);
    }

    printf("========================================\n");
    printf("Producer: Finished producing data.\n");

    // Cleanup
    // Unmap the shared memory
    if (munmap(shared, sizeof(struct shared_data)) == -1) {
        perror("munmap");
    }

    // Close the shared memory file descriptor
    close(shm_fd);

    // Note: We don't call shm_unlink() here because the consumer
    // still needs to access the shared memory. The consumer will
    // clean it up when done.

    printf("Producer: Cleanup complete. Exiting.\n");

    return 0;
}
