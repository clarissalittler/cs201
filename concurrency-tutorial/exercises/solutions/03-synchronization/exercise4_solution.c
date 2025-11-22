/*
 * SOLUTION: Exercise 4 - Dining Philosophers Problem
 *
 * This solution implements the classic dining philosophers problem with
 * deadlock prevention using resource ordering. It demonstrates how to
 * safely acquire multiple resources without creating circular dependencies.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_PHILOSOPHERS 5
#define MEALS_PER_PHILOSOPHER 5

// Fork mutexes (one per fork)
pthread_mutex_t forks[NUM_PHILOSOPHERS];

// Track meals eaten by each philosopher
int meals_eaten[NUM_PHILOSOPHERS] = {0};

/*
 * Helper function: return minimum of two integers
 */
int min(int a, int b) {
    return (a < b) ? a : b;
}

/*
 * Helper function: return maximum of two integers
 */
int max(int a, int b) {
    return (a > b) ? a : b;
}

/*
 * Philosopher thread function
 * Each philosopher alternates between thinking and eating
 */
void* philosopher(void* arg) {
    int id = (int)(long)arg;
    int left_fork = id;
    int right_fork = (id + 1) % NUM_PHILOSOPHERS;

    for (int meal = 0; meal < MEALS_PER_PHILOSOPHER; meal++) {
        // ========== THINKING PHASE ==========
        printf("Philosopher %d: Thinking...\n", id);
        usleep(rand() % 100000);  // Think for random time (0-100ms)

        // ========== HUNGRY PHASE ==========
        printf("Philosopher %d: Hungry, trying to pick up forks %d and %d\n",
               id, left_fork, right_fork);

        // DEADLOCK PREVENTION: Acquire forks in ascending order
        // This breaks the circular wait condition
        int first_fork = min(left_fork, right_fork);
        int second_fork = max(left_fork, right_fork);

        // Pick up first fork (lower numbered)
        pthread_mutex_lock(&forks[first_fork]);
        printf("Philosopher %d: Picked up fork %d\n", id, first_fork);

        // Small delay to make the problem more visible
        usleep(10);

        // Pick up second fork (higher numbered)
        pthread_mutex_lock(&forks[second_fork]);
        printf("Philosopher %d: Picked up fork %d\n", id, second_fork);

        // ========== EATING PHASE ==========
        printf("Philosopher %d: Eating (meal %d/%d)\n",
               id, meal + 1, MEALS_PER_PHILOSOPHER);
        usleep(rand() % 50000);  // Eat for random time (0-50ms)
        meals_eaten[id]++;

        // ========== FINISHED EATING ==========
        // Put down forks in reverse order (good practice, though not required)
        pthread_mutex_unlock(&forks[second_fork]);
        pthread_mutex_unlock(&forks[first_fork]);
        printf("Philosopher %d: Finished eating, put down forks %d and %d\n",
               id, first_fork, second_fork);
    }

    printf("Philosopher %d: Finished all %d meals\n", id, MEALS_PER_PHILOSOPHER);
    return NULL;
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];

    printf("Starting Dining Philosophers simulation...\n");
    printf("%d philosophers, %d forks, %d meals each\n",
           NUM_PHILOSOPHERS, NUM_PHILOSOPHERS, MEALS_PER_PHILOSOPHER);
    printf("Deadlock prevention: Resource ordering strategy\n\n");

    // Seed random number generator
    srand(time(NULL));

    // Initialize all fork mutexes
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    // Create philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_create(&philosophers[i], NULL, philosopher, (void*)(long)i);
    }

    // Wait for all philosophers to finish eating
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    // Print final results
    printf("\n===== SIMULATION COMPLETE =====\n");
    int total_meals = 0;
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        printf("Philosopher %d: Ate %d meals\n", i, meals_eaten[i]);
        total_meals += meals_eaten[i];
    }
    printf("Total meals: %d\n", total_meals);
    printf("Expected: %d\n", NUM_PHILOSOPHERS * MEALS_PER_PHILOSOPHER);

    if (total_meals == NUM_PHILOSOPHERS * MEALS_PER_PHILOSOPHER) {
        printf("SUCCESS: No deadlock! All philosophers ate.\n");
    } else {
        printf("ERROR: Some meals were missed!\n");
    }

    // Destroy all fork mutexes
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks[i]);
    }

    printf("\nDeadlock prevention: Always acquire resources in the same order!\n");

    return 0;
}

