/**
 * ============================================================================
 * Thread Arguments - Passing Data to Threads
 * ============================================================================
 * 
 * This example demonstrates various techniques for passing data to threads,
 * including:
 * - Passing simple values
 * - Passing multiple values using structures
 * - Handling thread-safety issues with arguments
 * - Common pitfalls and their solutions
 * 
 * Proper data passing is crucial for thread programming to avoid race conditions
 * and memory corruption.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* Thread function prototypes */
void* simple_argument_thread(void* arg);
void* array_argument_thread(void* arg);
void* struct_argument_thread(void* arg);
void* unsafe_argument_thread(void* arg);
void* safe_argument_thread(void* arg);

/* 
 * Structure for passing multiple values to a thread
 * This shows how to bundle multiple parameters together
 */
typedef struct {
    char* name;
    int id;
    float value;
} ThreadData;

int main() {
    pthread_t thread1, thread2, thread3, thread4, thread5;
    int rc;
    
    printf("Main thread: Demonstrating different ways to pass data to threads\n\n");
    
    /* ================== EXAMPLE 1: SIMPLE VALUE ================== */
    
    /*
     * Method 1: Passing a simple integer value
     * Since pthread_create accepts void*, we cast the integer to a pointer.
     * 
     * CAUTION: This approach has limitations and only works for small integers
     * that fit into a pointer. It's not suitable for 64-bit values on 32-bit systems.
     */
    printf("Example 1: Passing a simple integer value\n");
    
    int thread_num = 42;
    /* The integer is cast to a pointer (not dereferenced) */
    rc = pthread_create(&thread1, NULL, simple_argument_thread, (void*)(intptr_t)thread_num);
    if (rc != 0) {
        perror("pthread_create (thread1)");
        return EXIT_FAILURE;
    }
    
    /* Wait for thread1 to finish */
    pthread_join(thread1, NULL);
    printf("\n");
    
    /* ================== EXAMPLE 2: ARRAY ================== */
    
    /*
     * Method 2: Passing an array
     * We pass a pointer to the first element of the array.
     * 
     * CAUTION: The array must exist for the lifetime of the thread.
     * Also, no bounds checking is performed, so the thread must know the array size.
     */
    printf("Example 2: Passing an array\n");
    
    int numbers[] = {10, 20, 30, 40, 50};
    const int array_size = sizeof(numbers) / sizeof(numbers[0]);
    
    /* 
     * Create a small structure to pass both the array and its size
     * This is allocated on the heap so it exists until freed by the thread
     */
    int** array_package = malloc(sizeof(int*) + sizeof(int));
    array_package[0] = numbers;  /* The array */
    *((int*)&array_package[1]) = array_size;  /* The size */
    
    rc = pthread_create(&thread2, NULL, array_argument_thread, array_package);
    if (rc != 0) {
        perror("pthread_create (thread2)");
        free(array_package);
        return EXIT_FAILURE;
    }
    
    /* Wait for thread2 to finish */
    pthread_join(thread2, NULL);
    printf("\n");
    
    /* ================== EXAMPLE 3: STRUCT ================== */
    
    /*
     * Method 3: Using a structure to pass multiple values
     * This is the most flexible and clear approach for complex data.
     */
    printf("Example 3: Using a structure for multiple values\n");
    
    /* Allocate the structure on the heap */
    ThreadData* data = malloc(sizeof(ThreadData));
    if (data == NULL) {
        perror("malloc");
        return EXIT_FAILURE;
    }
    
    /* Initialize the structure */
    data->name = "Thread Data Example";
    data->id = 123;
    data->value = 3.14159f;
    
    rc = pthread_create(&thread3, NULL, struct_argument_thread, data);
    if (rc != 0) {
        perror("pthread_create (thread3)");
        free(data);
        return EXIT_FAILURE;
    }
    
    /* Wait for thread3 to finish */
    pthread_join(thread3, NULL);
    printf("\n");
    
    /* ================== EXAMPLE 4: COMMON PITFALL ================== */
    
    /*
     * Method 4: UNSAFE - Using a stack variable directly
     * This demonstrates a common threading error!
     */
    printf("Example 4: UNSAFE - Stack variable (potential race condition)\n");
    
    int value = 100;
    
    rc = pthread_create(&thread4, NULL, unsafe_argument_thread, &value);
    if (rc != 0) {
        perror("pthread_create (thread4)");
        return EXIT_FAILURE;
    }
    
    /* 
     * DANGER: Modifying the variable while the thread is using it
     * This creates a race condition!
     */
    printf("Main: About to change value from %d to 999\n", value);
    sleep(1);  /* Simulate some work before changing the value */
    value = 999;
    printf("Main: Changed value to %d\n", value);
    
    pthread_join(thread4, NULL);
    printf("\n");
    
    /* ================== EXAMPLE 5: SAFER APPROACH ================== */
    
    /*
     * Method 5: SAFE - Allocating memory specifically for the thread
     * This is the correct way to handle mutable data
     */
    printf("Example 5: SAFE - Heap-allocated variable\n");
    
    /* Allocate memory for the thread's value */
    int* safe_value = malloc(sizeof(int));
    if (safe_value == NULL) {
        perror("malloc");
        return EXIT_FAILURE;
    }
    
    *safe_value = 100;
    
    rc = pthread_create(&thread5, NULL, safe_argument_thread, safe_value);
    if (rc != 0) {
        perror("pthread_create (thread5)");
        free(safe_value);
        return EXIT_FAILURE;
    }
    
    /* 
     * Changing our local copy doesn't affect the thread's copy,
     * since it has its own allocated memory.
     */
    printf("Main: Thread5 has its own copy of the value\n");
    
    pthread_join(thread5, NULL);
    
    printf("\nMain thread: All demonstrations complete.\n");
    
    return EXIT_SUCCESS;
}

