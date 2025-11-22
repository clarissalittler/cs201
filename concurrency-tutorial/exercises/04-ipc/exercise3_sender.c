/*
 * Exercise 3: Message Queue Chat - Sender
 *
 * Objective: Create a message sender using POSIX message queues that
 * supports priority-based messaging.
 *
 * Requirements:
 * - Create a POSIX message queue
 * - Allow user to send messages with different priorities
 * - Support priority levels: 0 (low), 1 (medium), 2 (high)
 * - Send "exit" message to signal shutdown
 * - Clean up queue on exit
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
    // TODO: Declare message queue descriptor
    // mqd_t mq;

    // TODO: Set up queue attributes
    // struct mq_attr attr;
    // attr.mq_flags = 0;
    // attr.mq_maxmsg = 10;
    // attr.mq_msgsize = MAX_SIZE;
    // attr.mq_curmsgs = 0;

    // TODO: Open/create the message queue
    // mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    // Check for errors

    printf("Sender: Message queue created.\n");
    printf("Enter messages with priority (format: priority message)\n");
    printf("Priority levels: 0=low, 1=medium, 2=high\n");
    printf("Type 'exit' to quit.\n\n");

    // TODO: Implement message sending loop
    // while(1) {
    //     - Declare buffer and priority variable
    //     - Prompt user for priority (0-2)
    //     - Read priority
    //     - Clear input buffer if needed
    //     - Prompt user for message
    //     - Read message with fgets()
    //     - Remove newline character
    //     - Send message with mq_send(mq, buffer, strlen(buffer)+1, priority)
    //     - If message is "exit", break
    // }

    // TODO: Clean up
    // - Close message queue with mq_close()

    printf("Sender: Queue closed.\n");
    return 0;
}
