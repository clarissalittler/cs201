/**
 * ============================================================================
 * Thread Return Values - Retrieving Results from Threads
 * ============================================================================
 * 
 * This example demonstrates different methods for retrieving results from
 * threads, including:
 * - Using thread return values
 * - Passing result pointers to threads
 * - Using global variables (with caution)
 * - Combining multiple results from multiple threads
 * 
 * Correctly retrieving results from threads is essential for parallel
 * computations and dividing work among multiple threads.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

/* Thread function prototypes */
void* return_value_thread(void* arg);
void* output_param_thread(void* arg);
void* aggregate_thread(void* arg);

/* 
 * Structure for thread calculation data
 * Used by the aggregate_thread function
 */
typedef struct {
    int start;
    int end;
    long long result;  /* This will hold the result */
} RangeSum;

/* Global array of results for the array sum example */
#define NUM_THREADS 4
#define ARRAY_SIZE 1000
int global_array[ARRAY_SIZE];
long long partial_sums[NUM_THREADS];

int main() {
    pthread_t thread1, thread2, thread3;
    pthread_t sum_threads[NUM_THREADS];
    int rc;
    
    printf("Main thread: Demonstrating thread return value techniques\n\n");
    
    /* ================== METHOD 1: THREAD RETURN VALUE ================== */
    
    /*
     * Method 1: Using thread return value
     * This is the most straightforward method for a single value.
     */
    printf("Method 1: Using thread return value\n");
    
    /* Create a thread that returns a dynamically allocated string */
    rc = pthread_create(&thread1, NULL, return_value_thread, NULL);
    if (rc != 0) {
        perror("pthread_create (thread1)");
        return EXIT_FAILURE;
    }
    
    /* 
     * Wait for the thread to complete and retrieve its return value
     * The second parameter to pthread_join will receive the thread's return value
     */
    void* thread_return;
    pthread_join(thread1, &thread_return);
    
    /* Print the returned string and clean up */
    printf("Thread1 returned: %s\n", (char*)thread_return);
    free(thread_return);  /* Don't forget to free the allocated memory */
    printf("\n");
    
    /* ================== METHOD 2: OUTPUT PARAMETER ================== */
    
    /*
     * Method 2: Using an output parameter
     * This allows returning multiple values or complex data structures.
     */
    printf("Method 2: Using an output parameter\n");
    
    /* 
     * Allocate a structure to hold the calculation result
     * The thread will write directly to this structure
     */
    double* result = malloc(sizeof(double));
    if (result == NULL) {
        perror("malloc");
        return EXIT_FAILURE;
    }
    
    /* Pass the result pointer to the thread */
    rc = pthread_create(&thread2, NULL, output_param_thread, result);
    if (rc != 0) {
        perror("pthread_create (thread2)");
        free(result);
        return EXIT_FAILURE;
    }
    
    /* Wait for the thread to complete */
    pthread_join(thread2, NULL);
    
    /* The result has been written directly to our result variable */
    printf("Thread2 calculated: %f\n", *result);
    free(result);
    printf("\n");
    
    /* ================== METHOD 3: AGGREGATING MULTIPLE RESULTS ================== */
    
    /*
     * Method 3: Aggregating results from multiple threads
     * This demonstrates dividing work among multiple threads and combining results.
     */
    printf("Method 3: Aggregating results from multiple threads\n");
    
    /* Initialize our test array with values */
    for (int i = 0; i < ARRAY_SIZE; i++) {
        global_array[i] = i + 1;  /* Fill with 1, 2, 3, ..., ARRAY_SIZE */
    }
    
    /* Calculate the expected sum for verification */
    long long expected_sum = ((long long)ARRAY_SIZE * (ARRAY_SIZE + 1)) / 2;
    printf("Initialized array with %d elements (sum should be %lld)\n", 
           ARRAY_SIZE, expected_sum);
    
    /* Create an array of range structures for each thread */
    RangeSum ranges[NUM_THREADS];
    
    /* Divide the array into equal segments for each thread */
    int segment_size = ARRAY_SIZE / NUM_THREADS;
    
    /* Create the worker threads, each processing a segment of the array */
    for (int i = 0; i < NUM_THREADS; i++) {
        /* Set up the range for this thread */
        ranges[i].start = i * segment_size;
        ranges[i].end = (i == NUM_THREADS - 1) ? ARRAY_SIZE : (i + 1) * segment_size;
        ranges[i].result = 0;
        
        /* Create the thread, passing its range structure */
        rc = pthread_create(&sum_threads[i], NULL, aggregate_thread, &ranges[i]);
        if (rc != 0) {
            perror("pthread_create (sum thread)");
            return EXIT_FAILURE;
        }
    }
    
    /* Wait for all threads to complete and combine their results */
    long long total_sum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(sum_threads[i], NULL);
        total_sum += ranges[i].result;
        
        printf("Thread %d computed sum for range [%d-%d] = %lld\n", 
               i, ranges[i].start, ranges[i].end - 1, ranges[i].result);
    }
    
    /* Verify the result */
    printf("Total sum: %lld (Expected: %lld)\n", total_sum, expected_sum);
    printf("Result is %s\n", (total_sum == expected_sum) ? "CORRECT" : "WRONG");
    printf("\n");
    
    /* ================== METHOD 4: USING GLOBAL VARIABLES ================== */
    
    /*
     * Method 4: Using global variables
     * This demonstrates using global storage for thread results.
     * CAUTION: This approach requires careful synchronization in real applications!
     */
    printf("Method 4: Using global variables for results\n");
    
    /* Create a thread that stores its result in a global variable */
    pthread_t global_thread;
    rc = pthread_create(&global_thread, NULL, return_value_thread, "Use global variable");
    if (rc != 0) {
        perror("pthread_create (global_thread)");
        return EXIT_FAILURE;
    }
    
    /* Wait for the thread to complete */
    pthread_join(global_thread, NULL);
    
    /*
     * We'd normally access the global result here
     * (Not implemented in this example for simplicity)
     */
    printf("Global variable approach demonstration complete\n");
    printf("Note: In real code, global variables need proper synchronization!\n");
    
    printf("\nMain thread: All demonstrations complete.\n");
    
    return EXIT_SUCCESS;
}

