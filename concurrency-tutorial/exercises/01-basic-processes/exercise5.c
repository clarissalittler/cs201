/*
 * Exercise 5: Process Tree  
 * Difficulty: 🔴 Advanced
 * 
 * OBJECTIVE:
 * Create a binary tree of processes where each process creates exactly 2 
 * children, to a depth of 3 levels. Each process should print its level 
 * in the tree and wait for its children to complete.
 * 
 * REQUIREMENTS:
 * 1. Create a binary tree structure:
 *    Level 1: 1 process (root)
 *    Level 2: 2 processes  
 *    Level 3: 4 processes
 * 2. Each process should:
 *    - Print its level and a unique identifier
 *    - Create 2 children (unless at max depth)
 *    - Wait for both children to complete
 *    - Print a completion message
 * 3. Processes at level 3 (leaves) don't create children
 * 4. Handle all error conditions
 * 
 * TREE STRUCTURE:
 *       Root(1)
 *      /       \
 *   L2-A(2)   L2-B(2)
 *   /   \     /   \
 * L3-A L3-B L3-C L3-D(3)
 * 
 * EXPECTED OUTPUT (partial):
 * Level 1: Root process starting (PID=1001)
 * Level 2: Process A starting (PID=1002)  
 * Level 2: Process B starting (PID=1003)
 * Level 3: Process A starting (PID=1004)
 * Level 3: Process B starting (PID=1005)
 * Level 3: Process C starting (PID=1006)
 * Level 3: Process D starting (PID=1007)
 * Level 3: Process A completed
 * Level 3: Process B completed
 * Level 2: Process A completed
 * ... etc
 * 
 * HINTS:
 * - Pass the current level as a parameter or global variable
 * - Use a process identifier (like A, B for level 2)
 * - Each process needs to wait() twice for its two children
 * - Consider using recursion or iteration for tree creation
 * 
 * BONUS CHALLENGES:
 * 1. Add timing to see how long the tree takes to complete
 * 2. Have each leaf process do some "work" (computation)
 * 3. Count total number of processes created
 * 
 * LEARNING GOALS:
 * - Complex process hierarchies
 * - Coordinating multiple children per parent
 * - Understanding process tree structures
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void create_process_tree(int level, char process_id) {
    // TODO: Print current process information
    // Format: "Level X: Process Y starting (PID=Z)"
    
    // TODO: Check if we're at maximum depth (level 3)
    // If so, just print completion and return
    
    // TODO: Create first child
    // TODO: Handle fork() error
    // TODO: In first child: recursively call create_process_tree with level+1
    
    // TODO: Create second child  
    // TODO: Handle fork() error
    // TODO: In second child: recursively call create_process_tree with level+1
    
    // TODO: Parent waits for both children
    // TODO: Print completion message
}

int main() {
    printf("Creating binary process tree...\n");
    
    // TODO: Start the tree creation from level 1
    create_process_tree(1, 'R'); // R for Root
    
    printf("Process tree completed\n");
    return 0;
}