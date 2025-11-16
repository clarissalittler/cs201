// PEDAGOGICAL PURPOSE:
// This program demonstrates encapsulation of mutex with data using structs.
// It shows how to protect multiple independent counters with separate mutexes.
// Key learning objectives:
// 1. Understanding struct-based encapsulation of data + mutex
// 2. Learning why each protected resource needs its own mutex
// 3. Seeing how to pass structs to threads for organized synchronization
// 4. Understanding the benefits of grouping related data with its protection
// 5. Learning to manage multiple independent critical sections
// 6. Seeing proper mutex initialization for struct members
// 7. Understanding thread argument patterns with complex data
// 8. Learning scalable patterns for protecting multiple resources

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// PROTECTED COUNTER STRUCT:
// Encapsulates data with its mutex
//
// DESIGN PRINCIPLE:
// "Keep data and its protection together"
//
// WHY THIS IS GOOD:
// - Clear ownership (this mutex protects THIS counter)
// - Can't forget to use the right mutex
// - Scalable (easy to create many protected counters)
// - Self-documenting (struct shows what's protected)
//
// CONTRAST WITH GLOBALS:
// Instead of:
//   int counter1;
//   int counter2;
//   pthread_mutex_t mutex1;
//   pthread_mutex_t mutex2;
//   (Which mutex protects which counter? Unclear!)
//
// We have:
//   struct protectedCounter c1;
//   struct protectedCounter c2;
//   (Each counter has its own mutex, clear!)
struct protectedCounter{
  int counter;           // The data to protect
  pthread_mutex_t mutex; // The mutex that protects it
};

// THREAD WORKER FUNCTION:
// Increments a protected counter
//
// RECEIVES:
// Pointer to a protectedCounter struct
// Can access both counter and mutex
void* threadCounter(void* arg){

  // CAST ARGUMENT TO STRUCT POINTER:
  // arg is void* (generic pointer)
  // We know it's actually struct protectedCounter*
  // Cast to proper type for use
  struct protectedCounter* c = (struct protectedCounter*) arg;

  // LOCK THE MUTEX:
  // Use arrow operator to access struct member
  // &c->mutex means "address of mutex field in struct c points to"
  //
  // DESIGN NOTE:
  // The mutex is INSIDE the struct
  // This makes it obvious which mutex protects which data
  pthread_mutex_lock(&c->mutex);

  // SIMULATE SLOW OPERATION:
  // Random delay 1-3 seconds
  // Exaggerates the critical section
  // Shows that mutex prevents interleaving
  sleep(rand()%3+1);

  // INCREMENT THE COUNTER:
  // Read-modify-write operation
  // Protected by the mutex
  // Multiple threads can't interfere
  //
  // COULD ALSO WRITE:
  //   c->counter = c->counter + 1;
  //   c->counter += 1;
  // All equivalent
  c->counter = c->counter+1;

  // UNLOCK THE MUTEX:
  pthread_mutex_unlock(&c->mutex);

  return NULL;
}

