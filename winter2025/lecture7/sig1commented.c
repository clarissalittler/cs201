// PEDAGOGICAL PURPOSE:
// This program demonstrates basic signal handling with SIGINT (Ctrl+C).
// Key learning objectives:
// 1. Installing a custom signal handler with signal()
// 2. Overriding default signal behavior (Ctrl+C normally kills the process)
// 3. Understanding that signals interrupt normal program flow
// 4. Using exit() to terminate from a signal handler
// 5. The difference between printf() in main vs signal handlers
// 6. Infinite loops that can only be escaped via signals
// 7. Signal handlers provide graceful shutdown opportunities

#include <stdio.h>
#include <signal.h>      // Contains signal(), SIGINT
#include <unistd.h>      // Contains sleep(), getpid()
#include <stdlib.h>      // Contains exit()

// SIGNAL HANDLER FUNCTION:
// This function is called when SIGINT (Ctrl+C) is received
void handler(int sig){
  // PARAMETER: sig
  // - Contains the signal number that triggered this handler
  // - For SIGINT, sig will equal SIGINT (typically 2)
  // - Allows one handler to handle multiple signals
  //
  // WHAT THIS HANDLER DOES:
  // 1. Prints a message acknowledging the signal
  // 2. Terminates the program gracefully

  // PRINT MESSAGE:
  printf("Caught that sigint!\n");
  // Acknowledges that we intercepted the Ctrl+C
  //
  // NOTE: printf() is NOT technically safe in signal handlers
  // It uses buffered I/O which can corrupt state
  // For this simple example it works, but production code
  // should use write() instead:
  //   write(1, "Caught that sigint!\n", 20);
  // We'll see this in more advanced examples

  // EXIT THE PROGRAM:
  exit(0);
  // Immediately terminates the entire process
  // exit(0) indicates successful termination
  //
  // WHAT HAPPENS:
  // - All cleanup routines are called (atexit handlers, etc.)
  // - All open file descriptors are closed
  // - Process is removed from the process table
  // - Parent receives exit status 0 via wait()
  //
  // WITHOUT exit():
  // If we just returned from handler, program would resume
  // the infinite loop and keep printing "Beep boop"
}

int main(){

  // ANNOUNCE SIGNAL HANDLER:
  printf("We have a ctrl-c handler here! I.E. SIGINT (%d)\n",SIGINT);
  // Informs user that this program handles Ctrl+C differently
  // Also prints the numeric value of SIGINT (usually 2)
  //
  // WHY PRINT SIGINT?
  // - Educational: shows that SIGINT is just a number
  // - Debugging: confirms the signal number on this system
  // - Usually SIGINT = 2 on all Unix-like systems

  // INSTALL SIGNAL HANDLER:
  signal(SIGINT, handler);
  // FUNCTION: signal(signal_number, handler_function)
  //
  // WHAT THIS DOES:
  // - Associates SIGINT with our handler() function
  // - Overrides the default SIGINT behavior (terminate process)
  // - Now when user presses Ctrl+C, handler() is called
  //
  // IMPORTANT:
  // This doesn't block SIGINT, it replaces the default action
  // SIGINT still arrives, but now we control what happens
  //
  // RETURN VALUE (unused here):
  // signal() returns the previous handler (usually SIG_DFL)
  // Could save it: void (*old_handler)(int) = signal(SIGINT, handler);

  // INFINITE LOOP:
  while(1){
    // This loop runs forever (or until we handle a signal)

    // PRINT STATUS:
    printf("Beep boop\n");
    // Regular output to show the program is running
    // This is the "normal operation" that continues
    // until interrupted by a signal

    // SLEEP:
    sleep(1);
    // Pause execution for 1 second
    // During sleep, the process can receive signals
    // If a signal arrives, sleep is interrupted
    // and the signal handler runs
    //
    // WHY sleep()?
    // - Prevents output from scrolling too fast
    // - Makes it easy to send signals (user has time to press Ctrl+C)
    // - Realistic: many programs spend time waiting, not computing
  }

  // UNREACHABLE CODE:
  return 0;
  // We never break out of the while(1) loop
  // The only way to exit is:
  // 1. Press Ctrl+C → SIGINT → handler() → exit(0)
  // 2. Send SIGKILL (kill -9) → immediate termination (can't catch)
  // 3. Send SIGTERM (kill) → default behavior is termination
}

// EXECUTION TRACE:
//
// Time    Action                                  Output
// ----    ------                                  ------
// T0      main() starts
// T1      printf("We have a ctrl-c...")           "We have a ctrl-c handler... (2)"
// T2      signal(SIGINT, handler)                 Handler installed
// T3      Enter while loop
// T4      printf("Beep boop\n")                   "Beep boop"
// T5      sleep(1) - process suspends
// T6      sleep ends, loop continues
// T7      printf("Beep boop\n")                   "Beep boop"
// T8      sleep(1) - process suspends
// [User presses Ctrl+C]
// T9      SIGINT arrives!
// T10     sleep() interrupted
// T11     handler(SIGINT) called
// T12     printf("Caught that sigint!\n")         "Caught that sigint!"
// T13     exit(0) - process terminates
// [Process ends]

