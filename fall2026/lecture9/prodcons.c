#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// the classic condition-variable program: a bounded buffer with a
// producer and a consumer. this is the pattern inside every work
// queue, thread pool, and pipe you have ever used -- when we said a
// pipe's read() "blocks until there's data", THIS is roughly the
// machinery doing the blocking.
//
// two condition variables, one per question a thread can be stuck on:
//   notFull  -- "is there room to put something in?"  (producer waits)
//   notEmpty -- "is there anything to take out?"      (consumer waits)
// both share the one mutex that protects the buffer itself.
//
// the producer here is fast and the consumer is slow, so you'll see
// the buffer fill up and the producer get blocked -- backpressure,
// for free

#define BUFSIZE 8
#define ITEMS   30

int buffer[BUFSIZE];
int count = 0;   // how many items are in the buffer right now
int in    = 0;   // next slot to write
int out   = 0;   // next slot to read

pthread_mutex_t mut      = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  notFull  = PTHREAD_COND_INITIALIZER;
pthread_cond_t  notEmpty = PTHREAD_COND_INITIALIZER;

void* producer(void* p){
  (void)p;
  for(int i = 0; i < ITEMS; i++){
    pthread_mutex_lock(&mut);
    while(count == BUFSIZE){          // buffer full -> sleep until not
      printf("producer: buffer full, waiting...\n");
      pthread_cond_wait(&notFull, &mut);
    }
    buffer[in] = i;
    in = (in + 1) % BUFSIZE;          // wrap around: it's a ring
    count++;
    printf("producer: put %2d  (count now %d)\n", i, count);
    pthread_cond_signal(&notEmpty);   // maybe the consumer was waiting
    pthread_mutex_unlock(&mut);
  }
  return NULL;
}

void* consumer(void* p){
  (void)p;
  for(int i = 0; i < ITEMS; i++){
    pthread_mutex_lock(&mut);
    while(count == 0){                // buffer empty -> sleep until not
      pthread_cond_wait(&notEmpty, &mut);
    }
    int x = buffer[out];
    out = (out + 1) % BUFSIZE;
    count--;
    printf("consumer: got %2d  (count now %d)\n", x, count);
    pthread_cond_signal(&notFull);    // maybe the producer was waiting
    pthread_mutex_unlock(&mut);

    usleep(100000);                   // pretend consuming takes work
  }
  return NULL;
}

int main(){
  pthread_t prod, cons;
  pthread_create(&prod, NULL, producer, NULL);
  pthread_create(&cons, NULL, consumer, NULL);
  pthread_join(prod, NULL);
  pthread_join(cons, NULL);
  return 0;
}
