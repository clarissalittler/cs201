// PEDAGOGICAL PURPOSE:
// This program builds on threadIncremental.c by adding GRACEFUL SHUTDOWN capability.
// It demonstrates the CORRECT way to combine threads and signal handling.
// Key learning objectives:
// 1. Using sigaction() instead of signal() for more reliable signal handling
// 2. Coordinating thread termination via volatile sig_atomic_t flag
// 3. Understanding why signal() can be problematic with threads
// 4. Proper cleanup sequence: stop threads -> join threads -> destroy mutex
// 5. The SA_RESTART flag and its effect on blocking system calls
// 6. Best practices for graceful shutdown in multi-threaded programs

#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"
#include "signal.h"      // Signal handling
#include "sys/types.h"   // System types

// GLOBAL MUTEX:
// Protects shared game state from race conditions
pthread_mutex_t mut;

// SHUTDOWN FLAG:
// volatile: Tells compiler this can change unexpectedly (by signal handler)
// sig_atomic_t: Guarantees atomic read/write operations
// This is the PROPER way to communicate between signal handler and threads
volatile sig_atomic_t toContinue = 1;

// SHARED GAME STATE:
// Protected by mutex 'mut'
int income = 0;
int gen1 = 0;
int gen2 = 0;
int gen3 = 0;

char menuString[] = "Press 1 to tick up your income\nPress 2 to buy a first-level generator (cost 10)\nPress 3 to buy a second-level generator (cost 1000)\nPress 4 to buy a third-level generator (cost 100000)\n";

// SIGNAL HANDLER:
// Called when user presses Ctrl+C (SIGINT)
// This is simpler than in test1.c - just sets the flag
void cleanup(int signum){
  // Set flag to 0 to signal all threads to stop
  // Both heartbeat thread and main loop check this flag
  toContinue = 0;

  // NOTE: No message printed here
  // Simpler approach - let main thread handle final output
}

void printMenu(){
  printf("Money %d\nGen1: %d\nGen2: %d\nGen3: %d\n",
	 income,gen1,gen2,gen3);
  printf(menuString);
}

// HEARTBEAT THREAD:
// Now with termination condition!
void* heartbeat(void* arg){

  // CONTROLLED LOOP:
  // Continues while toContinue == 1
  // When signal handler sets it to 0, loop exits
  while(toContinue){
    pthread_mutex_lock(&mut);
    printMenu();
    gen2 += gen3;
    gen1 += gen2;
    income += gen1;
    pthread_mutex_unlock(&mut);
    sleep(1);
  }

  return NULL;  // NOW REACHABLE! Thread exits when toContinue becomes 0
}

