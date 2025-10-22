#include <stdio.h>
#include <pthread.h>

void* thread_function(void* arg) {
    printf("Thread executing\n");
    return NULL;
}

int main() {
    pthread_t thread;
    char* message = "Hello from main thread";   
    // Create a new thread
    pthread_create(&thread, NULL, thread_function, NULL);
 
    // Wait for thread to complete
    pthread_join(thread, NULL);
    printf("Thread joined\n");
    
    return 0;
}
