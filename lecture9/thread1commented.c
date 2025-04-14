#include <stdio.h>      // Standard Input/Output library for functions like printf
#include <pthread.h>    // POSIX Threads library for thread creation and management
#include <unistd.h>     // Provides access to the POSIX operating system API (e.g., sleep)
#include <time.h>       // Time-related functions (not used in this example but included)
#include <stdlib.h>     // Standard library for functions like exit

// Function that each thread will execute
// It takes a void pointer as an argument and returns a void pointer
void* ourPrinter(void* arg){
    // Cast the void pointer to a char pointer to access the message
    char* msg = (char*) arg;

    // Print the message to the console
    printf("Our thread says: %s", msg);

    // Return NULL to indicate that the thread has finished execution
    return NULL;
}

int main(){
    // Declare two pthread_t variables to store thread identifiers
    pthread_t thread1;
    pthread_t thread2;

    // Define messages that each thread will print
    char* msg1 = "Hi there, I'm one thread\n";
    char* msg2 = "Hi there, I'm a different thread\n";

    /* 
     * Create the first thread.
     * Parameters:
     * - &thread1: Pointer to the pthread_t variable where the thread ID will be stored
     * - NULL: Thread attributes (NULL means default attributes)
     * - ourPrinter: The function the thread will execute
     * - msg1: The argument to pass to the ourPrinter function
     *
     * pthread_create returns 0 on success. In a real application, you should check for errors.
     */
    pthread_create(&thread1, NULL, ourPrinter, msg1);

    /* 
     * Create the second thread with similar parameters as the first.
     * This thread will execute the ourPrinter function with msg2 as its argument.
     */
    pthread_create(&thread2, NULL, ourPrinter, msg2);

    /* 
     * Wait for the first thread to finish execution.
     * Parameters:
     * - thread1: The thread to wait for
     * - NULL: Pointer to store the return value of the thread (NULL means we don't care)
     *
     * pthread_join blocks the calling thread (main thread) until the specified thread terminates.
     */
    pthread_join(thread1, NULL);

    /* 
     * Wait for the second thread to finish execution in the same manner.
     */
    pthread_join(thread2, NULL);

    // Return 0 to indicate that the program finished successfully
    return 0;
}
gcc -pthread -o thread_example thread_example.c
./thread_example
Our thread says: Hi there, I'm one thread
Our thread says: Hi there, I'm a different thread
