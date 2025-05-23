/*
 * Exercise 1: First Fork
 * Difficulty: 🟢 Beginner
 * 
 * OBJECTIVE:
 * Create a program that forks to create one child process. Both parent and 
 * child should print their process ID and their parent's process ID.
 * 
 * REQUIREMENTS:
 * 1. Use fork() to create exactly one child process
 * 2. Both parent and child must print:
 *    - Their own process ID (PID)
 *    - Their parent's process ID (PPID)
 *    - A message identifying whether they are parent or child
 * 3. Handle the case where fork() fails
 * 4. Make the output clear and distinguishable
 * 
 * EXPECTED OUTPUT (PIDs will vary):
 * Parent process: PID=1234, PPID=1000
 * Child process: PID=1235, PPID=1234
 * 
 * HINTS:
 * - fork() returns 0 to the child, child's PID to parent, -1 on error
 * - Use getpid() to get current process ID
 * - Use getppid() to get parent process ID
 * - Use perror() for error handling
 * 
 * BONUS CHALLENGE:
 * Add a small delay and observe if the parent or child prints first.
 * What does this tell you about process scheduling?
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    // TODO: Declare a variable to store the return value of fork()
    
    // TODO: Call fork() and store the result
    
    // TODO: Check for fork() failure (return value < 0)
    
    // TODO: Check if this is the child process (return value == 0)
    // If child: print child information
    
    // TODO: Handle the parent process case (return value > 0)
    // If parent: print parent information
    
    return 0;
}