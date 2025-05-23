/*
 * Exercise 3: Process Chain
 * Difficulty: 🟡 Intermediate
 * 
 * OBJECTIVE:
 * Create a chain of 3 processes where each process creates the next one.
 * Each process should print its PID and parent PID, then wait for its 
 * child to complete before exiting.
 * 
 * REQUIREMENTS:
 * 1. Create a chain: Process A → Process B → Process C
 * 2. Each process should print: "Process [A/B/C]: PID=X, PPID=Y"
 * 3. Each process must wait for its child to complete before exiting
 * 4. Process C (the last one) has no child to create
 * 5. Handle all error conditions appropriately
 * 
 * EXPECTED OUTPUT:
 * Process A: PID=1001, PPID=1000
 * Process B: PID=1002, PPID=1001  
 * Process C: PID=1003, PPID=1002
 * Process C completed
 * Process B completed  
 * Process A completed
 * 
 * HINTS:
 * - Use a variable to track which "generation" each process is
 * - Use wait() to wait for child processes
 * - The original process starts as generation 1
 * - Only create a child if current generation < 3
 * 
 * LEARNING GOALS:
 * - Understand process hierarchies
 * - Learn about process synchronization with wait()
 * - Practice conditional process creation
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    // TODO: Define which generation this process is (start with 1)
    
    // TODO: Print current process information
    // Format: "Process [A/B/C]: PID=X, PPID=Y"
    
    // TODO: Check if we should create another process (generation < 3)
    
    // TODO: If yes, fork a child process
    
    // TODO: Handle fork() error
    
    // TODO: In child process:
    // - This will become the next generation
    // - Continue the chain (recursive behavior)
    
    // TODO: In parent process:
    // - Wait for child to complete
    // - Print completion message
    
    return 0;
}