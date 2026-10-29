// Shared memory supplies storage; a process-shared semaphore protects updates.
#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

struct shared_state {
    sem_t lock; 
    int counter;
};

void increment(struct shared_state* state) {
    for (int i = 0; i < ITERATIONS; ++i) {
        sem_wait(&state->lock);
        state->counter++;
        sem_post(&state->lock);
    }
}

int main(void) {
    struct shared_state* state = mmap(NULL, sizeof(struct shared_state),
                                      PROT_READ | PROT_WRITE,
                                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    state->counter = 0;
    // pshared=1: shared between processes. Initial value=1: one at a time.
    sem_init(&state->lock, 1, 1);

    pid_t pid = fork();
    
    if (!pid) {
        increment(state);
        return 0;
    }
    increment(state);
    wait(NULL);

    printf("counter = %d; expected %d\n", state->counter, 2 * 10000);
    sem_destroy(&state->lock);
    munmap(state, sizeof(struct shared_state));
    return 0;
}
