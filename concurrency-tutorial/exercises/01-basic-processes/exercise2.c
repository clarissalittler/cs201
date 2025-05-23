/*
 * Exercise 2: Process Counter
 * Difficulty: 🟢 Beginner
 * 
 * OBJECTIVE:
 * Write a program where parent and child processes each increment a shared 
 * counter variable 5 times, printing the value each time. Observe what 
 * happens to the variable in each process.
 * 
 * REQUIREMENTS:
 * 1. Initialize a counter variable to 0
 * 2. Fork to create a child process
 * 3. Both parent and child should:
 *    - Increment the counter 5 times
 *    - Print the counter value and process type after each increment
 *    - Add a small delay between increments to make output visible
 * 4. Handle fork() errors appropriately
 * 
 * EXPECTED BEHAVIOR:
 * You should observe that each process has its own copy of the counter
 * variable after the fork. Changes in one process don't affect the other.
 * 
 * SAMPLE OUTPUT (order may vary):
 * Parent: counter = 1
 * Child: counter = 1  
 * Parent: counter = 2
 * Child: counter = 2
 * ... etc
 * 
 * HINTS:
 * - Use usleep(100000) for a 100ms delay between increments
 * - The counter variable is copied to both processes after fork()
 * - Each process has its own memory space
 * 
 * LEARNING GOAL:
 * Understand that processes have separate memory spaces and don't share
 * variables by default.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    // TODO: Initialize counter variable to 0
    
    // TODO: Fork to create child process
    
    // TODO: Handle fork() error
    
    // TODO: Implement child process logic
    // Loop 5 times:
    //   - Increment counter
    //   - Print "Child: counter = X"
    //   - Sleep briefly
    
    // TODO: Implement parent process logic  
    // Loop 5 times:
    //   - Increment counter
    //   - Print "Parent: counter = X"
    //   - Sleep briefly
    
    return 0;
}