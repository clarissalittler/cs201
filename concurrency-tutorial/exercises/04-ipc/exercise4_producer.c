/*
 * Exercise 4: Shared Memory - Producer
 *
 * Objective: Create a producer that writes data to shared memory,
 * using semaphores for synchronization.
 *
 * Requirements:
 * - Create/open shared memory segment
 * - Initialize semaphore for synchronization
 * - Produce 10 data values
 * - Write each value to shared memory
 * - Use semaphore to prevent race conditions
 * - Clean up properly (but don't unlink - let consumer do that)
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "exercise4_shared.h"

int main() {
    // TODO: Open/create the shared memory object
    // int shm_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    // Check for errors

    // TODO: Set the size of the shared memory
    // ftruncate(shm_fd, sizeof(struct shared_data));

    // TODO: Map the shared memory into process address space
    // struct shared_data* shared = mmap(NULL, sizeof(struct shared_data),
    //                                   PROT_READ | PROT_WRITE,
    //                                   MAP_SHARED, shm_fd, 0);
    // Check for errors

    // TODO: Initialize the semaphore
    // sem_init(&shared->mutex, 1, 1);
    // Note: Second parameter = 1 means shared between processes

    printf("Producer: Starting to produce data...\n");

    // TODO: Produce and write data
    // for(int i = 0; i < 10; i++) {
    //     - Wait on semaphore (sem_wait)
    //     - Write data to shared memory
    //     - Set is_ready flag
    //     - Print what was produced
    //     - Signal semaphore (sem_post)
    //     - Sleep briefly to simulate work
    // }

    printf("Producer: Finished producing data.\n");

    // TODO: Clean up
    // - Unmap shared memory (munmap)
    // - Close shared memory file descriptor (close)
    // Note: Don't shm_unlink() - let consumer do that

    return 0;
}
