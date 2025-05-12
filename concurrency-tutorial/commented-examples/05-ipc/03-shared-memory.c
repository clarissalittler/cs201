/**
 * ============================================================================
 * Shared Memory - High-Performance IPC for Data Sharing
 * ============================================================================
 * 
 * This example demonstrates using System V shared memory for inter-process
 * communication. Shared memory provides a way for multiple processes to access
 * the same memory region, enabling high-performance data sharing.
 * 
 * Key concepts demonstrated:
 * - Creating and attaching shared memory segments
 * - Synchronizing access with semaphores
 * - Sharing structured data between processes
 * - Proper cleanup of shared memory resources
 * 
 * This file contains both producer and consumer functionality that can be
 * selected at runtime to demonstrate communication between separate processes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <time.h>

/* Key for identifying shared memory and semaphore */
#define SHM_KEY 0x1234
#define SEM_KEY 0x5678

/* Size of shared memory segment */
#define SHM_SIZE 1024

/* Number of items to produce/consume */
#define NUM_ITEMS 10

/* Indices for our semaphore array */
#define SEM_MUTEX 0  /* For mutual exclusion */
#define SEM_EMPTY 1  /* Number of empty slots */
#define SEM_FULL  2  /* Number of filled slots */
#define NUM_SEMS  3  /* Total number of semaphores */

/* Shared data structure */
typedef struct {
    int buffer[SHM_SIZE / sizeof(int)];  /* Buffer for storing items */
    int in;                              /* Index for insertion */
    int out;                             /* Index for removal */
    int buffer_size;                     /* Size of the buffer */
} SharedData;

/* Function prototypes */
void run_producer();
void run_consumer();
void usage(const char* program_name);

/* Semaphore utility functions */
int create_semaphores();
int remove_semaphores(int sem_id);
int semaphore_op(int sem_id, int sem_num, int op);
int semaphore_p(int sem_id, int sem_num);  /* Decrement (wait) */
int semaphore_v(int sem_id, int sem_num);  /* Increment (signal) */

/* Helper functions for setting up Union semun for older systems */
#if defined(__linux__)
union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
};
#endif

