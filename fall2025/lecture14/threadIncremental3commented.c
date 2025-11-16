// PEDAGOGICAL PURPOSE:
// This program demonstrates using signal() instead of sigaction() for signal handling
// in a multi-threaded context, and shows async-signal-safe output in handlers.
// Key learning objectives:
// 1. Difference between signal() and sigaction() approaches
// 2. Using write() for async-signal-safe output in signal handlers
// 3. Understanding why printf() is dangerous in signal handlers
// 4. The potential issue with signal() and blocking syscalls
// 5. Comparing signal() simplicity vs sigaction() control
// 6. Best practices for user feedback during shutdown

#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"
#include "signal.h"
#include "sys/types.h"
#include "string.h"     // For strlen()

pthread_mutex_t mut;

// SHUTDOWN FLAG:
// Same pattern as threadIncremental2.c
volatile sig_atomic_t toContinue = 1;

int income = 0;
int gen1 = 0;
int gen2 = 0;
int gen3 = 0;

char menuString[] = "Press 1 to tick up your income\nPress 2 to buy a first-level generator (cost 10)\nPress 3 to buy a second-level generator (cost 1000)\nPress 4 to buy a third-level generator (cost 100000)\n";

// ENHANCED SIGNAL HANDLER:
// This version provides user feedback during shutdown
void cleanup(int signum){

  // Set the shutdown flag
  toContinue = 0;

  // USER MESSAGE:
  // Pre-initialized char array (not dynamically allocated)
  char final[] = "Make one last purchase before you go!\n";

  // ASYNC-SIGNAL-SAFE OUTPUT:
  // write() is async-signal-safe, printf() is NOT
  //
  // WHY write() instead of printf()?
  // printf():
  //   - Uses internal buffers
  //   - Might lock mutexes
  //   - Not async-signal-safe
  //   - Could deadlock if main thread was in printf() when signal arrived
  //
  // write():
  //   - Direct system call
  //   - No buffering, no locks
  //   - Async-signal-safe
  //   - Safe to use in signal handlers
  //
  // Parameters:
  //   1 - file descriptor (1 = stdout)
  //   final - buffer containing message
  //   strlen(final) - number of bytes to write
  write(1,final,strlen(final));
}

void printMenu(){
  printf("Money %d\nGen1: %d\nGen2: %d\nGen3: %d\n",
	 income,gen1,gen2,gen3);
  printf(menuString);
}

void* heartbeat(void* arg){
  while(toContinue){
    pthread_mutex_lock(&mut);
    printMenu();
    gen2 += gen3;
    gen1 += gen2;
    income += gen1;
    pthread_mutex_unlock(&mut);
    sleep(1);
  }

  return NULL;
}

int main(){

  pthread_t pulse;
  pthread_mutex_init(&mut,NULL);
  pthread_create(&pulse,NULL,heartbeat,NULL);

  // SIGNAL HANDLER REGISTRATION:
  // Using signal() instead of sigaction()
  //
  // signal() - The simpler (but less controlled) approach:
  //   - Single function call
  //   - Less portable (behavior varies)
  //   - Can't specify flags
  //   - Handler might reset after first call (implementation-dependent)
  //
  // In modern systems, signal() often behaves like sigaction() with SA_RESTART
  // This means scanf() WILL restart after the signal
  // User must type something to let scanf() complete and exit

  signal(SIGINT,cleanup);

  // MAIN LOOP:
  // Same as threadIncremental2.c
  while(toContinue){
    int choice;

    // POTENTIAL BLOCKING ISSUE:
    // With signal() on many systems, SA_RESTART is implicitly set
    // This means when SIGINT arrives:
    //   1. cleanup() executes, sets toContinue = 0, prints message
    //   2. scanf() RESTARTS (doesn't return with error)
    //   3. scanf() continues waiting for input
    //   4. User must type a number for scanf() to complete
    //   5. Loop checks toContinue, sees 0, exits
    //
    // This is why the message says "Make one last purchase"
    // You actually NEED to type something to exit!
    scanf("%d",&choice);

    switch(choice){

    case 1:
      pthread_mutex_lock(&mut);
      income++;
      pthread_mutex_unlock(&mut);
      break;

    case 2:
      pthread_mutex_lock(&mut);
      if(income >= 10){
	income = income -10;
	gen1++;
      }
      pthread_mutex_unlock(&mut);
      break;

    case 3:
      pthread_mutex_lock(&mut);
      if(income >= 1000){
	income = income - 1000;
	gen2++;
      }
      pthread_mutex_unlock(&mut);
      break;

    case 4:
      pthread_mutex_lock(&mut);
      if(income >= 100000){
	income -= 100000;
	gen3++;
      }
      pthread_mutex_unlock(&mut);
      break;

    default:
      break;
    }
  }

  // CLEANUP:
  pthread_join(pulse,NULL);
  pthread_mutex_destroy(&mut);

  printf("You ended with %d $\n",income);

  return 0;
}

