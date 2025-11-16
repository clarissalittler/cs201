// PEDAGOGICAL PURPOSE:
// This program demonstrates catching SIGINT to prevent Ctrl+C from killing the process.
// Key learning objectives:
// 1. Understanding SIGINT (the signal sent by Ctrl+C)
// 2. Installing a handler for SIGINT to override default termination behavior
// 3. Creating a process that cannot be stopped with Ctrl+C
// 4. Understanding that signal handlers give control over program termination
// 5. Learning that some signals (like SIGKILL) cannot be caught
// 6. Demonstrating the difference between catchable and uncatchable signals
// 7. Understanding write() system call for minimal output in handlers

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"     // Contains write(), sleep(), getpid()
#include "signal.h"     // Contains signal(), SIGINT

// SIGNAL HANDLER FOR SIGINT:
// This function is called when user presses Ctrl+C
// Instead of terminating (the default), we just print a message
//
// NAMING: "uninterruptable" because the handler prevents interruption
// This is somewhat of a misnomer - the process IS interrupted (handler runs)
// But it's not TERMINATED, which is what we usually mean by "interrupted"
void uninterruptable(int signum){
  // MINIMAL SIGNAL HANDLER:
  // We just print "Ouch!" and return
  // The newline at start ensures it appears on a new line
  // (User pressed Enter, so we're mid-line)
  //
  // WHY write() INSTEAD OF printf()?
  // - write() is async-signal-safe
  // - printf() is NOT safe in signal handlers
  // - write(1, ...) writes to file descriptor 1 (stdout)
  //
  // THE MAGIC NUMBER 6:
  // This is the length of the string "\nOuch!\n"
  // write() needs to know exactly how many bytes to write
  // Alternatively could use: write(1, "\nOuch!\n", strlen("\nOuch!\n"))
  write(1,"\nOuch!\n",6);

  // RETURN AND CONTINUE:
  // After printing, we return from the handler
  // Execution resumes in the main loop
  // The program keeps running - Ctrl+C didn't kill it!
}

int main(){

  // INSTALL SIGNAL HANDLER:
  // This is the key line that makes the program "uninterruptable"
  //
  // signal(SIGINT, uninterruptable) means:
  // - When SIGINT arrives (from Ctrl+C)
  // - Call the uninterruptable() function
  // - Don't do the default action (terminate)
  //
  // SIGINT - Signal INTerrupt:
  // - Sent when user presses Ctrl+C in terminal
  // - Default action: terminate the process
  // - Can be caught and handled (unlike SIGKILL)
  signal(SIGINT, uninterruptable); //install the signal handler

  // INFINITE LOOP:
  // The program runs forever, printing status every second
  // User cannot stop it with Ctrl+C (only prints "Ouch!")
  while(1){
    // BOASTFUL MESSAGE:
    // Proclaims that the process cannot be stopped
    // Shows the PID so user knows which process is running
    printf("I'm Mr. %d and I cannot be stopped!\n\n",getpid());

    // SLEEP:
    // Wait 1 second before next iteration
    // During sleep, user can press Ctrl+C
    // Sleep will be interrupted, handler runs, sleep resumes or returns
    sleep(1);
  }

  // UNREACHABLE CODE:
  // We never exit the loop
  // The only ways to stop this program:
  // 1. kill -9 <PID> (sends SIGKILL, cannot be caught)
  // 2. kill -15 <PID> (sends SIGTERM, also terminates by default)
  // 3. Close the terminal (sends SIGHUP)
  return 0;
}

// EXECUTION TRACE:
//
// Time    Action                                  Output
// ----    ------                                  ------
// T0      main() starts
// T1      Install SIGINT handler
// T2      Enter while(1) loop
// T3      printf()                                "I'm Mr. 12345 and I cannot be stopped!"
// T4      sleep(1) - process suspends
// [User presses Ctrl+C]
// T5      Keyboard sends SIGINT to process
// T6      sleep() interrupted
// T7      uninterruptable() handler called
// T8      write()                                 "\nOuch!"
// T9      Handler returns
// T10     Loop continues (sleep may or may not resume)
// T11     printf()                                "I'm Mr. 12345 and I cannot be stopped!"
// T12     sleep(1)
// [User presses Ctrl+C again]
// T13     SIGINT arrives
// T14     Handler called
// T15     write()                                 "\nOuch!"
// T16     Handler returns
// T17     Loop continues
// ...     Pattern repeats forever

