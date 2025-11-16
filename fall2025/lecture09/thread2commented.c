// PEDAGOGICAL PURPOSE:
// This program demonstrates RACE CONDITIONS - one of the most important concepts
// in concurrent programming. This is a classic example of the "lost update" problem.
// Key learning objectives:
// 1. Understanding race conditions (non-deterministic bugs from concurrent access)
// 2. Why shared memory without synchronization is dangerous
// 3. The read-modify-write problem
// 4. How timing affects correctness in concurrent programs
// 5. Why the final count is usually WRONG (less than expected 10,000)
//
// CRITICAL CONCEPT:
// This program has a BUG by design! It's meant to show what goes wrong.
// The "correct" answer should be 10,000, but you'll almost never get it.
//
// KEY CONCEPTS:
// - Race condition: Bug that depends on timing of concurrent operations
// - Atomic operations: Operations that complete without interruption
// - Lost update: One thread's update overwrites another's
// - Non-determinism: Different results on different runs

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>     // For sleep() (currently commented out)

// SHARED GLOBAL VARIABLE:
// This is THE critical variable that all threads will access
// ALL 100 threads will read and modify this same variable
// This is where the race condition occurs!
int num = 0;            // Should end up being 10,000 (100 threads * 100 increments)
                        // But it WON'T! Usually gets 100-5,000 (varies each run)

// WHY global?
// - All threads need to access the same variable
// - Global variables are in shared memory (all threads can see it)
// - This demonstrates the danger of shared mutable state

// THREAD FUNCTION:
// Each of the 100 threads will execute this function
// Each thread will increment 'num' 100 times
// Expected result: 100 threads * 100 increments = 10,000
// Actual result: Much less! (race condition causes lost updates)
void* thread_function(void* arg) {
  // EACH THREAD LOOPS 100 TIMES:
  for(int i=0; i<100; i++){
    // THE RACE CONDITION HAPPENS HERE:
    // These three lines look like simple increment, but they're NOT atomic!

    // STEP 1: Read current value
    int num2 = num;     // Thread COLLECTS the current state of num
                        // Example: Thread A reads num=100
                        //          Thread B reads num=100 (same value!)

    // OPTIONAL SLEEP (commented out):
    // sleep(rand()%2);  // Randomly sleep 0 or 1 second
    // WHY commented out? Would make program too slow (100 threads * 100 * 1 sec)
    // But uncommenting would make race conditions MORE visible
    // Sleep increases chance of interleaving between threads

    // STEP 2: Modify the value
    num2 = num2+1;      // Thread does work on its LOCAL copy
                        // Example: Thread A computes num2=101
                        //          Thread B computes num2=101 (SAME!)

    // STEP 3: Write back the value
    num = num2;         // Thread STORES the update back to shared variable
                        // Example: Thread A writes num=101
                        //          Thread B writes num=101 (OVERWRITES A's update!)
                        //
                        // LOST UPDATE: Thread A's increment is LOST!
                        // Both threads incremented, but num only went up by 1, not 2!
  }

  printf("Thread executing\n");
  // After all 100 increments, this thread prints
  // We'll see this print 100 times (one per thread)

  return NULL;
}

int main() {
  // SEED RANDOM NUMBER GENERATOR:
  srand(time(0));       // Seed with current time
                        // This makes rand() produce different values each run
                        // (though sleep is commented out, left for experiments)

  // THREAD ARRAY:
  // We'll create 100 threads, so we need 100 thread handles
  pthread_t thread[100];

  // CREATE 100 THREADS:
  for(int i=0;i<100;i++){
    // Create thread #i
    // Each thread will execute thread_function()
    // Each will increment the shared 'num' variable 100 times
    pthread_create(&thread[i], NULL, thread_function, NULL);
  }
  // AFTER THIS LOOP:
  // - 100 threads are running concurrently
  // - All are accessing the same 'num' variable
  // - Race conditions are happening continuously!
  // - Main thread continues immediately (doesn't wait)

  // WAIT FOR ALL THREADS TO COMPLETE:
  // We must join ALL threads before checking the final result
  // WHY? If we don't wait, main() might exit before threads finish
  for(int i=0; i<100; i++){
    pthread_join(thread[i], NULL);
    // Wait for thread #i to complete
    // This blocks until that specific thread finishes
  }
  // AFTER THIS LOOP:
  // - All 100 threads have finished
  // - All increments (should be 10,000) have been attempted
  // - But many updates were lost due to race condition

  // PRINT FINAL RESULT:
  printf("And the total is: %d\n",num);
  // EXPECTED: 10,000 (100 threads * 100 increments each)
  // ACTUAL: Usually 100 to 8,000 (varies WILDLY between runs!)
  // WHY WRONG? Race conditions caused lost updates

  return 0;
}