int main(){

  // SEED RANDOM:
  srand(time(NULL));

  // THREAD ARRAY:
  // Create 10 threads
  pthread_t threads[10];

  // FIRST PROTECTED COUNTER:
  // Declare and initialize the struct
  //
  // INITIALIZATION:
  // c1.counter = 0;  (start at zero)
  // c1.mutex must be initialized with pthread_mutex_init
  struct protectedCounter c1;
  c1.counter = 0;
  pthread_mutex_init(&c1.mutex,NULL);

  // SECOND PROTECTED COUNTER:
  // Independent from c1
  // Has its own counter and its own mutex
  //
  // KEY INSIGHT:
  // c1 and c2 are INDEPENDENT
  // Thread can hold c1.mutex while another holds c2.mutex
  // No conflict! Different locks, different data
  struct protectedCounter c2;
  c2.counter = 0;
  pthread_mutex_init(&c2.mutex,NULL);

  // CREATE 10 THREADS:
  // Odd threads increment c1
  // Even threads increment c2
  //
  // PATTERN:
  // Thread 0 (even) → c2
  // Thread 1 (odd)  → c1
  // Thread 2 (even) → c2
  // Thread 3 (odd)  → c1
  // ...
  //
  // RESULT:
  // 5 threads increment c1 (1, 3, 5, 7, 9)
  // 5 threads increment c2 (0, 2, 4, 6, 8)
  for(int i =0; i < 10; i++){

    // CHECK IF ODD OR EVEN:
    if(i % 2 == 1){

      // ODD: Increment c1
      // Pass address of c1 to thread
      // Thread will lock c1.mutex, increment c1.counter
      pthread_create(&threads[i],NULL,threadCounter,&c1);

    } else {

      // EVEN: Increment c2
      // Pass address of c2 to thread
      // Thread will lock c2.mutex, increment c2.counter
      pthread_create(&threads[i],NULL,threadCounter,&c2);
    }
  }

  // WAIT FOR ALL THREADS:
  // Ensure all increments complete before checking results
  for(int i=0; i < 10; i++){
    pthread_join(threads[i],NULL);
  }

  // PRINT RESULTS:
  // c1 should be 5 (incremented by 5 threads)
  // c2 should be 5 (incremented by 5 threads)
  printf("The value of our first counter is: %d\n",c1.counter);
  printf("The value of our second counter is: %d\n",c2.counter);

  // CLEANUP:
  // Destroy both mutexes
  // Must destroy after all threads are done using them
  pthread_mutex_destroy(&c1.mutex);
  pthread_mutex_destroy(&c2.mutex);

  return 0;
}

// EXPECTED OUTPUT:
// The value of our first counter is: 5
// The value of our second counter is: 5

// WHY SEPARATE MUTEXES?
//
// OPTION 1: One mutex for both counters
//   pthread_mutex_t global_mutex;
//   Problem: c1 threads block c2 threads unnecessarily
//   Thread incrementing c1 blocks thread incrementing c2
//   No parallelism!
//
// OPTION 2: Separate mutex for each counter (this program)
//   c1 has c1.mutex
//   c2 has c2.mutex
//   Benefit: c1 threads and c2 threads can run in parallel
//   Only threads accessing SAME counter block each other
//   Better performance!

// PARALLELISM ANALYSIS:
//
// WITH ONE GLOBAL MUTEX:
// T0: Thread0 locks global_mutex, increments c2
// T1: Thread1 tries to lock global_mutex, BLOCKS (even though it wants c1!)
// T2: Thread0 unlocks, Thread1 locks, increments c1
// Total time: ~20 seconds (10 threads × ~2 seconds each, sequential)
//
// WITH SEPARATE MUTEXES:
// T0: Thread0 locks c2.mutex, increments c2
// T1: Thread1 locks c1.mutex, increments c1 (parallel with Thread0!)
// T2: Thread2 tries c2.mutex, BLOCKS (Thread0 has it)
// T3: Thread3 tries c1.mutex, BLOCKS (Thread1 has it)
// Total time: ~10 seconds (5 threads × ~2 seconds per counter, parallel)

// STRUCT DESIGN PATTERN:
//
// TRADITIONAL APPROACH (error-prone):
//   int counter1, counter2;
//   pthread_mutex_t mutex1, mutex2;
//   // Which mutex protects which counter?
//   // Easy to make mistakes:
//   pthread_mutex_lock(&mutex1);
//   counter2++;  // WRONG MUTEX!
//   pthread_mutex_unlock(&mutex1);
//
// STRUCT APPROACH (self-documenting):
//   struct protectedCounter {
//     int counter;
//     pthread_mutex_t mutex;
//   };
//   struct protectedCounter c1, c2;
//   // Clear relationship:
//   pthread_mutex_lock(&c1.mutex);
//   c1.counter++;  // Correct! Using c1's mutex for c1's data
//   pthread_mutex_unlock(&c1.mutex);

