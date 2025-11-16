// PEDAGOGICAL PURPOSE:
// This program demonstrates using signals to modify program state and control flow.
// Key learning objectives:
// 1. Using volatile sig_atomic_t to safely share state between main and handler
// 2. Signal handlers can modify variables that affect main loop behavior
// 3. Opposing signals can increase/decrease a counter
// 4. Loop condition based on signal-modified state (signal-driven termination)
// 5. Demonstrating that signals provide external control over program logic
// 6. Understanding race-free state modification with sig_atomic_t
// 7. Building on sigusr1.c by adding stateful behavior

#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "unistd.h"
#include "string.h"
#include "sys/types.h"

// SHARED STATE VARIABLE:
// This variable is modified by signal handlers and checked in main loop
//
// WHY volatile sig_atomic_t?
// 1. volatile - tells compiler value can change unexpectedly
//    - Prevents optimization that might cache the value
//    - Forces compiler to read from memory each time
// 2. sig_atomic_t - guarantees atomic read/write operations
//    - No partial updates (no torn reads/writes)
//    - Safe to use in signal handlers
//
// THE STATE VARIABLE:
// - Starts at 0
// - SIGUSR1 increases it (moving toward exit)
// - SIGUSR2 decreases it (moving away from exit)
// - When state reaches 5, program exits
volatile sig_atomic_t state = 0;

// SIGNAL HANDLER FUNCTION:
// This handler modifies the state variable based on which signal arrived
// The state changes affect the main loop's termination condition
void sigh_andler(int signum){
  // SIGUSR1: INCREASE STATE (move toward exit)
  if(signum == SIGUSR1){
    // INCREMENT STATE:
    // Each SIGUSR1 moves us closer to exiting
    // When state reaches 5, main loop will terminate
    state +=1;

    char msg[] = "I'm gonna leave soon\n";
    write(1,msg,strlen(msg));

    // HANDLER RETURNS:
    // Unlike sigusr1.c where SIGUSR1 called exit(),
    // here we just modify state and return
    // This demonstrates indirect control over program flow
  }
  else if(signum == SIGUSR2){
    // SIGUSR2: DECREASE STATE (move away from exit)
    // This "cancels out" previous SIGUSR1 signals
    state -= 1;

    char msg2[] = "Actually I'm vibing\n";
    write(1,msg2,strlen(msg2));

    // STATE CAN GO NEGATIVE:
    // There's no check preventing state < 0
    // This means many SIGUSR2 signals require many SIGUSR1 to counter
  }
  // After modifying state, handler returns
  // Main loop will see the new state value on next iteration
}

int main(){

  // INSTALL SIGNAL HANDLERS:
  // Same handler for both signals, uses signum to distinguish
  signal(SIGUSR1,sigh_andler);
  signal(SIGUSR2,sigh_andler);

  // CONDITIONAL LOOP:
  // This is the key difference from sigusr1.c
  // The loop continues while state < 5
  // Signals control when we exit by modifying state
  //
  // WHY while(state < 5) instead of while(1)?
  // - Demonstrates external control over program flow
  // - Requires accumulation of signals (not just one)
  // - Shows state-driven termination
  while(state < 5){
    // PRINT STATUS:
    // Shows the PID for sending signals
    printf("I'm Mr. %d and I'm just minding my own business...\n",getpid());

    // SLEEP:
    // During sleep, signals can arrive and modify state
    sleep(1);

    // LOOP CONDITION CHECK:
    // After each iteration, we check if state >= 5
    // If five net SIGUSR1 signals have arrived, we exit
  }

  // NORMAL TERMINATION:
  // Unlike sigusr1.c which exits from handler,
  // here we exit normally from main
  // This happens when state reaches 5
  return 0;
  // When main returns, process terminates gracefully
}