// CONCEPTUAL EXPLANATION:
//
// WHAT IS SIGINT?
// SIGINT = Signal Interrupt
// - Generated when user presses Ctrl+C in the terminal
// - Default action: terminate the process
// - Intended as a user-friendly way to stop programs
// - Can be caught and handled (unlike SIGKILL)
//
// KEYBOARD TO SIGNAL TRANSLATION:
// 1. User presses Ctrl+C
// 2. Terminal driver detects this key combination
// 3. Terminal sends SIGINT to foreground process group
// 4. All processes in foreground receive SIGINT
// 5. Each process handles it according to its signal disposition

// SIGNAL DISPOSITIONS:
//
// For any signal, a process can have one of these dispositions:
//
// 1. DEFAULT (SIG_DFL):
//    - Use the signal's default action
//    - For SIGINT: terminate the process
//
// 2. IGNORE (SIG_IGN):
//    - Ignore the signal completely
//    - Signal arrives but has no effect
//    - Example: signal(SIGINT, SIG_IGN);
//
// 3. CUSTOM HANDLER:
//    - Call a specific function when signal arrives
//    - This program uses this approach
//    - Example: signal(SIGINT, uninterruptable);
//
// This program uses disposition #3 (custom handler)

// WHY MAKE A PROGRAM UNINTERRUPTABLE?
//
// LEGITIMATE USES:
// 1. Critical operations - Don't interrupt during database write
// 2. Cleanup required - Catch signal, clean up, then exit
// 3. Confirmation - Ask user "Are you sure?" before exiting
// 4. State saving - Save work before terminating
//
// EXAMPLE: Text editor
// signal(SIGINT, handle_interrupt);
//
// void handle_interrupt(int sig){
//   write(1, "\nSave changes? (y/n): ", 22);
//   // Read response
//   // If yes, save file
//   // Then exit
// }

// SIGNALS YOU CAN'T CATCH:
//
// SIGKILL (kill -9):
// - Cannot be caught, blocked, or ignored
// - Always terminates the process immediately
// - Used when process is stuck or malicious
// - No cleanup possible
//
// SIGSTOP (kill -STOP):
// - Cannot be caught, blocked, or ignored
// - Suspends (pauses) the process
// - Resume with SIGCONT
//
// These are the kernel's "emergency override"
// No process can defend against them

// HOW TO STOP THIS PROGRAM:
//
// METHOD 1: SIGKILL (kill -9)
// $ kill -9 <PID>
// Or: $ kill -SIGKILL <PID>
// - Immediately terminates
// - Cannot be caught
// - No cleanup
//
// METHOD 2: SIGTERM (kill)
// $ kill <PID>
// Or: $ kill -15 <PID>
// Or: $ kill -SIGTERM <PID>
// - Polite termination request
// - This program hasn't installed a SIGTERM handler
// - So default action applies (terminate)
//
// METHOD 3: Close the terminal
// - Terminal sends SIGHUP (hangup)
// - Default action: terminate
// - Process might become orphan instead (depends on setup)

// PRACTICAL EXAMPLE SESSION:
//
// Terminal 1:
// $ gcc uninterruptable.c -o uninterruptable
// $ ./uninterruptable
// I'm Mr. 12345 and I cannot be stopped!
//
// I'm Mr. 12345 and I cannot be stopped!
//
// [User presses Ctrl+C]
//
// Ouch!
// I'm Mr. 12345 and I cannot be stopped!
//
// [User presses Ctrl+C again]
//
// Ouch!
// I'm Mr. 12345 and I cannot be stopped!
//
// [User frustrated, opens another terminal]
//
// Terminal 2:
// $ kill -9 12345
//
// Terminal 1:
// Killed
// $