/**
 * Thread function that takes a simple integer value as its argument.
 */
void* simple_argument_thread(void* arg) {
    /* Convert the void pointer back to an integer */
    int value = (int)(intptr_t)arg;
    
    printf("Thread1: Received value %d\n", value);
    
    return NULL;
}

/**
 * Thread function that takes an array as its argument.
 */
void* array_argument_thread(void* arg) {
    /* Unpack the array and its size */
    int** package = (int**)arg;
    int* array = package[0];
    int size = *((int*)&package[1]);
    
    printf("Thread2: Received array with %d elements: ", size);
    
    /* Process the array */
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
    
    /* Clean up the package (but not the array itself) */
    free(package);
    
    return NULL;
}

/**
 * Thread function that takes a structure as its argument.
 */
void* struct_argument_thread(void* arg) {
    /* Cast the void pointer to the actual structure type */
    ThreadData* data = (ThreadData*)arg;
    
    printf("Thread3: Received structure with values:\n");
    printf("  Name: %s\n", data->name);
    printf("  ID: %d\n", data->id);
    printf("  Value: %f\n", data->value);
    
    /* Clean up the allocated structure */
    free(data);
    
    return NULL;
}

/**
 * Thread function demonstrating the UNSAFE way to pass a mutable argument.
 * This is prone to race conditions if the original variable changes.
 */
void* unsafe_argument_thread(void* arg) {
    /* We received a pointer to an integer on the stack of main() */
    int* value_ptr = (int*)arg;
    
    /* Read the initial value */
    printf("Thread4: Initial value: %d\n", *value_ptr);
    
    /* Simulate some processing time */
    sleep(2);
    
    /* 
     * Read the value again. If main() has modified it in the meantime,
     * we'll get a different value!
     */
    printf("Thread4: Value after delay: %d (may have changed!)\n", *value_ptr);
    
    return NULL;
}

/**
 * Thread function demonstrating the SAFE way to pass a mutable argument.
 * The thread owns its copy of the data, so it's not affected by external changes.
 */
void* safe_argument_thread(void* arg) {
    /* We received a pointer to heap-allocated memory */
    int* value_ptr = (int*)arg;
    
    /* Read the value */
    printf("Thread5: Received value: %d\n", *value_ptr);
    
    /* Simulate some processing time */
    sleep(2);
    
    /* The value remains the same since it's our own copy */
    printf("Thread5: Value after delay: %d (unchanged because we own it)\n", *value_ptr);
    
    /* Clean up the allocated memory */
    free(value_ptr);
    
    return NULL;
}

/**
 * EXPLANATION:
 * 
 * Thread Argument Techniques:
 * 
 * 1. Simple Value Passing:
 *    - Cast an integer or small value to a void pointer: (void*)(intptr_t)value
 *    - In the thread, cast back: (int)(intptr_t)arg
 *    - Only works for values small enough to fit in a pointer
 *    - No memory management required
 * 
 * 2. Array Passing:
 *    - Pass a pointer to the array: &array[0] or just array
 *    - Often need to also pass the array size
 *    - Array must remain valid for the thread's lifetime
 *    - Be careful about array scope and lifetime
 * 
 * 3. Structure Passing:
 *    - Define a structure that contains all needed parameters
 *    - Allocate the structure on the heap (malloc)
 *    - Pass the structure pointer to the thread
 *    - Thread should free the structure when done
 *    - Most flexible approach for complex data
 * 
 * 4. Common Pitfalls:
 *    - Passing pointers to stack variables that go out of scope
 *    - Race conditions when original data is modified while the thread runs
 *    - Not synchronizing access to shared data
 *    - Memory leaks from not freeing allocated memory
 * 
 * 5. Best Practices:
 *    - Use heap allocation for thread arguments when data might change
 *    - Copy data rather than sharing, unless sharing is intentional
 *    - Document ownership clearly (who frees the memory)
 *    - Use thread-local storage for thread-specific data
 *    - For shared data, use proper synchronization (mutexes)
 * 
 * 6. Alternative Approaches:
 *    - Global variables (use with caution, thread-safety issues)
 *    - Thread-local storage (for thread-specific data)
 *    - Thread pools with work queues (more advanced pattern)
 */