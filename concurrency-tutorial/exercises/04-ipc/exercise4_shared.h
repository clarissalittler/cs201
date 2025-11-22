/*
 * Exercise 4: Shared Memory - Header File
 *
 * This header defines the shared data structure used by both
 * producer and consumer processes.
 */

#ifndef EXERCISE4_SHARED_H
#define EXERCISE4_SHARED_H

#include <semaphore.h>

// TODO: Define the shared data structure
// Structure should contain:
// - A semaphore for mutual exclusion (sem_t mutex)
// - Data fields to share (e.g., int value, int is_ready)

// Example structure:
// struct shared_data {
//     sem_t mutex;        // Semaphore for synchronization
//     int value;          // Data value
//     int sequence;       // Sequence number
//     int is_ready;       // Flag: 1 if data is ready, 0 otherwise
// };

// TODO: Define the shared memory name
// #define SHARED_MEM_NAME "/ipc_exercise_shm"

#endif // EXERCISE4_SHARED_H
