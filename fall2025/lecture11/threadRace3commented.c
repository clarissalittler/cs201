// PEDAGOGICAL PURPOSE:
// This program demonstrates ENCAPSULATION of mutex-protected data using a struct.
// Key learning objectives:
// 1. Understanding how to bundle data with its protection mechanism
// 2. Learning the "guarded object" design pattern
// 3. Seeing how to pass struct pointers to threads via void*
// 4. Understanding the relationship between data and its mutex
// 5. Learning better software engineering practices for concurrent code
// 6. Understanding how to access struct members through pointers
// 7. Seeing the advantages of encapsulation over global variables

#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"


// GUARDED COUNTER STRUCT:
// This demonstrates a DESIGN PATTERN for concurrent programming
// We bundle the DATA with its PROTECTION MECHANISM
//
// WHY THIS IS BETTER than threadRace2.c:
// 1. ENCAPSULATION: The counter and its mutex are together
// 2. CLARITY: It's obvious they're related
// 3. SCALABILITY: Easy to create multiple independent guarded counters
// 4. MAINTENANCE: Changes to protection strategy are localized
//
// ANALOGY:
// Instead of having a lock and treasure in different places,
// we put the treasure in a locked box
struct guardedCounter {
  // THE PROTECTED DATA:
  int counter;

  // THE PROTECTION MECHANISM:
  // This mutex is specifically for protecting this counter
  // If we had multiple guardedCounter instances,
  // each would have its own independent mutex
  pthread_mutex_t mutex;
};

// THREAD FUNCTION:
// Now takes a pointer to a guardedCounter struct
void* incer(void* arg){
  // CASTING void* TO THE ACTUAL TYPE:
  // arg comes in as void* (generic pointer) because that's what pthread requires
  // We know it's actually a pointer to a guardedCounter
  // So we cast it: (struct guardedCounter*)arg
  //
  // WHY THE CAST IS NECESSARY:
  // void* has no type information
  // We can't use -> on a void*
  // The cast tells the compiler "treat this as a struct guardedCounter*"
  //
  // STORING IN A LOCAL VARIABLE:
  // This makes the code more readable
  // We could write (struct guardedCounter*)arg everywhere, but g is cleaner
  struct guardedCounter* g = (struct guardedCounter*)arg;

  // ACQUIRE THE LOCK:
  // Note: &g->mutex
  //   g is a pointer to a struct
  //   g->mutex accesses the mutex member
  //   &g->mutex gets the address of that mutex member
  //
  // EQUIVALENT SYNTAX:
  // &g->mutex is the same as &((*g).mutex)
  // But -> is much more readable for pointer-to-struct member access
  //
  // IMPORTANT:
  // We're locking THIS struct's mutex
  // If there were other guardedCounter instances, they'd have different mutexes
  pthread_mutex_lock(&g->mutex); // only one thread will win, everyone else waits until the mutex is *un* locked

  // CRITICAL SECTION BEGINS:

  // READ the counter from the struct:
  // g->counter accesses the counter member of the struct
  int temp = g->counter;

  // SIMULATED WORK:
  sleep(rand()%3+1); // simulated work

  // WRITE back to the counter in the struct:
  g->counter = temp+1;

  // CRITICAL SECTION ENDS:

  // RELEASE THE LOCK:
  pthread_mutex_unlock(&g->mutex);

  return NULL;
}

int main(){
  // SEED RANDOM NUMBER GENERATOR:
  srand(time(NULL));

  // THREAD ARRAY:
  pthread_t threads[5];

  // CREATE AND INITIALIZE THE GUARDED COUNTER:
  // struct guardedCounter c declares a local variable of this type
  // This is allocated on the stack (not heap)
  struct guardedCounter c;

  // INITIALIZE THE COUNTER:
  // We must initialize the counter member to 0
  // Unlike global variables, local struct members are NOT auto-initialized to 0
  c.counter = 0;

  // INITIALIZE THE MUTEX:
  // &(c.mutex) gets the address of the mutex member
  // The parentheses ensure we're taking the address of the whole member
  //
  // ALTERNATIVE SYNTAX:
  // Could also write: pthread_mutex_init(&c.mutex, NULL);
  // The parentheses in &(c.mutex) are not strictly necessary but add clarity
  pthread_mutex_init(&(c.mutex),NULL); // pass the pointer to the mutex

  // CREATE 5 THREADS:
  // IMPORTANT: We pass &c (address of the struct) as the argument!
  for(int i=0; i<5;i++){
    // PASSING DATA TO THREADS:
    // pthread_create(..., incer, &c)
    //   The fourth parameter is the argument passed to incer
    //   We pass &c (pointer to our guardedCounter)
    //   Inside incer, this arrives as void* arg
    //
    // ALL THREADS GET THE SAME POINTER:
    // All 5 threads receive a pointer to the SAME struct c
    // They all share the same counter and the same mutex
    // This is what makes synchronization necessary!
    pthread_create(threads+i,NULL,incer,&c);
  }

  // WAIT FOR ALL THREADS TO COMPLETE:
  for(int i=0; i<5;i++){
    pthread_join(threads[i],NULL);
  }

  // PRINT THE RESULT:
  // Access the counter from our struct
  // Should ALWAYS be 5 (each thread incremented once)
  printf("The counter is: %d\n",c.counter);

  // DESTROY THE MUTEX:
  // Clean up the mutex that's inside our struct
  pthread_mutex_destroy(&(c.mutex));

  return 0;
}