int main(int argc, char* argv[]) {
    /* Check command-line arguments */
    if (argc != 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    printf("Shared Memory IPC Demonstration\n\n");
    
    /* Run as producer or consumer based on command-line argument */
    if (strcmp(argv[1], "producer") == 0) {
        run_producer();
    } else if (strcmp(argv[1], "consumer") == 0) {
        run_consumer();
    } else {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

/**
 * Run in producer mode: Create shared memory, initialize the data structure,
 * and produce items for the consumer.
 */
void run_producer() {
    int shm_id;
    SharedData* shared_data;
    int sem_id;
    int i;
    
    printf("Producer: Starting up...\n");
    
    /* 
     * Create the shared memory segment
     * IPC_CREAT | 0666 creates the segment with read/write permissions for all users
     */
    shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    
    printf("Producer: Created shared memory segment (ID: %d)\n", shm_id);
    
    /* 
     * Attach the shared memory segment to our address space
     * The pointer returned by shmat points to the shared memory
     */
    shared_data = (SharedData*)shmat(shm_id, NULL, 0);
    if (shared_data == (SharedData*)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    
    printf("Producer: Attached shared memory at address %p\n", (void*)shared_data);
    
    /* Initialize the shared data structure */
    shared_data->in = 0;
    shared_data->out = 0;
    shared_data->buffer_size = SHM_SIZE / sizeof(int);
    
    printf("Producer: Initialized shared data (buffer size: %d)\n", 
           shared_data->buffer_size);
    
    /* Create and initialize semaphores */
    sem_id = create_semaphores();
    printf("Producer: Created and initialized semaphores (ID: %d)\n", sem_id);
    
    /* Seed the random number generator */
    srand(time(NULL));
    
    /* Produce items and place them in the shared buffer */
    for (i = 0; i < NUM_ITEMS; i++) {
        /* Generate a random item (simple integer in this example) */
        int item = rand() % 1000;
        
        printf("Producer: Waiting for empty slot...\n");
        semaphore_p(sem_id, SEM_EMPTY);  /* Wait for empty slot */
        
        printf("Producer: Waiting for mutex...\n");
        semaphore_p(sem_id, SEM_MUTEX);  /* Enter critical section */
        
        /* Store the item in the buffer at the next insertion position */
        shared_data->buffer[shared_data->in] = item;
        shared_data->in = (shared_data->in + 1) % shared_data->buffer_size;
        
        printf("Producer: Produced item %d: %d\n", i + 1, item);
        
        semaphore_v(sem_id, SEM_MUTEX);  /* Exit critical section */
        semaphore_v(sem_id, SEM_FULL);   /* Signal a slot is filled */
        
        /* Sleep to simulate varying production rates */
        usleep((rand() % 500 + 100) * 1000);  /* 100-600ms */
    }
    
    printf("Producer: Produced all %d items.\n", NUM_ITEMS);
    
    /*
     * Note: We intentionally don't clean up the shared memory or semaphores here.
     * The consumer will handle cleanup after it's done.
     * In a real application, there would be more sophisticated handoff and cleanup.
     */
    
    /* Detach from the shared memory segment */
    if (shmdt(shared_data) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    
    printf("Producer: Detached from shared memory. Exiting.\n");
}

/**
 * Run in consumer mode: Attach to shared memory and consume items
 * produced by the producer.
 */
void run_consumer() {
    int shm_id;
    SharedData* shared_data;
    int sem_id;
    int i;
    
    printf("Consumer: Starting up...\n");
    
    /* 
     * Get the existing shared memory segment
     * We use the same key as the producer
     */
    shm_id = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shm_id == -1) {
        if (errno == ENOENT) {
            fprintf(stderr, "Consumer: Shared memory segment does not exist.\n");
            fprintf(stderr, "Consumer: Make sure to start the producer first.\n");
        } else {
            perror("shmget");
        }
        exit(EXIT_FAILURE);
    }
    
    printf("Consumer: Found shared memory segment (ID: %d)\n", shm_id);
    
    /* Attach to the shared memory segment */
    shared_data = (SharedData*)shmat(shm_id, NULL, 0);
    if (shared_data == (SharedData*)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    
    printf("Consumer: Attached shared memory at address %p\n", (void*)shared_data);
    
    /* Get the existing semaphore set */
    sem_id = semget(SEM_KEY, NUM_SEMS, 0666);
    if (sem_id == -1) {
        if (errno == ENOENT) {
            fprintf(stderr, "Consumer: Semaphore set does not exist.\n");
            fprintf(stderr, "Consumer: Make sure to start the producer first.\n");
        } else {
            perror("semget");
        }
        exit(EXIT_FAILURE);
    }
    
    printf("Consumer: Found semaphore set (ID: %d)\n", sem_id);
    
    /* Consume items from the shared buffer */
    for (i = 0; i < NUM_ITEMS; i++) {
        int item;
        
        printf("Consumer: Waiting for filled slot...\n");
        semaphore_p(sem_id, SEM_FULL);   /* Wait for a filled slot */
        
        printf("Consumer: Waiting for mutex...\n");
        semaphore_p(sem_id, SEM_MUTEX);  /* Enter critical section */
        
        /* Retrieve the item from the buffer */
        item = shared_data->buffer[shared_data->out];
        shared_data->out = (shared_data->out + 1) % shared_data->buffer_size;
        
        printf("Consumer: Consumed item %d: %d\n", i + 1, item);
        
        semaphore_v(sem_id, SEM_MUTEX);  /* Exit critical section */
        semaphore_v(sem_id, SEM_EMPTY);  /* Signal a slot is empty */
        
        /* Sleep to simulate varying consumption rates */
        usleep((rand() % 500 + 200) * 1000);  /* 200-700ms */
    }
    
    printf("Consumer: Consumed all %d items.\n", NUM_ITEMS);
    
    /* Clean up the resources */
    
    /* Detach from the shared memory segment */
    if (shmdt(shared_data) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    
    printf("Consumer: Detached from shared memory.\n");
    
    /* Remove the shared memory segment */
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
    
    printf("Consumer: Removed shared memory segment.\n");
    
    /* Remove the semaphore set */
    if (remove_semaphores(sem_id) == -1) {
        perror("remove_semaphores");
        exit(EXIT_FAILURE);
    }
    
    printf("Consumer: Removed semaphore set. Exiting.\n");
}

/**
 * Create and initialize the semaphore set.
 * 
 * @return Semaphore ID on success, -1 on error
 */
int create_semaphores() {
    int sem_id;
    union semun arg;
    unsigned short init_values[NUM_SEMS] = {1, SHM_SIZE / sizeof(int), 0};
    
    /* Create the semaphore set with NUM_SEMS semaphores */
    sem_id = semget(SEM_KEY, NUM_SEMS, IPC_CREAT | 0666);
    if (sem_id == -1) {
        return -1;
    }
    
    /* Initialize the semaphore values */
    arg.array = init_values;
    if (semctl(sem_id, 0, SETALL, arg) == -1) {
        return -1;
    }
    
    return sem_id;
}

/**
 * Remove the semaphore set.
 * 
 * @param sem_id Semaphore ID to remove
 * @return 0 on success, -1 on error
 */
int remove_semaphores(int sem_id) {
    return semctl(sem_id, 0, IPC_RMID, 0);
}

/**
 * Perform a semaphore operation.
 * 
 * @param sem_id Semaphore ID
 * @param sem_num Semaphore index in the set
 * @param op Operation to perform (positive = increment, negative = decrement)
 * @return 0 on success, -1 on error
 */
int semaphore_op(int sem_id, int sem_num, int op) {
    struct sembuf sb;
    
    sb.sem_num = sem_num;  /* Semaphore index in the set */
    sb.sem_op = op;        /* Operation value */
    sb.sem_flg = 0;        /* No special flags */
    
    return semop(sem_id, &sb, 1);
}

/**
 * Semaphore P operation (decrement/wait).
 * 
 * @param sem_id Semaphore ID
 * @param sem_num Semaphore index in the set
 * @return 0 on success, -1 on error
 */
int semaphore_p(int sem_id, int sem_num) {
    return semaphore_op(sem_id, sem_num, -1);
}

/**
 * Semaphore V operation (increment/signal).
 * 
 * @param sem_id Semaphore ID
 * @param sem_num Semaphore index in the set
 * @return 0 on success, -1 on error
 */
int semaphore_v(int sem_id, int sem_num) {
    return semaphore_op(sem_id, sem_num, 1);
}

/**
 * Display usage information for the program.
 */
void usage(const char* program_name) {
    fprintf(stderr, "Usage: %s <mode>\n", program_name);
    fprintf(stderr, "  mode: 'producer' or 'consumer'\n");
    fprintf(stderr, "\nTo demonstrate shared memory communication:\n");
    fprintf(stderr, "  1. Run as producer in one terminal: %s producer\n", program_name);
    fprintf(stderr, "  2. Run as consumer in another terminal: %s consumer\n", program_name);
}

/**
 * EXPLANATION:
 * 
 * Shared Memory - High-Performance IPC:
 * 
 * 1. What is Shared Memory?
 *    - A region of memory that can be accessed by multiple processes
 *    - Provides the highest performance IPC mechanism
 *    - Direct memory access without kernel involvement for data transfer
 *    - Requires explicit synchronization (no built-in coordination)
 * 
 * 2. System V Shared Memory API:
 *    - shmget(): Create or get a shared memory segment
 *    - shmat(): Attach a shared memory segment to process address space
 *    - shmdt(): Detach a shared memory segment
 *    - shmctl(): Control operations on a shared memory segment
 * 
 * 3. Key Concepts:
 *    - Key: A unique identifier for the shared memory (like a filename)
 *    - ID: A system-assigned identifier for the segment
 *    - Permissions: Control who can access the segment
 *    - Attachment: Mapping the shared memory into a process address space
 * 
 * 4. Synchronization Requirements:
 *    - Shared memory provides no built-in synchronization
 *    - Common synchronization mechanisms:
 *      * Semaphores (used in this example)
 *      * Mutexes (in shared memory for process-shared mutexes)
 *      * File locks
 *      * Message passing as a control channel
 * 
 * 5. Semaphores for Synchronization:
 *    - Semaphores are counters that control access to shared resources
 *    - Types used in this example:
 *      * SEM_MUTEX: Binary semaphore for mutual exclusion (value 0 or 1)
 *      * SEM_EMPTY: Counting semaphore for empty buffer slots
 *      * SEM_FULL: Counting semaphore for filled buffer slots
 *    - P operation (wait/decrement): If value > 0, decrement; otherwise, block
 *    - V operation (signal/increment): Increment value and wake up blocked processes
 * 
 * 6. Producer-Consumer Pattern:
 *    - Producer creates items and adds them to the buffer
 *    - Consumer removes items from the buffer and processes them
 *    - Bounded buffer implemented as a circular array
 *    - Synchronization ensures:
 *      * Buffer doesn't overflow (producer waits when buffer is full)
 *      * Consumer doesn't read from an empty buffer
 *      * No race conditions during buffer access
 * 
 * 7. Advantages of Shared Memory:
 *    - Highest performance IPC (minimal kernel involvement)
 *    - Flexible data structures (not just byte streams)
 *    - Easy to use for producer-consumer patterns
 *    - Good for large data transfers
 * 
 * 8. Limitations and Challenges:
 *    - No built-in synchronization
 *    - Complex resource management
 *    - Potential for race conditions and deadlocks
 *    - Limited to a single machine (not network-capable)
 * 
 * 9. System V vs. POSIX Shared Memory:
 *    - System V: Older interface, used in this example
 *    - POSIX: Newer interface with shm_open(), mmap(), etc.
 *    - POSIX is more consistent with other APIs but less widely supported
 * 
 * 10. Running This Example:
 *    - Start the producer in one terminal
 *    - Start the consumer in another terminal
 *    - Observe the synchronized producer-consumer pattern
 *    - Consumer cleans up resources after both processes complete
 */