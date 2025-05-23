/*
 * SOLUTION: Thread Exercise 3 - Thread Return Values
 * 
 * This solution demonstrates how to return values from threads and
 * proper memory management for thread communication.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Thread function to calculate factorial of 5
void* calculate_factorial(void* arg) {
    (void)arg; // Suppress unused parameter warning
    printf("Thread 1: Calculating factorial of 5\n");
    
    // Allocate memory for result
    int* result = malloc(sizeof(int));
    if (result == NULL) {
        printf("Error: Memory allocation failed in factorial thread\n");
        return NULL;
    }
    
    // Calculate 5! = 5 × 4 × 3 × 2 × 1
    *result = 1;
    for (int i = 1; i <= 5; i++) {
        *result *= i;
    }
    
    printf("Thread 1: Factorial calculation complete\n");
    
    // Return pointer to result
    return (void*)result;
}

// Thread function to calculate sum of numbers 1-10  
void* calculate_sum(void* arg) {
    (void)arg; // Suppress unused parameter warning
    printf("Thread 2: Calculating sum of 1 to 10\n");
    
    // Allocate memory for result
    int* result = malloc(sizeof(int));
    if (result == NULL) {
        printf("Error: Memory allocation failed in sum thread\n");
        return NULL;
    }
    
    // Calculate sum 1+2+3+...+10
    *result = 0;
    for (int i = 1; i <= 10; i++) {
        *result += i;
    }
    
    printf("Thread 2: Sum calculation complete\n");
    
    // Return pointer to result
    return (void*)result;
}

// Thread function to calculate 2^8
void* calculate_power(void* arg) {
    (void)arg; // Suppress unused parameter warning
    printf("Thread 3: Calculating 2^8\n");
    
    // Allocate memory for result
    int* result = malloc(sizeof(int));
    if (result == NULL) {
        printf("Error: Memory allocation failed in power thread\n");
        return NULL;
    }
    
    // Calculate 2^8 = 2×2×2×2×2×2×2×2
    *result = 1;
    for (int i = 0; i < 8; i++) {
        *result *= 2;
    }
    
    printf("Thread 3: Power calculation complete\n");
    
    // Return pointer to result
    return (void*)result;
}

int main() {
    pthread_t threads[3];
    void* results[3];
    int result;
    
    // Function pointers for different calculations
    void* (*thread_functions[])(void*) = {
        calculate_factorial,
        calculate_sum,
        calculate_power
    };
    
    const char* descriptions[] = {
        "factorial of 5",
        "sum of 1 to 10", 
        "2^8"
    };
    
    printf("Main thread: Starting calculation threads\n");
    
    // Create threads
    for (int i = 0; i < 3; i++) {
        result = pthread_create(&threads[i], NULL, thread_functions[i], NULL);
        
        if (result != 0) {
            printf("Error: Failed to create thread %d (error: %d)\n", 
                   i + 1, result);
            
            // Clean up any threads already created
            for (int j = 0; j < i; j++) {
                pthread_join(threads[j], &results[j]);
                if (results[j] != NULL) {
                    free(results[j]);
                }
            }
            return EXIT_FAILURE;
        }
    }
    
    // Wait for all threads and collect results
    for (int i = 0; i < 3; i++) {
        result = pthread_join(threads[i], &results[i]);
        
        if (result != 0) {
            printf("Error: Failed to join thread %d (error: %d)\n", 
                   i + 1, result);
            results[i] = NULL;
        }
    }
    
    // Print all results
    printf("\nMain thread: Results:\n");
    
    for (int i = 0; i < 3; i++) {
        if (results[i] != NULL) {
            int value = *(int*)results[i];
            printf("Main thread: %s = %d\n", descriptions[i], value);
        } else {
            printf("Main thread: %s = ERROR (calculation failed)\n", 
                   descriptions[i]);
        }
    }
    
    // Free allocated memory
    for (int i = 0; i < 3; i++) {
        if (results[i] != NULL) {
            free(results[i]);
        }
    }
    
    printf("Main thread: All calculations completed\n");
    return 0;
}

/*
 * EXPLANATION:
 * 
 * 1. RETURNING VALUES FROM THREADS:
 *    - Threads return void* (generic pointer)
 *    - Allocate memory with malloc() for return values
 *    - Return pointer to allocated memory
 *    - Retrieve with pthread_join() second parameter
 * 
 * 2. MEMORY MANAGEMENT:
 *    - Each thread allocates memory for its result
 *    - Main thread retrieves pointers via pthread_join()
 *    - Main thread must free() all allocated memory
 *    - Always check malloc() return value
 * 
 * 3. ERROR HANDLING:
 *    - Check malloc() for NULL return
 *    - Check pthread_create() return value
 *    - Check pthread_join() return value
 *    - Handle partial failures gracefully
 * 
 * 4. PTHREAD_JOIN() USAGE:
 *    - Second parameter receives thread's return value
 *    - Pass address of void* variable: &results[i]
 *    - Cast returned pointer back to appropriate type
 * 
 * 5. CALCULATION DETAILS:
 *    - Factorial: 5! = 5×4×3×2×1 = 120
 *    - Sum: 1+2+3+4+5+6+7+8+9+10 = 55
 *    - Power: 2^8 = 256
 * 
 * ALTERNATIVE APPROACHES:
 * 
 * 1. GLOBAL ARRAY:
 * int results[3];
 * // Thread stores result directly in global array
 * results[thread_id] = calculated_value;
 * 
 * 2. STRUCTURE PASSING:
 * typedef struct {
 *     int thread_id;
 *     int result;
 * } thread_data_t;
 * 
 * 3. RETURN VALUE CASTING:
 * // For small integers, can cast directly
 * return (void*)(long)result;
 * // Retrieve with:
 * int value = (long)returned_pointer;
 * 
 * MEMORY CONSIDERATIONS:
 * - malloc() allocates heap memory (survives thread exit)
 * - Local variables die when thread function exits
 * - Global variables are shared (need synchronization)
 * - Always pair malloc() with free()
 * 
 * WHY USE MALLOC()?
 * - Thread function local variables are destroyed when function exits
 * - pthread_join() happens after thread function exits
 * - Need persistent memory to hold results
 * - Heap memory (malloc) persists until explicitly freed
 * 
 * BEST PRACTICES:
 * - Always check malloc() return value
 * - Always free() allocated memory
 * - Initialize pointers to NULL
 * - Handle partial failures gracefully
 * - Document memory ownership clearly
 */