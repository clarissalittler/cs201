// PEDAGOGICAL PURPOSE:
// This program demonstrates a MESSAGE QUEUE WORKER (consumer) that processes tasks.
// Key learning objectives:
// 1. Understanding the worker/consumer side of message queues
// 2. Learning how workers compete for tasks (first come, first served)
// 3. Seeing practical task processing and result reporting
// 4. Understanding how multiple workers can run simultaneously
// 5. Learning about simulated work with sleep()
// 6. Demonstrating automatic load balancing with message queues

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#define PROJECT_ID 123
#define TASK_MSG 1       // Type for tasks
#define RESULT_MSG 2     // Type for results
#define MAX_TEXT 512

// MESSAGE STRUCTURE (must match task_distributor.c)
struct message {
    long mtype;             // Message type
    int task_id;            // Task ID
    int difficulty;         // Task difficulty (1-10)
    char mtext[MAX_TEXT];   // Task description or result
};

// SIMULATE WORK:
// Sleeps based on task difficulty
void do_work(int difficulty, const char* task, char* result) {
    printf("Working on task: %s (Difficulty: %d/10)\n", task, difficulty);

    // SIMULATED WORK:
    // In real application, would actually process task
    // Here we just sleep to simulate varying work times
    sleep(difficulty);

    // GENERATE RESULT:
    // Format result message with worker info and timing
    snprintf(result, MAX_TEXT,
             "Task completed by worker %d.\nTask was: %s\nTime taken: %d seconds",
             getpid(), task, difficulty);
}

int main() {
    key_t key;
    int msgid;
    struct message task_msg, result_msg;

    // STEP 1: GET MESSAGE QUEUE KEY
    // MUST match task_distributor.c
    // Same ftok() call = same key = same message queue
    key = ftok(".", PROJECT_ID);

    // STEP 2: ACCESS EXISTING MESSAGE QUEUE
    //
    // msgget(key, 0666 | IPC_CREAT):
    //   - Opens existing queue if distributor created it
    //   - Creates if doesn't exist (but usually distributor creates first)
    //   - IPC_CREAT: Create if needed (doesn't fail if exists)
    //
    // IMPORTANT:
    //   - Distributor should run first
    //   - But workers can start before or after distributor
    //   - IPC_CREAT makes it flexible
    msgid = msgget(key, 0666 | IPC_CREAT);

    printf("Worker %d started. Waiting for tasks...\n", getpid());

    // SEED RANDOM FOR THIS WORKER:
    // Each worker gets unique seed
    srand(getpid());

    // STEP 3: PROCESS TASKS
    // Worker will process 2 tasks, then exit
    //
    // WHY 2 TASKS?
    //   - Distributor sends 5 tasks
    //   - If 3 workers each do 2 tasks: total = 6 (one worker does 1)
    //   - Automatic load balancing
    //   - First available worker gets next task
    //
    // REAL WORKERS:
    //   - Would loop forever: while(1)
    //   - Or until shutdown signal received
    for (int i = 0; i < 2; i++) {
        // RECEIVE A TASK:
        //
        // msgrcv(msgid, &task_msg, size, TASK_MSG, 0):
        //   - Receive message of type TASK_MSG (type 1)
        //   - BLOCKS until task is available
        //   - If multiple workers waiting: first one woken up gets it
        //   - Kernel handles synchronization (no race conditions)
        //
        // COMPETITION:
        //   - Multiple workers call msgrcv() simultaneously
        //   - All block waiting for tasks
        //   - Distributor sends task (type 1)
        //   - ONE worker receives it
        //   - Other workers continue waiting
        //   - Automatic, fair work distribution
        msgrcv(msgid, &task_msg, sizeof(task_msg) - sizeof(long),
               TASK_MSG, 0);

        printf("\nReceived task #%d\n", task_msg.task_id);

        // PROCESS THE TASK:
        // Simulate work based on difficulty
        char result[MAX_TEXT];
        do_work(task_msg.difficulty, task_msg.mtext, result);

        // PREPARE RESULT MESSAGE:
        // Copy task info to result
        result_msg.mtype = RESULT_MSG;  // Type 2 = result
        result_msg.task_id = task_msg.task_id;
        result_msg.difficulty = task_msg.difficulty;
        snprintf(result_msg.mtext, MAX_TEXT, "%s", result);

        // SEND RESULT BACK:
        //
        // msgsnd(msgid, &result_msg, size, 0):
        //   - Send result message (type 2)
        //   - Distributor is waiting for type 2 messages
        //   - Distributor receives results in order they arrive
        //   - Not necessarily in task ID order!
        msgsnd(msgid, &result_msg, sizeof(result_msg) - sizeof(long), 0);

        printf("Sent result for task #%d\n", task_msg.task_id);
    }

    printf("Worker %d finished processing tasks.\n", getpid());

    return 0;
}

// EXPECTED OUTPUT (Worker #12345):
// Worker 12345 started. Waiting for tasks...
//
// Received task #1
// Working on task: Find all prime numbers below 500 (Difficulty: 7/10)
// (7 second pause)
// Sent result for task #1
//
// Received task #4
// Working on task: Sort an array of 10 random numbers (Difficulty: 2/10)
// (2 second pause)
// Sent result for task #4
//
// Worker 12345 finished processing tasks.

// LOAD BALANCING:
//
// SCENARIO: 5 tasks, 3 workers
//
// Time  Worker1         Worker2         Worker3
// ----  -------         -------         -------
// 0     Get task #1     Get task #2     Get task #3
// 3     (working)       Done, get #4    (working)
// 5     Done, get #5    (working)       Done, idle
// 7     Done, idle      Done, idle      -
//
// AUTOMATIC:
//   - No manual assignment
//   - Fastest worker gets most tasks
//   - Slow workers don't hold up queue
//   - Perfect for heterogeneous systems

// MESSAGE QUEUE GUARANTEES:
//
// 1. ATOMICITY:
//    - Only ONE process receives each message
//    - No duplicates
//    - No lost messages
//
// 2. ORDERING:
//    - Messages of same type are FIFO
//    - Task #1 sent before #2 → #1 received before #2
//    - But results may arrive out of order (different processing times)
//
// 3. PERSISTENCE:
//    - Messages survive process death
//    - If worker crashes, task remains in queue
//    - Another worker can pick it up
//
// 4. KERNEL-MANAGED:
//    - No explicit synchronization needed
//    - No semaphores, no mutexes
//    - Kernel handles all coordination

// REAL-WORLD APPLICATIONS:
//
// 1. JOB QUEUES:
//    - Web servers distribute requests to worker processes
//    - Background job processing (emails, reports, etc.)
//
// 2. MICROSERVICES:
//    - Task queue between services
//    - Asynchronous processing
//
// 3. PARALLEL COMPUTING:
//    - Distribute computation across CPUs
//    - Map-reduce paradigm
//
// 4. PRINT SPOOLING:
//    - Print jobs queued for printer
//    - CUPS uses message queues

// TRY IT:
// 1. Run distributor: ./distributor &
// 2. Run 3 workers: ./worker & ./worker & ./worker &
// 3. Watch automatic load distribution
// 4. EXPERIMENT: Run 1 worker vs 10 workers (timing difference)
// 5. EXPERIMENT: Start workers AFTER distributor sends tasks
//    Workers still get tasks (messages persisted)
// 6. CHALLENGE: Make workers run indefinitely (while(1))
// 7. CHALLENGE: Add different task types with priorities
// 8. CHALLENGE: Implement result validation in distributor
