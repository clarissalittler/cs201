/*
 * SOLUTION: Exercise 2 - Process Counter
 * 
 * This solution demonstrates that processes have separate memory spaces.
 * Each process has its own copy of variables after fork().
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    int counter = 0; // Shared before fork, separate after fork
    pid_t pid;
    
    printf("Initial counter value: %d\n", counter);
    
    // Fork to create child process
    pid = fork();
    
    // Handle fork() error
    if (pid < 0) {
        perror("Fork failed");
        return EXIT_FAILURE;
    }
    
    if (pid == 0) {
        // Child process logic
        printf("Child: Starting with counter = %d\n", counter);
        
        for (int i = 0; i < 5; i++) {
            counter++;
            printf("Child: counter = %d (iteration %d)\n", counter, i+1);
            usleep(100000); // 100ms delay
        }
        
        printf("Child: Final counter value = %d\n", counter);
    } else {
        // Parent process logic
        printf("Parent: Starting with counter = %d\n", counter);
        
        for (int i = 0; i < 5; i++) {
            counter++;
            printf("Parent: counter = %d (iteration %d)\n", counter, i+1);
            usleep(120000); // 120ms delay (slightly different from child)
        }
        
        printf("Parent: Final counter value = %d\n", counter);
    }
    
    return 0;
}

/*
 * EXPLANATION:
 * 
 * 1. MEMORY SEPARATION:
 *    - Before fork(): One process, one counter variable
 *    - After fork(): Two processes, each with their own copy of counter
 *    - Changes in one process don't affect the other
 * 
 * 2. COPY-ON-WRITE:
 *    - Modern systems use copy-on-write optimization
 *    - Memory is shared until one process modifies it
 *    - First modification triggers actual copying
 * 
 * 3. EXPECTED BEHAVIOR:
 *    - Both processes start with counter = 0
 *    - Each process increments its own copy 5 times
 *    - Final values: Parent counter = 5, Child counter = 5
 *    - Changes are independent
 * 
 * 4. OUTPUT INTERLEAVING:
 *    - The usleep() calls slow down execution
 *    - This makes the interleaved output more visible
 *    - Different delays help distinguish parent vs child
 * 
 * KEY LEARNING:
 * Processes do NOT share memory by default. This is fundamental to
 * process isolation and security in operating systems.
 * 
 * EXPERIMENT:
 * Try running this multiple times and observe:
 * - Does the output order change?
 * - Are the final counter values always 5 for both processes?
 * - What happens if you remove the usleep() calls?
 */