// EXPECTED OUTPUT:
// The counter is: 5

// COMPARISON WITH threadRace2.c:
//
// threadRace2.c:
//   int counter = 0;              (global)
//   pthread_mutex_t mutex;        (global)
//
// threadRace3.c:
//   struct guardedCounter {       (encapsulated)
//     int counter;
//     pthread_mutex_t mutex;
//   };

// ADVANTAGES OF THE STRUCT APPROACH:
//
// 1. ENCAPSULATION:
//    The data and its protection mechanism are bundled together
//    It's clear they're related
//
// 2. MULTIPLE INSTANCES:
//    Easy to create multiple independent guarded counters:
//      struct guardedCounter c1, c2, c3;
//    Each has its own counter and its own mutex
//    No interference between them!
//
// 3. NO GLOBALS:
//    The struct can be a local variable
//    Better scoping and modularity
//
// 4. PARAMETER PASSING:
//    We can pass the struct to functions/threads
//    More flexible than global variables
//
// 5. EXTENSIBILITY:
//    Easy to add more fields:
//      struct guardedCounter {
//        int counter;
//        char* name;
//        time_t last_updated;
//        pthread_mutex_t mutex;
//      };

// THE VOID* DANCE:
//
// WHY SO MUCH CASTING?
// pthread_create requires:
//   void* (*start_routine)(void*)
//
// This is the most GENERIC function signature:
//   - Takes void* (can be a pointer to ANYTHING)
//   - Returns void* (can be a pointer to ANYTHING)
//
// This allows pthread to work with ANY data type
// But WE must tell the compiler the actual types using casts

// STRUCT MEMBER ACCESS SYNTAX:
//
// Given: struct guardedCounter c;
//        struct guardedCounter* g = &c;
//
// DIRECT ACCESS (for struct variable):
//   c.counter       - access counter member
//   c.mutex         - access mutex member
//   &c.counter      - address of counter member
//   &c.mutex        - address of mutex member
//
// POINTER ACCESS (for struct pointer):
//   g->counter      - access counter member through pointer
//   g->mutex        - access mutex member through pointer
//   &g->counter     - address of counter member
//   &g->mutex       - address of mutex member
//
// EQUIVALENCE:
//   g->counter  is the same as  (*g).counter
//   g->mutex    is the same as  (*g).mutex
//
// But -> is much more readable!

// DESIGN PATTERN: MONITOR/GUARDED OBJECT
//
// This is a classic concurrency design pattern:
// 1. Bundle data with its synchronization primitive
// 2. All access to the data must go through synchronized methods/functions
// 3. The mutex "guards" the data
//
// This pattern appears in many languages:
//   - Java: synchronized methods
//   - Python: threading.Lock with context managers
//   - Rust: Mutex<T> type
//   - C++: std::mutex with RAII locks

// COMMON PATTERNS FOR STRUCT + MUTEX:
//
// PATTERN 1: Mutex as first member (this example)
// struct guardedCounter {
//   pthread_mutex_t mutex;
//   int counter;
// };
//
// PATTERN 2: Mutex as last member (also common)
// struct guardedCounter {
//   int counter;
//   pthread_mutex_t mutex;
// };
//
// Both work, but convention often puts mutex last
// This example puts it after the data, but either is fine

// TRY IT:
// 1. Compile: gcc threadRace3.c -o threadRace3 -pthread
// 2. Run: ./threadRace3
// 3. Verify you always get 5
// 4. EXPERIMENT: Create TWO guardedCounters (c1 and c2)
//    Create threads that increment c1, and threads that increment c2
//    Verify they don't interfere with each other
// 5. EXPERIMENT: Add a char* name field to the struct
//    Print which counter each thread is incrementing
// 6. COMPARE: Look at threadRace2.c and threadRace3.c side by side
//    Notice how this version is more modular and maintainable
// 7. CHALLENGE: Write helper functions:
//    void guardedCounter_init(struct guardedCounter* gc);
//    void guardedCounter_increment(struct guardedCounter* gc);
//    int guardedCounter_get(struct guardedCounter* gc);
//    void guardedCounter_destroy(struct guardedCounter* gc);
//    This further encapsulates the mutex locking!
