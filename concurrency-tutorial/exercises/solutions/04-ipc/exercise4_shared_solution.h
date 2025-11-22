/*
 * Exercise 4 Solution: Shared Memory - Header File
 *
 * This header defines the shared data structure used by both
 * producer and consumer processes.
 *
 * Key concepts:
 * - Shared data structure definition
 * - Semaphore for synchronization
 * - Shared memory naming
 */

#ifndef EXERCISE4_SHARED_H
#define EXERCISE4_SHARED_H

#include <semaphore.h>

/*
 * Shared data structure
 * This structure will be mapped into the address space of both
 * producer and consumer processes.
 */
struct shared_data {
    sem_t mutex;        // Semaphore for mutual exclusion
    int value;          // The data value being shared
    int sequence;       // Sequence number of the data
    int is_ready;       // Flag: 1 if data is ready to be consumed, 0 otherwise
};

/*
 * Shared memory name
 * POSIX shared memory names must start with '/' and should be unique
 */
#define SHARED_MEM_NAME "/ipc_exercise_shm"

#endif // EXERCISE4_SHARED_H
