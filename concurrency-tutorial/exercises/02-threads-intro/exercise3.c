/*
 * Exercise 3: Thread Return Values
 * Difficulty: 🟡 Intermediate
 * 
 * OBJECTIVE:
 * Create threads that perform simple calculations and return results to the 
 * main thread. The main thread should collect and print all results.
 * 
 * REQUIREMENTS:
 * 1. Create 3 threads that perform different calculations:
 *    - Thread 1: Calculate factorial of 5
 *    - Thread 2: Calculate sum of numbers 1-10
 *    - Thread 3: Calculate 2^8 (2 to the power of 8)
 * 2. Each thread should return its calculated result
 * 3. Main thread should collect all results and print them
 * 4. Use proper memory management for return values
 * 
 * EXPECTED OUTPUT:
 * Main thread: Starting calculation threads
 * Thread 1: Calculating factorial of 5
 * Thread 2: Calculating sum of 1 to 10
 * Thread 3: Calculating 2^8
 * Main thread: Factorial of 5 = 120
 * Main thread: Sum of 1 to 10 = 55
 * Main thread: 2^8 = 256
 * Main thread: All calculations completed
 * 
 * HINTS:
 * - Threads return void*, so allocate memory for results
 * - Use malloc() to allocate memory for return values
 * - Free the allocated memory after retrieving results
 * - pthread_join() second parameter receives the return value
 * 
 * CALCULATION DETAILS:
 * - Factorial of 5: 5! = 5 × 4 × 3 × 2 × 1 = 120
 * - Sum 1-10: 1+2+3+4+5+6+7+8+9+10 = 55  
 * - 2^8: 2×2×2×2×2×2×2×2 = 256
 * 
 * LEARNING GOALS:
 * - Returning values from threads
 * - Memory allocation for thread communication
 * - Proper resource cleanup
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// TODO: Define thread function for factorial calculation
// void* calculate_factorial(void* arg)
// Calculate 5! and return result via malloc'd integer

// TODO: Define thread function for sum calculation  
// void* calculate_sum(void* arg)
// Calculate sum 1-10 and return result via malloc'd integer

// TODO: Define thread function for power calculation
// void* calculate_power(void* arg)  
// Calculate 2^8 and return result via malloc'd integer

int main() {
    // TODO: Declare thread ID array
    // pthread_t threads[3];
    
    // TODO: Declare void pointers for return values
    // void* results[3];
    
    printf("Main thread: Starting calculation threads\n");
    
    // TODO: Create thread 1 for factorial calculation
    // TODO: Create thread 2 for sum calculation
    // TODO: Create thread 3 for power calculation
    
    // TODO: Wait for all threads and collect results
    // for (int i = 0; i < 3; i++) {
        // TODO: Join thread and get result
        // pthread_join(threads[i], &results[i]);
    // }
    
    // TODO: Print all results
    // Cast results[i] back to int* and dereference
    
    // TODO: Free allocated memory
    // for (int i = 0; i < 3; i++) {
        // TODO: free(results[i]);
    // }
    
    printf("Main thread: All calculations completed\n");
    return 0;
}