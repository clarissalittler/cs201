/*
 * Exercise 3: Message Queue Chat - Receiver
 *
 * Objective: Create a message receiver using POSIX message queues that
 * receives messages in priority order.
 *
 * Requirements:
 * - Open existing message queue
 * - Receive messages (automatically in priority order)
 * - Display received messages with their priority
 * - Exit when "exit" message is received
 * - Unlink (remove) the queue on exit
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
    // TODO: Declare message queue descriptor and buffer
    // mqd_t mq;
    // char buffer[MAX_SIZE + 1];
    // unsigned int priority;
    // ssize_t bytes_read;

    // TODO: Open the message queue for reading
    // mq = mq_open(QUEUE_NAME, O_RDONLY);
    // Check for errors

    printf("Receiver: Waiting for messages...\n");
    printf("Messages will be received in priority order (high to low).\n\n");

    // TODO: Implement message receiving loop
    // while(1) {
    //     - Receive message with mq_receive(mq, buffer, MAX_SIZE, &priority)
    //     - If bytes_read >= 0:
    //         * Null-terminate the string
    //         * Print message with priority
    //         * Check if message is "exit", break if so
    //     - Handle errors
    // }

    // TODO: Clean up
    // - Close the queue with mq_close()
    // - Unlink the queue with mq_unlink()

    printf("Receiver: Message queue closed and unlinked.\n");
    return 0;
}
