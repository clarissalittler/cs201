#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#define PROJECT_ID 123
#define TASK_MSG 1
#define RESULT_MSG 2
#define MAX_TEXT 512

// Message structure
struct message {
    long mtype;             // Message type
    int task_id;            // Task ID
    int difficulty;         // Task difficulty (1-10)
    char mtext[MAX_TEXT];   // Task description or result
};

int main() {
    key_t key;
    int msgid;
    struct message task_msg, result_msg;
    
    // Generate a key for the message queue
    key = ftok(".", PROJECT_ID);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    
    // Create a message queue
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    
    printf("Task Distribution System\n");
    printf("------------------------\n");
    
    // Seed random number generator
    srand(time(NULL));
    
    // Array of task descriptions
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
    
    // Distribute 5 tasks
    for (int i = 1; i <= 5; i++) {
        // Prepare a task message
        task_msg.mtype = TASK_MSG;
        task_msg.task_id = i;
        task_msg.difficulty = rand() % 10 + 1;  // Random difficulty 1-10
        
        // Select a random task
        int task_idx = rand() % num_tasks;
        snprintf(task_msg.mtext, MAX_TEXT, "%s", tasks[task_idx]);
        
        // Send the task
        if (msgsnd(msgid, &task_msg, sizeof(task_msg) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
        
        printf("Sent task #%d: %s (Difficulty: %d/10)\n", 
               task_msg.task_id, task_msg.mtext, task_msg.difficulty);
        
        // Sleep for a bit
        sleep(1);
    }
    
    printf("\nAll tasks sent. Waiting for results...\n\n");
    
    // Wait for and receive all results
    for (int i = 1; i <= 5; i++) {
        // Receive the result message
        if (msgrcv(msgid, &result_msg, sizeof(result_msg) - sizeof(long), 
                   RESULT_MSG, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        
        printf("Received result for task #%d (Difficulty: %d/10):\n%s\n\n", 
               result_msg.task_id, result_msg.difficulty, result_msg.mtext);
    }
    
    // Clean up the message queue
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }
    
    printf("All tasks completed. Message queue removed.\n");
    
    return 0;
}