// EXECUTION TRACE EXAMPLE:
//
// Time    Action                                  state   Output
// ----    ------                                  -----   ------
// T0      main() starts                           0
// T1      Install signal handlers                 0
// T2      Check while(0 < 5) - true              0
// T3      printf()                                0       "I'm Mr. 12345..."
// T4      sleep(1)                                0
// [User: kill -SIGUSR1 12345]
// T5      sigh_andler(SIGUSR1) called            0
// T6      state += 1                              1
// T7      write()                                 1       "I'm gonna leave soon"
// T8      Handler returns                         1
// T9      Check while(1 < 5) - true              1
// T10     printf()                                1       "I'm Mr. 12345..."
// T11     sleep(1)                                1
// [User: kill -SIGUSR2 12345]
// T12     sigh_andler(SIGUSR2) called            1
// T13     state -= 1                              0
// T14     write()                                 0       "Actually I'm vibing"
// T15     Handler returns                         0
// T16     Check while(0 < 5) - true              0
// T17     printf()                                0       "I'm Mr. 12345..."
// [User sends SIGUSR1 five times quickly]
// T18-T27 Five handlers run, state goes 1,2,3,4,5         "I'm gonna leave soon" x5
// T28     Check while(5 < 5) - FALSE             5
// T29     Exit loop                               5
// T30     return 0                                5
// [Process terminates]

// CONCEPTUAL EXPLANATION:
//
// SIGNAL-DRIVEN STATE MACHINE:
// This program is a simple state machine:
// - State starts at 0
// - SIGUSR1 transitions: state → state+1
// - SIGUSR2 transitions: state → state-1
// - When state reaches 5, program terminates
//
// STATE ACCUMULATION:
// Unlike sigusr1.c where one signal exits immediately,
// here we need FIVE net SIGUSR1 signals to exit
// "Net" means SIGUSR1 count minus SIGUSR2 count
//
// Examples:
// - 5 SIGUSR1 → state = 5 → exits
// - 10 SIGUSR1, 5 SIGUSR2 → state = 5 → exits
// - 3 SIGUSR1, 2 SIGUSR2 → state = 1 → keeps running
// - 0 SIGUSR1, 10 SIGUSR2 → state = -10 → keeps running (needs 15 SIGUSR1!)

// WHY THIS PATTERN?
//
// EXTERNAL CONTROL:
// Signals provide external control over program behavior
// Other processes can influence this program's execution
//
// APPLICATIONS:
// This pattern is useful for:
// 1. Graceful shutdown - accumulate shutdown signals before exiting
// 2. Voting systems - multiple processes vote to change state
// 3. Throttling - count up with events, count down with time
// 4. Resource limits - track resource usage via signals
//
// SAFER THAN IMMEDIATE EXIT:
// Requiring multiple signals prevents accidental termination
// One stray signal won't kill the process

// SIGNAL SAFETY AND ATOMICITY:
//
// WHY sig_atomic_t?
// Consider this without atomicity:
//
// Bad scenario (if state were a regular int):
// 1. Main loop reads state (partial read: 0x0000)
// 2. Signal arrives mid-read
// 3. Handler modifies state
// 4. Main loop finishes read (gets corrupt value)
// 5. Logic error!
//
// With sig_atomic_t:
// - Reads are atomic (all-or-nothing)
// - Writes are atomic
// - No corruption possible
//
// WHY volatile?
// Consider this without volatile:
//
// Bad scenario (if state weren't volatile):
// 1. Compiler optimizes: reads state once, caches in register
// 2. Loop uses cached value forever
// 3. Handler modifies memory
// 4. Loop never sees the change!
// 5. Infinite loop even when state >= 5!
//
// With volatile:
// - Compiler reads from memory every time
// - Always sees latest value
// - Loop terminates correctly

