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

// Simulates work by sleeping based on task difficulty
void do_work(int difficulty, const char* task, char* result) {
    printf("Working on task: %s (Difficulty: %d/10)\n", task, difficulty);
    
    // Simulate work by sleeping (difficulty determines time)
    sleep(difficulty);
    
    // Generate a simple result
    snprintf(result, MAX_TEXT, "Task completed by worker %d.\nTask was: %s\nTime taken: %d seconds", 
             getpid(), task, difficulty);
}

int main() {
    key_t key;
    int msgid;
    struct message task_msg, result_msg;
    
    // Generate the same key as the distributor
    key = ftok(".", PROJECT_ID);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    
    // Access the message queue (or create it if it doesn't exist)
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    
    printf("Worker %d started. Waiting for tasks...\n", getpid());
    
    // Seed random for this worker
    srand(getpid());
    
    // Worker will process 2 tasks
    for (int i = 0; i < 2; i++) {
        // Receive a task message
        if (msgrcv(msgid, &task_msg, sizeof(task_msg) - sizeof(long), 
                   TASK_MSG, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        
        printf("\nReceived task #%d\n", task_msg.task_id);
        
        // Process the task
        char result[MAX_TEXT];
        do_work(task_msg.difficulty, task_msg.mtext, result);
        
        // Prepare result message
        result_msg.mtype = RESULT_MSG;
        result_msg.task_id = task_msg.task_id;
        result_msg.difficulty = task_msg.difficulty;
        snprintf(result_msg.mtext, MAX_TEXT, "%s", result);
        
        // Send the result
        if (msgsnd(msgid, &result_msg, sizeof(result_msg) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
        
        printf("Sent result for task #%d\n", task_msg.task_id);
    }
    
    printf("Worker %d finished processing tasks.\n", getpid());
    
    return 0;
}
