// PEDAGOGICAL PURPOSE:
// This program attempts (unsuccessfully) to block the SIGSTOP signal.
// Key learning objectives:
// 1. Understanding that SIGSTOP cannot be caught, blocked, or ignored
// 2. Demonstrating signal masking with sigprocmask()
// 3. Using sigset_t to represent sets of signals
// 4. Adding signals to a signal set with sigaddset()
// 5. The difference between blockable and unblockable signals
// 6. Why the kernel enforces certain signals (SIGSTOP, SIGKILL)
// 7. This is an educational "what won't work" example

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Contains sleep(), getpid()
#include <signal.h>      // Contains signal types and functions

int main(){

  // SIGNAL SETS:
  sigset_t blocks;
  // Will hold the set of signals we want to block
  // sigset_t is an opaque type representing a set of signals
  // Don't access directly; use sigemptyset, sigaddset, etc.

  sigset_t prevs;
  // this is the type for the signal blocking flags
  // Will store the PREVIOUS signal mask (before our changes)
  // Allows us to restore the original mask later
  // Not used in this program, but good practice to save it

  // INITIALIZE SIGNAL SET:
  sigemptyset(&blocks);
  // Set the 'blocks' set to empty (no signals)
  // MUST call this before using the signal set
  // Otherwise contents are undefined
  //
  // AFTER THIS:
  // blocks = {} (empty set)

  // ADD SIGSTOP TO THE SET:
  sigaddset(&blocks,SIGSTOP);
  // Add SIGSTOP to the 'blocks' set
  // SIGSTOP = signal that suspends (stops) a process
  // Usually sent by Ctrl+Z in terminal
  //
  // AFTER THIS:
  // blocks = {SIGSTOP}
  //
  // INTENT:
  // We're trying to block SIGSTOP
  // Goal: Make this process immune to Ctrl+Z
  //
  // SPOILER:
  // This won't work! SIGSTOP cannot be blocked!

  // APPLY SIGNAL MASK:
  sigprocmask(SIG_BLOCK,&blocks,&prevs);
  // sigprocmask() modifies the process's signal mask
  //
  // PARAMETERS:
  // 1. SIG_BLOCK: Add signals in 'blocks' to current mask
  // 2. &blocks: Set of signals to add
  // 3. &prevs: Where to store the previous mask (optional, can be NULL)
  //
  // OTHER OPTIONS FOR PARAMETER 1:
  // - SIG_UNBLOCK: Remove signals from mask
  // - SIG_SETMASK: Replace mask entirely
  //
  // WHAT IT DOES:
  // Tells the kernel: "Don't deliver SIGSTOP to this process"
  //
  // WHAT ACTUALLY HAPPENS:
  // Kernel ignores the request!
  // SIGSTOP cannot be blocked (special kernel rule)
  // The sigprocmask() call succeeds, but has no effect on SIGSTOP
  //
  // WHY DOES KERNEL IGNORE IT?
  // SIGSTOP and SIGKILL are special:
  // - Always deliverable
  // - Cannot be caught, blocked, or ignored
  // - Ensures system administrator can always control processes
  // - Prevents malicious programs from being unkillable

  // the last is an optional argument to save what the previous state of the blocked signals datatype was

  // INFINITE LOOP:
  while(1){
    // Run forever (until stopped externally)

    // PRINT STATUS:
    printf("Here I am!: %d\n",getpid());
    // Show we're running and display our PID
    // PID is useful for sending signals from another terminal
    //
    // WHAT TO EXPECT:
    // This will print every second
    // Even though we "blocked" SIGSTOP, Ctrl+Z will still work!

    // WAIT:
    sleep(1);
    // Pause for 1 second between prints
    // Makes output readable and gives time to test Ctrl+Z
  }

  // UNREACHABLE CODE:
  return 0;
  // Never reached because while(1) loops forever
}

