#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* Number of philosophers for the dining philosophers problem */
#define NUM_PHILOSOPHERS 5

/* 
 * DEADLOCK_SOLUTION controls which solution is demonstrated:
 * 0 = No solution (will deadlock)
 * 1 = Resource hierarchy solution (ordered lock acquisition)
 * 2 = Resource allocation solution (limit concurrent philosophers)
 */
#define DEADLOCK_SOLUTION 1

/* Mutexes representing chopsticks/resources */
pthread_mutex_t chopsticks[NUM_PHILOSOPHERS];

/* Additional mutex for resource allocation solution */
pthread_mutex_t table_mutex;

/* Thread function prototypes for different solutions */
void* philosopher_no_solution(void* arg);
void* philosopher_hierarchy_solution(void* arg);
void* philosopher_allocation_solution(void* arg);

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];
    int i;
    
    printf("Deadlock Demonstration: The Dining Philosophers Problem\n\n");
    
    /* Initialize all the chopstick mutexes */
    for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&chopsticks[i], NULL);
    }
    
    /* Initialize the table mutex for the resource allocation solution */
    pthread_mutex_init(&table_mutex, NULL);
    
    /* Display which solution we're using */
    if (DEADLOCK_SOLUTION == 0) {
        printf("Using no deadlock prevention (WILL DEADLOCK!)\n");
    } else if (DEADLOCK_SOLUTION == 1) {
        printf("Using hierarchical resource ordering solution\n");
    } else if (DEADLOCK_SOLUTION == 2) {
        printf("Using resource allocation control solution\n");
    }
    
    /* Create the philosopher threads */
    for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        ids[i] = i;
        
        /* Choose the appropriate thread function based on solution approach */
        void* (*thread_func)(void*) = NULL;
        
        if (DEADLOCK_SOLUTION == 0) {
            thread_func = philosopher_no_solution;
        } else if (DEADLOCK_SOLUTION == 1) {
            thread_func = philosopher_hierarchy_solution;
        } else if (DEADLOCK_SOLUTION == 2) {
            thread_func = philosopher_allocation_solution;
        }
        
        if (pthread_create(&philosophers[i], NULL, thread_func, &ids[i]) != 0) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }
    
    /* 
     * If we're using the deadlock-prone version, set a timeout to interrupt
     * the program after the deadlock has likely occurred
     */
    if (DEADLOCK_SOLUTION == 0) {
        printf("\nNOTE: Program will automatically terminate after 10 seconds "
               "since deadlock is expected.\n\n");
        sleep(10);
        printf("\n\nDEADLOCK DETECTED! Program is in a deadlocked state.\n");
        printf("All philosophers are waiting for resources held by other philosophers.\n");
        printf("Terminating program...\n");
        
        /* Clean up and exit */
        for (i = 0; i < NUM_PHILOSOPHERS; i++) {
            pthread_cancel(philosophers[i]);
        }
        
        for (i = 0; i < NUM_PHILOSOPHERS; i++) {
            pthread_join(philosophers[i], NULL);
            pthread_mutex_destroy(&chopsticks[i]);
        }
        
        pthread_mutex_destroy(&table_mutex);
        return EXIT_SUCCESS;
    }
    
    /* Wait for all philosopher threads to complete */
    for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }
    
    /* Clean up the mutexes */
    for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&chopsticks[i]);
    }
    pthread_mutex_destroy(&table_mutex);
    
    printf("\nAll philosophers have completed their meals without deadlock.\n");
    
    return EXIT_SUCCESS;
}

/**
 * Philosopher thread function with NO deadlock prevention.
 * This version WILL deadlock if all philosophers pick up their left chopstick
 * simultaneously, then wait for their right chopstick.
 * 
 * @param arg Pointer to the philosopher's ID
 * @return Always NULL
 */
void* philosopher_no_solution(void* arg) {
    int id = *((int*)arg);
    int left_chopstick = id;
    int right_chopstick = (id + 1) % NUM_PHILOSOPHERS;
    
    printf("Philosopher %d is thinking...\n", id);
    
    /* Simulate thinking time */
    usleep(rand() % 1000000);  /* 0-1 second */
    
    printf("Philosopher %d is hungry and trying to pick up chopsticks...\n", id);
    
    /* Pick up left chopstick */
    printf("Philosopher %d is picking up left chopstick (%d)...\n", id, left_chopstick);
    pthread_mutex_lock(&chopsticks[left_chopstick]);
    printf("Philosopher %d picked up left chopstick (%d)!\n", id, left_chopstick);
    
    /* Slight delay to make deadlock more likely */
    usleep(100000);  /* 100ms */
    
    /* Pick up right chopstick */
    printf("Philosopher %d is picking up right chopstick (%d)...\n", id, right_chopstick);
    pthread_mutex_lock(&chopsticks[right_chopstick]);
    printf("Philosopher %d picked up right chopstick (%d)!\n", id, right_chopstick);
    
    /* Eat */
    printf("Philosopher %d is eating...\n", id);
    usleep(rand() % 1000000);  /* 0-1 second */
    
    /* Put down chopsticks */
    pthread_mutex_unlock(&chopsticks[left_chopstick]);
    printf("Philosopher %d put down left chopstick (%d).\n", id, left_chopstick);
    
    pthread_mutex_unlock(&chopsticks[right_chopstick]);
    printf("Philosopher %d put down right chopstick (%d).\n", id, right_chopstick);
    
    printf("Philosopher %d finished eating.\n", id);
    
    return NULL;
}

