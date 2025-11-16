// PEDAGOGICAL PURPOSE:
// This program demonstrates an incremental game using signals and fork() for inter-process communication.
// Key learning objectives:
// 1. Using fork() to create a separate "timer" process that sends periodic signals
// 2. Signal-based communication between parent and child processes
// 3. Using SIGUSR1 to trigger periodic updates (heartbeat pattern)
// 4. Understanding volatile sig_atomic_t for signal-safe shared variables
// 5. Using write() instead of printf() inside signal handlers (signal safety)
// 6. Implementing a game loop that responds to both user input and periodic events
// 7. Using kill() to send signals to another process
// 8. Understanding the parent-child relationship and getppid()

#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "sys/types.h"
#include "signal.h"
#include "string.h"

// SIGNAL-SAFE GLOBAL VARIABLES:
// These variables are accessed both in main and in the signal handler
// volatile sig_atomic_t ensures these are safe to access from signal handlers
//
// WHY volatile?
// - Tells compiler the value can change unexpectedly (from signal handler)
// - Prevents compiler optimizations that cache the value
//
// WHY sig_atomic_t?
// - Guarantees atomic reads/writes (no partial updates)
// - Standard type for variables modified in signal handlers
volatile sig_atomic_t income = 0;   // Player's current money
volatile sig_atomic_t gen1 = 0;     // Number of tier-1 generators owned
volatile sig_atomic_t gen2 = 0;     // Number of tier-2 generators owned
volatile sig_atomic_t gen3 = 0;     // Number of tier-3 generators owned

// MENU STRING:
// Defined as global constant to avoid recreating it repeatedly
// Shows player options for the incremental game
char menuString[] = "Press 1 to tick up your income\nPress 2 to buy a first-level generator (cost 10)\nPress 3 to buy a second-level generator (cost 1000)\nPress 4 to buy a third-level generator (cost 100000)\n";


// SIGNAL-SAFE PRINTING FUNCTION:
// This function displays the current game state
// Uses write() and sprintf() which are safer in signal context than printf()
//
// WHY write() instead of printf()?
// - printf() uses buffered I/O and is NOT signal-safe
// - write() is a system call and IS signal-safe
// - Signal handlers should only use async-signal-safe functions
void printMenu(){
  char resources[100];
  // sprintf() formats the game state into a buffer
  // We then use write() to output it atomically
  sprintf(resources,"Money %d\nGen1: %d\nGen2: %d\nGen3: %d\n",income,gen1,gen2,gen3);
  write(STDOUT_FILENO,resources,strlen(resources));
  write(STDOUT_FILENO,menuString,strlen(menuString));
}


// SIGNAL HANDLER: Heartbeat Function
// This function is called every second when the child sends SIGUSR1
// It updates the game state based on generator production
//
// SIGNAL HANDLER RULES:
// 1. Only use async-signal-safe functions (write, not printf)
// 2. Only modify volatile sig_atomic_t variables
// 3. Keep it short and simple
// 4. Don't call malloc, printf, or other unsafe functions
void heartbeat(int signumb){
  printMenu();

  // INCREMENTAL GAME LOGIC:
  // Each generator tier produces resources for the tier below
  // Tier-3 generators produce tier-2 generators
  gen2 += gen3;
  // Tier-2 generators produce tier-1 generators
  gen1 += gen2;
  // Tier-1 generators produce income
  income += gen1;

  // This creates an exponential growth pattern:
  // More high-tier generators → more low-tier generators → more income
}


int main(){

  // FORKING A TIMER PROCESS:
  // fork() creates a child process that will act as a periodic timer
  // The child's job: send SIGUSR1 to parent every second
  pid_t pinger = fork();

  // CHILD PROCESS CODE:
  // After fork(), pinger == 0 in the child process
  if(pinger == 0){
    // INFINITE TIMER LOOP:
    while(1){
      // GET PARENT'S PID:
      // getppid() returns the parent process ID
      // The child needs this to know WHERE to send signals
      pid_t parent = getppid();

      // WAIT ONE SECOND:
      // This creates the "heartbeat" - one tick per second
      sleep(1);

      // SEND SIGNAL TO PARENT:
      // kill() sends a signal to another process
      // Despite the name, it doesn't always kill - it can send any signal
      // Here we send SIGUSR1 (user-defined signal 1)
      kill(parent,SIGUSR1);

      // WHY THIS PATTERN?
      // - Separates timing logic from game logic
      // - Parent can focus on user input
      // - Child provides regular "ticks"
      // - Demonstrates inter-process communication
    }
    // Child never exits (infinite loop)
  }

  // PARENT PROCESS CODE:
  // After fork(), pinger contains the child's PID in the parent process

  // INSTALL SIGNAL HANDLER:
  // signal() registers heartbeat() as the handler for SIGUSR1
  // Now when SIGUSR1 arrives, heartbeat() will be called
  signal(SIGUSR1,heartbeat);

  // SHOW INITIAL MENU:
  printMenu();

  // MAIN GAME LOOP:
  // Parent process handles user input while child sends periodic signals
  while(1){
    int choice;

    // READ USER INPUT:
    // scanf() blocks until user enters a number
    // While waiting, the heartbeat signal handler can still interrupt
    scanf("%d",&choice);

    // PROCESS USER CHOICE:
    switch(choice){
    case 1:
      // MANUAL INCOME INCREASE:
      // Player clicks to earn 1 money
      income++;
      break;

    case 2:
      // BUY TIER-1 GENERATOR:
      // Cost: 10 income
      // Benefit: Generates 1 income per second
      if(income >= 10){
	income = income -10;
	gen1++;
      }
      break;

    case 3:
      // BUY TIER-2 GENERATOR:
      // Cost: 1000 income
      // Benefit: Generates 1 tier-1 generator per second
      if(income >= 1000){
	income = income - 1000;
	gen2++;
      }
      break;

    case 4:
      // BUY TIER-3 GENERATOR:
      // Cost: 100000 income
      // Benefit: Generates 1 tier-2 generator per second
      if(income >= 100000){
	income -= 100000;
	gen3++;
      }
    }
    // Note: No default case, invalid input is ignored
    // Note: No break after case 4 (falls through, but loop continues)
  }

  // UNREACHABLE CODE:
  // The infinite loop means we never reach here
  // In a real program, we'd want a way to exit and clean up the child

  return 0;
}

