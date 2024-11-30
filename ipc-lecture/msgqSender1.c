#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>

#define queueName "/ourQueue"
#define maxSize 1024
#define msgStop "exit"

int main(){
  mqd_t mq;
  struct mq_attr attr;
  char buffer[maxSize];

  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = maxSize;
  attr.mq_curmsgs = 0;

  mq = mq_open(queueName, O_CREAT | O_WRONLY, 0644, &attr);
  printf("Sender: Enter messages to send to the queue. Type 'exit' to end\n");

  while(1){
    printf("Enter message: ");
    fgets(buffer,maxSize,stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    mq_send(mq, buffer, strlen(buffer)+1,0);
    if(strcmp(buffer, msgStop) == 0){
      break;
    }

  }

  mq_close(mq);

  printf("Sender: queue closed\n");
  return 0;
}