/**
 * Philosopher thread function with HIERARCHICAL ORDERING solution.
 * This solution prevents deadlock by ensuring all philosophers acquire 
 * chopsticks in the same global order (lowest index first).
 * 
 * @param arg Pointer to the philosopher's ID
 * @return Always NULL
 */
void* philosopher_hierarchy_solution(void* arg) {
    int id = *((int*)arg);
    int left_chopstick = id;
    int right_chopstick = (id + 1) % NUM_PHILOSOPHERS;
    int first_chopstick, second_chopstick;
    
    printf("Philosopher %d is thinking...\n", id);
    
    /* Simulate thinking time */
    usleep(rand() % 1000000);  /* 0-1 second */
    
    printf("Philosopher %d is hungry...\n", id);
    
    /* 
     * SOLUTION: Always pick up chopsticks in ascending index order
     * This prevents the circular wait condition needed for deadlock
     */
    if (left_chopstick < right_chopstick) {
        first_chopstick = left_chopstick;
        second_chopstick = right_chopstick;
    } else {
        first_chopstick = right_chopstick;
        second_chopstick = left_chopstick;
    }
    
    /* Pick up first chopstick (lower index) */
    printf("Philosopher %d is picking up chopstick %d...\n", id, first_chopstick);
    pthread_mutex_lock(&chopsticks[first_chopstick]);
    
    /* Slight delay (doesn't affect correctness) */
    usleep(100000);  /* 100ms */
    
    /* Pick up second chopstick (higher index) */
    printf("Philosopher %d is picking up chopstick %d...\n", id, second_chopstick);
    pthread_mutex_lock(&chopsticks[second_chopstick]);
    
    /* Eat */
    printf("Philosopher %d is eating...\n", id);
    usleep(rand() % 1000000);  /* 0-1 second */
    
    /* Put down chopsticks in reverse order (not strictly necessary) */
    pthread_mutex_unlock(&chopsticks[second_chopstick]);
    printf("Philosopher %d put down chopstick %d.\n", id, second_chopstick);
    
    pthread_mutex_unlock(&chopsticks[first_chopstick]);
    printf("Philosopher %d put down chopstick %d.\n", id, first_chopstick);
    
    printf("Philosopher %d finished eating and is thinking again.\n", id);
    
    return NULL;
}

/**
 * Philosopher thread function with RESOURCE ALLOCATION solution.
 * This solution prevents deadlock by limiting the number of philosophers
 * that can try to pick up chopsticks at the same time.
 * 
 * @param arg Pointer to the philosopher's ID
 * @return Always NULL
 */
void* philosopher_allocation_solution(void* arg) {
    int id = *((int*)arg);
    int left_chopstick = id;
    int right_chopstick = (id + 1) % NUM_PHILOSOPHERS;
    int i;
    
    /* Repeat the thinking-eating cycle multiple times */
    for (i = 0; i < 3; i++) {
        printf("Philosopher %d is thinking (cycle %d)...\n", id, i+1);
        
        /* Simulate thinking time */
        usleep(rand() % 1000000);  /* 0-1 second */
        
        printf("Philosopher %d is hungry (cycle %d)...\n", id, i+1);
        
        /* 
         * SOLUTION: Control access to the table
         * Only allow NUM_PHILOSOPHERS-1 philosophers to sit at the table,
         * which guarantees at least one philosopher can pick up both chopsticks
         */
        pthread_mutex_lock(&table_mutex);
        printf("Philosopher %d is at the table (cycle %d).\n", id, i+1);
        
        /* Pick up left chopstick */
        pthread_mutex_lock(&chopsticks[left_chopstick]);
        printf("Philosopher %d picked up left chopstick (%d).\n", id, left_chopstick);
        
        /* Pick up right chopstick */
        pthread_mutex_lock(&chopsticks[right_chopstick]);
        printf("Philosopher %d picked up right chopstick (%d).\n", id, right_chopstick);
        
        /* Release the table lock so other philosophers can sit */
        pthread_mutex_unlock(&table_mutex);
        
        /* Eat */
        printf("Philosopher %d is eating (cycle %d)...\n", id, i+1);
        usleep(rand() % 1000000);  /* 0-1 second */
        
        /* Put down chopsticks */
        pthread_mutex_unlock(&chopsticks[right_chopstick]);
        printf("Philosopher %d put down right chopstick (%d).\n", id, right_chopstick);
        
        pthread_mutex_unlock(&chopsticks[left_chopstick]);
        printf("Philosopher %d put down left chopstick (%d).\n", id, left_chopstick);
        
        printf("Philosopher %d finished eating (cycle %d).\n", id, i+1);
    }
    
    printf("Philosopher %d has finished all eating cycles.\n", id);
    
    return NULL;
}