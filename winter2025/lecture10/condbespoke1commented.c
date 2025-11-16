// PEDAGOGICAL PURPOSE:
// This program introduces condition variables for thread synchronization and communication.
// It demonstrates a simple producer-consumer pattern where threads coordinate using conditions.
// Key learning objectives:
// 1. Understanding pthread_cond_t (condition variables)
// 2. Learning pthread_cond_wait() for waiting on conditions
// 3. Learning pthread_cond_signal() for waking waiting threads
// 4. Understanding why condition variables need a mutex
// 5. Seeing the pattern: lock, check condition, wait, unlock
// 6. Understanding spurious wakeups and why to use while loops
// 7. Learning the condition variable lifecycle: init, wait/signal, destroy
// 8. Seeing practical thread coordination beyond just mutexes

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// SYNCHRONIZATION PRIMITIVES:
// We need BOTH mutex and condition variable
//
// MUTEX:
// Protects shared data (numInputs, inputArr)
// Ensures only one thread modifies at a time
//
// CONDITION VARIABLE:
// Allows threads to wait for a condition
// Allows threads to signal when condition is met
pthread_mutex_t mut;
pthread_cond_t enoughInputs;

// SHARED DATA:
// These variables are accessed by both threads
// Protected by mutex 'mut'
int numInputs = 0;     // How many numbers have been entered
int inputArr[2];       // Array to hold the two input numbers

// ADDER THREAD:
// Waits for two numbers to be ready, then adds them
// Demonstrates WAITING on a condition
//
// PURPOSE:
// This thread can't do its job until data is ready
// Instead of busy-waiting (wasteful), it SLEEPS until signaled
void* adder(void* arg){

  // LOCK THE MUTEX:
  // Must lock before accessing shared data
  // Also required for pthread_cond_wait
  pthread_mutex_lock(&mut);

  // WAIT FOR CONDITION:
  // Wait until we have at least 2 inputs
  //
  // WHY WHILE LOOP?
  // Condition variables can have "spurious wakeups"
  // Thread might wake up even if condition isn't true
  // Always recheck condition after waking
  //
  // PATTERN:
  // while (condition not met) {
  //   wait
  // }
  //
  // NOT:
  // if (condition not met) {
  //   wait  // DANGEROUS! Might have spurious wakeup
  // }
  while(numInputs < 2){

    // PTHREAD_COND_WAIT:
    // This is the MAGIC function for thread coordination
    //
    // WHAT IT DOES (atomically):
    // 1. Unlock the mutex
    // 2. Put thread to sleep (waiting for signal)
    // 3. When signaled, wake up and RE-LOCK mutex
    //
    // WHY UNLOCK MUTEX?
    // If we kept mutex locked while sleeping:
    //   - reader thread couldn't acquire mutex
    //   - reader thread couldn't add inputs
    //   - We'd sleep forever (deadlock!)
    //
    // PARAMETERS:
    // 1. Condition variable to wait on
    // 2. Mutex to unlock/relock
    //
    // WHEN IT RETURNS:
    // - Mutex is locked again
    // - We're ready to check condition
    // - Might be spurious wakeup, so check in while loop
    pthread_cond_wait(&enoughInputs,&mut);
  }
  // AT THIS POINT:
  // - Mutex is locked (pthread_cond_wait relocked it)
  // - numInputs >= 2 (condition is met)
  // - Safe to access inputArr

  // GET THE RESULT POINTER:
  // arg points to an int where we should store result
  // Cast void* to int*
  int* num = (int*)arg;

  // COMPUTE THE SUM:
  // Add the two numbers from the array
  // Store result where main() can find it
  //
  // SAFE ACCESS:
  // We hold the mutex, so this is safe
  // reader thread won't modify array while we're reading
  *num = inputArr[1] + inputArr[0];

  // UNLOCK THE MUTEX:
  // Done accessing shared data
  // Release the lock
  pthread_mutex_unlock(&mut);

  return NULL;
}