// EXECUTION FLOW WITH SIGNAL HANDLER:
//
// Normal operation:
// 1. Heartbeat thread prints menu every second
// 2. Main thread waits at scanf()
// 3. User types choice, switch processes it
//
// Shutdown sequence:
// 1. User presses Ctrl+C while scanf() is blocking
// 2. SIGINT delivered to process
// 3. cleanup() handler executes:
//    a. Sets toContinue = 0
//    b. Uses write() to print "Make one last purchase..."
//    c. Returns
// 4. scanf() RESTARTS (because signal() implicitly sets SA_RESTART)
// 5. User must type a number
// 6. scanf() returns with user's input
// 7. Switch processes the choice
// 8. while loop condition checked: toContinue is 0, loop exits
// 9. Heartbeat thread's loop also sees toContinue = 0, exits
// 10. main joins heartbeat thread
// 11. Mutex destroyed, final score printed
// 12. Program exits

// COMPARING THREE APPROACHES:
//
// threadIncremental.c:
//   - No signal handling
//   - Infinite loops
//   - Must kill with Ctrl+C (abrupt termination)
//   - No cleanup
//
// threadIncremental2.c:
//   - sigaction() with sa_flags = 0
//   - scanf() doesn't restart
//   - Can exit immediately after Ctrl+C
//   - Clean shutdown
//   - No user feedback in handler
//
// threadIncremental3.c (this file):
//   - signal()
//   - scanf() restarts (on most systems)
//   - Must type something after Ctrl+C
//   - Clean shutdown
//   - User feedback with write()

// ASYNC-SIGNAL-SAFETY:
//
// Safe functions (can use in signal handlers):
//   - write()
//   - _exit()
//   - Most system calls
//   - Assignment to sig_atomic_t variables
//
// Unsafe functions (NEVER use in signal handlers):
//   - printf(), fprintf(), etc. (use write() instead)
//   - malloc(), free() (might deadlock)
//   - pthread_mutex_lock() (might deadlock)
//   - Most library functions
//
// Why unsafe?
// If signal interrupts malloc(), and handler calls malloc():
//   1. malloc's internal mutex already locked
//   2. Handler tries to lock same mutex
//   3. DEADLOCK

// WHY write() INSTEAD OF printf()?
//
// Example of danger:
//
// Thread timeline:
// T0: Main thread calls printf() in printMenu()
// T1: printf() acquires internal buffer lock
// T2: SIGINT arrives, interrupts printf()
// T3: Signal handler calls printf()
// T4: Handler's printf() tries to acquire same lock
// T5: DEADLOCK - waiting for lock held by interrupted code
//
// With write():
// T0: Main thread calls printf() in printMenu()
// T1: printf() acquires internal buffer lock
// T2: SIGINT arrives, interrupts printf()
// T3: Signal handler calls write()
// T4: write() makes direct syscall, no locks needed
// T5: Handler completes successfully
// T6: Main thread resumes, completes printf()

// TRY IT:
// Compile: gcc threadIncremental3.c -o threadIncremental3 -pthread
// Run: ./threadIncremental3
// Play the game
// Press Ctrl+C
// Observe:
//   1. Message appears: "Make one last purchase before you go!"
//   2. Program doesn't exit immediately
//   3. You must type a number (any number)
//   4. Then program exits cleanly
//   5. Final score is printed
//
// COMPARE:
// Run threadIncremental2 and threadIncremental3 side-by-side
// Notice the difference in how Ctrl+C behaves
// threadIncremental2: exits after Ctrl+C (might need one keystroke)
// threadIncremental3: requires input after Ctrl+C
