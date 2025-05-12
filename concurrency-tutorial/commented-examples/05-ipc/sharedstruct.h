/* sharedstruct.h - Header file for shared memory examples */
#ifndef SHAREDSTRUCT_H
#define SHAREDSTRUCT_H

#include <semaphore.h>

/* Shared memory name */
#define sharedName "/shared_example"

/* Define the shared data structure */
struct sharedData {
    int buffer[10];     /* Circular buffer */
    int in;             /* Index for inserting */
    int out;            /* Index for removing */
    int count;          /* Number of items in the buffer */
    sem_t mutex;        /* Semaphore for synchronization */
    int payload1;       /* First payload value */
    int payload2;       /* Second payload value */
};

/* Also define the SharedData type for backward compatibility */
typedef struct sharedData SharedData;

#endif /* SHAREDSTRUCT_H */