// PEDAGOGICAL PURPOSE:
// This program demonstrates MESSAGE QUEUES for task distribution (producer).
// Key learning objectives:
// 1. Understanding System V message queues for IPC
// 2. Learning ftok() to generate unique keys
// 3. Understanding message types and selective message receiving
// 4. Seeing the producer-consumer pattern with message queues
// 5. Learning about structured messages vs raw byte streams
// 6. Understanding msgget(), msgsnd(), msgrcv(), msgctl()

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>     // For ftok(), IPC_CREAT
#include <sys/msg.h>     // For msgget(), msgsnd(), msgrcv(), msgctl()
#include <time.h>

#define PROJECT_ID 123
#define TASK_MSG 1       // Message type for tasks
#define RESULT_MSG 2     // Message type for results
#define MAX_TEXT 512

// MESSAGE STRUCTURE:
// All messages have this format
struct message {
    long mtype;             // Message type (required, must be first field)
    int task_id;            // Task identifier
    int difficulty;         // Task difficulty (1-10)
    char mtext[MAX_TEXT];   // Task description or result
};

int main() {
    key_t key;
    int msgid;
    struct message task_msg, result_msg;

    // STEP 1: GENERATE UNIQUE KEY
    //
    // ftok(".", PROJECT_ID):
    //   - "." : Current directory (must exist)
    //   - PROJECT_ID: Project identifier (arbitrary number)
    //   - Returns key_t value unique to this combination
    //
    // WHY ftok()?
    //   - Message queues need a unique identifier
    //   - ftok() generates consistent key from file path + ID
    //   - Worker process uses same ftok() call to get same key
    //   - This is how they find the same message queue
    //
    // IMPORTANT:
    //   - All processes must use same path and PROJECT_ID
    //   - File must exist (we use "." which always exists)
    key = ftok(".", PROJECT_ID);

    // STEP 2: CREATE/ACCESS MESSAGE QUEUE
    //
    // msgget(key, flags):
    //   - key: From ftok()
    //   - 0666: Permissions (rw-rw-rw-)
    //   - IPC_CREAT: Create if doesn't exist
    //   - Returns message queue ID (msgid)
    //
    // MESSAGE QUEUE:
    //   - Kernel-managed FIFO for messages
    //   - Persists until explicitly removed
    //   - Multiple processes can send/receive
    //   - Messages can have types (for selective receiving)
    msgid = msgget(key, 0666 | IPC_CREAT);

    printf("Task Distribution System\n");
    printf("------------------------\n");

    // SEED RANDOM NUMBER GENERATOR:
    srand(time(NULL));

    // TASK DESCRIPTIONS:
    // Array of different tasks to distribute
    char *tasks[] = {
        "Calculate the sum of numbers from 1 to 1000",
        "Find all prime numbers below 500",
        "Compute the Fibonacci sequence up to the 20th number",
        "Find the GCD of 56 and 98",
        "Count the vowels in 'supercalifragilisticexpialidocious'",
        "Reverse the string 'The quick brown fox jumps over the lazy dog'",
        "Calculate the factorial of 12",
        "Implement a simple calculator",
        "Check if 1001 is a palindrome",
        "Sort an array of 10 random numbers"
    };
    int num_tasks = sizeof(tasks) / sizeof(tasks[0]);

    // STEP 3: DISTRIBUTE TASKS
    // Send 5 tasks to the message queue
    for (int i = 1; i <= 5; i++) {
        // PREPARE TASK MESSAGE:
        task_msg.mtype = TASK_MSG;           // Type 1 = task
        task_msg.task_id = i;
        task_msg.difficulty = rand() % 10 + 1;  // Random 1-10

        // SELECT RANDOM TASK:
        int task_idx = rand() % num_tasks;
        snprintf(task_msg.mtext, MAX_TEXT, "%s", tasks[task_idx]);

        // SEND TASK:
        //
        // msgsnd(msgid, &task_msg, size, flags):
        //   - msgid: Queue ID from msgget()
        //   - &task_msg: Pointer to message
        //   - sizeof(task_msg) - sizeof(long): Message size (excluding mtype)
        //   - 0: Flags (0 = block if queue full)
        //
        // WHAT HAPPENS:
        //   - Message is copied to kernel message queue
        //   - Any worker can receive it
        //   - First worker to call msgrcv() gets it
        msgsnd(msgid, &task_msg, sizeof(task_msg) - sizeof(long), 0);

        printf("Sent task #%d: %s (Difficulty: %d/10)\n",
               task_msg.task_id, task_msg.mtext, task_msg.difficulty);

        sleep(1);  // Pace task distribution
    }

    printf("\nAll tasks sent. Waiting for results...\n\n");

    // STEP 4: RECEIVE RESULTS
    // Workers will process tasks and send back results
    for (int i = 1; i <= 5; i++) {
        // RECEIVE RESULT MESSAGE:
        //
        // msgrcv(msgid, &result_msg, size, type, flags):
        //   - msgid: Queue ID
        //   - &result_msg: Buffer for received message
        //   - size: Maximum message size
        //   - RESULT_MSG: Only receive messages of type 2 (results)
        //   - 0: Flags (0 = block until message available)
        //
        // SELECTIVE RECEIVING:
        //   - type > 0: Receive first message of that type
        //   - type = 0: Receive first message of any type
        //   - type < 0: Receive first message with type <= |type|
        //
        // BLOCKING:
        //   - Blocks until a result message is available
        //   - Workers are processing tasks in parallel
        //   - As each worker finishes, sends result
        msgrcv(msgid, &result_msg, sizeof(result_msg) - sizeof(long),
               RESULT_MSG, 0);

        printf("Received result for task #%d (Difficulty: %d/10):\n%s\n\n",
               result_msg.task_id, result_msg.difficulty, result_msg.mtext);
    }

    // STEP 5: CLEANUP MESSAGE QUEUE
    //
    // msgctl(msgid, IPC_RMID, NULL):
    //   - msgid: Queue to remove
    //   - IPC_RMID: Remove immediately
    //   - NULL: No additional data needed
    //
    // WHY REMOVE?
    //   - Message queues persist in kernel
    //   - Would remain after program exits
    //   - Check with: ipcs -q
    //   - Remove with: ipcrm -q <msgid>
    msgctl(msgid, IPC_RMID, NULL);

    printf("All tasks completed. Message queue removed.\n");

    return 0;
}

