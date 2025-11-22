/*
 * Exercise 4: Dining Philosophers Problem
 * Difficulty: 🔴 Advanced
 *
 * OBJECTIVE:
 * Implement the classic dining philosophers problem with deadlock prevention.
 * Five philosophers sit at a round table with five forks. Each philosopher
 * needs two forks to eat but must avoid deadlock and starvation.
 *
 * REQUIREMENTS:
 * 1. Create 5 philosopher threads and 5 fork mutexes
 * 2. Each philosopher alternates between thinking and eating
 * 3. To eat, a philosopher needs BOTH adjacent forks (left and right)
 * 4. Implement deadlock prevention using resource ordering
 * 5. Each philosopher should eat exactly 5 times
 * 6. Print detailed state changes (thinking, hungry, eating, finished)
 * 7. Program must complete without deadlock
 *
 * THE DEADLOCK PROBLEM:
 * If all philosophers pick up their left fork simultaneously, then wait for
 * their right fork, deadlock occurs - nobody can proceed!
 *
 * DEADLOCK PREVENTION STRATEGY:
 * Use resource ordering: Always acquire forks in ascending order by fork ID.
 * This breaks the circular wait condition.
 * - If left_fork < right_fork: lock left, then right
 * - If left_fork > right_fork: lock right, then left
 *
 * SAMPLE OUTPUT:
 * Starting Dining Philosophers simulation...
 * 5 philosophers, 5 forks, 5 meals each
 *
 * Philosopher 0: Thinking...
 * Philosopher 1: Thinking...
 * ...
 * Philosopher 0: Hungry, trying to pick up forks 0 and 1
 * Philosopher 0: Picked up fork 0
 * Philosopher 0: Picked up fork 1
 * Philosopher 0: Eating (meal 1/5)
 * Philosopher 2: Hungry, trying to pick up forks 2 and 3
 * Philosopher 0: Finished eating, putting down forks
 * Philosopher 1: Hungry, trying to pick up forks 1 and 2
 * ...
 *
 * ===== SIMULATION COMPLETE =====
 * Philosopher 0: Ate 5 meals
 * Philosopher 1: Ate 5 meals
 * ...
 * Total meals: 25
 * SUCCESS: No deadlock!
 *
 * FORK LAYOUT (circular table):
 *     Ph0
 *   F0   F1
 * Ph4     Ph1
 *   F4   F2
 *     Ph3
 *      F3
 *     Ph2
 *
 * Each philosopher Ph(i) needs forks F(i) and F((i+1)%5)
 *
 * HINTS:
 * - Create fork mutexes: pthread_mutex_t forks[5]
 * - Left fork for philosopher i: forks[i]
 * - Right fork for philosopher i: forks[(i + 1) % 5]
 * - To prevent deadlock, pick lower-numbered fork first:
 *   int first = min(left_fork_id, right_fork_id);
 *   int second = max(left_fork_id, right_fork_id);
 *   pthread_mutex_lock(&forks[first]);
 *   pthread_mutex_lock(&forks[second]);
 * - Use usleep() to simulate thinking and eating time
 * - Track meals eaten in a global array: int meals_eaten[5]
 *
 * LEARNING GOALS:
 * - Understand the dining philosophers problem
 * - Recognize deadlock conditions (mutual exclusion, hold and wait,
 *   no preemption, circular wait)
 * - Implement deadlock prevention via resource ordering
 * - Handle multiple resource acquisition safely
 * - Understand the tradeoff between concurrency and safety
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

// TODO: Define constants
// #define NUM_PHILOSOPHERS 5
// #define MEALS_PER_PHILOSOPHER 5

// TODO: Declare fork mutexes
// pthread_mutex_t forks[NUM_PHILOSOPHERS];

// TODO: Track meals eaten by each philosopher
// int meals_eaten[NUM_PHILOSOPHERS] = {0};

// TODO: Helper function to get minimum of two values
// int min(int a, int b) {
//     return (a < b) ? a : b;
// }

// TODO: Helper function to get maximum of two values
// int max(int a, int b) {
//     return (a > b) ? a : b;
// }

// TODO: Implement philosopher thread function
// void* philosopher(void* arg) {
//     int id = (int)(long)arg;
//     int left_fork = id;
//     int right_fork = (id + 1) % NUM_PHILOSOPHERS;
//
//     for (int meal = 0; meal < MEALS_PER_PHILOSOPHER; meal++) {
//         // TODO: Thinking phase
//         // printf("Philosopher %d: Thinking...\n", id);
//         // usleep(rand() % 100000);  // Think for random time
//
//         // TODO: Hungry - trying to acquire forks
//         // printf("Philosopher %d: Hungry, trying to pick up forks %d and %d\n",
//         //        id, left_fork, right_fork);
//
//         // TODO: Acquire forks in order to prevent deadlock
//         // Determine which fork to pick up first
//         // int first_fork = min(left_fork, right_fork);
//         // int second_fork = max(left_fork, right_fork);
//
//         // TODO: Lock first fork
//         // pthread_mutex_lock(&forks[first_fork]);
//         // printf("Philosopher %d: Picked up fork %d\n", id, first_fork);
//
//         // TODO: Lock second fork
//         // pthread_mutex_lock(&forks[second_fork]);
//         // printf("Philosopher %d: Picked up fork %d\n", id, second_fork);
//
//         // TODO: Eating phase
//         // printf("Philosopher %d: Eating (meal %d/%d)\n",
//         //        id, meal + 1, MEALS_PER_PHILOSOPHER);
//         // usleep(rand() % 50000);  // Eat for random time
//         // meals_eaten[id]++;
//
//         // TODO: Put down forks (release in any order, but good practice to reverse)
//         // pthread_mutex_unlock(&forks[second_fork]);
//         // pthread_mutex_unlock(&forks[first_fork]);
//         // printf("Philosopher %d: Finished eating, put down forks %d and %d\n",
//         //        id, first_fork, second_fork);
//     }
//
//     printf("Philosopher %d: Finished all meals\n", id);
//     return NULL;
// }

int main() {
    // TODO: Declare thread array
    // pthread_t philosophers[NUM_PHILOSOPHERS];

    printf("Starting Dining Philosophers simulation...\n");
    printf("%d philosophers, %d forks, %d meals each\n\n",
           5, 5, 3);  // TODO: Replace with constants after defining them

    // TODO: Initialize fork mutexes
    // for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    //     pthread_mutex_init(&forks[i], NULL);
    // }

    // TODO: Create philosopher threads
    // for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    //     pthread_create(&philosophers[i], NULL, philosopher, (void*)(long)i);
    // }

    // TODO: Wait for all philosophers to finish
    // for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    //     pthread_join(philosophers[i], NULL);
    // }

    // TODO: Print results
    // printf("\n===== SIMULATION COMPLETE =====\n");
    // int total_meals = 0;
    // for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    //     printf("Philosopher %d: Ate %d meals\n", i, meals_eaten[i]);
    //     total_meals += meals_eaten[i];
    // }
    // printf("Total meals: %d\n", total_meals);
    // printf("SUCCESS: No deadlock!\n");

    // TODO: Cleanup fork mutexes
    // for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    //     pthread_mutex_destroy(&forks[i]);
    // }

    printf("\nDeadlock prevention: Always acquire resources in the same order!\n");

    return 0;
}

/*
 * EXPLANATION OF DEADLOCK PREVENTION:
 *
 * The Four Conditions for Deadlock:
 * 1. Mutual Exclusion: Only one thread can hold a fork at a time ✓
 * 2. Hold and Wait: Philosophers hold one fork while waiting for another ✓
 * 3. No Preemption: Forks cannot be forcibly taken ✓
 * 4. Circular Wait: Philosopher 0 waits for 1, who waits for 2, ... who waits for 0
 *
 * Our Solution:
 * Break the circular wait by establishing a global ordering on forks.
 * Always acquire lower-numbered fork first, then higher-numbered fork.
 * This ensures no circular dependency chain can form.
 *
 * Alternative Solutions:
 * 1. Limit concurrent eaters (use semaphore to allow max 4 eating simultaneously)
 * 2. Odd-even strategy (odd philosophers pick left first, even pick right first)
 * 3. Waiter solution (central coordinator grants permission to pick up forks)
 * 4. Timeout and retry (detect deadlock via timeout, release and retry)
 */