// COMPARISON WITH sigusr1.c:
//
// sigusr1.c:
// - SIGUSR1 → exit immediately
// - One signal, immediate effect
// - Handler calls exit()
//
// sigusr2.c:
// - SIGUSR1 → increment state
// - Five signals needed to exit
// - Handler modifies state, main loop exits naturally
// - SIGUSR2 can counter SIGUSR1
//
// KEY DIFFERENCE:
// sigusr1.c: Direct control (signal → exit)
// sigusr2.c: Indirect control (signal → state → exit)

// POTENTIAL ISSUES:
//
// NEGATIVE STATE:
// If you send many SIGUSR2 signals first:
// $ kill -SIGUSR2 12345  # state = -1
// $ kill -SIGUSR2 12345  # state = -2
// $ kill -SIGUSR2 12345  # state = -3
//
// Now you need 8 SIGUSR1 signals to exit!
// (3 to get back to 0, then 5 more to reach 5)
//
// FIX:
// Could add a check in handler:
// if(signum == SIGUSR2 && state > 0){
//   state -= 1;
// }
// This prevents state from going negative

// RACE CONDITIONS:
// This code is relatively safe because:
// 1. sig_atomic_t guarantees atomic access
// 2. Only simple increment/decrement operations
// 3. Only one variable modified
//
// However, if we had complex logic:
// if(state > 0 && state < 10){
//   state = state * 2;  // NOT SAFE!
// }
// This is NOT atomic - signal could arrive between read and write

// PRACTICAL USAGE:
//
// Terminal 1:
// $ ./sigusr2
// I'm Mr. 12345 and I'm just minding my own business...
// I'm Mr. 12345 and I'm just minding my own business...
//
// Terminal 2:
// $ kill -SIGUSR1 12345  # state: 0→1
// $ kill -SIGUSR1 12345  # state: 1→2
// $ kill -SIGUSR1 12345  # state: 2→3
//
// Terminal 1:
// I'm gonna leave soon
// I'm Mr. 12345 and I'm just minding my own business...
// I'm gonna leave soon
// I'm Mr. 12345 and I'm just minding my own business...
// I'm gonna leave soon
// I'm Mr. 12345 and I'm just minding my own business...
//
// Terminal 2:
// $ kill -SIGUSR2 12345  # state: 3→2
//
// Terminal 1:
// Actually I'm vibing
// I'm Mr. 12345 and I'm just minding my own business...
//
// Terminal 2:
// $ kill -SIGUSR1 12345  # state: 2→3
// $ kill -SIGUSR1 12345  # state: 3→4
// $ kill -SIGUSR1 12345  # state: 4→5
//
// Terminal 1:
// I'm gonna leave soon
// I'm Mr. 12345 and I'm just minding my own business...
// I'm gonna leave soon
// I'm Mr. 12345 and I'm just minding my own business...
// I'm gonna leave soon
// [Process exits normally]

// COMMON QUESTIONS:
//
// Q: Why do we need 5 signals? Why not just 1?
// A: This demonstrates state accumulation and voting
//    In real applications, you might want confirmation before major actions
//
// Q: What if signals arrive while in the handler?
// A: By default, SIGUSR1 is blocked while handling SIGUSR1
//    So you can't recursively handle the same signal
//    But SIGUSR2 can arrive while handling SIGUSR1
//
// Q: Why not use a global flag like "bool should_exit"?
// A: We could! But this teaches signal-safe state modification
//    And demonstrates accumulation pattern
//
// Q: Can state overflow?
// A: In theory yes, but sig_atomic_t is typically int-sized
//    Would need billions of signals to overflow

// TRY IT:
// Compile: gcc sigusr2.c -o sigusr2
// Run: ./sigusr2
//
// Experiments:
// 1. Send 5 SIGUSR1 signals → program exits
// 2. Send 3 SIGUSR1, 2 SIGUSR2, 4 SIGUSR1 → program exits (net: 5)
// 3. Send 10 SIGUSR2, then 15 SIGUSR1 → program exits (net: 5)
// 4. Watch the messages to understand the state changes