// EXECUTION TRACE (with Ctrl+Z attempt):
//
// Terminal:
// $ gcc stopTest.c -o stopTest
// $ ./stopTest
// Here I am!: 12345
// Here I am!: 12345
// Here I am!: 12345
// [User presses Ctrl+Z]
// ^Z
// [1]+ Stopped    ./stopTest
// $
//
// WHAT HAPPENED:
// Despite sigprocmask(SIG_BLOCK, SIGSTOP, ...):
// - Ctrl+Z sent SIGTSTP (actually, not SIGSTOP directly)
// - Process stopped anyway!
// - Shell shows "Stopped" message
// - Prompt returns
//
// IMPORTANT CLARIFICATION:
// Ctrl+Z actually sends SIGTSTP, not SIGSTOP!
// SIGTSTP CAN be blocked or caught
// SIGSTOP cannot (but is rarely sent directly)
//
// See stopTest2.c for handling SIGTSTP

// CONCEPTUAL EXPLANATION:
//
// SIGNAL MASKING:
// Each process has a "signal mask" - a set of blocked signals
// When a signal is blocked:
// - Kernel doesn't deliver it immediately
// - Signal becomes "pending"
// - Delivered later when unblocked
//
// SIGNAL SETS:
// sigset_t represents a set of signals
// Think of it like a bitset or boolean array
// Operations:
// - sigemptyset(&set) - clear all
// - sigfillset(&set) - add all
// - sigaddset(&set, SIG) - add one signal
// - sigdelset(&set, SIG) - remove one signal
// - sigismember(&set, SIG) - check if present
//
// SIGNAL MASK OPERATIONS:
// SIG_BLOCK: current_mask = current_mask ∪ new_signals
// SIG_UNBLOCK: current_mask = current_mask - remove_signals
// SIG_SETMASK: current_mask = new_signals
//
// BLOCKED vs IGNORED:
// Blocked: Signal is pending, will be delivered when unblocked
// Ignored: Signal is discarded immediately (signal(SIG, SIG_IGN))

// THE UNKILLABLE SIGNALS:
//
// SIGSTOP AND SIGKILL:
// These two signals are ALWAYS delivered:
// - Cannot be caught (no handler can intercept)
// - Cannot be blocked (sigprocmask has no effect)
// - Cannot be ignored (signal(SIG_IGN) doesn't work)
//
// WHY?
// System administrators must be able to:
// - SIGSTOP: Pause any process (for debugging, resource control)
// - SIGKILL: Terminate any process (kill -9)
// If programs could block these, malicious code could run forever
//
// SECURITY IMPLICATION:
// Even root-owned processes can't evade SIGKILL
// Prevents runaway processes from taking over the system
//
// OTHER UNBLOCKABLE ASPECTS:
// SIGSTOP and SIGKILL also can't have handlers installed
// signal(SIGSTOP, handler) fails (returns SIG_ERR)
// signal(SIGKILL, handler) fails (returns SIG_ERR)

// SIGSTOP vs SIGTSTP:
//
// SIGSTOP:
// - Cannot be caught, blocked, or ignored
// - Usually sent by kill -STOP <pid>
// - Unconditionally stops the process
//
// SIGTSTP:
// - CAN be caught, blocked, or ignored!
// - Sent by Ctrl+Z (terminal stop)
// - Default action is to stop the process
// - But programs can override this
//
// CONFUSION:
// This program tries to block SIGSTOP
// But Ctrl+Z sends SIGTSTP (which CAN be caught)
// To make this program work as intended, need to handle SIGTSTP
// See stopTest2.c for the correct approach

// WHY THIS PROGRAM?
//
// PEDAGOGICAL VALUE:
// Shows what WON'T work
// Demonstrates that some signals are truly unstoppable
// Teaches the distinction between SIGSTOP and SIGTSTP
// Motivates the next example (stopTest2.c)
//
// COMMON MISCONCEPTION:
// "I'll block SIGSTOP so users can't Ctrl+Z my program"
// Reality: Ctrl+Z sends SIGTSTP (blockable), not SIGSTOP
// But even if you handle SIGTSTP, kill -STOP always works

