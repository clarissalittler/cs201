/*
 * SOLUTION: Exercise 5 - Process Tree
 * 
 * This solution creates a binary tree of processes with 3 levels.
 * Each internal node creates 2 children and waits for both to complete.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void create_process_tree(int level, char process_id) {
    // Print current process information
    char level_names[][10] = {"", "Root", "Level 2", "Level 3"};
    printf("%s: Process %c starting (PID=%d, PPID=%d)\n", 
           level_names[level], process_id, getpid(), getppid());
    
    // Check if we're at maximum depth (level 3)
    if (level == 3) {
        // Leaf process - simulate some work
        usleep(100000); // 100ms of "work"
        printf("Level 3: Process %c completed\n", process_id);
        return;
    }
    
    // Create child process identifiers
    char left_child_id, right_child_id;
    if (level == 1) {
        left_child_id = 'A';
        right_child_id = 'B';
    } else if (level == 2) {
        if (process_id == 'A') {
            left_child_id = 'C';
            right_child_id = 'D';
        } else { // process_id == 'B'
            left_child_id = 'E';
            right_child_id = 'F';
        }
    }
    
    // Create first child (left child)
    pid_t left_child = fork();
    
    if (left_child < 0) {
        perror("Fork failed for left child");
        return;
    }
    
    if (left_child == 0) {
        // This is the left child process
        create_process_tree(level + 1, left_child_id);
        exit(0); // Child exits after completing its subtree
    }
    
    // Create second child (right child)
    pid_t right_child = fork();
    
    if (right_child < 0) {
        perror("Fork failed for right child");
        // Still need to wait for left child
        wait(NULL);
        return;
    }
    
    if (right_child == 0) {
        // This is the right child process
        create_process_tree(level + 1, right_child_id);
        exit(0); // Child exits after completing its subtree
    }
    
    // Parent waits for both children
    int status;
    
    // Wait for left child
    waitpid(left_child, &status, 0);
    
    // Wait for right child  
    waitpid(right_child, &status, 0);
    
    // Print completion message
    printf("%s: Process %c completed\n", level_names[level], process_id);
}

int main() {
    printf("Creating binary process tree...\n");
    printf("Tree structure:\n");
    printf("       Root(R)\n");
    printf("      /       \\\n");
    printf("    A(2)     B(2)\n");
    printf("   /   \\     /   \\\n");
    printf("  C(3) D(3) E(3) F(3)\n");
    printf("\n");
    fflush(stdout); // Ensure output is flushed before forking
    
    // Start the tree creation from level 1
    create_process_tree(1, 'R'); // R for Root
    
    printf("Process tree completed successfully\n");
    return 0;
}

/*
 * ALTERNATIVE ITERATIVE APPROACH:
 * 
 * For educational purposes, here's a non-recursive version:
 */

/*
int main() {
    printf("Creating binary process tree (iterative approach)...\n");
    
    // Level 1: Root process
    printf("Level 1: Root process starting (PID=%d)\n", getpid());
    
    // Create level 2 processes
    for (int i = 0; i < 2; i++) {
        pid_t level2_pid = fork();
        
        if (level2_pid == 0) {
            // Level 2 process
            char process_name = 'A' + i;
            printf("Level 2: Process %c starting (PID=%d, PPID=%d)\n",
                   process_name, getpid(), getppid());
            
            // Create level 3 processes
            for (int j = 0; j < 2; j++) {
                pid_t level3_pid = fork();
                
                if (level3_pid == 0) {
                    // Level 3 process (leaf)
                    char leaf_name = 'C' + (i * 2) + j;
                    printf("Level 3: Process %c starting (PID=%d, PPID=%d)\n",
                           leaf_name, getpid(), getppid());
                    
                    usleep(100000); // Simulate work
                    printf("Level 3: Process %c completed\n", leaf_name);
                    exit(0);
                }
            }
            
            // Level 2 process waits for both its children
            wait(NULL);
            wait(NULL);
            printf("Level 2: Process %c completed\n", process_name);
            exit(0);
        }
    }
    
    // Root waits for both level 2 processes
    wait(NULL);
    wait(NULL);
    printf("Level 1: Root process completed\n");
    
    return 0;
}
*/

/*
 * EXPLANATION:
 * 
 * 1. BINARY TREE STRUCTURE:
 *    - Each internal node has exactly 2 children
 *    - Depth is 3 levels (root, internal, leaves)
 *    - Total processes: 1 + 2 + 4 = 7 processes
 * 
 * 2. RECURSIVE DESIGN:
 *    - Function calls itself for child processes
 *    - Each process manages its own subtree
 *    - Clean separation of concerns
 * 
 * 3. SYNCHRONIZATION PATTERN:
 *    - Each parent waits for both children
 *    - Uses waitpid() for specific child control
 *    - Ensures proper cleanup order
 * 
 * 4. PROCESS COORDINATION:
 *    - Leaves complete first
 *    - Internal nodes complete after their children
 *    - Root completes last
 * 
 * 5. EXECUTION FLOW:
 *    a) Root creates A and B
 *    b) A creates C and D; B creates E and F
 *    c) Leaves (C,D,E,F) complete
 *    d) Internal nodes (A,B) complete
 *    e) Root completes
 * 
 * PROCESS TREE VISUALIZATION:
 * 
 * Time →
 * Root ────────────────────────── completes last
 *  ├─ A ──────────────── completes after C,D
 *  │  ├─ C ── completes
 *  │  └─ D ── completes  
 *  └─ B ──────────────── completes after E,F
 *     ├─ E ── completes
 *     └─ F ── completes
 * 
 * KEY CONCEPTS:
 * - Complex process hierarchies
 * - Multiple child management
 * - Recursive process creation
 * - Synchronized termination
 * 
 * REAL-WORLD APPLICATIONS:
 * - Parallel divide-and-conquer algorithms
 * - Distributed computation trees
 * - Hierarchical task decomposition
 * - MapReduce-style processing
 * 
 * PERFORMANCE CONSIDERATIONS:
 * - All leaf processes can run in parallel
 * - Tree depth affects maximum parallelism
 * - Process creation overhead vs. work done
 */