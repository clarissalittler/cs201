/*
 * Exercise 4: Shared Memory - Consumer
 *
 * Objective: Create a consumer that reads data from shared memory,
 * using semaphores for synchronization.
 *
 * Requirements:
 * - Open existing shared memory segment
 * - Read 10 data values from shared memory
 * - Use semaphore to prevent race conditions
 * - Display consumed values
 * - Clean up and unlink shared memory
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "exercise4_shared.h"

int main() {
    // TODO: Open the existing shared memory object
    // int shm_fd = shm_open(SHARED_MEM_NAME, O_RDWR, 0666);
    // Check for errors

    // TODO: Map the shared memory into process address space
    // struct shared_data* shared = mmap(NULL, sizeof(struct shared_data),
    //                                   PROT_READ | PROT_WRITE,
    //                                   MAP_SHARED, shm_fd, 0);
    // Check for errors

    printf("Consumer: Starting to consume data...\n");

    // TODO: Consume data
    // for(int i = 0; i < 10; i++) {
    //     - Wait on semaphore (sem_wait)
    //     - Read data from shared memory
    //     - Print the consumed data
    //     - Clear is_ready flag (optional)
    //     - Signal semaphore (sem_post)
    //     - Sleep briefly
    // }

    printf("Consumer: Finished consuming data.\n");

    // TODO: Clean up
    // - Destroy semaphore (sem_destroy)
    // - Unmap shared memory (munmap)
    // - Close shared memory file descriptor (close)
    // - Unlink shared memory (shm_unlink)

    return 0;
}