// EXPECTED OUTPUT:
// Task Distribution System
// ------------------------
// Sent task #1: Find all prime numbers below 500 (Difficulty: 7/10)
// Sent task #2: Calculate the factorial of 12 (Difficulty: 3/10)
// ...
// All tasks sent. Waiting for results...
//
// Received result for task #1 (Difficulty: 7/10):
// Task completed by worker 12345.
// Task was: Find all prime numbers below 500
// Time taken: 7 seconds
// ...

// MESSAGE QUEUE vs OTHER IPC:
//
// ADVANTAGES:
//   - Messages have types (selective receiving)
//   - Structured data (not just byte streams)
//   - Kernel manages queue (no shared memory sync needed)
//   - Multiple producers/consumers easily
//
// DISADVANTAGES:
//   - System V API is older, less elegant
//   - Size limits (check with ipcs -l)
//   - Persists in kernel (must explicitly remove)
//   - Less portable than POSIX alternatives

// WORK DISTRIBUTION PATTERN:
//
// 1. Distributor creates tasks
// 2. Distributor sends tasks to queue (type 1)
// 3. Multiple workers read tasks (type 1)
// 4. First available worker gets each task
// 5. Worker processes task
// 6. Worker sends result to queue (type 2)
// 7. Distributor reads results (type 2)
// 8. Automatic load balancing!

// TRY IT:
// 1. Compile: gcc task_distributor.c -o distributor
// 2. Compile: gcc worker.c -o worker
// 3. Run distributor: ./distributor &
// 4. Run workers: ./worker & ./worker & ./worker &
// 5. Watch tasks distributed and results collected
// 6. EXPERIMENT: Run different numbers of workers
// 7. CHALLENGE: Add priority tasks (use different message types)
