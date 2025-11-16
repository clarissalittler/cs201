// PEDAGOGICAL PURPOSE:
// This program demonstrates using alarm() to schedule signals and pause() to wait for them.
// Key learning objectives:
// 1. Using alarm() to schedule SIGALRM signals
// 2. Understanding that alarm() creates a timer-based signal
// 3. Using pause() to suspend execution until a signal arrives
// 4. Signal handlers can be triggered by timers, not just user input
// 5. How pause() interacts with signal handlers
// 6. Creating time-based events in programs
// 7. Simple timeout mechanisms

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>     // Contains bool type (though not used here)
#include <unistd.h>      // Contains pause(), alarm()
#include <signal.h>      // Contains signal(), SIGALRM

// ALARM SIGNAL HANDLER:
// Called when the alarm timer expires (SIGALRM arrives)
void alarm_handler(int sig){
  // ANNOUNCE ALARM:
  printf("The bells have been rung!\n");
  // Metaphorical "bells" indicate the alarm has gone off
  // This message tells us the timer expired
  //
  // NOTE: printf() in handlers is technically unsafe
  // For production code, use write() instead:
  //   write(1, "The bells have been rung!\n", 26);
  //
  // HANDLER COMPLETES:
  // After printing, this handler returns
  // Execution resumes in main() after pause()
}

int main(){
  // ANNOUNCE PROGRAM PURPOSE:
  printf("This program will end once the bells have been rung\n");
  // Tells user what to expect
  // Program will wait until alarm goes off

  // INSTALL ALARM HANDLER:
  signal(SIGALRM, alarm_handler);
  // Associates SIGALRM with our alarm_handler function
  // When alarm timer expires, alarm_handler will be called
  //
  // DEFAULT SIGALRM BEHAVIOR:
  // Without a handler, SIGALRM terminates the process
  // With our handler, we can respond gracefully

  // SET THE ALARM:
  alarm(5);
  // alarm(N) schedules a SIGALRM to be sent in N seconds
  // After 5 seconds, kernel sends SIGALRM to this process
  //
  // WHAT HAPPENS:
  // 1. Kernel starts a 5-second timer
  // 2. After 5 seconds, kernel sends SIGALRM to our process
  // 3. Our alarm_handler is called
  // 4. Timer is cancelled (one-shot, not repeating)
  //
  // IMPORTANT:
  // - Only ONE alarm can be active at a time per process
  // - Calling alarm() again resets the timer
  // - alarm(0) cancels any pending alarm
  //
  // RETURN VALUE (ignored here):
  // alarm() returns seconds remaining on previous alarm
  // If no previous alarm, returns 0

  // ANNOUNCE SLEEPING:
  printf("We slumber\n");
  // Metaphorical "sleep" - we're waiting for the alarm
  // Program is about to pause

  // WAIT FOR SIGNAL:
  pause();
  // pause() suspends execution until a signal arrives
  //
  // WHAT pause() DOES:
  // 1. Puts process to sleep (blocked state)
  // 2. Waits for ANY signal to arrive
  // 3. When signal arrives:
  //    a. Signal handler is called
  //    b. pause() returns -1 (always returns -1)
  //    c. errno is set to EINTR (interrupted)
  //
  // WHY USE pause()?
  // - Efficient: doesn't consume CPU while waiting
  // - Signal-driven: wakes up exactly when signal arrives
  // - Alternative to busy-waiting: while(!flag) {}
  //
  // WHAT HAPPENS HERE:
  // 1. pause() is called
  // 2. Process sleeps for 5 seconds
  // 3. SIGALRM arrives (alarm goes off)
  // 4. alarm_handler() is called
  // 5. alarm_handler() prints and returns
  // 6. pause() returns
  // 7. Execution continues to next line

  // ANNOUNCE AWAKENING:
  printf("We have awoken!\n");
  // Indicates that alarm has gone off and we've resumed
  // This proves that pause() returned after signal was handled
  //
  // TIMING:
  // This prints approximately 5 seconds after "We slumber"
  // The delay is exactly the alarm duration

  // PROGRAM EXITS:
  return 0;
  // Normal termination
  // Alarm was handled, pause returned, program completes
}