// SIGNAL HANDLING BEST PRACTICES:
//
// 1. GRACEFUL CLEANUP:
// volatile sig_atomic_t should_exit = 0;
//
// void handle_sigint(int sig){
//   should_exit = 1;
// }
//
// signal(SIGINT, handle_sigint);
// while(!should_exit){
//   // do work
// }
// // cleanup code here
//
// 2. SAFE FUNCTIONS ONLY:
// In signal handlers, only use async-signal-safe functions:
// - write() ✓
// - _exit() ✓
// - signal() ✓
// - printf() ✗
// - malloc() ✗
// - Most library functions ✗
//
// 3. MINIMAL WORK:
// Keep handlers short and simple
// Set a flag, write a message, that's it
// Don't do complex logic in handlers

// SECURITY IMPLICATIONS:
//
// DENIAL OF SERVICE:
// A program that ignores SIGINT can annoy users
// But users can always use kill -9
// Not a real security issue
//
// MALWARE:
// Malware might catch SIGINT to make it harder to kill
// But kill -9 always works
// Process limits and permissions control malicious processes
//
// SYSTEM DESIGN:
// - SIGKILL cannot be caught (kernel's ultimate power)
// - Users can always force-kill their own processes
// - Root can always force-kill any process
// - This prevents runaway processes from taking over

// COMPARISON WITH OTHER SIGNALS:
//
// SIGINT (Ctrl+C):
// - User wants to interrupt
// - Catchable
// - Polite termination request
//
// SIGTERM (kill):
// - System/user wants process to terminate
// - Catchable
// - Polite termination request
//
// SIGQUIT (Ctrl+\):
// - User wants to quit and dump core
// - Catchable
// - Usually used for debugging
//
// SIGKILL (kill -9):
// - Immediate termination
// - NOT catchable
// - Nuclear option
//
// SIGHUP (terminal close):
// - Terminal disconnected
// - Catchable
// - Daemons often use this to reload config

// COMMON QUESTIONS:
//
// Q: Why doesn't Ctrl+C work?
// A: We installed a custom handler that prints "Ouch!" instead of exiting
//
// Q: How do I stop this program?
// A: Use kill -9 <PID> or kill -15 <PID>
//
// Q: Can I make it ignore Ctrl+C completely (no "Ouch!")?
// A: Yes! Use: signal(SIGINT, SIG_IGN);
//
// Q: What if I press Ctrl+C many times quickly?
// A: Each press sends a SIGINT, handler runs each time
//    You'll see multiple "Ouch!" messages
//
// Q: Can I restore default Ctrl+C behavior?
// A: Yes! Call: signal(SIGINT, SIG_DFL);
//    After this, Ctrl+C will terminate normally
//
// Q: Is this "uninterruptable" name accurate?
// A: Not really - the handler still interrupts execution
//    "Unkillable-by-Ctrl+C" would be more accurate
//    But the process is still killable by SIGKILL

// PEDAGOGICAL VALUE:
//
// This program teaches:
// 1. Signals can be caught and handled
// 2. Handling changes program behavior
// 3. Not all termination methods can be blocked
// 4. SIGINT is catchable, SIGKILL is not
// 5. Signal handlers give fine-grained control
//
// This is a foundation for:
// - Writing robust programs (cleanup on exit)
// - Understanding process control
// - Learning system programming
// - Building daemons and servers

// TRY IT:
// Compile: gcc uninterruptable.c -o uninterruptable
// Run: ./uninterruptable
//
// Expected output:
// I'm Mr. 12345 and I cannot be stopped!
//
// I'm Mr. 12345 and I cannot be stopped!
//
// [Press Ctrl+C]
//
// Ouch!
// I'm Mr. 12345 and I cannot be stopped!
//
// [Press Ctrl+C again]
//
// Ouch!
// I'm Mr. 12345 and I cannot be stopped!
//
// Experiments:
// 1. Press Ctrl+C multiple times - see "Ouch!" each time
// 2. Try Ctrl+Z (sends SIGTSTP) - process suspends (we didn't handle it)
// 3. Resume with 'fg' command
// 4. Use kill -9 <PID> to force termination
// 5. Modify to use SIG_IGN instead - Ctrl+C does nothing (no "Ouch!")
