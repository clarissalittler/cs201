/*
 * Exercise 3 Solution: Message Queue Chat - Receiver
 *
 * This program receives messages from a POSIX message queue.
 * Messages are automatically received in priority order (highest first).
 *
 * Key concepts:
 * - Opening existing POSIX message queues
 * - Receiving messages with mq_receive()
 * - Automatic priority-based delivery
 * - Unlinking (removing) queues with mq_unlink()
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
    char buffer[MAX_SIZE + 1];
    unsigned int priority;
    ssize_t bytes_read;

    // Open the message queue for reading
    // O_RDONLY: Open for reading only
    // Note: The queue should already exist (created by sender)
    mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        fprintf(stderr, "Receiver: Make sure sender has created the queue first.\n");
        return 1;
    }

    printf("Receiver: Message queue opened successfully.\n");
    printf("========================================\n");
    printf("Priority-based Message Queue System\n");
    printf("========================================\n");
    printf("Waiting for messages...\n");
    printf("Messages will be displayed in priority order (high to low).\n\n");

    // Main message receiving loop
    int message_count = 0;
    while (1) {
        // Receive message from queue
        // mq_receive() blocks until a message is available
        // Messages are automatically retrieved in priority order
        // The priority parameter receives the priority of the message
        bytes_read = mq_receive(mq, buffer, MAX_SIZE, &priority);

        if (bytes_read >= 0) {
            // Null-terminate the received string
            buffer[bytes_read] = '\0';

            // Display the message with its priority
            message_count++;
            const char* priority_str;
            switch (priority) {
                case 0:  priority_str = "LOW"; break;
                case 1:  priority_str = "MEDIUM"; break;
                case 2:  priority_str = "HIGH"; break;
                default: priority_str = "UNKNOWN"; break;
            }

            printf("[Message #%d] Priority: %s (%u)\n",
                   message_count, priority_str, priority);
            printf("Content: %s\n\n", buffer);

            // Check for exit message
            if (strcmp(buffer, MSG_STOP) == 0) {
                printf("Receiver: Exit message received. Shutting down...\n");
                break;
            }
        }
        else {
            // Error receiving message
            perror("mq_receive");
            break;
        }
    }

    // Close the message queue
    if (mq_close(mq) == -1) {
        perror("mq_close");
        return 1;
    }

    // Unlink (remove) the message queue
    // This removes the queue from the system
    if (mq_unlink(QUEUE_NAME) == -1) {
        perror("mq_unlink");
        return 1;
    }

    printf("Receiver: Message queue closed and unlinked.\n");
    printf("Total messages received: %d\n", message_count);

    return 0;
}