// SIGNAL FLOW:
//
// Normal execution:    main() → while → printf → sleep → printf → sleep → ...
// User presses Ctrl+C: [Ctrl+C] → kernel → SIGINT → handler() → exit(0)
//
// BEFORE signal():
//   Ctrl+C → kernel → SIGINT → default action → process dies immediately
//
// AFTER signal():
//   Ctrl+C → kernel → SIGINT → handler() → custom action → graceful exit

// WHAT ARE SIGNALS?
//
// Signals are asynchronous notifications sent to processes
// Think of them as "software interrupts"
//
// COMMON SIGNALS:
// SIGINT (2)    - Interrupt from keyboard (Ctrl+C)
// SIGTERM (15)  - Termination request (kill command default)
// SIGKILL (9)   - Force kill (cannot be caught or ignored)
// SIGSEGV (11)  - Segmentation fault (invalid memory access)
// SIGALRM (14)  - Timer alarm
// SIGUSR1/2     - User-defined signals
//
// DEFAULT ACTIONS:
// Most signals terminate the process by default
// SIGINT's default: terminate the process
// By installing a handler, we override this default

// WHY HANDLE SIGINT?
//
// REAL-WORLD USES:
// 1. Cleanup before exit (close files, save state, etc.)
// 2. Ignore the signal (for critical operations)
// 3. Graceful shutdown (finish current task, then exit)
// 4. User confirmation ("Really quit? (y/n)")
// 5. Save work in progress
//
// EXAMPLE:
// A text editor might:
// - Catch SIGINT
// - Prompt: "Save before exiting? (y/n)"
// - Save if user says yes
// - Then exit cleanly

// SIGNAL HANDLER SAFETY:
//
// ASYNC-SIGNAL-SAFE FUNCTIONS:
// Only certain functions are safe to call from signal handlers
// Safe: write(), _exit(), signal(), kill(), getpid()
// Unsafe: printf(), malloc(), most library functions
//
// WHY THE RESTRICTION?
// Imagine:
// 1. main() is in the middle of malloc()
// 2. Signal arrives, handler is called
// 3. Handler calls malloc()
// 4. Now malloc's internal state is corrupted!
// 5. CRASH or memory corruption
//
// THIS PROGRAM:
// Uses printf() in handler (technically unsafe)
// Works for simple demos, but production code should use write()

// CONCEPTUAL EXPLANATION:
//
// WHAT HAPPENS AT Ctrl+C?
// 1. Terminal driver detects Ctrl+C keypress
// 2. Terminal sends SIGINT to foreground process group
// 3. Kernel delivers SIGINT to our process
// 4. Kernel checks if we have a handler installed
// 5. If yes: kernel calls our handler function
// 6. If no: kernel uses default action (terminate)
//
// SIGNAL DELIVERY:
// Signals can arrive at ANY TIME
// Even in the middle of a function call!
// The kernel suspends normal execution
// Runs the handler
// Then (usually) resumes where we left off
//
// INTERRUPTING SYSTEM CALLS:
// When SIGINT arrives during sleep():
// - sleep() is interrupted
// - Handler runs
// - sleep() either resumes or returns early
// Since we exit() in handler, sleep never resumes

// COMMON QUESTIONS:
//
// Q: Can I prevent Ctrl+C from killing my program?
// A: Yes! Install a handler and don't call exit()
//    Just print a message and return from handler
//    The program will continue running
//
// Q: What if I want to ignore SIGINT completely?
// A: signal(SIGINT, SIG_IGN);
//    Now Ctrl+C does nothing
//
// Q: How do I restore default behavior?
// A: signal(SIGINT, SIG_DFL);
//    Now Ctrl+C kills the process again
//
// Q: Can I catch SIGKILL?
// A: No! SIGKILL cannot be caught, blocked, or ignored
//    It always kills the process immediately
//    This ensures there's always a way to kill runaway processes
//
// Q: What happens if signal arrives twice?
// A: By default, the signal is blocked while handler runs
//    Second signal is queued and delivered after handler returns
//    (Or may be lost - signals don't queue infinitely)

// COMPARISON WITH sig2.c:
//
// sig1.c (this file):
// - Handler calls exit() - process terminates on Ctrl+C
// - Simple: Ctrl+C → message → exit
//
// sig2.c (next example):
// - Handler doesn't exit - process continues after Ctrl+C
// - Uses a counter to exit after N signals
// - Demonstrates stateful signal handling

// TRY IT:
// Compile: gcc sig1.c -o sig1
// Run: ./sig1
//
// Expected output:
// We have a ctrl-c handler here! I.E. SIGINT (2)
// Beep boop
// Beep boop
// Beep boop
// [Press Ctrl+C]
// Caught that sigint!
// [Program exits]
//
// Experiments:
// 1. Run and press Ctrl+C - see custom message before exit
//
// 2. Modify handler to NOT call exit():
//    void handler(int sig) {
//        printf("Nice try!\n");
//    }
//    Now Ctrl+C doesn't kill the program!
//
// 3. Add a counter to limit how many times it can be ignored:
//    static int count = 0;
//    if (++count >= 3) exit(0);
//
// 4. Ignore SIGINT instead of handling it:
//    signal(SIGINT, SIG_IGN);
//    Now Ctrl+C does nothing (no message, no exit)
//
// 5. Try to kill with SIGKILL:
//    In another terminal: kill -9 <PID>
//    Process dies immediately (can't catch SIGKILL)
