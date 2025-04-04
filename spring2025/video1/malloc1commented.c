/**
 * @file dynamic_memory_example.c
 * @brief Educational example demonstrating dynamic memory allocation in C.
 *
 * TUTORIAL: Dynamic Memory Allocation in C (using malloc and free)
 *
 * 1.  **Memory in C Programs:**
 *     - **Stack Memory:** Used for local variables (declared inside functions),
 *       function parameters, and function call management. Memory allocation
 *       and deallocation are automatic (managed by the compiler). Size is
 *       usually limited and fixed when the program starts. Fast access.
 *     - **Heap Memory (Free Store):** A large pool of memory available to the
 *       program during runtime. Used for data whose size might not be known
 *       at compile time or that needs to persist beyond a single function call.
 *       Memory allocation and deallocation are *manual* - the programmer must
 *       explicitly request and release memory. Slower access than stack.
 *
 * 2.  **Why Dynamic Allocation?**
 *     - When you don't know the size of data needed until the program runs
 *       (e.g., reading user input to determine array size).
 *     - When you need data structures (like linked lists, trees) that can
 *       grow or shrink during execution.
 *     - When you need data to exist longer than the function that created it.
 *
 * 3.  **`malloc()` (Memory Allocation):**
 *     - Function signature: `void* malloc(size_t size);`
 *     - Located in `<stdlib.h>`.
 *     - Purpose: Allocates a block of `size` bytes of memory from the heap.
 *     - `size_t`: An unsigned integer type representing sizes (often `unsigned long`).
 *     - Return Value:
 *       - On success: Returns a pointer (`void*`) to the *beginning* of the
 *         allocated memory block. A `void*` is a generic pointer and must be
 *         *cast* to the appropriate pointer type (e.g., `int*`, `char*`)
 *         before use, so the compiler knows how to interpret the data stored there.
 *       - On failure: Returns `NULL` (if the system cannot allocate the
 *         requested memory). **Always check the return value of `malloc`!**
 *
 * 4.  **`sizeof` Operator:**
 *     - Used to determine the size (in bytes) of a data type (e.g., `sizeof(int)`)
 *       or a variable.
 *     - Crucial for `malloc` to ensure portability, as data type sizes (like `int`)
 *       can vary between different systems (e.g., 32-bit vs. 64-bit).
 *     - `malloc(100 * sizeof(int))` requests enough memory for 100 integers,
 *       regardless of whether an `int` is 2, 4, or 8 bytes on that system.
 *
 * 5.  **Using Allocated Memory:**
 *     - Once `malloc` returns a valid pointer (cast to the correct type, e.g., `int* ptr`),
 *       you can treat that pointer as the start of an array of that type.
 *     - Access elements using array notation: `ptr[i]`
 *     - Or pointer arithmetic: `*(ptr + i)`
 *
 * 6.  **`free()` (Memory Deallocation):**
 *     - Function signature: `void free(void* ptr);`
 *     - Located in `<stdlib.h>`.
 *     - Purpose: Releases the block of memory previously allocated by `malloc`
 *       (or `calloc`, `realloc`) back to the heap, making it available for reuse.
 *     - Argument: The pointer `ptr` *must* be the exact pointer value returned
 *       by the allocation function (`malloc`, etc.), or `NULL`.
 *     - **CRITICAL:** Failing to `free` memory that is no longer needed causes
 *       a **memory leak**. The program holds onto memory it won't use, and if
 *       this happens repeatedly, the program might consume all available memory
 *       and crash or cause system instability.
 *     - **NEVER** `free` the same memory block twice (double free).
 *     - **NEVER** use memory after it has been freed (use-after-free). Both lead
 *       to undefined behavior (crashes, corruption).
 *
 * 7.  **Dangling Pointers and `NULL`:**
 *     - After calling `free(ptr)`, the memory `ptr` pointed to is invalid, but
 *       the pointer variable `ptr` *still holds the old address*. This is called
 *       a **dangling pointer**.
 *     - Accessing memory via a dangling pointer is dangerous (use-after-free).
 *     - **Best Practice:** Immediately after freeing memory, set the pointer
 *       variable to `NULL`: `free(ptr); ptr = NULL;`. This prevents accidental
 *       use of the dangling pointer. Checking if a pointer is `NULL` before use
 *       is a common safety measure.
 *
 * This example demonstrates allocating a large array on the heap, initializing it,
 * printing some information, and then correctly freeing the allocated memory.
 */