// READER THREAD:
// Reads two numbers from user
// Demonstrates SIGNALING a condition
//
// PURPOSE:
// This thread produces data that adder thread needs
// After producing, it signals adder to wake up
void* reader(void* arg){

  // LOCK THE MUTEX:
  // Must lock before accessing shared data
  pthread_mutex_lock(&mut);

  // READ TWO NUMBERS:
  // Loop until we have 2 inputs
  //
  // NOTE: This could block on scanf
  // But we hold the mutex!
  // This is OKAY because:
  //   - adder is sleeping in pthread_cond_wait
  //   - pthread_cond_wait UNLOCKED the mutex
  //   - So we can acquire it here
  while(numInputs < 2){

    // PROMPT USER:
    printf("Enter a number:\n");

    // READ INPUT:
    // scanf blocks until user types a number
    // While blocked, we HOLD THE MUTEX
    // This is fine because adder is sleeping (not trying to acquire mutex)
    scanf("%d",&inputArr[numInputs]);

    // INCREMENT COUNTER:
    // Track how many inputs we've received
    numInputs++;
  }

  // SIGNAL THE CONDITION:
  // Wake up the adder thread
  //
  // PTHREAD_COND_SIGNAL:
  // Wakes up ONE thread waiting on this condition variable
  // If multiple threads waiting, one is chosen (unspecified which)
  //
  // WHAT HAPPENS:
  // - adder thread is sleeping in pthread_cond_wait
  // - pthread_cond_signal wakes it up
  // - adder thread tries to reacquire mutex
  // - adder thread BLOCKS (we still hold mutex!)
  // - When we unlock, adder acquires mutex and continues
  //
  // ALTERNATIVE: pthread_cond_broadcast
  // Would wake up ALL waiting threads
  // Not needed here (only one adder thread)
  pthread_cond_signal(&enoughInputs);

  // UNLOCK THE MUTEX:
  // Release the lock
  // This allows adder thread to acquire it
  pthread_mutex_unlock(&mut);

  return NULL;
}

int main(){

  // INITIALIZE MUTEX:
  // Must initialize before using
  pthread_mutex_init(&mut,NULL);

  // INITIALIZE CONDITION VARIABLE:
  // Must initialize before using
  //
  // PARAMETERS:
  // 1. Condition variable to initialize
  // 2. Attributes (NULL = default)
  //
  // DEFAULT ATTRIBUTES:
  // - Process-private (only threads in this process)
  // - No special clock for timed waits
  pthread_cond_init(&enoughInputs,NULL);

  // THREAD HANDLES:
  pthread_t readThread;
  pthread_t addThread;

  // RESULT VARIABLE:
  // Will hold the sum computed by adder thread
  // We pass &num to adder so it can store result here
  int num = 0;

  // CREATE READER THREAD:
  // This thread will read user input
  // No arguments needed
  pthread_create(&readThread, NULL, reader, NULL);

  // CREATE ADDER THREAD:
  // This thread will wait for data, then add
  // Pass &num so it can store result
  pthread_create(&addThread, NULL, adder, &num);

  // TYPICAL EXECUTION FLOW:
  //
  // T0: Both threads created
  // T1: addThread locks mutex, checks numInputs < 2 (true)
  // T2: addThread calls pthread_cond_wait (unlocks mutex, sleeps)
  // T3: readThread locks mutex (succeeds, adder unlocked it)
  // T4: readThread prompts for first number
  // T5: User enters first number, readThread stores it
  // T6: readThread prompts for second number
  // T7: User enters second number, readThread stores it
  // T8: readThread calls pthread_cond_signal (wakes adder)
  // T9: addThread wakes up, tries to lock mutex (blocks, reader has it)
  // T10: readThread unlocks mutex
  // T11: addThread acquires mutex
  // T12: addThread checks condition (numInputs >= 2, true!)
  // T13: addThread exits while loop, computes sum
  // T14: addThread unlocks mutex, returns

  // WAIT FOR BOTH THREADS:
  pthread_join(readThread,NULL);
  pthread_join(addThread, NULL);

  // PRINT RESULT:
  // adder thread stored result in num
  printf("After much labor and effort the numbers added to: %d\n",num);

  // CLEANUP:
  // Destroy synchronization primitives
  // Must do this after all threads are done using them
  pthread_mutex_destroy(&mut);
  pthread_cond_destroy(&enoughInputs);

  return 0;
}

// EXPECTED OUTPUT:
// Enter a number:
// 5
// Enter a number:
// 7
// After much labor and effort the numbers added to: 12