// SIGNAL MASKING USE CASES:
//
// WHEN TO BLOCK SIGNALS:
// 1. Critical sections: Block signals during sensitive operations
//    sigprocmask(SIG_BLOCK, &set, &old);
//    // Critical code here
//    sigprocmask(SIG_SETMASK, &old, NULL);
//
// 2. Avoid race conditions:
//    Block signal before checking flag, unblock after
//
// 3. Defer handling:
//    Block now, handle later at convenient time
//
// WHAT TO BLOCK:
// User-defined signals (SIGUSR1, SIGUSR2)
// Application-specific signals
// NOT SIGSTOP or SIGKILL (can't be blocked anyway)

// COMMON QUESTIONS:
//
// Q: Why doesn't this program prevent Ctrl+Z?
// A: It tries to block SIGSTOP, but Ctrl+Z sends SIGTSTP.
//    SIGTSTP CAN be caught/blocked (see stopTest2.c).
//    Also, SIGSTOP can't be blocked anyway.
//
// Q: Can I make my program truly unstoppable?
// A: No. SIGKILL (kill -9) always works.
//    This is by design - ensures system control.
//
// Q: What happens to blocked signals?
// A: They become "pending" and are delivered when unblocked.
//    Exception: Some signals merge (multiple SIGINTs → one SIGINT).
//
// Q: How do I see which signals are blocked?
// A: sigprocmask(SIG_BLOCK, NULL, &current);
//    Then check with sigismember(&current, SIGINT).
//
// Q: Can child processes inherit signal mask?
// A: Yes! Signal mask is inherited across fork().
//    But reset across exec().

// TESTING THIS PROGRAM:
//
// Terminal 1:
// $ ./stopTest
// Here I am!: 12345
// Here I am!: 12345
// [Press Ctrl+Z]
// ^Z
// [1]+ Stopped    ./stopTest
//
// The process DOES get stopped, despite our attempt to block SIGSTOP!
//
// Terminal 2 (while program is running):
// $ kill -STOP 12345    # Send SIGSTOP directly
// [Process stops]
//
// $ fg                  # Resume in foreground
// Here I am!: 12345
// Here I am!: 12345
//
// $ kill -KILL 12345    # Force kill
// [Process dies immediately]

// COMPARISON WITH stopTest2.c:
//
// stopTest.c (this file):
// - Tries to block SIGSTOP (doesn't work)
// - Ctrl+Z still stops the process
// - Demonstrates what NOT to do
//
// stopTest2.c:
// - Handles SIGTSTP (the actual Ctrl+Z signal)
// - Successfully prevents Ctrl+Z from stopping
// - Shows the correct approach
// - But still can't prevent kill -STOP

// TRY IT:
// Compile: gcc stopTest.c -o stopTest
// Run: ./stopTest
//
// Observe the PID printed, then:
//
// Experiment 1 - Try Ctrl+Z:
// [In the running program, press Ctrl+Z]
// Process stops! (despite our attempt to block)
//
// Experiment 2 - Resume:
// $ fg
// Process resumes
//
// Experiment 3 - Send SIGSTOP directly:
// [While running, in another terminal]
// $ kill -STOP <PID>
// Process stops
//
// Experiment 4 - Send SIGKILL:
// $ kill -KILL <PID>
// Process dies (unkillable signal works)
//
// Experiments:
// 1. Print the signal mask to verify SIGSTOP is "blocked":
//    sigset_t current;
//    sigprocmask(0, NULL, &current);
//    if (sigismember(&current, SIGSTOP))
//        printf("SIGSTOP is blocked (but it won't matter!)\n");
//
// 2. Try blocking other signals that CAN be blocked:
//    sigaddset(&blocks, SIGINT);
//    Now Ctrl+C won't work! (but Ctrl+Z still will via SIGTSTP)
//
// 3. Compare with stopTest2.c which handles SIGTSTP correctly
//
// 4. Try to install a handler for SIGSTOP:
//    signal(SIGSTOP, handler);  // This fails!
//    Check return value: if (signal(...) == SIG_ERR) perror("signal");
//
// 5. Use 'kill -l' to see all signals:
//    $ kill -l
//    Notice SIGSTOP (19) and SIGTSTP (20) are different
