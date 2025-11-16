// PEDAGOGICAL PURPOSE:
// This program combines fork() with signal handling for inter-process communication.
// Key learning objectives:
// 1. Using fork() to create a child that sends signals to its parent
// 2. Child process using getppid() to find parent's PID
// 3. Child using kill() to send signals to parent
// 4. Automated signal generation (child sends signals periodically)
// 5. Parent process responds to signals with state changes
// 6. Understanding parent-child cooperation via signals
// 7. Demonstrating that signal-driven programs can be self-contained
// 8. Building on sigusr2.c by adding automated signal generation

#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "unistd.h"
#include "string.h"
#include "sys/types.h"

// SHARED STATE VARIABLE:
// Same as sigusr2.c - tracks progress toward exit
// Modified by signal handler, checked by main loop
volatile sig_atomic_t state = 0;

// SIGNAL HANDLER:
// Identical to sigusr2.c
// SIGUSR1 increments state (toward exit)
// SIGUSR2 decrements state (away from exit)
void sigh_andler(int signum){
  if(signum == SIGUSR1){
    state +=1;
    char msg[] = "I'm gonna leave soon\n";
    write(1,msg,strlen(msg));
  }
  else if(signum == SIGUSR2){
    state -= 1;
    char msg2[] = "Actually I'm vibing\n";
    write(1,msg2,strlen(msg2));
  }
}

int main(){

  // INSTALL SIGNAL HANDLERS:
  // Parent process will respond to signals
  signal(SIGUSR1,sigh_andler);
  signal(SIGUSR2,sigh_andler);

  // FORK: CREATE SIGNAL-SENDING CHILD
  // This is the key addition compared to sigusr2.c
  // The child will automatically send SIGUSR1 signals to parent
  pid_t pid = fork();

  // CHILD PROCESS CODE:
  // The child's job is to send SIGUSR1 signals to its parent
  if(pid == 0){
    // this is the child

    // SEND FIVE SIGNALS:
    // Loop 5 times, sending one SIGUSR1 each iteration
    // This will cause parent's state to reach 5 and exit
    for(int i=0; i<5;i++){
      // SEND SIGNAL TO PARENT:
      // getppid() returns parent's PID
      // kill(pid, sig) sends signal sig to process pid
      // Despite the name "kill", this doesn't necessarily terminate
      kill(getppid(), SIGUSR1);

      // WAIT BEFORE NEXT SIGNAL:
      // sleep(2) creates a 2-second delay
      // This spaces out the signals (sends one every 2 seconds)
      // Parent has time to process each signal and print status
      sleep(2);
    }
    // CHILD EXITS AFTER LOOP:
    // After sending 5 signals, child's work is done
    // Child process terminates (implicit return 0)
    // Parent may or may not still be running
  }

  // PARENT PROCESS CODE:
  // After fork(), pid contains child's PID (non-zero)
  // Parent continues with the rest of main()

  // CONDITIONAL LOOP:
  // Same as sigusr2.c - loop while state < 5
  // Child will send 5 SIGUSR1 signals, making state reach 5
  while(state < 5){
    printf("I'm Mr. %d and I'm just minding my own business...\n",getpid());
    sleep(1);

    // RECEIVING SIGNALS:
    // While parent sleeps or loops, child sends SIGUSR1
    // Signal interrupts parent, handler increments state
    // After ~10 seconds (5 signals × 2 second spacing), state reaches 5
  }

  // PARENT EXITS:
  // When state >= 5, parent exits the loop and terminates
  // At this point, child might still be alive or might have exited
  return 0;
}

