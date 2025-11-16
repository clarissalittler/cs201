// PEDAGOGICAL PURPOSE:
// This program demonstrates stateful signal handling with a countdown mechanism.
// Key learning objectives:
// 1. Signal handlers can maintain state between invocations
// 2. Using volatile sig_atomic_t for variables modified in signal handlers
// 3. Signal handlers can conditionally exit (countdown pattern)
// 4. The difference between one-shot and multi-shot signal handling
// 5. Why volatile and sig_atomic_t are important for signal safety
// 6. Creating interactive signal-based behavior (countdown to "explosion")

#include <stdio.h>
#include <signal.h>      // Contains signal(), SIGINT
#include <unistd.h>      // Contains sleep(), getpid()
#include <stdlib.h>      // Contains exit()

// GLOBAL VARIABLE FOR SIGNAL HANDLER:
volatile sig_atomic_t num = 3;
// This variable is shared between main() and the signal handler
//
// WHY volatile?
// - Tells compiler the value can change at any time (by signal handler)
// - Prevents compiler from caching the value in a register
// - Forces the compiler to read from memory each time
// - Without volatile, compiler might optimize away repeated reads
//
// WHY sig_atomic_t?
// - Guaranteed to be atomic (read/write happens in one operation)
// - No risk of reading partially-updated values
// - Safe to read/write from signal handlers and main code
// - Typically an int, but guaranteed safe for signals
//
// WHAT IT REPRESENTS:
// A countdown: 3 → 2 → 1 → 0 → BOOM!
// Decremented each time SIGINT is received

// SIGNAL HANDLER FUNCTION:
void handler(int sig){
  // This handler is called each time SIGINT arrives
  // It maintains a countdown state

  // CHECK IF COUNTDOWN IS STILL ACTIVE:
  if(num > 0){
    // Still have remaining "lives"

    // ACKNOWLEDGE THE SIGNAL:
    printf("Caught that sigint!\n");
    // Tells user we received the signal

    // SHOW COUNTDOWN:
    printf("Explosion in...%d\n",num);
    // Displays how many more Ctrl+C presses until "explosion"
    // Creates suspense and interactivity

    // DECREMENT COUNTER:
    num--;
    // Reduce the countdown by 1
    // Next signal will see a smaller value
    //
    // THREAD SAFETY:
    // This is safe because:
    // 1. num is sig_atomic_t (atomic read/write)
    // 2. Only handler modifies it (no race with main)
    // 3. Signal is blocked while handler runs (no recursion)

    // RETURN TO MAIN:
    // Handler returns, program continues where interrupted
    // Usually in the sleep() call
    // Main loop continues printing "Beep boop"
  }
  else{
    // Countdown reached 0!

    // FINAL MESSAGE:
    printf("Boom!\n");
    // The "explosion" - dramatic end to the countdown

    // EXIT THE PROGRAM:
    exit(0);
    // Terminate after the countdown completes
    // User has pressed Ctrl+C enough times (4 times total)
  }
}

int main(){

  // ANNOUNCE SIGNAL HANDLER:
  printf("We have a ctrl-c handler here! I.E. SIGINT (%d)\n",SIGINT);
  // Informs user about the custom behavior

  // INSTALL SIGNAL HANDLER:
  signal(SIGINT, handler);
  // Associate SIGINT (Ctrl+C) with our handler function
  // Now each Ctrl+C will call handler() instead of killing the process

  // INFINITE LOOP:
  while(1){
    // Main program continues running until handler calls exit()

    // PRINT STATUS:
    printf("Beep boop\n");
    // Shows the program is still alive and working
    // Also provides visual feedback between Ctrl+C presses

    // WAIT:
    sleep(1);
    // Pause for 1 second
    // During sleep, signals can arrive and interrupt
  }

  // UNREACHABLE CODE:
  return 0;
  // Never reached because:
  // 1. while(1) loops forever
  // 2. Only exit is through handler calling exit()
}

