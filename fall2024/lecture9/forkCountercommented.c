#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Standard library functions (e.g., srand, rand, exit)
#include <unistd.h>     // POSIX API (e.g., fork, sleep)
#include <pthread.h>    // POSIX threads (not directly used here, but included)
#include <semaphore.h>  // Semaphore functions
#include <sys/mman.h>   // Memory management (e.g., mmap, munmap)
#include <sys/wait.h>   // Waiting for process termination (wait)
#include <time.h>       // Time functions (e.g., time, srand)

/*
 * Structure to hold a shared counter and a semaphore for synchronization.
 * - 'counter' is the shared integer that both parent and child processes will increment.
 * - 'semaphore' ensures that only one process modifies 'counter' at a time.
 */
struct guardCounter {
    int counter;
    sem_t semaphore;
};

int main() {
    // Initialize the random number generator with the current time to ensure different sequences each run
    srand(time(0));

    /*
     * Create a shared memory region to hold 'guardCounter' structure.
     * - NULL: Let the system choose the address.
     * - sizeof(struct guardCounter): Size of the memory region.
     * - PROT_READ | PROT_WRITE: Memory can be read and written.
     * - MAP_SHARED | MAP_ANONYMOUS: Shared between processes and not backed by any file.
     * - -1 and 0: File descriptor and offset are irrelevant for anonymous mapping.
     */
    struct guardCounter* ourCounter = mmap(NULL, sizeof(struct guardCounter),
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // Initialize the semaphore in shared memory.
    // - &ourCounter->semaphore: Address of the semaphore.
    // - 1: Semaphore is shared between processes.
    // - 1: Initial value of the semaphore (unlocked).
    if (sem_init(&ourCounter->semaphore, 1, 1) == -1) {
        perror("sem_init failed");
        exit(EXIT_FAILURE);
    }

    // Initialize the shared counter to zero
    ourCounter->counter = 0;

    // Create a new child process using fork
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed; print error and exit
        perror("Fork failed");
        sem_destroy(&ourCounter->semaphore);
        munmap(ourCounter, sizeof(struct guardCounter));
        exit(EXIT_FAILURE);
    }

    /*
     * Both parent and child processes execute the following for-loop independently.
     * Each iteration attempts to safely increment the shared counter.
     */
    for(int i = 0; i < 5; i++) {
        // Wait (lock) the semaphore before accessing the shared counter
        if (sem_wait(&ourCounter->semaphore) == -1) {
            perror("sem_wait failed");
            exit(EXIT_FAILURE);
        }

        // Critical Section Begins
        // Read the current value of the counter
        int temp = ourCounter->counter;
        
        // Simulate some work by sleeping for 0 to 2 seconds
        sleep(rand() % 3);
        
        // Increment the counter and store it back
        ourCounter->counter = temp + 1;
        // Critical Section Ends

        // Post (unlock) the semaphore to allow other processes to access the counter
        if (sem_post(&ourCounter->semaphore) == -1) {
            perror("sem_post failed");
            exit(EXIT_FAILURE);
        }
    }

    // Only the parent process will execute the following block
    if(pid != 0){
        // Wait for the child process to finish execution
        if (wait(NULL) == -1) {
            perror("wait failed");
            exit(EXIT_FAILURE);
        }

        // Print the final value of the shared counter
        printf("Survey says! %d\n", ourCounter->counter);

        // Clean up: Destroy the semaphore
        if (sem_destroy(&ourCounter->semaphore) == -1) {
            perror("sem_destroy failed");
            // Continue to munmap even if sem_destroy fails
        }

        // Unmap the shared memory region
        if (munmap(ourCounter, sizeof(struct guardCounter)) == -1) {
            perror("munmap failed");
            exit(EXIT_FAILURE);
        }
    }

    // Parent and child processes exit here
    return 0;
}
Survey says! 10