// EXECUTION TRACE:
//
// Time    Parent Process                          Child Process               state
// ----    --------------                          -------------               -----
// T0      main() starts                                                       0
// T1      Install signal handlers                                             0
// T2      fork() called                           fork() called               0
// T3      fork() returns child PID                fork() returns 0            0
// T4      (skips if block)                        Enters if(pid==0) block     0
// T5      Check while(0<5) - true                 Enter for loop (i=0)        0
// T6      printf()                                kill(parent, SIGUSR1)       0
// T7      "I'm Mr. 12345..."                      sleep(2)                    0
// T8      sleep(1)                                Still sleeping              0
// T9      **SIGNAL ARRIVES**                      Still sleeping              0
// T10     sigh_andler() called                    Still sleeping              0
// T11     state += 1                              Still sleeping              1
// T12     write("I'm gonna leave soon")           Still sleeping              1
// T13     Handler returns                         Still sleeping              1
// T14     sleep() resumes                         Still sleeping              1
// T15     Check while(1<5) - true                 Still sleeping              1
// T16     printf()                                Still sleeping              1
// T17     "I'm Mr. 12345..."                      Wakes up                    1
// T18     sleep(1)                                Loop: i=1                   1
// T19     Sleeping                                kill(parent, SIGUSR1)       1
// T20     Sleeping                                sleep(2)                    1
// T21     **SIGNAL ARRIVES**                      Still sleeping              1
// T22     Handler increments state                Still sleeping              2
// T23     Handler returns                         Still sleeping              2
// ...     Pattern continues                       Continues loop              ...
// T50     Check while(5<5) - FALSE               May still be alive           5
// T51     Exit loop, return 0                     May continue sleeping        5
// T52     Parent terminates                       Child becomes orphan         5

// CONCEPTUAL EXPLANATION:
//
// SELF-CONTAINED SIGNAL DEMONSTRATION:
// This program doesn't require external signals from kill command
// It creates its own signal sender (the child process)
// This makes it a complete, self-running demonstration
//
// WHY THIS PATTERN?
// Previous programs (sigusr1.c, sigusr2.c) required user interaction:
// - Run the program
// - Note the PID
// - Manually send signals with kill command
//
// This program is autonomous:
// - Just run it
// - Watch it demonstrate signal handling automatically
// - No user interaction needed (except starting it)

// PARENT-CHILD COOPERATION:
//
// DIVISION OF LABOR:
// Parent:
// - Sets up signal handlers
// - Runs main business logic (loop with sleep)
// - Responds to signals by modifying state
// - Exits when state reaches threshold
//
// Child:
// - Sends periodic signals to parent
// - Controls timing (2 seconds between signals)
// - Determines when parent should exit (sends 5 signals)
//
// COMMUNICATION:
// - Child → Parent: SIGUSR1 signals (via kill())
// - Parent knows child's PID (from fork() return)
// - Child knows parent's PID (from getppid())

// TIMING ANALYSIS:
//
// CHILD'S SCHEDULE:
// T=0: send signal 1, sleep 2
// T=2: send signal 2, sleep 2
// T=4: send signal 3, sleep 2
// T=6: send signal 4, sleep 2
// T=8: send signal 5, sleep 2
// T=10: exit loop
//
// PARENT'S SCHEDULE:
// Every 1 second: print status
// When signal arrives: increment state, print message
// When state reaches 5: exit
//
// Expected total runtime: ~10 seconds
// (5 signals × 2 seconds spacing)

// PROCESS LIFECYCLE:
//
// PARENT LIFETIME:
// - Lives until state >= 5
// - Receives 5 signals over ~10 seconds
// - Exits normally from main
//
// CHILD LIFETIME:
// - Lives only during for loop
// - Sends 5 signals over ~10 seconds
// - Exits after loop completes
//
// ZOMBIE PROCESSES:
// When child exits before parent:
// - Child becomes zombie (process descriptor remains)
// - Parent hasn't called wait() to collect child's status
// - When parent exits, init adopts and reaps the zombie
//
// When parent exits before child:
// - Child becomes orphan
// - init (PID 1) adopts the child
// - Child continues running until its for loop completes

// COMPARISON WITH PREVIOUS PROGRAMS:
//
// sigusr1.c:
// - Manual signal sending required
// - One signal exits immediately
// - Demonstrates basic signal handling
//
// sigusr2.c:
// - Manual signal sending required
// - Multiple signals needed to exit
// - Demonstrates state accumulation
//
// sigusr3.c (this program):
// - Automatic signal sending (child process)
// - Multiple signals needed to exit
// - Demonstrates inter-process communication
// - Self-contained demonstration