// EXECUTION TRACE EXAMPLE:
//
// Time    Parent Process              Child Process
// ----    --------------              -------------
// T0      Starts, initializes vars
// T1      fork() called               fork() returns
// T2      fork() returns (child PID)  Enters while(1) loop
// T3      Installs signal handler     Gets parent PID
// T4      printMenu()                 sleep(1)
// T5      scanf() blocks...           Wakes up
// T6      Still waiting...            kill(parent, SIGUSR1)
// T7      INTERRUPTED by signal!      sleep(1) again
// T8      heartbeat() executes        Still sleeping
// T9      printMenu() in handler      Still sleeping
// T10     Updates gen2, gen1, income  Still sleeping
// T11     Returns from handler        Still sleeping
// T12     scanf() resumes             Wakes up
// T13     User enters "1"             kill(parent, SIGUSR1)
// T14     income++                    sleep(1)
// T15     Loop continues              Continues loop
// ...     Pattern repeats             Pattern repeats

// CONCEPTUAL EXPLANATION:
//
// SIGNAL-BASED TIMER PATTERN:
// This program demonstrates a common Unix pattern:
// - Create a child process as a dedicated timer
// - Child sends periodic signals to parent
// - Parent installs handler to respond to signals
// - Parent's main loop handles other tasks
//
// WHY FORK FOR TIMING?
// Alternative approaches and why this is better:
// 1. Could use alarm() - but less flexible, only one alarm at a time
// 2. Could use select/poll - more complex, overkill for simple timing
// 3. Fork approach - clean separation of concerns, easy to understand
//
// SIGNAL SAFETY CONCERNS:
// Signal handlers can interrupt code at ANY time, including:
// - In the middle of scanf()
// - In the middle of printf()
// - In the middle of malloc()
// This is why we use:
// - volatile sig_atomic_t for shared data
// - write() instead of printf() in handlers
// - Simple operations only in handlers
//
// GAME MECHANICS:
// This is an "idle game" or "incremental game":
// - Player starts clicking manually (case 1)
// - Buys generators to automate income
// - Higher-tier generators produce lower-tier generators
// - Creates exponential growth
//
// INTER-PROCESS COMMUNICATION:
// This demonstrates two IPC mechanisms:
// 1. Signals - for asynchronous events (timer ticks)
// 2. Parent-child relationship - child knows parent's PID via getppid()
//
// RACE CONDITIONS:
// This code has potential race conditions:
// - income, gen1, gen2, gen3 accessed in both main and handler
// - sig_atomic_t guarantees atomic access, but not complex operations
// - For production code, might need more sophisticated synchronization
//
// COMMON QUESTIONS:
//
// Q: Why doesn't the parent wait for the child?
// A: The parent runs an infinite loop handling user input. The child
//    also runs an infinite loop sending signals. Neither ever exits.
//
// Q: What happens when we Ctrl+C?
// A: By default, SIGINT kills both processes. Both terminate.
//
// Q: Could the child send signals too fast?
// A: No, sleep(1) ensures exactly 1 signal per second.
//
// Q: What if scanf() is interrupted by a signal?
// A: scanf() will be interrupted, the handler runs, then scanf() resumes.
//    This is called "automatic restart" of system calls.
//
// Q: Why not just use threads instead of fork?
// A: This is a pedagogical example teaching signals and processes.
//    Threads would be simpler but teach different concepts.

// TRY IT:
// Compile: gcc forkIncremental.c -o forkIncremental
// Run: ./forkIncremental
//
// Try this:
// 1. Just watch for 10 seconds (nothing happens - no generators yet)
// 2. Press 1 a few times to get income to 10
// 3. Press 2 to buy a tier-1 generator
// 4. Watch as income increases by 1 every second
// 5. Save up to 1000 and buy tier-2 generator
// 6. Watch exponential growth!
//
// Notice how the menu reprints every second (from heartbeat signal)
