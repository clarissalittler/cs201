// PEDAGOGICAL PURPOSE:
// This is the RECOMMENDED approach combining the best features from previous examples.
// It uses sigaction() for reliable signal handling WITHOUT the SA_RESTART flag,
// allowing scanf() to return immediately when interrupted by a signal.
// Key learning objectives:
// 1. Best practices for signal handling in multi-threaded programs
// 2. Understanding the subtle but important difference from threadIncremental2.c
// 3. Why threadIncremental2 and threadIncremental3_sigaction are nearly identical
// 4. The importance of SA_RESTART flag (or lack thereof)
// 5. Combining reliability of sigaction() with user feedback
// 6. This is essentially threadIncremental2.c - showing it's the right approach

#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"
#include "signal.h"
#include "sys/types.h"

pthread_mutex_t mut;

volatile sig_atomic_t toContinue = 1;

int income = 0;
int gen1 = 0;
int gen2 = 0;
int gen3 = 0;

char menuString[] = "Press 1 to tick up your income\nPress 2 to buy a first-level generator (cost 10)\nPress 3 to buy a second-level generator (cost 1000)\nPress 4 to buy a third-level generator (cost 100000)\n";

// SIGNAL HANDLER:
// Simple and safe - just sets flag
// No output here (unlike threadIncremental3.c)
void cleanup(int signum){
  toContinue = 0;
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

  // ADVANCED SIGNAL HANDLING:
  // This section demonstrates the PROPER way to handle signals in threaded programs
  //
  // COMPARISON OF APPROACHES:
  //
  // signal() (threadIncremental3.c):
  //   signal(SIGINT, cleanup);
  //   - Simple but less control
  //   - Often implicitly sets SA_RESTART
  //   - scanf() restarts, user must type to exit
  //
  // sigaction() with sa_flags=0 (this file & threadIncremental2.c):
  //   struct sigaction sa;
  //   sa.sa_handler = cleanup;
  //   sa.sa_flags = 0;  // NO SA_RESTART
  //   sigaction(SIGINT, &sa, NULL);
  //   - More complex but better control
  //   - SA_RESTART explicitly NOT set
  //   - scanf() returns immediately with error
  //   - Cleaner user experience

  // COMMENT EXPLANATION:
  // "scanf blocking is a real pain here"
  // Problem: scanf() blocks waiting for input
  // Without proper signal handling, Ctrl+C wouldn't work cleanly
  //
  // "we have to use entirely different ways of setting handlers"
  // Solution: Use sigaction() with sa_flags = 0
  //
  // "in order to make it less painful"
  // Result: scanf() doesn't restart, exits quickly
  //
  // "OR we accept that you need to just type something to end the game"
  // Alternative: Use signal() and require user to type (threadIncremental3.c)

  struct sigaction sigsettings;

  // Set handler function pointer
  sigsettings.sa_handler = cleanup;

  // THE CRITICAL FLAG:
  // sa_flags = 0 means:
  //   - SA_RESTART is NOT set
  //   - Interrupted syscalls return EINTR instead of restarting
  //   - scanf() will return when signal arrives
  //   - Loop can check toContinue and exit immediately
  //
  // Compare to sa_flags = SA_RESTART:
  //   - Interrupted syscalls automatically restart
  //   - scanf() continues waiting for input
  //   - User must type something to let scanf() complete
  //   - Less responsive shutdown
  sigsettings.sa_flags = 0; // makes scanf not block

  // INSTALL HANDLER:
  // Parameters:
  //   SIGINT - handle Ctrl+C
  //   &sigsettings - new signal action
  //   NULL - don't need to save old action
  sigaction(SIGINT,&sigsettings,NULL);

  // MAIN LOOP:
  while(toContinue){
    int choice;

    // scanf() BEHAVIOR WITH sa_flags = 0:
    //
    // Normal operation:
    //   - scanf() blocks, waiting for input
    //   - User types number, scanf() returns
    //   - choice contains user's input
    //
    // When SIGINT arrives:
    //   1. scanf() is interrupted
    //   2. cleanup() sets toContinue = 0
    //   3. cleanup() returns
    //   4. scanf() returns with error (because SA_RESTART not set)
    //   5. choice contains garbage, but doesn't matter
    //   6. switch statement executes (harmless with garbage input)
    //   7. Loop condition checked: toContinue = 0
    //   8. Loop exits immediately
    //
    // This is CLEANER than requiring user to type something
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

  // ORDERLY SHUTDOWN:
  // Wait for heartbeat thread to finish
  pthread_join(pulse,NULL);

  // Clean up resources
  pthread_mutex_destroy(&mut);

  // Final message to user
  printf("You ended with %d $\n",income);

  return 0;
}

// WHY THIS FILE EXISTS (vs threadIncremental2.c):
//
// This file is NEARLY IDENTICAL to threadIncremental2.c
// The only real difference is the comments!
//
// Purpose of having both files:
//   1. Educational progression
//   2. Shows this is the "final" recommended approach
//   3. Emphasizes importance of sigaction() over signal()
//   4. Allows comparison with threadIncremental3.c (signal() version)

// EVOLUTION OF THE CODE:
//
// threadIncremental.c:
//   - Basic mutex usage
//   - No graceful shutdown
//   - Infinite loops
//
// threadIncremental2.c:
//   - Added sigaction() with sa_flags = 0
//   - Graceful shutdown
//   - scanf() doesn't restart
//
// threadIncremental3.c:
//   - Uses signal() instead
//   - Adds user message with write()
//   - scanf() restarts, needs user input
//
// threadIncremental3_sigaction.c (THIS FILE):
//   - Best practices: sigaction() with sa_flags = 0
//   - Clean shutdown without requiring input
//   - This is the RECOMMENDED approach

// DETAILED FLAG BEHAVIOR:
//
// When a signal interrupts scanf():
//
// With sa_flags = SA_RESTART:
//   scanf() -> signal arrives -> handler runs -> scanf() RESTARTS
//   User experience: Must type something after Ctrl+C
//
// With sa_flags = 0 (this file):
//   scanf() -> signal arrives -> handler runs -> scanf() RETURNS with EINTR
//   User experience: Immediate shutdown after Ctrl+C
//
// The sa_flags = 0 approach provides better UX for this use case

// THREAD SAFETY CONSIDERATIONS:
//
// 1. sig_atomic_t for flag:
//    - Ensures atomic read/write
//    - Both threads can safely check toContinue
//
// 2. Mutex for shared state:
//    - Protects income, gen1, gen2, gen3
//    - Prevents race conditions
//
// 3. Minimal signal handler:
//    - Just sets flag (safest approach)
//    - No printf(), no malloc(), no mutex operations
//    - Cannot deadlock

// COMPARISON TABLE:
//
// Feature                  | threadIncr2 | threadIncr3 | threadIncr3_sigaction
// -------------------------|-------------|-------------|----------------------
// Signal function          | sigaction   | signal      | sigaction
// SA_RESTART set?          | No          | Yes (impl.) | No
// scanf() behavior         | Returns     | Restarts    | Returns
// Needs input after Ctrl+C?| No          | Yes         | No
// User message in handler? | No          | Yes         | No
// Recommended?             | Yes         | No          | Yes

// TRY IT:
// Compile: gcc threadIncremental3_sigaction.c -o threadIncremental3_sigaction -pthread
// Run: ./threadIncremental3_sigaction
// Play the game
// Press Ctrl+C
// Observe:
//   1. Program exits almost immediately
//   2. Don't need to type anything
//   3. Final score is displayed
//   4. Clean, professional shutdown
//
// EXPERIMENT:
// Change sa_flags from 0 to SA_RESTART
// Recompile and run
// Press Ctrl+C
// Notice you now must type a number to exit
// This demonstrates the importance of the SA_RESTART flag
//
// COMPARE ALL THREE:
// Run threadIncremental.c - Ctrl+C kills it abruptly
// Run threadIncremental3.c - Ctrl+C, must type to exit
// Run threadIncremental3_sigaction.c - Ctrl+C, immediate clean exit