// INTER-PROCESS COMMUNICATION (IPC):
//
// This program demonstrates a fundamental IPC pattern:
// - Process A (child) wants to notify Process B (parent)
// - Child uses signals as notification mechanism
// - Parent responds to notifications
//
// OTHER IPC MECHANISMS:
// - Pipes: for data streaming
// - Shared memory: for high-performance data sharing
// - Message queues: for structured messages
// - Sockets: for network communication
// - Signals: for asynchronous notifications (what we're using)
//
// WHEN TO USE SIGNALS:
// Signals are best for:
// - Simple notifications (something happened)
// - Asynchronous events (don't want to poll)
// - Small amounts of information (which signal = type of event)
//
// Signals are NOT good for:
// - Transferring data (signals don't carry much info)
// - Reliable delivery (signals can be lost)
// - Ordering guarantees (signals may arrive out of order)

// FORK + SIGNALS PATTERN:
//
// This pattern appears frequently:
// 1. Parent sets up signal handlers
// 2. Parent forks child
// 3. Child performs task and signals parent when done
// 4. Parent waits (sleeps, blocks on I/O, etc.)
// 5. Signal wakes parent or modifies parent's state
//
// Real-world examples:
// - Web server: child handles request, signals parent when done
// - Shell: child runs command, signals parent (SIGCHLD) when exits
// - Daemon: child does work, signals parent to report status

// COMMON QUESTIONS:
//
// Q: What happens if child sends signals too fast?
// A: Signals of the same type don't queue. If parent is still handling
//    SIGUSR1 and another arrives, the second might be lost.
//    The sleep(2) prevents this by spacing them out.
//
// Q: Does the child know when parent exits?
// A: Not directly. Child just sends signals and exits.
//    If parent exits early, child's kill() might fail (ESRCH error).
//
// Q: What if we want child to send SIGUSR2 instead?
// A: Change kill(getppid(), SIGUSR1) to kill(getppid(), SIGUSR2)
//    Then parent's state would decrease instead of increase
//
// Q: Can parent send signals back to child?
// A: Yes! Parent has child's PID from fork() return value
//    Parent could do: kill(pid, SIGUSR1)
//    But child would need signal handlers installed
//
// Q: Why doesn't parent wait() for child?
// A: This is a simple demonstration. In production code,
//    parent should wait() to reap child's exit status

// POTENTIAL IMPROVEMENTS:
//
// 1. Parent waits for child:
//    while(state < 5){
//      printf(...);
//      sleep(1);
//    }
//    wait(NULL);  // Clean up zombie child
//
// 2. Child checks for errors:
//    if(kill(getppid(), SIGUSR1) == -1){
//      perror("Parent died");
//      exit(1);
//    }
//
// 3. More sophisticated timing:
//    Child could send SIGUSR2 occasionally to slow parent's exit
//    This creates a dynamic interaction

// TRY IT:
// Compile: gcc sigusr3.c -o sigusr3
// Run: ./sigusr3
//
// Expected output (over ~10 seconds):
// I'm Mr. 12345 and I'm just minding my own business...
// I'm gonna leave soon
// I'm Mr. 12345 and I'm just minding my own business...
// I'm Mr. 12345 and I'm just minding my own business...
// I'm gonna leave soon
// I'm Mr. 12345 and I'm just minding my own business...
// I'm gonna leave soon
// I'm Mr. 12345 and I'm just minding my own business...
// I'm Mr. 12345 and I'm just minding my own business...
// I'm gonna leave soon
// I'm Mr. 12345 and I'm just minding my own business...
// I'm gonna leave soon
// [Program exits]
//
// Try these experiments:
// 1. Run with strace to see the signals: strace ./sigusr3
// 2. Modify sleep times to change timing
// 3. Change number of signals sent
// 4. Add SIGUSR2 signals from child to see state go down