int main(){

  pthread_t pulse;
  pthread_mutex_init(&mut,NULL);
  pthread_create(&pulse,NULL,heartbeat,NULL);

  // SIGNAL SETUP:
  // WHY sigaction() instead of signal()?
  //
  // signal() has portability issues:
  //   - Behavior varies across Unix systems
  //   - Handler might reset after first signal
  //   - Cannot specify flags
  //
  // sigaction() is more reliable and portable:
  //   - Consistent behavior across systems
  //   - Handler persists (doesn't reset)
  //   - Can control restart behavior

  // scanf BLOCKING PROBLEM:
  // scanf() blocks waiting for input
  // When SIGINT arrives while scanf() is blocking:
  //   - If SA_RESTART is set: scanf() restarts automatically
  //   - If SA_RESTART is NOT set: scanf() returns with error
  //
  // Without SA_RESTART, user must enter input to unblock scanf()
  // This is mentioned in the comment but not fully addressed here

  // CONFIGURE SIGNAL ACTION:
  struct sigaction sigsettings;

  // Set handler function
  sigsettings.sa_handler = cleanup;

  // FLAGS = 0 means NO flags set
  // Specifically, SA_RESTART is NOT set
  // This means scanf() will NOT automatically restart after signal
  // scanf() will return with error, allowing loop to check toContinue
  sigsettings.sa_flags = 0; // makes scanf not block

  // INSTALL SIGNAL HANDLER:
  // Parameters:
  //   1. SIGINT - signal to handle (Ctrl+C)
  //   2. &sigsettings - new action to install
  //   3. NULL - don't save old action
  sigaction(SIGINT,&sigsettings,NULL);

  // MAIN LOOP:
  // Now checks toContinue instead of infinite loop!
  while(toContinue){
    int choice;

    // BLOCKING READ:
    // Waits for user input
    // When SIGINT arrives:
    //   - cleanup() sets toContinue = 0
    //   - scanf() returns (because SA_RESTART not set)
    //   - Loop condition checked, loop exits
    scanf("%d",&choice);

    // PROCESS USER CHOICE:
    // Same as before, but now can exit gracefully
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

  // CLEANUP SECTION:
  // NOW REACHABLE because while loop can exit!

  // WAIT FOR HEARTBEAT THREAD:
  // pthread_join() blocks until heartbeat thread finishes
  // Heartbeat thread will exit its loop when toContinue == 0
  // This ensures clean shutdown before destroying mutex
  pthread_join(pulse,NULL);

  // DESTROY MUTEX:
  // Releases resources associated with mutex
  // Should only be called when no threads are using it
  pthread_mutex_destroy(&mut);

  // FINAL MESSAGE:
  // Show player's final score
  printf("You ended with %d $\n",income);

  return 0;
}

// SHUTDOWN SEQUENCE:
//
// 1. User presses Ctrl+C
// 2. SIGINT signal delivered to process
// 3. cleanup() handler executes:
//    - Sets toContinue = 0
//    - Returns
// 4. scanf() in main loop returns (doesn't restart because SA_RESTART not set)
// 5. Main loop checks toContinue, sees 0, exits loop
// 6. Heartbeat thread's loop checks toContinue, sees 0, exits loop
// 7. Heartbeat thread returns NULL
// 8. main() calls pthread_join(), waits for heartbeat to finish
// 9. main() destroys mutex
// 10. main() prints final message
// 11. Program exits cleanly

// SIGNAL HANDLER DESIGN PATTERNS:
//
// PATTERN 1: Minimal handler (used here)
//   - Just set a flag
//   - Let main code handle cleanup
//   - Safest approach
//
// PATTERN 2: Handler with async-safe output (see threadIncremental3.c)
//   - Set flag + print message
//   - Use write(), not printf()
//   - More user-friendly
//
// PATTERN 3: Complex handler (NOT recommended)
//   - Try to do cleanup in handler
//   - Risk of deadlock, corruption
//   - Very hard to get right

// sigaction() vs signal():
//
// signal() (old, less reliable):
//   signal(SIGINT, handler);
//   - Simple
//   - Handler might reset after first call (varies by system)
//   - Can't specify flags
//
// sigaction() (modern, reliable):
//   struct sigaction sa;
//   sa.sa_handler = handler;
//   sa.sa_flags = 0 or SA_RESTART;
//   sigaction(SIGINT, &sa, NULL);
//   - More complex
//   - Handler persists
//   - Can control restart behavior
//   - Portable behavior

// BLOCKING SYSCALLS AND SIGNALS:
//
// When a signal interrupts a blocking syscall like scanf():
//
// With SA_RESTART:
//   - System call automatically restarts
//   - Appears as if signal never happened (from syscall's perspective)
//   - User must enter input to let scanf complete
//
// Without SA_RESTART (this program):
//   - System call returns with error (EINTR)
//   - scanf() returns, allowing code to proceed
//   - Loop can check toContinue and exit
//
// This is why the comment says "makes scanf not block"
// It doesn't mean scanf becomes non-blocking
// It means scanf won't restart, allowing graceful exit

// TRY IT:
// Compile: gcc threadIncremental2.c -o threadIncremental2 -pthread
// Run: ./threadIncremental2
// Play the game for a bit
// Press Ctrl+C
// Observe:
//   1. Program exits gracefully
//   2. Final score is printed
//   3. No "Killed" message (clean shutdown)
//
// EXPERIMENT:
// 1. Add sa_flags = SA_RESTART
// 2. Recompile and run
// 3. Press Ctrl+C
// 4. Notice you must type a number to exit
// 5. This is because scanf() restarts instead of returning
