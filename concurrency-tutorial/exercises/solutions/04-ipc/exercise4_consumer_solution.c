/*
 * Exercise 4 Solution: Shared Memory - Consumer
 *
 * This consumer opens existing shared memory and reads data.
 * It uses semaphores to ensure synchronized access with the producer.
 *
 * Key concepts:
 * - Opening existing shared memory with shm_open()
 * - Mapping memory with mmap()
 * - Synchronization with sem_wait() and sem_post()
 * - Cleaning up shared resources with sem_destroy() and shm_unlink()
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "exercise4_shared_solution.h"

#define NUM_ITEMS 10

int main() {
    int shm_fd;
    struct shared_data* shared;

    printf("Consumer: Starting up...\n");

    // Open the existing shared memory object
    // O_RDWR: Open for reading and writing
    // Note: No O_CREAT flag - we expect producer to create it first
    shm_fd = shm_open(SHARED_MEM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        fprintf(stderr, "Consumer: Make sure producer is running first!\n");
        return 1;
    }

    printf("Consumer: Shared memory opened\n");

    // Map the shared memory into this process's address space
    shared = mmap(NULL, sizeof(struct shared_data),
                  PROT_READ | PROT_WRITE,
                  MAP_SHARED, shm_fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        return 1;
    }

    printf("Consumer: Shared memory mapped\n");
    printf("Consumer: Starting to consume data...\n");
    printf("========================================\n");

    // Consume data
    for (int i = 0; i < NUM_ITEMS; i++) {
        // Wait (lock) the semaphore
        // This ensures exclusive access to shared memory
        sem_wait(&shared->mutex);

        // Critical section: read shared data
        if (shared->is_ready) {
            printf("Consumer: Consumed item #%d, value = %d\n",
                   shared->sequence, shared->value);
            shared->is_ready = 0;  // Mark as consumed
        } else {
            printf("Consumer: No data ready at iteration %d\n", i + 1);
        }

        // Signal (unlock) the semaphore
        // This allows producer to access shared memory
        sem_post(&shared->mutex);

        // Sleep briefly to simulate work
        sleep(1);
    }

    printf("========================================\n");
    printf("Consumer: Finished consuming data.\n");

    // Cleanup
    // Destroy the semaphore
    // This should be done by the last process to use it
    if (sem_destroy(&shared->mutex) == -1) {
        perror("sem_destroy");
    }

    // Unmap the shared memory
    if (munmap(shared, sizeof(struct shared_data)) == -1) {
        perror("munmap");
    }

    // Close the shared memory file descriptor
    close(shm_fd);

    // Unlink (remove) the shared memory object
    // This removes it from the system
    // Should be done by the last process to use it
    if (shm_unlink(SHARED_MEM_NAME) == -1) {
        perror("shm_unlink");
    }

    printf("Consumer: Cleanup complete. Exiting.\n");

    return 0;
}
