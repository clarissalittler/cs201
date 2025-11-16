// PEDAGOGICAL PURPOSE:
// This program demonstrates a classic producer-consumer pattern using mutex locks.
// It implements a simple "idle/incremental game" where one thread updates state
// periodically while the main thread handles user input.
// Key learning objectives:
// 1. Using pthread_mutex_t to protect shared data (critical sections)
// 2. Understanding why mutexes are necessary (preventing race conditions)
// 3. Coordinating two threads accessing the same variables
// 4. Proper mutex lifecycle: init -> lock/unlock -> destroy
// 5. The danger of infinite loops in multi-threaded programs
// 6. Why this program has a bug (unreachable cleanup code)

#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"   // POSIX threads library

// GLOBAL MUTEX:
// pthread_mutex_t is a mutual exclusion lock
// Ensures only ONE thread can access shared data at a time
// This prevents race conditions and data corruption
pthread_mutex_t mut;

// SHARED GAME STATE:
// These variables are accessed by BOTH threads:
//   1. Main thread (user input)
//   2. Heartbeat thread (periodic updates)
// Without mutex protection, we'd have race conditions!
int income = 0;   // Player's current money
int gen1 = 0;     // Number of level-1 generators owned
int gen2 = 0;     // Number of level-2 generators owned
int gen3 = 0;     // Number of level-3 generators owned

// MENU STRING:
// Displayed to user every second
// Defines valid choices and their costs
char menuString[] = "Press 1 to tick up your income\nPress 2 to buy a first-level generator (cost 10)\nPress 3 to buy a second-level generator (cost 1000)\nPress 4 to buy a third-level generator (cost 100000)\n";


// PRINT MENU FUNCTION:
// Displays current game state and available options
void printMenu(){
  printf("Money %d\nGen1: %d\nGen2: %d\nGen3: %d\n",
	 income,gen1,gen2,gen3);
  printf(menuString);
}

// HEARTBEAT THREAD WORKER:
// This function runs in a separate thread
// It updates game state every second (the "game loop")
void* heartbeat(void* arg){

  // INFINITE LOOP:
  // This thread runs forever, updating state every second
  // PROBLEM: No way to exit gracefully!
  // The program can only be killed with Ctrl+C (abrupt termination)
  while(1){

    // CRITICAL SECTION START:
    // pthread_mutex_lock() acquires the mutex
    // If another thread holds it, this thread BLOCKS (waits)
    // This ensures exclusive access to shared variables
    pthread_mutex_lock(&mut);

    // ATOMIC OPERATIONS:
    // All these operations happen as one indivisible unit
    // No other thread can see partial updates
    printMenu();           // Display current state

    // GAME LOGIC:
    // Generators produce resources hierarchically:
    //   - Each gen3 produces 1 gen2 per second
    //   - Each gen2 produces 1 gen1 per second
    //   - Each gen1 produces 1 income per second
    gen2 += gen3;
    gen1 += gen2;
    income += gen1;

    // CRITICAL SECTION END:
    // pthread_mutex_unlock() releases the mutex
    // Now other threads can acquire it
    pthread_mutex_unlock(&mut);

    // DELAY:
    // Sleep for 1 second between updates
    // WHY outside the critical section?
    // We don't want to hold the lock while sleeping!
    // That would prevent user input from being processed
    sleep(1);
  }

  return NULL;  // UNREACHABLE CODE (infinite loop above)
}