/**
 * Thread function that returns a dynamically allocated string.
 * 
 * @param arg Optional custom message to include
 * @return Dynamically allocated string with the result
 */
void* return_value_thread(void* arg) {
    /* Allocate memory for our return string */
    char* result = malloc(100);
    if (result == NULL) {
        perror("malloc in thread");
        pthread_exit(NULL);  /* Return NULL on error */
    }
    
    /* Simulate some calculation or processing */
    sleep(1);
    
    /* Fill in the result */
    if (arg != NULL) {
        /* Use the provided message */
        snprintf(result, 100, "Thread processed custom message: %s", (char*)arg);
    } else {
        /* Use a default message */
        strcpy(result, "Thread calculation complete!");
    }
    
    /* 
     * Return the dynamically allocated result
     * The calling thread must free this memory!
     */
    return result;
}

/**
 * Thread function that writes its result to a provided output parameter.
 * 
 * @param arg Pointer to a double where the result will be stored
 * @return Always NULL (result is stored via the parameter)
 */
void* output_param_thread(void* arg) {
    /* Cast the argument to the correct type */
    double* result_ptr = (double*)arg;
    
    /* Simulate a calculation (compute pi using the Nilakantha series) */
    double pi = 3.0;
    double sign = 1.0;
    double denominator = 2.0;
    
    /* Perform 10,000 iterations for a reasonable approximation */
    for (int i = 0; i < 10000; i++) {
        pi += sign * (4.0 / (denominator * (denominator + 1.0) * (denominator + 2.0)));
        denominator += 2.0;
        sign = -sign;
    }
    
    /* Store the result in the provided output parameter */
    *result_ptr = pi;
    
    /* Print pi for verification */
    printf("Thread2: Calculated pi = %f\n", pi);
    
    /* No return value needed since we're using the output parameter */
    return NULL;
}

/**
 * Thread function that computes the sum of a segment of the global array.
 * 
 * @param arg Pointer to a RangeSum structure with the segment range
 * @return Always NULL (result is stored in the structure)
 */
void* aggregate_thread(void* arg) {
    /* Cast the argument to the correct type */
    RangeSum* range = (RangeSum*)arg;
    long long sum = 0;
    
    /* Compute the sum for our assigned segment of the array */
    for (int i = range->start; i < range->end; i++) {
        sum += global_array[i];
    }
    
    /* Store the result in the range structure */
    range->result = sum;
    
    return NULL;
}

/**
 * EXPLANATION:
 * 
 * Thread Return Value Techniques:
 * 
 * 1. Using pthread_join Return Value:
 *    - Thread returns a pointer via return or pthread_exit()
 *    - Main thread retrieves the pointer via pthread_join's second parameter
 *    - Good for single, simple values or dynamically allocated complex structures
 *    - Memory management responsibility must be clear (usually joined thread frees)
 * 
 * 2. Output Parameters:
 *    - Main thread allocates memory for the result
 *    - Thread receives a pointer to this memory and writes its result there
 *    - Good for predetermined result types or when thread shouldn't allocate memory
 *    - No need to worry about freeing thread-allocated memory
 *    - Must ensure the memory remains valid until thread completes
 * 
 * 3. Aggregating Multiple Results:
 *    - Divide work among multiple threads, each computing a partial result
 *    - Each thread stores its result in its own data structure
 *    - Main thread combines/aggregates the partial results
 *    - Good for parallel computations on large datasets
 *    - Enables effective utilization of multi-core processors
 * 
 * 4. Global Variables (with caution):
 *    - Thread stores result in a global variable or array
 *    - Main thread accesses the global after thread completes
 *    - Simple but requires careful synchronization for thread safety
 *    - Generally not recommended for complex applications
 *    - Can lead to hard-to-debug race conditions
 * 
 * 5. Advanced Techniques (not shown):
 *    - Thread-local storage for per-thread data
 *    - Condition variables for signaling completion
 *    - Message queues for passing results between threads
 *    - Thread pools with task-result pairs
 * 
 * 6. Best Practices:
 *    - Document memory ownership clearly
 *    - Avoid returning pointers to stack variables
 *    - Use appropriate synchronization for shared data
 *    - Consider the overhead of memory allocation vs. reuse
 *    - Always join threads to prevent resource leaks
 *    - Test thoroughly for race conditions and memory leaks
 */