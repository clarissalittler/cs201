// mq_receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>    // For O_* constants
#include <sys/stat.h> // For mode constants

#define queueName "/ourQueue"
#define maxSize 1024
#define msgStop "exit"

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char buffer[maxSize + 1];
    ssize_t bytes_read;

    // Open the message queue for reading
    mq = mq_open(queueName, O_RDONLY);
    printf("Receiver: Waiting for messages...\n");

    while (1) {
      // Receive the message
      bytes_read = mq_receive(mq, buffer, maxSize, NULL);
      if (bytes_read >= 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the string
        printf("Receiver: Received message: %s\n", buffer);

        // Exit condition
        if (strcmp(buffer, msgStop) == 0) {
          break;
        }
      } else {
        perror("Receiver: mq_receive");
      }
    }

    // Close the queue
    mq_close(mq);

    // Unlink the queue (remove it)
    mq_unlink(queueName);

    printf("Receiver: Message queue closed and unlinked.\n");
    return 0;
}