// THE RACE CONDITION EXPLAINED IN DETAIL:
//
// Let's trace what happens when two threads access 'num' simultaneously:
//
// SCENARIO: num currently = 100
//
// Thread A:                        Thread B:
// --------                         --------
// int num2 = num;  (reads 100)
//                                  int num2 = num;  (reads 100)
// num2 = num2+1;   (computes 101)
//                                  num2 = num2+1;   (computes 101)
// num = num2;      (writes 101)
//                                  num = num2;      (writes 101, OVERWRITES!)
//
// RESULT: num = 101 (increased by 1, not 2!)
// BOTH threads incremented, but one update was LOST
//
// This is called a "LOST UPDATE" - a classic race condition

// WHY IS THIS NON-ATOMIC?
//
// The C statement "num = num + 1" seems like one operation
// But it compiles to MULTIPLE assembly instructions:
//
//   movl num(%rip), %eax     ; Load num into register
//   addl $1, %eax             ; Add 1 to register
//   movl %eax, num(%rip)     ; Store register back to num
//
// Between ANY of these instructions, another thread can run!
//
// EXAMPLE INTERLEAVING:
// Thread A: movl num(%rip), %eax    ; A reads num=100 into %eax
// Thread B: movl num(%rip), %eax    ; B reads num=100 into %eax
// Thread A: addl $1, %eax            ; A computes %eax=101
// Thread B: addl $1, %eax            ; B computes %eax=101
// Thread A: movl %eax, num(%rip)    ; A writes num=101
// Thread B: movl %eax, num(%rip)    ; B writes num=101 (overwrites!)
//
// Two increments, but num only increased by 1!

// MATHEMATICAL ANALYSIS:
//
// EXPECTED RESULT:
// - 100 threads
// - Each does 100 increments
// - Total: 100 * 100 = 10,000
// - Starting from num=0
// - Final value should be: 10,000
//
// ACTUAL RESULT (typical):
// - Anywhere from 100 to 9,999 (yes, could theoretically get 10,000 by luck!)
// - Usually around 1,000-5,000 on a multi-core system
// - Different every time you run it!
//
// PERCENTAGE LOST:
// If you get 3,000: (10,000 - 3,000) / 10,000 = 70% of updates LOST!

// WHY DOES THE NUMBER VARY?
//
// The number of lost updates depends on:
// 1. HOW MANY cores your CPU has (more cores = more parallelism = more races)
// 2. WHEN threads get scheduled (OS scheduler is non-deterministic)
// 3. HOW FAST your CPU is (faster = more likely to interleave)
// 4. WHAT ELSE is running (affects scheduling)
//
// Run it multiple times - you'll get different answers!
//   ./thread2
//   And the total is: 3471
//   ./thread2
//   And the total is: 2893
//   ./thread2
//   And the total is: 4102
//
// This non-determinism makes race conditions HARD to debug!

// HOW TO FIX THIS (preview of future topics):
//
// SOLUTION 1: MUTEX (Mutual Exclusion Lock)
//   pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
//
//   void* thread_function(void* arg) {
//     for(int i=0; i<100; i++){
//       pthread_mutex_lock(&lock);     // LOCK
//       int num2 = num;
//       num2 = num2+1;
//       num = num2;
//       pthread_mutex_unlock(&lock);   // UNLOCK
//     }
//     return NULL;
//   }
//
//   Now only ONE thread can execute the critical section at a time
//   Result: ALWAYS 10,000 (correct!)
//
// SOLUTION 2: ATOMIC OPERATIONS
//   #include <stdatomic.h>
//   atomic_int num = 0;
//
//   void* thread_function(void* arg) {
//     for(int i=0; i<100; i++){
//       atomic_fetch_add(&num, 1);     // Atomic increment
//     }
//     return NULL;
//   }
//
//   Hardware-level atomic operation
//   Result: ALWAYS 10,000 (correct!)
//
// SOLUTION 3: Don't share mutable state!
//   Each thread increments its OWN counter
//   Then sum all counters at the end
//   (This is the "functional programming" approach)

// TIMING ANALYSIS:
//
// Without sleep():
// - 100 threads * 100 increments = 10,000 operations
// - Each operation takes ~nanoseconds
// - Total time: milliseconds
// - FAST, but race conditions happen
//
// With sleep(rand()%2) uncommented:
// - Average sleep: 0.5 seconds per increment
// - 100 threads * 100 increments * 0.5s = 5,000 seconds if sequential
// - But parallel, so ~500 seconds (threads run simultaneously)
// - TOO SLOW for demonstration, but great for learning
// - More sleep = MORE visible race conditions (easier to see the bug)