// EXECUTION TRACE:
//
// Time     Action                                  Output
// ----     ------                                  ------
// T0       main() starts
// T1       printf("This program...")               "This program will end once..."
// T2       signal(SIGALRM, alarm_handler)          Handler installed
// T3       alarm(5)                                5-second timer starts
// T4       printf("We slumber\n")                  "We slumber"
// T5       pause()                                 Process blocks, waiting
// [5 seconds pass - process is sleeping]
// T10      SIGALRM arrives!                        Timer expired
// T11      alarm_handler(SIGALRM) called
// T12      printf("The bells...")                  "The bells have been rung!"
// T13      alarm_handler returns
// T14      pause() returns -1
// T15      printf("We have awoken!\n")             "We have awoken!"
// T16      return 0                                Process exits

// EXPECTED OUTPUT:
// This program will end once the bells have been rung
// We slumber
// [5-second pause]
// The bells have been rung!
// We have awoken!

// TIMING DIAGRAM:
//
// Second   Process State              Event
// ------   -------------              -----
// 0        Running                    main() starts, prints, sets alarm
// 0        Blocked in pause()         Waiting for signal
// 1        Blocked in pause()         Timer counting down...
// 2        Blocked in pause()         Timer counting down...
// 3        Blocked in pause()         Timer counting down...
// 4        Blocked in pause()         Timer counting down...
// 5        Signal handler running     SIGALRM arrives, handler executes
// 5        Running                    pause() returns, continues
// 5        Exits                      return 0

// CONCEPTUAL EXPLANATION:
//
// ALARM MECHANISM:
// alarm() is a simple timer facility:
// - Schedule a signal to be sent in the future
// - One-shot (not repeating)
// - Only one alarm per process
// - Useful for timeouts and periodic actions
//
// PAUSE MECHANISM:
// pause() is for signal-driven waiting:
// - Suspends until ANY signal arrives
// - More efficient than polling (while(!flag) {})
// - CPU time given to other processes
// - Ideal for waiting for events
//
// COMBINING alarm() + pause():
// Common pattern for simple delays:
// - alarm(N): "Wake me up in N seconds"
// - pause(): "I'll sleep until you wake me"
// - Handler: "Okay, I'm awake!"
// - Continue: Process resumes execution

// ALTERNATIVE APPROACHES:
//
// INSTEAD OF pause(), COULD USE:
//
// 1. sleep(5):
//    Simpler, but less flexible
//    Can't distinguish what woke us up
//
// 2. Busy wait:
//    while (!alarm_fired) {}
//    Wastes CPU, inefficient
//
// 3. select() or poll() with timeout:
//    More complex, but more powerful
//    Can wait on I/O AND timeout
//
// 4. timer_create() + timer_settime():
//    More modern, more flexible
//    Can create multiple timers
//    More complex API

// ALARM DETAILS:
//
// ONLY ONE ALARM:
// Each process can have only one pending alarm
// If you call alarm() twice:
//   alarm(10);
//   alarm(5);
// The 10-second alarm is cancelled, replaced by 5-second
//
// CANCELLING AN ALARM:
// alarm(0);  // Cancels any pending alarm
//
// RETURN VALUE:
// unsigned int remaining = alarm(10);
// Returns seconds remaining on previous alarm (0 if none)
//
// INHERITANCE:
// Child processes created by fork() do NOT inherit pending alarms
// Child must set its own alarms
//
// EXEC:
// alarm() survives across exec() calls
// The new program inherits the alarm