// EXECUTION TRACE:
//
// Time    Action                                  num Value    Output
// ----    ------                                  ---------    ------
// T0      main() starts                           3
// T1      printf("We have a ctrl-c...")                        "We have a ctrl-c... (2)"
// T2      signal(SIGINT, handler)                 3            Handler installed
// T3      Enter while loop                        3
// T4      printf("Beep boop\n")                   3            "Beep boop"
// T5      sleep(1)                                3
// T6      printf("Beep boop\n")                   3            "Beep boop"
// T7      sleep(1)                                3
// [User presses Ctrl+C - 1st time]
// T8      SIGINT → handler(SIGINT) called         3
// T9      num > 0? Yes (3 > 0)                    3
// T10     printf("Caught that sigint!\n")         3            "Caught that sigint!"
// T11     printf("Explosion in...%d\n", 3)        3            "Explosion in...3"
// T12     num--                                   2            num becomes 2
// T13     handler returns                         2
// T14     sleep resumes/returns                   2
// T15     printf("Beep boop\n")                   2            "Beep boop"
// T16     sleep(1)                                2
// [User presses Ctrl+C - 2nd time]
// T17     SIGINT → handler(SIGINT) called         2
// T18     num > 0? Yes (2 > 0)                    2
// T19     printf("Caught that sigint!\n")         2            "Caught that sigint!"
// T20     printf("Explosion in...%d\n", 2)        2            "Explosion in...2"
// T21     num--                                   1            num becomes 1
// T22     handler returns                         1
// [User presses Ctrl+C - 3rd time]
// T23     SIGINT → handler(SIGINT) called         1
// T24     num > 0? Yes (1 > 0)                    1
// T25     printf("Caught that sigint!\n")         1            "Caught that sigint!"
// T26     printf("Explosion in...%d\n", 1)        1            "Explosion in...1"
// T27     num--                                   0            num becomes 0
// T28     handler returns                         0
// [User presses Ctrl+C - 4th time]
// T29     SIGINT → handler(SIGINT) called         0
// T30     num > 0? No (0 is not > 0)              0
// T31     else branch taken                       0
// T32     printf("Boom!\n")                       0            "Boom!"
// T33     exit(0)                                 0            Process terminates

// TYPICAL OUTPUT:
// We have a ctrl-c handler here! I.E. SIGINT (2)
// Beep boop
// Beep boop
// [Ctrl+C]
// Caught that sigint!
// Explosion in...3
// Beep boop
// Beep boop
// [Ctrl+C]
// Caught that sigint!
// Explosion in...2
// Beep boop
// [Ctrl+C]
// Caught that sigint!
// Explosion in...1
// Beep boop
// [Ctrl+C]
// Boom!
// [Program exits]

// CONCEPTUAL EXPLANATION:
//
// STATEFUL SIGNAL HANDLING:
// Unlike sig1.c which always exits on first Ctrl+C,
// this program maintains STATE across signal deliveries:
// - Each SIGINT decrements the counter
// - Handler behavior changes based on counter value
// - Creates interactive, multi-stage behavior
//
// THE COUNTDOWN PATTERN:
// Common in real applications:
// - "Press Ctrl+C again to confirm quit"
// - "Interrupt received, press again to force"
// - Giving user feedback before taking drastic action
//
// VOLATILE EXPLAINED:
// Without volatile:
//   while (num > 0) { ... }
//   Compiler might load 'num' once and never re-read it
//   Change by signal handler would be invisible!
//
// With volatile:
//   Compiler must read 'num' from memory each time
//   Changes by signal handler are visible immediately
//
// SIG_ATOMIC_T EXPLAINED:
// Why not just 'int'?
// - 'int' operations might not be atomic on all platforms
// - Reading/writing might happen in multiple steps
// - Signal could arrive mid-update → corrupted value
// - sig_atomic_t guarantees atomic access

// SIGNAL HANDLER GUARANTEES:
//
// WHILE HANDLER RUNS:
// - The signal being handled is blocked (no recursive handler calls)
// - Other signals can still arrive (unless explicitly blocked)
// - Handler has its own stack frame
// - Handler can modify volatile sig_atomic_t variables
//
// AFTER HANDLER RETURNS:
// - Execution resumes where interrupted (usually)
// - Interrupted system calls may return with EINTR
// - Some system calls auto-restart (platform-dependent)