/*
 * EXPLANATION OF DINING PHILOSOPHERS PROBLEM:
 *
 * 1. THE CLASSIC PROBLEM:
 *    - 5 philosophers sit at a round table
 *    - 5 forks placed between them (1 between each pair)
 *    - Each philosopher needs 2 forks to eat
 *    - Philosopher i needs fork i and fork (i+1)%5
 *    - Goal: Allow all to eat without deadlock
 *
 * 2. THE DEADLOCK SCENARIO:
 *    If all philosophers:
 *    1. Pick up their left fork simultaneously
 *    2. Try to pick up their right fork
 *    3. Wait forever because right fork is held by neighbor
 *
 *    Timeline of deadlock:
 *    Phil 0: picks up fork 0, waits for fork 1
 *    Phil 1: picks up fork 1, waits for fork 2
 *    Phil 2: picks up fork 2, waits for fork 3
 *    Phil 3: picks up fork 3, waits for fork 4
 *    Phil 4: picks up fork 4, waits for fork 0
 *    → CIRCULAR WAIT → DEADLOCK!
 *
 * 3. FOUR CONDITIONS FOR DEADLOCK (Coffman conditions):
 *    ALL four must be true for deadlock to occur:
 *
 *    a) Mutual Exclusion:
 *       - Resources (forks) can be held by only one thread at a time
 *       - ✓ True in our case (mutex per fork)
 *
 *    b) Hold and Wait:
 *       - Thread holds resources while waiting for others
 *       - ✓ True (hold left fork, wait for right fork)
 *
 *    c) No Preemption:
 *       - Resources cannot be forcibly taken away
 *       - ✓ True (can't steal forks from philosophers)
 *
 *    d) Circular Wait:
 *       - Circular chain of threads waiting for resources
 *       - ✓ Possible (Phil 0→1→2→3→4→0)
 *
 * 4. OUR SOLUTION: RESOURCE ORDERING
 *    Break the circular wait condition by establishing a global order:
 *
 *    Rule: Always acquire lower-numbered fork first
 *
 *    Implementation:
 *    - Philosopher i needs forks i and (i+1)%5
 *    - first = min(i, (i+1)%5)
 *    - second = max(i, (i+1)%5)
 *    - lock(first), then lock(second)
 *
 *    Why it works:
 *    - No circular dependency can form
 *    - All threads agree on acquisition order
 *    - At least one philosopher can always make progress
 *
 * 5. EXAMPLE WITH RESOURCE ORDERING:
 *    Philosopher 0: forks 0,1 → pick 0 first, then 1
 *    Philosopher 1: forks 1,2 → pick 1 first, then 2
 *    Philosopher 2: forks 2,3 → pick 2 first, then 3
 *    Philosopher 3: forks 3,4 → pick 3 first, then 4
 *    Philosopher 4: forks 4,0 → pick 0 first, then 4  ← KEY!
 *
 *    Philosopher 4 picks fork 0 first (not fork 4)!
 *    This breaks the circular chain.
 *
 * 6. ALTERNATIVE SOLUTIONS:
 *
 *    a) Limit Concurrent Eaters (Semaphore):
 *       - Allow max N-1 philosophers to try eating simultaneously
 *       - At least one can always complete
 *       - Implementation: sem_init(&sem, 0, NUM_PHILOSOPHERS-1)
 *
 *    b) Odd-Even Strategy:
 *       - Odd-numbered philosophers pick left fork first
 *       - Even-numbered philosophers pick right fork first
 *       - Breaks symmetry
 *
 *    c) Waiter Solution:
 *       - Central coordinator grants permission to pick up forks
 *       - Ensures safe state before allowing pickup
 *       - More overhead
 *
 *    d) Timeout and Retry:
 *       - Use pthread_mutex_trylock() with timeout
 *       - If can't get both forks, release and retry
 *       - Risk: livelock (all keep retrying)
 *
 *    e) Chandy-Misra Solution:
 *       - Forks have "clean" and "dirty" states
 *       - Request/grant protocol
 *       - More complex but elegant
 *
 * 7. WHY RESOURCE ORDERING IS PREFERRED:
 *    ✓ Simple to implement
 *    ✓ No extra synchronization overhead
 *    ✓ Guaranteed deadlock freedom
 *    ✓ Good performance
 *    ✗ Requires global knowledge of resource IDs
 *
 * 8. STARVATION CONCERNS:
 *    - This solution prevents deadlock
 *    - But doesn't guarantee fairness
 *    - Some philosopher might eat less frequently
 *    - For fairness, need additional mechanisms (ticket system, etc.)
 *
 * 9. REAL-WORLD ANALOGS:
 *    - Database transactions locking multiple tables
 *    - Process allocation of multiple devices
 *    - Network routing with multiple paths
 *    - Resource allocation in operating systems
 *
 * 10. TESTING THIS SOLUTION:
 *     - Run multiple times - should never deadlock
 *     - Try different numbers of philosophers
 *     - Try different meal counts
 *     - Verify all philosophers complete
 *     - Check total meals equals expected
 *
 * 11. WHAT IF WE DIDN'T USE RESOURCE ORDERING?
 *     If each philosopher just did:
 *     lock(left_fork);
 *     lock(right_fork);
 *
 *     Then with enough runs, you'd eventually see deadlock where
 *     all philosophers hold their left fork and wait forever for right.
 *
 * KEY TAKEAWAYS:
 * - Deadlock occurs when all 4 Coffman conditions are met
 * - Breaking any one condition prevents deadlock
 * - Resource ordering breaks circular wait
 * - Always acquire locks in a consistent global order
 * - This pattern applies to any multi-resource locking scenario
 */