// PAUSE DETAILS:
//
// RETURN VALUE:
// pause() always returns -1
// Sets errno to EINTR (interrupted system call)
//
// WAKES ON ANY SIGNAL:
// pause() doesn't care which signal arrives
// Could be SIGALRM, SIGINT, SIGUSR1, etc.
// Handler runs, then pause() returns
//
// RACE CONDITION:
// Classic bug:
//   alarm(5);
//   if (!alarm_fired) pause();  // BUG!
// What if alarm fires between the check and pause()?
// We'd block forever (alarm already fired)
//
// BETTER PATTERN:
//   signal(SIGALRM, handler);
//   alarm(5);
//   pause();  // Don't check flag, just wait
//   // Handler sets flag if needed

// SIGNAL HANDLER EXECUTION:
//
// WHEN SIGALRM ARRIVES:
// 1. Process state: blocked in pause()
// 2. Kernel delivers SIGALRM
// 3. Process state: running (in handler)
// 4. Handler executes
// 5. Handler returns
// 6. Process state: running (in main)
// 7. pause() returns -1
//
// INTERRUPT SEMANTICS:
// The signal "interrupts" pause()
// pause() doesn't complete normally
// It's interrupted by the signal
// This is why errno = EINTR

// COMMON QUESTIONS:
//
// Q: Can I use alarm() for repeating events?
// A: Not directly. alarm() is one-shot.
//    For repeating, handler must call alarm() again:
//    void handler(int sig) {
//        printf("Tick\n");
//        alarm(1);  // Schedule next alarm
//    }
//
// Q: What if alarm fires before pause()?
// A: Handler runs immediately.
//    Then pause() blocks forever (no more signals pending).
//    This is a race condition - avoid by:
//    - Setting alarm after pause() (doesn't work)
//    - Using signal blocking (more advanced)
//    - Using sleep() instead if exact timing doesn't matter
//
// Q: Can alarm() take fractional seconds?
// A: No, only whole seconds.
//    For sub-second timers, use:
//    - usleep() (microseconds, but deprecated)
//    - nanosleep() (nanoseconds)
//    - setitimer() (microsecond resolution)
//    - timer_create() (modern POSIX timers)
//
// Q: What if I press Ctrl+C during pause()?
// A: SIGINT arrives, default handler kills process.
//    If you install a SIGINT handler, it runs,
//    then pause() returns.
//
// Q: Does alarm() work during sleep()?
// A: Yes! Alarm fires even if process is sleeping.
//    sleep() returns early (interrupted).

// REAL-WORLD USES:
//
// TIMEOUTS:
// alarm(30);
// read(fd, buf, size);  // Wait for input
// alarm(0);             // Cancel alarm if read succeeds
// If read takes >30 seconds, SIGALRM fires, interrupt it
//
// PERIODIC TASKS:
// void handler(int sig) {
//     do_periodic_work();
//     alarm(60);  // Every 60 seconds
// }
//
// WATCHDOG TIMER:
// Reset alarm periodically if making progress
// If no reset, alarm fires → assume hang, take action
//
// DEBUGGING:
// alarm(10);
// // Run untrusted code
// If it hangs, alarm kills it after 10 seconds

// TRY IT:
// Compile: gcc alarms.c -o alarms
// Run: ./alarms
//
// Expected output:
// This program will end once the bells have been rung
// We slumber
// [Wait 5 seconds]
// The bells have been rung!
// We have awoken!
//
// Experiments:
// 1. Change alarm duration:
//    alarm(10);
//    Now wait 10 seconds instead of 5
//
// 2. Add a countdown in the handler:
//    static int count = 5;
//    printf("Alarm! %d\n", count--);
//    if (count > 0) alarm(1);  // Repeating alarm
//
// 3. Remove pause() and use sleep(10) instead:
//    After 5 seconds, alarm fires during sleep
//    sleep returns early, continues
//
// 4. Press Ctrl+C during the 5-second wait:
//    Process dies (SIGINT not handled)
//
// 5. Handle both SIGALRM and SIGINT:
//    signal(SIGINT, alarm_handler);
//    Press Ctrl+C to wake from pause early
//
// 6. Set alarm to 0:
//    alarm(0);
//    Now pause() blocks forever (no alarm to wake it)
//    Must Ctrl+C to exit