// MEMORY LAYOUT:
//
// struct protectedCounter c1:
// +------------------+
// | counter: 0       |  (4 bytes, int)
// | mutex: {...}     |  (platform-dependent, ~40 bytes)
// +------------------+
//
// struct protectedCounter c2:
// +------------------+
// | counter: 0       |
// | mutex: {...}     |
// +------------------+
//
// Completely independent structs
// Independent mutexes
// No shared state between c1 and c2

// KEY CONCEPTS:
//
// 1. ENCAPSULATION:
//    Group data with its protection
//    Makes code clearer and safer
//    Prevents using wrong mutex
//
// 2. INDEPENDENT CRITICAL SECTIONS:
//    Different mutexes protect different data
//    Threads can run in parallel if accessing different data
//    Better performance than single global mutex
//
// 3. STRUCT MEMBER ACCESS:
//    c.mutex (dot) if c is struct
//    c->mutex (arrow) if c is pointer to struct
//    &c.mutex (address of struct member)
//
// 4. SCALABILITY:
//    Easy to add more counters:
//    struct protectedCounter c3, c4, c5;
//    Each gets its own mutex automatically
//
// 5. MUTEX GRANULARITY:
//    Fine-grained: Separate mutex per resource (this program)
//      + Better parallelism
//      - More complex
//    Coarse-grained: One mutex for all resources
//      + Simpler
//      - Less parallelism

// DESIGN PRINCIPLES:
//
// 1. "Protect data, not code"
//    Mutex belongs with the data it protects
//
// 2. "Make wrong code look wrong"
//    Struct makes it obvious which mutex protects which data
//
// 3. "Minimize critical section"
//    Each counter has its own mutex
//    Critical sections are independent
//
// 4. "Independent data, independent locks"
//    If data is unrelated, use separate mutexes
//    Allows parallelism

// COMPARISON TO threadmutex.c:
//
// threadmutex.c:
//   - Global counter
//   - Global mutex
//   - All threads compete for same mutex
//   - 10 threads, one counter, final value: 10
//
// mutexreview.c (this file):
//   - Two counters (in structs)
//   - Two mutexes (in structs)
//   - Threads split between two groups
//   - 10 threads, two counters, final values: 5 and 5
//
// ADVANTAGE HERE:
//   Better organization, better scalability

// COMPILE AND RUN:
// gcc mutexreview.c -o mutexreview -pthread
// ./mutexreview
//
// Expected output:
// The value of our first counter is: 5
// The value of our second counter is: 5

// EXPERIMENT:
// 1. Use same mutex for both counters
//    pthread_mutex_t global_mutex;
//    pthread_mutex_init(&global_mutex, NULL);
//    In threadCounter: pthread_mutex_lock(&global_mutex);
//    See that it still works, but is less efficient
//
// 2. Create 100 threads instead of 10
//    Should get: c1 = 50, c2 = 50
//
// 3. Add timing (time ./mutexreview)
//    Then change to use single global mutex
//    Compare times (separate mutexes should be faster)
//
// 4. Add a third counter c3
//    Every 3rd thread increments c3
//    if (i % 3 == 0) → c3
//    Results: c1, c2, c3 each incremented by ~3-4 threads
//
// 5. Print which counter each thread incremented:
//    printf("Thread %d incrementing counter %p\n", i, c);
//    Observe the pattern

// REAL-WORLD APPLICATIONS:
//
// BANK ACCOUNTS:
//   struct Account {
//     int balance;
//     pthread_mutex_t mutex;
//   };
//   struct Account alice, bob;
//   Transfer from alice to bob needs both mutexes
//   (But be careful of deadlock! Always lock in same order)
//
// HASH TABLE:
//   struct Bucket {
//     LinkedList* items;
//     pthread_mutex_t mutex;
//   };
//   struct Bucket buckets[100];
//   Each bucket has its own lock
//   Threads can access different buckets in parallel
//
// CACHE:
//   struct CacheLine {
//     void* data;
//     pthread_mutex_t mutex;
//   };
//   Fine-grained locking per cache line
//   Better than one lock for entire cache