int main(){

  // THREAD VARIABLE:
  // Will hold the identifier for our heartbeat thread
  pthread_t pulse;

  // MUTEX INITIALIZATION:
  // pthread_mutex_init() prepares the mutex for use
  // Parameters:
  //   1. &mut - pointer to mutex to initialize
  //   2. NULL - use default attributes
  // CRITICAL: Must initialize before using!
  pthread_mutex_init(&mut,NULL);

  // CREATE HEARTBEAT THREAD:
  // Spawns a new thread running the heartbeat() function
  // This thread runs concurrently with main thread
  pthread_create(&pulse,NULL,heartbeat,NULL);

  // MAIN LOOP (USER INPUT):
  // Main thread handles user input
  // Runs concurrently with heartbeat thread
  // PROBLEM: This is also an infinite loop!
  while(1){
    int choice;

    // READ USER INPUT:
    // scanf() blocks until user enters a number
    // While waiting, heartbeat thread continues running
    scanf("%d",&choice);

    // PROCESS USER CHOICE:
    // Each case acquires mutex, modifies state, releases mutex
    switch(choice){

    case 1:  // Manual income increase
      // CRITICAL SECTION:
      // Lock -> Modify -> Unlock pattern
      pthread_mutex_lock(&mut);
      income++;
      pthread_mutex_unlock(&mut);
      break;

    case 2:  // Buy gen1 (costs 10)
      pthread_mutex_lock(&mut);
      // CHECK-THEN-ACT pattern:
      // Must be atomic to prevent race conditions
      // Imagine if another thread modified income between check and purchase!
      if(income >= 10){
	income = income -10;
	gen1++;
      }
      pthread_mutex_unlock(&mut);
      break;

    case 3:  // Buy gen2 (costs 1000)
      pthread_mutex_lock(&mut);
      if(income >= 1000){
	income = income - 1000;
	gen2++;
      }
      pthread_mutex_unlock(&mut);
      break;

    case 4:  // Buy gen3 (costs 100000)
      pthread_mutex_lock(&mut);
      if(income >= 100000){
	income -= 100000;
	gen3++;
      }
      pthread_mutex_unlock(&mut);
      // NOTE: Missing break statement!
      // Falls through to default case (harmless here)

    default:
      break;
    }
  }

  // UNREACHABLE CLEANUP CODE:
  // The infinite loop above means we never get here
  // This is a design flaw - no graceful shutdown mechanism

  pthread_join(pulse,NULL);      // Would wait for heartbeat thread to finish
  pthread_mutex_destroy(&mut);   // Would cleanup mutex

  return 0;
}

// WHY DO WE NEED MUTEXES?
//
// Without mutexes, this sequence could happen:
//
// Time  Heartbeat Thread        Main Thread
// ----  ----------------        -----------
// T0    Read income (100)
// T1                            Read income (100)
// T2    income += gen1 (110)
// T3                            income++ (101)
// T4    Write income (110)
// T5                            Write income (101)
//
// Result: income = 101 (should be 111!)
// This is a RACE CONDITION - the final value depends on timing
//
// WITH MUTEXES:
// Only one thread can execute its critical section at a time
// Updates happen atomically, preventing data corruption

// EXECUTION FLOW:
//
// 1. main() initializes mutex and creates heartbeat thread
// 2. Two threads now run concurrently:
//    - Heartbeat thread: Updates game state every second
//    - Main thread: Waits for user input
// 3. When heartbeat needs to update state:
//    - Acquires mutex
//    - Updates variables
//    - Releases mutex
//    - Sleeps 1 second
// 4. When user enters choice:
//    - Main thread acquires mutex
//    - Modifies state based on choice
//    - Releases mutex
// 5. If both try to acquire mutex simultaneously:
//    - One succeeds, enters critical section
//    - Other blocks (waits) until mutex is released

// CRITICAL SECTION PROPERTIES:
//
// 1. Mutual Exclusion: Only one thread at a time
// 2. Progress: If no thread is in critical section, one can enter
// 3. Bounded Waiting: Threads don't wait forever
// 4. Keep it SHORT: Don't hold locks longer than necessary

// BUGS IN THIS PROGRAM:
//
// 1. No way to exit gracefully (infinite loops)
// 2. Cleanup code is unreachable
// 3. Missing break in case 4
// 4. No error checking on pthread calls
// 5. printMenu() called inside critical section (slow I/O while holding lock)

// TRY IT:
// Compile: gcc threadIncremental.c -o threadIncremental -pthread
// Run: ./threadIncremental
// Observe the menu updating every second
// Try option 1 a few times to increase income
// Buy a gen1 (option 2) once you have 10 money
// Watch income increase automatically each second
// Notice no race conditions or garbled output (thanks to mutexes!)
// To exit: Ctrl+C (no graceful shutdown in this version)
//
// EXPERIMENT:
// What happens if you remove mutex locks?
// Try commenting out lock/unlock calls and recompile
// Run it and observe potential data corruption or crashes
