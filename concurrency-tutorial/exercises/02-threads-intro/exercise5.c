/*
 * Exercise 5: Thread Pool Pattern
 * Difficulty: 🔴 Advanced
 * 
 * OBJECTIVE:
 * Implement a simple thread pool where multiple worker threads process tasks 
 * from a shared work queue. This version intentionally lacks synchronization 
 * to demonstrate the problems that occur without proper coordination.
 * 
 * REQUIREMENTS:
 * 1. Create a structure to represent work items
 * 2. Create a global array to hold work items (simple queue)
 * 3. Create 3 worker threads that process work from the queue
 * 4. Add 10 work items to the queue
 * 5. Observe race conditions and missed/duplicate work
 * 
 * WORK STRUCTURE:
 * Each work item should contain:
 * - Task ID number
 * - "Processing time" (simulated with sleep)
 * 
 * EXPECTED PROBLEMS (without synchronization):
 * - Multiple threads might process the same task
 * - Some tasks might never be processed
 * - Queue index corruption
 * - Inconsistent output
 * 
 * SAMPLE OUTPUT (problems will vary):
 * Main thread: Adding 10 tasks to work queue
 * Worker 1: Starting work
 * Worker 2: Starting work  
 * Worker 3: Starting work
 * Worker 1: Processing task 1 (2 seconds)
 * Worker 2: Processing task 1 (2 seconds)  // DUPLICATE!
 * Worker 3: Processing task 2 (1 second)
 * Worker 1: Completed task 1
 * Worker 2: Completed task 1              // DUPLICATE!
 * Worker 3: Completed task 2
 * ...
 * Main thread: Some tasks were processed multiple times!
 * Main thread: Some tasks were never processed!
 * 
 * LEARNING GOALS:
 * - Understanding the need for synchronization
 * - Observing queue corruption
 * - Seeing duplicate and missed work
 * - Motivation for proper thread coordination
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// TODO: Define work item structure
// typedef struct {
//     int task_id;
//     int processing_time;  // seconds to "work"
// } work_item_t;

// TODO: Define global work queue variables
// work_item_t work_queue[10];
// int queue_size = 0;
// int current_task = 0;  // Index of next task to process

// TODO: Define worker thread function
// void* worker_thread(void* arg)
// This function should:
// 1. Get worker ID from argument
// 2. Print start message
// 3. Loop while there are tasks:
//    - Check if current_task < queue_size
//    - Get next task (work_queue[current_task])
//    - Increment current_task
//    - Print "processing" message
//    - Sleep for task's processing_time
//    - Print "completed" message
// 4. Print worker shutdown message
// 5. Return NULL
//
// NOTE: This has intentional race conditions!

int main() {
    // TODO: Define constants
    // const int NUM_WORKERS = 3;
    // const int NUM_TASKS = 10;
    
    // TODO: Declare thread array
    // pthread_t workers[NUM_WORKERS];
    
    printf("Main thread: Adding %d tasks to work queue\n", NUM_TASKS);
    
    // TODO: Initialize work queue
    // for (int i = 0; i < NUM_TASKS; i++) {
    //     work_queue[i].task_id = i + 1;
    //     work_queue[i].processing_time = 1 + (i % 3);  // 1-3 seconds
    //     queue_size++;
    // }
    
    printf("Main thread: Starting %d worker threads\n", NUM_WORKERS);
    
    // TODO: Create worker threads
    // for (int i = 0; i < NUM_WORKERS; i++) {
        // TODO: Create thread, pass worker ID
    // }
    
    // TODO: Wait for all workers
    // for (int i = 0; i < NUM_WORKERS; i++) {
        // TODO: Join thread
    // }
    
    // TODO: Print analysis
    // printf("\nMain thread: All workers finished\n");
    // printf("Tasks processed: %d (should be %d)\n", current_task, NUM_TASKS);
    // printf("\nProblems observed:\n");
    // printf("- Race conditions in queue access\n");
    // printf("- Possible duplicate task processing\n");
    // printf("- Possible missed tasks\n");
    // printf("- Non-deterministic behavior\n");
    // printf("\nNext exercises will show proper synchronization!\n");
    
    return 0;
}