// CONDITION VARIABLE PATTERN:
//
// THREAD THAT WAITS:
//   pthread_mutex_lock(&mutex);
//   while (condition not met) {
//     pthread_cond_wait(&cond, &mutex);
//   }
//   // Use the data
//   pthread_mutex_unlock(&mutex);
//
// THREAD THAT SIGNALS:
//   pthread_mutex_lock(&mutex);
//   // Make condition true
//   pthread_cond_signal(&cond);
//   pthread_mutex_unlock(&mutex);

// WHY CONDITION VARIABLES?
//
// WITHOUT CONDITION VARIABLES (busy waiting):
//   void* adder(void* arg) {
//     while (numInputs < 2) {
//       // Spin, wasting CPU
//     }
//     // Add numbers
//   }
//   PROBLEM: Wastes CPU continuously checking
//
// WITH CONDITION VARIABLES (efficient waiting):
//   void* adder(void* arg) {
//     pthread_mutex_lock(&mut);
//     while (numInputs < 2) {
//       pthread_cond_wait(&enoughInputs, &mut);  // Sleep until signaled
//     }
//     // Add numbers
//     pthread_mutex_unlock(&mut);
//   }
//   SOLUTION: Thread sleeps, uses no CPU until woken

// WHY MUTEX WITH CONDITION VARIABLE?
//
// Condition variables ALWAYS used with mutex because:
//
// 1. SHARED DATA PROTECTION:
//    Checking condition (numInputs < 2) accesses shared data
//    Must hold mutex while accessing shared data
//
// 2. ATOMICITY:
//    pthread_cond_wait atomically unlocks and sleeps
//    Prevents race condition between checking and waiting
//
// 3. RACE CONDITION WITHOUT MUTEX:
//    // BROKEN CODE (without mutex):
//    if (numInputs < 2) {
//      // RACE! Signal might happen here
//      pthread_cond_wait(&enoughInputs, &mut);
//      // Miss the signal, sleep forever!
//    }

// SPURIOUS WAKEUPS:
//
// Threads can wake from pthread_cond_wait WITHOUT being signaled
// This is allowed by POSIX
// Reasons: implementation efficiency, interrupts
//
// ALWAYS USE WHILE LOOP:
//   while (condition not met) {
//     pthread_cond_wait(...);
//   }
//
// NOT IF STATEMENT:
//   if (condition not met) {
//     pthread_cond_wait(...);  // DANGEROUS!
//   }
//
// With while: Spurious wakeup → recheck → wait again → correct
// With if: Spurious wakeup → continue → WRONG!

// SIGNAL vs BROADCAST:
//
// pthread_cond_signal:
//   Wakes ONE waiting thread
//   Use when any one thread can handle the event
//   More efficient
//
// pthread_cond_broadcast:
//   Wakes ALL waiting threads
//   Use when all threads need to recheck condition
//   Example: shutdown signal for thread pool

// KEY CONCEPTS:
//
// 1. CONDITION VARIABLES:
//    Allow threads to sleep until condition is met
//    More efficient than busy-waiting
//    Always used with mutex
//
// 2. PTHREAD_COND_WAIT:
//    Atomically unlocks mutex and sleeps
//    When woken, relocks mutex before returning
//    May have spurious wakeups (always use while loop)
//
// 3. PTHREAD_COND_SIGNAL:
//    Wakes one waiting thread
//    Should be called while holding mutex
//    Safe to call even if no threads waiting
//
// 4. TYPICAL PATTERN:
//    Producer: lock, modify, signal, unlock
//    Consumer: lock, while(not ready) wait, use data, unlock

// COMPILE AND RUN:
// gcc condbespoke1.c -o condbespoke1 -pthread
// ./condbespoke1
//
// Try it:
// Enter two numbers when prompted
// See the sum printed

// EXPERIMENT:
// 1. Change while to if in adder thread
//    Sometimes works, sometimes doesn't (spurious wakeup)
//
// 2. Remove pthread_cond_signal from reader
//    Program hangs (adder never wakes up)
//
// 3. Move pthread_cond_signal before pthread_mutex_lock in reader
//    Signal is sent before adder is waiting
//    Adder sleeps forever (missed signal)
//
// 4. Add sleep(5) in reader between inputs
//    adder still waits patiently (not wasting CPU)
//
// 5. Use pthread_cond_broadcast instead of signal
//    Same result here (only one waiting thread)
