/**
 * ============================================================================
 * Process Trees and Exponential Process Creation
 * ============================================================================
 * 
 * This example demonstrates how processes can create a tree structure through
 * recursive forking. It shows how the number of processes can grow exponentially
 * and highlights the importance of careful process management.
 * 
 * We'll visualize a process tree where each process creates two children,
 * forming a binary tree of processes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* 
 * Maximum depth of our process tree.
 * WARNING: Setting this too high can create too many processes!
 * Even a small value like 4 creates 2^(DEPTH+1) - 1 = 31 processes.
 */
#define DEPTH 3

/* Function to create child processes recursively */
void create_process_tree(int current_depth, int max_depth, int parent_pid);

int main() {
    /* Save the PID of the initial process */
    pid_t initial_pid = getpid();
    
    printf("Starting process tree demonstration (initial PID: %d)\n", initial_pid);
    printf("This will create a binary tree of processes with depth %d\n", DEPTH);
    printf("Total number of processes created will be: 2^(%d+1) - 1 = %d\n\n", 
            DEPTH, (1 << (DEPTH+1)) - 1);
    
    /* Begin the recursive process creation */
    create_process_tree(0, DEPTH, initial_pid);
    
    /* 
     * Only the initial process should reach this point and wait for all
     * its immediate children
     */
    if (getpid() == initial_pid) {
        printf("\nInitial process (PID: %d) waiting for direct children...\n", initial_pid);
        
        /* Wait for all direct children */
        int status;
        while (wait(&status) > 0) {
            /* This loop continues until all direct children have terminated */
        }
        
        printf("All processes have terminated. Process tree demonstration complete.\n");
    }
    
    return EXIT_SUCCESS;
}

/**
 * Creates a binary tree of processes recursively.
 * 
 * @param current_depth The current depth in the tree
 * @param max_depth The maximum depth allowed
 * @param parent_pid The PID of the parent process
 */
void create_process_tree(int current_depth, int max_depth, int parent_pid) {
    /* Get the current process ID */
    pid_t my_pid = getpid();
    
    /* Print information about this process */
    printf("Process PID: %d, Depth: %d, Parent: %d\n", 
            my_pid, current_depth, getppid());
    
    /* Base case: if we've reached the maximum depth, stop recursion */
    if (current_depth >= max_depth) {
        /* Leaf process - doesn't create any children */
        sleep(1); /* Sleep briefly to allow tree visualization */
        printf("Process PID: %d (leaf) terminating\n", my_pid);
        return;
    }
    
    /* Otherwise, create two child processes (left and right) */
    pid_t left_pid, right_pid;
    
    /* Create the "left" child */
    left_pid = fork();
    
    if (left_pid < 0) {
        /* Fork failed */
        perror("Left fork failed");
        exit(EXIT_FAILURE);
    }
    else if (left_pid == 0) {
        /* Left child process - continue recursion */
        create_process_tree(current_depth + 1, max_depth, my_pid);
        exit(EXIT_SUCCESS); /* Ensure the child exits after its work */
    }
    
    /* Create the "right" child (only the parent does this) */
    right_pid = fork();
    
    if (right_pid < 0) {
        /* Fork failed */
        perror("Right fork failed");
        kill(left_pid, SIGTERM); /* Clean up the left child */
        exit(EXIT_FAILURE);
    }
    else if (right_pid == 0) {
        /* Right child process - continue recursion */
        create_process_tree(current_depth + 1, max_depth, my_pid);
        exit(EXIT_SUCCESS); /* Ensure the child exits after its work */
    }
    
    /* 
     * Parent process code continues here.
     * Wait for both children to terminate if this isn't the initial process.
     */
    if (my_pid != parent_pid) {
        printf("Process PID: %d waiting for children %d and %d\n", 
                my_pid, left_pid, right_pid);
        
        /* Wait for both children */
        waitpid(left_pid, NULL, 0);
        waitpid(right_pid, NULL, 0);
        
        printf("Process PID: %d - both children terminated\n", my_pid);
    }
}

/**
 * EXPLANATION:
 * 
 * Process Tree Concept:
 * 
 * 1. Process Hierarchy:
 *    - Processes form a tree-like structure in Unix-like systems
 *    - Each process (except init) has a parent
 *    - A process can have multiple children
 *    - The initial process in this program creates a binary tree
 * 
 * 2. Exponential Growth:
 *    - Each process creates two children, leading to exponential growth
 *    - With depth D, we get 2^(D+1) - 1 total processes
 *    - Even small values of D can create many processes
 * 
 * 3. Process Creation Cost:
 *    - Creating processes is relatively expensive
 *    - Each fork() duplicates the entire address space (copy-on-write)
 *    - System resources like PIDs and process table entries are consumed
 *    - Too many processes can overload the system
 * 
 * 4. Process Tree Visualization:
 *    - For DEPTH=3, the tree would look like:
 *
 *                       [Initial]
 *                       /      \
 *                     [L]      [R]
 *                    /  \     /  \
 *                  [LL] [LR] [RL] [RR]
 *                  / \  / \  / \  / \
 *                ... ... ... ... ... ...
 *
 * 5. Real-world Applications:
 *    - Work distribution systems (each process handles a subset of work)
 *    - Server architectures (process per connection or worker process pools)
 *    - Parallel computations (divide-and-conquer algorithms)
 * 
 * 6. Potential Issues:
 *    - Resource exhaustion (too many processes)
 *    - Zombie processes (if parents don't wait for children)
 *    - Orphan processes (if parents terminate before children)
 *    - Synchronization challenges for inter-process communication
 */