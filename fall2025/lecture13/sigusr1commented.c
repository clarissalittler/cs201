// PEDAGOGICAL PURPOSE:
// This program demonstrates basic signal handling with custom signal handlers.
// Key learning objectives:
// 1. Installing custom signal handlers with signal()
// 2. Understanding that signals can change program behavior
// 3. Using SIGUSR1 and SIGUSR2 (user-defined signals)
// 4. Responding differently to different signals
// 5. Using write() instead of printf() in signal handlers (signal safety)
// 6. Understanding that signal handlers can call exit()
// 7. One handler can handle multiple different signals
// 8. Using signal handlers to gracefully terminate a program

#include "stdio.h"
#include "stdlib.h"
#include "signal.h"     // Contains signal(), SIGUSR1, SIGUSR2 definitions
#include "unistd.h"     // Contains write(), sleep(), getpid()
#include "string.h"     // Contains strlen()

// SIGNAL HANDLER FUNCTION:
// This function is called when SIGUSR1 or SIGUSR2 arrives
// The parameter signum tells us which signal triggered the call
//
// NAMING NOTE: "sigh_andler" (pun on "sigh" + "handler")
// This is a playful name; normally you'd use "signal_handler" or "handle_signal"
//
// SIGNAL HANDLER RULES:
// 1. Keep it short and simple
// 2. Only use async-signal-safe functions
// 3. Use volatile sig_atomic_t for variables modified here
// 4. Don't call malloc, printf, or other complex functions
// 5. Can call exit() to terminate the program
void sigh_andler(int signum){
  // CHECK WHICH SIGNAL WAS RECEIVED:
  // The handler needs to know which signal caused it to be called
  if(signum == SIGUSR1){
    // SIGUSR1 MEANS: Time to exit
    // Display a goodbye message and exit

    char msg[] = "I guess we're done?\n";

    // WHY write() INSTEAD OF printf()?
    // - printf() is NOT async-signal-safe
    // - printf() uses buffered I/O, which can corrupt state
    // - write() is a system call and IS safe in signal handlers
    // - File descriptor 1 is STDOUT
    write(1,msg,strlen(msg));

    // EXIT FROM SIGNAL HANDLER:
    // exit() terminates the entire process immediately
    // The main loop will never resume
    // exit(0) indicates successful termination
    exit(0); // oh yeah we hadn't seen this yet, huh
    // Note: This is the first time students see exit() in this context
  }
  else if(signum == SIGUSR2){
    // SIGUSR2 MEANS: Just acknowledge it
    // Display a message but keep running

    char msg2[] = "Did you say something?\n";

    // WRITE AND RETURN:
    // After printing, we return from the handler
    // The main program will resume where it was interrupted
    write(1,msg2,strlen(msg2));

    // When we return here, execution goes back to wherever
    // the program was when the signal arrived
  }
  // If somehow we receive a different signal, we just return
  // (This shouldn't happen based on our signal() calls)
}

int main(){

  // INSTALL SIGNAL HANDLERS:
  // signal() associates a signal with a handler function
  // Format: signal(SIGNAL_NUMBER, handler_function)
  //
  // After these calls:
  // - When SIGUSR1 arrives → sigh_andler(SIGUSR1) is called
  // - When SIGUSR2 arrives → sigh_andler(SIGUSR2) is called
  signal(SIGUSR1,sigh_andler);
  signal(SIGUSR2,sigh_andler);

  // WHY THE SAME HANDLER?
  // One handler can handle multiple signals
  // It uses the signum parameter to distinguish them
  // Alternative: could have separate handlers for each signal

  // MAIN LOOP:
  // The program runs forever, printing status every second
  // It can be interrupted by signals at any time
  while(1){
    // PRINT STATUS:
    // Shows the PID so users know what to send signals to
    // This is safe to use printf() here (not in signal handler)
    printf("I'm Mr. %d and I'm just minding my own business...\n",getpid());

    // SLEEP AND WAIT:
    // During sleep, the process can receive signals
    // Signals will interrupt sleep and call the handler
    sleep(1);

    // After sleep and any signal handling, loop continues
  }

  // UNREACHABLE CODE:
  // We never break out of the while loop
  // The only way to exit is:
  // 1. Receive SIGUSR1 (calls exit in handler)
  // 2. Receive SIGTERM/SIGINT (default behavior kills process)
  return 0;
}

// EXECUTION TRACE EXAMPLE:
//
// Time    Action                                  Output
// ----    ------                                  ------
// T0      main() starts
// T1      Install SIGUSR1 handler
// T2      Install SIGUSR2 handler
// T3      Enter while loop
// T4      printf()                                "I'm Mr. 12345..."
// T5      sleep(1) - process suspends
// [User sends: kill -SIGUSR2 12345]
// T6      Signal arrives, sleep interrupted!
// T7      sigh_andler(SIGUSR2) called
// T8      write("Did you say something?\n")       "Did you say something?"
// T9      Handler returns
// T10     sleep() may resume or return
// T11     Loop continues
// T12     printf()                                "I'm Mr. 12345..."
// T13     sleep(1)
// [User sends: kill -SIGUSR1 12345]
// T14     Signal arrives, sleep interrupted!
// T15     sigh_andler(SIGUSR1) called
// T16     write("I guess we're done?\n")          "I guess we're done?"
// T17     exit(0) - process terminates
// [Process ends]

