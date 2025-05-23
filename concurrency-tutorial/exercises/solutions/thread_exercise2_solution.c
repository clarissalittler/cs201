/*
 * SOLUTION: Thread Exercise 2 - Thread Arguments
 * 
 * This solution demonstrates how to pass arguments to threads and
 * manage multiple threads simultaneously.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Thread function that accepts an integer argument
void* worker_thread(void* arg) {
    // Cast the void* argument back to integer
    int worker_number = (long)arg;
    
    // Print thread information
    printf("Worker %d: Thread ID = %lu, Argument = %d\n", 
           worker_number, pthread_self(), worker_number);
    
    // Simulate some work with variable duration
    usleep(100000 + (worker_number * 50000)); // 100-250ms
    
    printf("Worker %d: Work completed\n", worker_number);
    
    return NULL;
}

int main() {
    const int NUM_THREADS = 3;
    pthread_t threads[NUM_THREADS];
    int args[] = {10, 20, 30};
    int result;
    
    printf("Main thread: Creating %d worker threads\n", NUM_THREADS);
    
    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        // Pass argument as void* by casting through long
        result = pthread_create(&threads[i], NULL, worker_thread, 
                               (void*)(long)args[i]);
        
        if (result != 0) {
            printf("Error: Failed to create thread %d (error: %d)\n", 
                   i + 1, result);
            
            // Clean up any threads we've already created
            for (int j = 0; j < i; j++) {
                pthread_join(threads[j], NULL);
            }
            return EXIT_FAILURE;
        }
        
        printf("Main thread: Created worker %d\n", (args[i] / 10));
    }
    
    // Wait for all threads to complete
    printf("Main thread: Waiting for all workers to complete\n");
    
    for (int i = 0; i < NUM_THREADS; i++) {
        result = pthread_join(threads[i], NULL);
        
        if (result != 0) {
            printf("Error: Failed to join thread %d (error: %d)\n", 
                   i + 1, result);
        } else {
            printf("Main thread: Worker %d finished\n", (args[i] / 10));
        }
    }
    
    printf("Main thread: All workers completed\n");
    return 0;
}

/*
 * EXPLANATION:
 * 
 * 1. PASSING ARGUMENTS:
 *    - pthread_create() 4th parameter is void* argument
 *    - For simple types, cast to void*: (void*)(long)value
 *    - In thread function, cast back: int value = (long)arg
 * 
 * 2. WHY THE CASTING?
 *    - void* is a pointer, but we want to pass a value
 *    - Can't directly cast int to void* (size mismatch)
 *    - Cast through long for proper size conversion
 *    - Alternative: pass pointer to integer
 * 
 * 3. MULTIPLE THREAD MANAGEMENT:
 *    - Use arrays to store thread IDs
 *    - Create threads in loop
 *    - Join threads in separate loop
 *    - Handle errors properly for each thread
 * 
 * 4. ERROR HANDLING STRATEGY:
 *    - Check each pthread_create() result
 *    - If creation fails, clean up already-created threads
 *    - Check each pthread_join() result
 *    - Report specific error information
 * 
 * 5. EXECUTION ORDER:
 *    - Thread creation order is deterministic
 *    - Thread execution order is non-deterministic
 *    - Different timing per thread makes this visible
 * 
 * ALTERNATIVE ARGUMENT PASSING:
 * 
 * For more complex data, use structures:
 * 
 * typedef struct {
 *     int worker_id;
 *     int work_amount;
 *     char* work_description;
 * } thread_data_t;
 * 
 * // Pass pointer to structure:
 * pthread_create(&thread, NULL, worker, &thread_data);
 * 
 * // In thread function:
 * thread_data_t* data = (thread_data_t*)arg;
 * int id = data->worker_id;
 * 
 * MEMORY CONSIDERATIONS:
 * - If passing pointers, ensure memory remains valid
 * - Local variables may be destroyed before thread uses them
 * - Use malloc() or global/static variables for complex data
 * 
 * THREAD LIFECYCLE:
 * 1. Created by pthread_create()
 * 2. Executes thread function
 * 3. Function returns (thread terminates)
 * 4. Joined by pthread_join()
 * 5. Resources cleaned up
 */