// WHY THIS IS SAFER THAN sig1.c:
//
// sig1.c uses printf() in handler (technically unsafe)
// sig2.c also uses printf() (still unsafe, but for demo purposes)
//
// PRODUCTION CODE SHOULD USE:
// char msg[] = "Caught that sigint!\n";
// write(1, msg, strlen(msg));
//
// write() is async-signal-safe
// printf() is not (uses internal buffers that can corrupt)
//
// WE USE printf() FOR PEDAGOGY:
// - Simpler to understand
// - Works fine in single-threaded demos
// - Real-world code should use write()

// MEMORY VISIBILITY:
//
// SHARED VARIABLES:
// 'num' is accessed by both main() and handler()
// This creates a shared memory situation
//
// PROTECTION MECHANISMS:
// 1. volatile - ensures reads/writes aren't optimized away
// 2. sig_atomic_t - ensures operations are atomic
// 3. Signal blocking - prevents concurrent handler execution
//
// WHAT COULD GO WRONG WITHOUT THESE?
// - Compiler caches 'num' in register, handler change invisible
// - Non-atomic update: signal arrives mid-write, value corrupted
// - Multiple handlers run simultaneously, race condition

// COMMON QUESTIONS:
//
// Q: What if I press Ctrl+C really fast?
// A: Each signal is queued (to a point) and handled sequentially
//    Signals of the same type don't queue infinitely
//    You might lose some if pressed too rapidly
//
// Q: Can num go negative?
// A: No. When num is 0, handler calls exit()
//    No further decrements occur
//
// Q: What if main() also modified num?
// A: Could cause race conditions!
//    This is why we only modify num in the handler
//    Main only reads it (implicitly through program behavior)
//
// Q: Can I use regular int instead of sig_atomic_t?
// A: Might work on your platform, but not portable
//    sig_atomic_t guarantees safety across all platforms
//
// Q: Why initialize num to 3?
// A: Arbitrary choice - creates a 4-press sequence (3,2,1,0)
//    Could be any value - larger for more presses before exit

// COMPARISON WITH sig1.c:
//
// sig1.c:
// - No state tracking
// - First Ctrl+C always exits
// - Simpler logic
// - One-shot behavior
//
// sig2.c:
// - Tracks countdown state
// - Multiple Ctrl+C needed to exit
// - More complex logic
// - Multi-shot behavior
// - Demonstrates persistent state in handlers

// REAL-WORLD APPLICATIONS:
//
// CONFIRMATION PATTERN:
// Many programs use this pattern:
// "Ctrl+C detected. Press again to confirm quit."
// Prevents accidental termination
//
// GRACEFUL SHUTDOWN:
// First Ctrl+C: "Finishing current task..."
// Second Ctrl+C: "Forcing immediate shutdown..."
//
// DEBUGGING:
// First Ctrl+C: Toggle debug mode
// Second Ctrl+C: Dump state
// Third Ctrl+C: Exit

// TRY IT:
// Compile: gcc sig2.c -o sig2
// Run: ./sig2
//
// Expected interaction:
// We have a ctrl-c handler here! I.E. SIGINT (2)
// Beep boop
// Beep boop
// [Ctrl+C]
// Caught that sigint!
// Explosion in...3
// Beep boop
// [Ctrl+C]
// Caught that sigint!
// Explosion in...2
// Beep boop
// [Ctrl+C]
// Caught that sigint!
// Explosion in...1
// Beep boop
// [Ctrl+C]
// Boom!
//
// Experiments:
// 1. Change initial num value:
//    volatile sig_atomic_t num = 10;
//    Now requires 11 Ctrl+C presses to exit
//
// 2. Add different messages for each countdown value:
//    if (num == 3) printf("Are you sure?\n");
//    else if (num == 2) printf("Really sure?\n");
//    else if (num == 1) printf("Last chance!\n");
//
// 3. Don't exit at 0, just restart countdown:
//    if (num <= 0) num = 3;
//    Now you can never exit with Ctrl+C!
//
// 4. Print num in main loop to see it change:
//    printf("Beep boop (num=%d)\n", num);
//    Watch it decrease with each Ctrl+C
//
// 5. Try to press Ctrl+C very rapidly
//    See if you can make signals get lost (unlikely with only 4 needed)