// CONCEPTUAL EXPLANATION:
//
// WHAT ARE SIGNALS?
// Signals are asynchronous notifications sent to a process
// Think of them like software interrupts
// They can arrive at any time, interrupting normal execution
//
// SIGNAL TYPES:
// - SIGUSR1, SIGUSR2: User-defined signals (for custom purposes)
// - SIGTERM: Termination request (polite "please exit")
// - SIGKILL: Force kill (cannot be caught or ignored)
// - SIGINT: Interrupt (usually from Ctrl+C)
// - SIGSEGV: Segmentation fault
// - Many others...
//
// DEFAULT SIGNAL BEHAVIOR:
// Without custom handlers, SIGUSR1 and SIGUSR2 terminate the process
// By installing handlers, we override this default behavior
//
// SIGNAL DELIVERY:
// Signals can be sent by:
// 1. Keyboard (Ctrl+C sends SIGINT)
// 2. Operating system (SIGSEGV on invalid memory access)
// 3. kill command (kill -SIGUSR1 <PID>)
// 4. kill() system call (from another process)
// 5. Self (kill(getpid(), SIGUSR1))
//
// SIGNAL HANDLER EXECUTION:
// When a signal arrives:
// 1. Current execution is suspended (even inside system calls!)
// 2. Signal handler function is called
// 3. Handler executes
// 4. One of two things happens:
//    a) Handler calls exit() - process terminates
//    b) Handler returns - execution resumes where it was interrupted
//
// WHY TWO USER SIGNALS?
// SIGUSR1 and SIGUSR2 are provided for applications to define
// They have no predefined meaning, so programs can use them freely
// Common uses:
// - SIGUSR1: Reload configuration
// - SIGUSR2: Toggle debug mode
// - Or anything else you want!

// SIGNAL SAFETY:
//
// ASYNC-SIGNAL-SAFE FUNCTIONS:
// Only certain functions are safe to call from signal handlers
// Safe functions include:
// - write()
// - _exit() (but not exit() technically, though often used)
// - signal()
// - kill()
// - getpid()
//
// UNSAFE FUNCTIONS (don't call from handlers):
// - printf() - uses buffers, can corrupt
// - malloc()/free() - can corrupt heap
// - Most library functions
//
// WHY THE RESTRICTION?
// Imagine this scenario:
// 1. main() is in the middle of malloc()
// 2. Signal arrives, handler called
// 3. Handler calls malloc()
// 4. Now malloc's internal state is corrupted!
// 5. CRASH or corruption
//
// This is why we use write() instead of printf() in handlers

// PRACTICAL USAGE:
//
// How to test this program:
//
// Terminal 1:
// $ gcc sigusr1.c -o sigusr1
// $ ./sigusr1
// I'm Mr. 12345 and I'm just minding my own business...
// I'm Mr. 12345 and I'm just minding my own business...
// I'm Mr. 12345 and I'm just minding my own business...
//
// Terminal 2:
// $ kill -SIGUSR2 12345
//
// Terminal 1 output:
// Did you say something?
// I'm Mr. 12345 and I'm just minding my own business...
// I'm Mr. 12345 and I'm just minding my own business...
//
// Terminal 2:
// $ kill -SIGUSR1 12345
//
// Terminal 1 output:
// I guess we're done?
// [process terminates]

// COMPARISON: DIFFERENT SIGNALS, DIFFERENT BEHAVIOR
//
// SIGUSR1 → Exit gracefully
// - Handler prints message
// - Handler calls exit(0)
// - Process terminates cleanly
//
// SIGUSR2 → Acknowledge and continue
// - Handler prints message
// - Handler returns
// - Process continues running
//
// This demonstrates that signal handlers give you control
// over how your program responds to different events

// COMMON QUESTIONS:
//
// Q: Why use signals instead of checking a flag?
// A: Signals are asynchronous - they can arrive at ANY time
//    A flag requires the program to check it in a loop
//    Signals interrupt immediately, even during sleep or I/O
//
// Q: Can signals arrive during the handler?
// A: By default, a signal is blocked while its handler runs
//    (Prevents recursive handler calls)
//
// Q: What if multiple signals arrive at once?
// A: Signals of the same type don't queue (you might miss some)
//    Different signals can queue and will be handled separately
//
// Q: Can I ignore signals?
// A: Yes! signal(SIGUSR1, SIG_IGN); ignores SIGUSR1
//    You can also restore default: signal(SIGUSR1, SIG_DFL);
//
// Q: Why the naming pun "sigh_andler"?
// A: Just for fun! The program "sighs" when handling signals.
//    In real code, use descriptive names like handle_signal()

// TRY IT:
// Compile: gcc sigusr1.c -o sigusr1
// Run: ./sigusr1
//
// Experiments:
// 1. Note the PID printed
// 2. Send SIGUSR2 a few times (kill -SIGUSR2 <PID>)
//    Notice it prints "Did you say something?" and keeps running
// 3. Send SIGUSR1 once (kill -SIGUSR1 <PID>)
//    Notice it prints "I guess we're done?" and exits
// 4. Try Ctrl+C (SIGINT) - kills it immediately (no handler installed)