// CONCEPTUAL UNDERSTANDING:
//
// WHY is this a race condition?
// The outcome "races" on which thread gets to write last
// The final value depends on the TIMING of thread execution
//
// WHAT makes it a bug?
// The result is NON-DETERMINISTIC (unpredictable)
// The result is INCORRECT (not 10,000)
// The bug is HARD TO REPRODUCE (different every time)
//
// WHY is it dangerous?
// Imagine this is a bank account balance!
// Two threads doing deposits, but deposits get lost
// Money disappears! Customers complain! Company loses trust!
//
// Or a website visitor counter:
// Lost updates mean you undercount your traffic
// Leads to wrong business decisions

// COMPARISON WITH PROCESSES (fork):
//
// If we used fork() instead of pthread:
// - Each process has its OWN copy of 'num'
// - No race conditions (no shared memory)
// - But also no way to sum them up (without IPC)
// - Final answer in each process: 100 (their own increments)
//
// Threads share memory: FAST but DANGEROUS (need synchronization)
// Processes separate memory: SLOW but SAFE (no synchronization needed)

// REAL-WORLD EXAMPLES OF RACE CONDITIONS:
//
// 1. Therac-25 radiation machine (1980s):
//    Race condition in medical device -> patients overdosed -> deaths
//
// 2. Northeast Blackout (2003):
//    Race condition in power grid software -> 50 million people without power
//
// 3. Stock trading systems:
//    Race conditions can cause incorrect trades -> financial losses
//
// 4. Operating systems:
//    Race conditions in kernel can cause system crashes
//
// This is NOT a theoretical problem - it's REAL and IMPORTANT!

// DEBUGGING RACE CONDITIONS:
//
// HARD because:
// 1. Non-deterministic (happens sometimes, not always)
// 2. Timing-dependent (goes away when you add print statements!)
// 3. Hardware-dependent (works on single core, fails on multi-core)
// 4. "Heisenbugs" - disappear when you try to observe them
//
// TOOLS:
// 1. Thread Sanitizer (gcc -fsanitize=thread)
// 2. Valgrind with Helgrind tool
// 3. printf debugging (sometimes helps, sometimes hides the bug!)
// 4. Stress testing (run many times, look for variance)

// EDUCATIONAL VALUE:
//
// This example teaches:
// 1. Shared memory is powerful but dangerous
// 2. Concurrent programs need synchronization
// 3. "Correct" code can have timing bugs
// 4. Non-determinism makes debugging hard
// 5. Always protect shared data with locks/atomics

// TRY IT:
// Compile: gcc -pthread thread2.c -o thread2
// Run: ./thread2
// Run it 10 times - see different results each time!
//
// EXPERIMENTS:
// 1. Change to 10 threads - still wrong, but maybe closer to 1,000
// 2. Change to 1 thread - ALWAYS 100 (no race, only one thread!)
// 3. Uncomment sleep() - see even WORSE results (more time = more races)
// 4. Add print inside loop - might change behavior (timing changes!)
// 5. Run with Thread Sanitizer: gcc -pthread -fsanitize=thread thread2.c
//    It will DETECT the race condition and report it!
//
// THREAD SANITIZER OUTPUT (if you run it):
//   WARNING: ThreadSanitizer: data race (pid=...)
//   Write of size 4 at 0x... by thread T1:
//     #0 thread_function thread2.c:23
//   Previous write of size 4 at 0x... by thread T2:
//     #0 thread_function thread2.c:23
//
//   SUMMARY: ThreadSanitizer: data race thread2.c:23 in thread_function
//
// This PROVES the race condition exists!

// NEXT STEPS IN CONCURRENCY:
// - Mutexes (pthread_mutex_lock/unlock)
// - Condition variables (pthread_cond_wait/signal)
// - Semaphores (sem_wait/sem_post)
// - Read-write locks (pthread_rwlock)
// - Atomic operations (stdatomic.h)
// - Lock-free data structures
// - Deadlock prevention
// - Thread-safe programming patterns

// PHILOSOPHICAL QUESTION:
//
// Why do we still use threads if they're so dangerous?
// - Because they're FAST and EFFICIENT
// - With proper synchronization, they're SAFE
// - Modern software needs concurrency (multi-core CPUs)
// - The alternative (processes) is too expensive for many uses
//
// "With great power comes great responsibility"
// Threads give you power (speed, shared data)
// But you must use them responsibly (synchronization)

// FINAL THOUGHTS:
//
// This program is INTENTIONALLY BROKEN to teach you
// In real code, you would:
// 1. Identify shared mutable state (num)
// 2. Protect it with a mutex
// 3. Test thoroughly
// 4. Use Thread Sanitizer to verify
//
// Understanding this bug is the first step to writing correct concurrent code!
