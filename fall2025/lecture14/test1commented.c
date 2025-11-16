// PEDAGOGICAL PURPOSE:
// This program demonstrates multi-threading combined with signal handling in C.
// Key learning objectives:
// 1. Creating multiple threads that execute the same worker function
// 2. Using sig_atomic_t for thread-safe signal communication
// 3. Proper signal handler design (async-signal-safe functions only)
// 4. Coordinating thread termination via a shared flag
// 5. Understanding race conditions in concurrent output
// 6. Using structures to pass data to threads
// 7. Random state simulation in concurrent contexts

#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"  // POSIX threads library
#include "stdio.h"
#include "signal.h"   // For signal handling
#include "string.h"

// ENUM DEFINITION:
// Defines the possible states for our "little guy" simulation
// Each thread will randomly transition between these states
enum lilguyState {
  eating,     // State 0
  sleeping,   // State 1
  moving,     // State 2
  vibing      // State 3
};

// STRUCT DEFINITION:
// Data structure passed to each thread
// Each thread gets its own copy of this data
struct lilguyData {
  int id;                    // Unique identifier for this thread
  enum lilguyState state;    // Current state of this thread
};

// GLOBAL SIGNAL FLAG:
// sig_atomic_t ensures atomic reads/writes (no partial updates)
// This is CRITICAL for safe communication between signal handler and threads
// volatile tells compiler this can change unexpectedly (prevents optimization)
sig_atomic_t simulationRunning = 1;

// SIGNAL HANDLER:
// Called when user presses Ctrl+C (SIGINT)
// IMPORTANT: Signal handlers must only use async-signal-safe functions
void handler(int signum){
  char msg[] = "Alright everyone clean up and go!\n";

  // Set flag to 0 to tell all threads to stop
  // All threads check this flag in their while loop
  simulationRunning = 0;

  // WHY write() instead of printf()?
  // printf() is NOT async-signal-safe (uses internal buffers/locks)
  // write() is async-signal-safe (direct system call)
  // Using printf() here could cause deadlock or corruption
  write(1,msg,strlen(msg));  // 1 = stdout file descriptor
}

// UTILITY FUNCTION:
// Converts state enum to human-readable message
// Called by threads to display their current activity
void printState(enum lilguyState s){
  switch(s){
  case eating:
    printf("having a lil snack\n");
    break;
  case sleeping:
    printf("having a lil sleep\n");
    break;
  case vibing:
    printf("having a lil chill\n");
    break;
  case moving:
    printf("having a lil walk\n");
    // NOTE: Missing break statement here! Falls through to default
    // This is actually fine since there's no code after, but it's sloppy
  }
}

// THREAD WORKER FUNCTION:
// This function runs independently in each thread
// All threads execute this same function but with different data
void* threadWorker(void* arg){

  // DEREFERENCE AND COPY THREAD DATA:
  // arg is void* (generic pointer), must cast to correct type
  // We make a LOCAL COPY of the data structure
  // WHY? Because the original might be modified or go out of scope
  struct lilguyData dat = *((struct lilguyData*) arg);

  // MAIN THREAD LOOP:
  // Continues until signal handler sets simulationRunning to 0
  while(simulationRunning){

    // Print current state
    // NOTE: Multiple threads printing creates interleaved output
    // This demonstrates a RACE CONDITION (non-deterministic output order)
    printf("I'm Miss %d and I'm ",dat.id);
    printState(dat.state);

    // RANDOM STATE TRANSITION:
    // rand()%4 generates 0-3, corresponding to our 4 states
    // Each thread independently changes its state
    dat.state = rand()%4;

    // SLEEP:
    // Sleeps 1-3 seconds (rand()%3 gives 0-2, add 1 for 1-3)
    // This simulates work and makes the timing less predictable
    sleep(rand()%3+1);
  }

  // CLEANUP MESSAGE:
  // After simulationRunning becomes 0, thread exits gracefully
  printf("I'm miss %d and it's time to go!!\n",dat.id);

  return NULL;  // Thread exits normally
}


int main(){

  // SEED RANDOM NUMBER GENERATOR:
  // time(0) returns current time in seconds since epoch
  // This ensures different random sequences each run
  srand(time(0));

  // REGISTER SIGNAL HANDLER:
  // When SIGINT (Ctrl+C) is received, call handler()
  // This allows graceful shutdown instead of abrupt termination
  signal(SIGINT,handler);

  // THREAD ARRAY:
  // pthread_t is the thread identifier type
  // We create 5 threads that will run concurrently
  pthread_t threads[5];

  // DATA ARRAY:
  // One data structure per thread
  // Each thread gets its own initial state
  struct lilguyData datums[5];

  // INITIALIZE THREAD DATA:
  // Set up initial ID and random state for each thread
  for(int i=0; i<5; i++){
    datums[i].id = i;           // Unique ID: 0, 1, 2, 3, 4
    datums[i].state = rand()%4; // Random initial state
  }

  // CREATE THREADS:
  // pthread_create() spawns a new thread
  // Parameters:
  //   1. &threads[i] - pointer to thread identifier (output)
  //   2. NULL - default thread attributes
  //   3. threadWorker - function to execute
  //   4. &datums[i] - argument passed to thread function
  for(int i=0; i<5; i++){
    pthread_create(&threads[i],NULL,threadWorker,&datums[i]);
  }

  // WAIT FOR THREADS:
  // pthread_join() blocks until the specified thread terminates
  // This prevents main() from exiting before threads finish
  // Parameters:
  //   1. threads[i] - thread to wait for
  //   2. NULL - we don't care about thread's return value
  //
  // WHY is this necessary?
  // If main() exits, the entire process terminates, killing all threads
  for(int i=0;i<5;i++){
    pthread_join(threads[i],NULL);
  }

  return 0;
  // Program exits only after all 5 threads have finished
}

// EXECUTION FLOW:
//
// 1. main() initializes data and signal handler
// 2. 5 threads are created, all start executing threadWorker()
// 3. Each thread enters its while loop, printing and sleeping
// 4. Output is interleaved (non-deterministic order due to thread scheduling)
// 5. User presses Ctrl+C
// 6. Signal handler sets simulationRunning = 0
// 7. All threads exit their while loops
// 8. Each thread prints goodbye message and returns
// 9. main() finishes joining all threads
// 10. Program exits

// RACE CONDITION EXAMPLE:
// Thread 1: printf("I'm Miss 1 and I'm ")
// Thread 2: printf("I'm Miss 2 and I'm ")  <- might print before Thread 1 finishes
// Thread 1: printf("having a lil snack\n")
// Thread 2: printf("having a lil sleep\n")
//
// Possible output:
// "I'm Miss 1 and I'm I'm Miss 2 and I'm having a lil snack
// having a lil sleep"
//
// This is GARBLED because printf() is not atomic across multiple calls

// KEY CONCEPTS:
// 1. sig_atomic_t: Ensures atomic read/write for signal communication
// 2. Async-signal-safety: Signal handlers must use only safe functions
// 3. Thread synchronization via shared flag (simple but not ideal)
// 4. Race conditions in output (no mutex protection)
// 5. Graceful shutdown pattern

// TRY IT:
// Compile: gcc test1.c -o test1 -pthread
// Run: ./test1
// Let it run for a few seconds, observe the output
// Press Ctrl+C to trigger signal handler
// Notice how all threads terminate gracefully
// Run multiple times - notice different output orderings (non-determinism)
