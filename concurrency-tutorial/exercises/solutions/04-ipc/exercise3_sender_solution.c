/*
 * Exercise 3 Solution: Message Queue Chat - Sender
 *
 * This program demonstrates POSIX message queues with priority-based delivery.
 * Messages with higher priority are delivered first.
 *
 * Key concepts:
 * - Creating POSIX message queues with mq_open()
 * - Setting queue attributes
 * - Sending messages with priorities using mq_send()
 * - Priority levels (0-31, with higher numbers = higher priority)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>

#define QUEUE_NAME "/ipc_exercise_queue"
#define MAX_SIZE 1024
#define MSG_STOP "exit"

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE];
    unsigned int priority;

    // Set up queue attributes
    attr.mq_flags = 0;              // Blocking queue
    attr.mq_maxmsg = 10;            // Maximum number of messages in queue
    attr.mq_msgsize = MAX_SIZE;     // Maximum message size
    attr.mq_curmsgs = 0;            // Current number of messages (read-only)

    // Open/create the message queue
    // O_CREAT: Create if doesn't exist
    // O_WRONLY: Open for writing only
    // 0644: Permissions (owner read/write, group/others read)
    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    printf("Sender: Message queue created successfully.\n");
    printf("========================================\n");
    printf("Priority-based Message Queue System\n");
    printf("========================================\n");
    printf("Priority levels:\n");
    printf("  0 = Low priority\n");
    printf("  1 = Medium priority\n");
    printf("  2 = High priority\n");
    printf("\nMessages will be delivered to receiver in priority order.\n");
    printf("Type 'exit' as your message to quit.\n\n");

    // Main message sending loop
    while (1) {
        // Get priority from user
        printf("Enter priority (0-2): ");
        if (scanf("%u", &priority) != 1) {
            printf("Invalid priority. Please enter 0, 1, or 2.\n");
            // Clear input buffer
            while (getchar() != '\n');
            continue;
        }

        // Validate priority
        if (priority > 2) {
            printf("Priority must be 0, 1, or 2. Using 0 (low).\n");
            priority = 0;
        }

        // Clear input buffer after reading number
        while (getchar() != '\n');

        // Get message from user
        printf("Enter message: ");
        if (fgets(buffer, MAX_SIZE, stdin) == NULL) {
            break;
        }

        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = '\0';

        // Send message to queue with priority
        // Higher priority messages are delivered first
        if (mq_send(mq, buffer, strlen(buffer) + 1, priority) == -1) {
            perror("mq_send");
            break;
        }

        printf("Sent: \"%s\" with priority %u\n\n", buffer, priority);

        // Check for exit message
        if (strcmp(buffer, MSG_STOP) == 0) {
            printf("Sender: Exit message sent. Closing...\n");
            break;
        }
    }

    // Close the message queue
    if (mq_close(mq) == -1) {
        perror("mq_close");
        return 1;
    }

    printf("Sender: Queue closed successfully.\n");

    return 0;
}
