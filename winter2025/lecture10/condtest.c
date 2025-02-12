#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 20

int buffer[BUFFER_SIZE];
int count = 0;      // Number of items in the buffer.
int in = 0;         // Index for the next produced item.
int out = 0;        // Index for the next consumed item.

pthread_mutex_t mutex;
pthread_cond_t cond_not_empty;
pthread_cond_t cond_not_full;

void* producer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        // Simulate production time.
        sleep(1);

        pthread_mutex_lock(&mutex);

        // Wait until there is space in the buffer.
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&cond_not_full, &mutex);
        }

        // Produce an item (here, simply the loop index).
        int item = i;
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;
        printf("Producer %d produced item %d (buffer count: %d)\n", id, item, count);

        // Signal that the buffer is not empty.
        pthread_cond_signal(&cond_not_empty);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* consumer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex);

        // Wait until there is at least one item in the buffer.
        while (count == 0) {
            pthread_cond_wait(&cond_not_empty, &mutex);
        }

        // Consume an item.
        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        printf("Consumer %d consumed item %d (buffer count: %d)\n", id, item, count);

        // Signal that the buffer is not full.
        pthread_cond_signal(&cond_not_full);
        pthread_mutex_unlock(&mutex);

        // Simulate consumption time.
        sleep(2);
    }
    return NULL;
}

int main() {
  pthread_mutex_init(&mutex,NULL);
  pthread_cond_init(&cond_not_empty,NULL);
  pthread_cond_init(&cond_not_full,NULL);
  
  pthread_t prod_thread1, cons_thread1, prod_thread2, cons_thread2;
  int prod_id1 = 1, cons_id1 = 1;
  int prod_id2 = 2, cons_id2 = 2;
  
  pthread_create(&prod_thread1, NULL, producer, &prod_id1);
  pthread_create(&cons_thread1, NULL, consumer, &cons_id1);
  pthread_create(&prod_thread2, NULL, producer, &prod_id2);
  pthread_create(&cons_thread2, NULL, consumer, &cons_id2);

  
  pthread_join(prod_thread1, NULL);
  pthread_join(cons_thread1, NULL);
  
  // Cleanup.
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond_not_empty);
  pthread_cond_destroy(&cond_not_full);
  
  return 0;
}