#include <stdio.h>  // Include standard input/output library for functions like printf
#include <stdlib.h> // Include standard library for functions like malloc, free, and NULL

int main() { // Start of the main function, the program's entry point

    // Declare a pointer variable named 'bigArray'.
    // 'int*' means this variable is intended to hold the memory address
    // of an integer (or the starting address of a block of integers).
    // At this point, it doesn't point to any valid memory yet (it holds garbage).
    int* bigArray;

    // Define the desired number of integers for our array.
    // Using a constant makes the code easier to read and modify.
    const int numElements = 100000;

    // --- Dynamic Memory Allocation ---
    // Request memory from the heap.
    // 1. `sizeof(int)`: Calculate the size of one integer in bytes on this system.
    // 2. `numElements * sizeof(int)`: Calculate the total bytes needed for the entire array.
    // 3. `malloc(...)`: Attempt to allocate that many contiguous bytes on the heap.
    // 4. `(int*)`: Cast the `void*` returned by `malloc` to an `int*`. This tells the
    //    compiler to treat the allocated memory as a block of integers.
    // 5. `bigArray = ...`: Assign the starting address of the allocated block to our pointer.
    bigArray = (int*) malloc(numElements * sizeof(int));

    // --- Check for Allocation Failure ---
    // `malloc` returns NULL if it cannot allocate the requested memory (e.g., out of memory).
    // It's crucial to check for this!
    if (bigArray == NULL) {
        // Print an error message to standard error.
        fprintf(stderr, "Error: Memory allocation failed!\n");
        // Return a non-zero value to indicate program failure.
        return 1;
    }

    // If we reach here, allocation was successful, and 'bigArray' points to valid memory.

    // --- Initialize the Allocated Array ---
    // Loop through each element of the dynamically allocated array.
    printf("Initializing array...\n");
    for (int i = 0; i < numElements; i++) {
        // Use array notation `bigArray[i]` to access the i-th integer
        // in the allocated block. This is equivalent to `*(bigArray + i)`.
        // Assign the square of the index 'i' to the element.
        bigArray[i] = i * i;
    }
    printf("Initialization complete.\n");

    // --- Print a Few Elements (Optional - Avoid printing all 100k!) ---
    // Printing all 100,000 elements would flood the console.
    // Let's just print the first 10 and the last one as a sample.
    printf("Sample elements from the array:\n");
    for (int i = 0; i < 10; i++) {
        // Print the index and the value of the element at that index.
        printf("bigArray[%d] = %d\n", i, bigArray[i]);
    }
    // Print the last element
    int lastIndex = numElements - 1;
    printf("...\n");
    printf("bigArray[%d] = %d\n", lastIndex, bigArray[lastIndex]);


    // --- Print Debugging Information ---
    // Print the memory address stored in the 'bigArray' pointer variable.
    // `%p` is the format specifier for printing pointer addresses (usually in hexadecimal).
    // This shows where the allocated block starts in memory.
    printf("The starting address of the allocated block (value of bigArray) is: %p\n", (void*)bigArray);
    // Note: Casting to (void*) is the standard way to print pointers with %p.

    // Print the size of a single integer on this specific system.
    // `sizeof` returns a `size_t` type, which is printed using `%ld` (long unsigned integer).
    printf("Size of one 'int' on this system (sizeof(int)): %ld bytes\n", sizeof(int));

    // --- Deallocate the Memory ---
    // Release the memory block pointed to by 'bigArray' back to the heap system.
    // This is crucial to prevent memory leaks.
    // The memory manager can now reuse this memory for future allocations.
    printf("Freeing allocated memory...\n");
    free(bigArray);

    // --- Prevent Dangling Pointer ---
    // Although the memory is freed, the 'bigArray' variable *still holds the old address*.
    // This is now a "dangling pointer" because it points to invalid memory.
    // It's a very good practice to set the pointer to NULL immediately after freeing it.
    // This prevents accidental use of the invalid address later in the code.
    bigArray = NULL;

    // Print the value of the 'bigArray' pointer variable *after* freeing and setting to NULL.
    // It should now print (nil) or 0x0, indicating it doesn't point to anything valid.
    printf("After free() and setting to NULL, the value of bigArray is: %p\n", (void*)bigArray);

    // Indicate successful program execution by returning 0 from main.
    return 0;
}
