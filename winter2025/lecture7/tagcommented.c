// PEDAGOGICAL PURPOSE:
// This program demonstrates inter-process communication using signals between parent and child.
// Key learning objectives:
// 1. Processes can send signals to each other using kill()
// 2. Parent and child can communicate through SIGUSR1 and SIGUSR2
// 3. Each process can track how many times it's been signaled
// 4. Signal handlers can use separate counters for different processes
// 5. The kill() system call sends signals to specific PIDs
// 6. Loop synchronization between parent and child processes
// 7. Proper process cleanup with SIGTERM and wait()

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>      // Contains fork(), getpid(), getppid(), sleep()
#include <signal.h>      // Contains signal(), kill(), SIGUSR1, SIGUSR2, SIGTERM
#include <sys/wait.h>    // Contains wait()
#include <sys/types.h>   // Contains pid_t

/*
  This program will fork and then have the parent send SIGUSR1 to the child
  and the child will send SIGUSR2 to the parent and we'll handlers for each
 */

// GLOBAL COUNTER:
volatile sig_atomic_t num = 0;
// Counts how many times THIS process has been "tagged" (received signals)
// IMPORTANT: Each process has its own copy after fork()
// - Parent's 'num' counts SIGUSR2 signals (tagged by child)
// - Child's 'num' counts SIGUSR1 signals (tagged by parent)
//
// WHY volatile sig_atomic_t?
// - volatile: Ensures value is read from memory each time (not cached)
// - sig_atomic_t: Guarantees atomic read/write (signal-safe)

// CHILD'S SIGNAL HANDLER:
// Called when child receives SIGUSR1 from parent
void handler1(int sig){
  // this will be for the child receiving sigusr1

  // REPORT BEING TAGGED:
  printf("I've been tagged by my parent %d times\n",num);
  // Shows how many times parent has "tagged" the child
  // First time: "...0 times" (num hasn't been incremented yet)
  // Second time: "...1 times"
  // And so on...

  // INCREMENT COUNTER:
  num++;
  // Track that we've been tagged one more time
  // This counter controls the child's loop
}

// PARENT'S SIGNAL HANDLER:
// Called when parent receives SIGUSR2 from child
void handler2(int sig){
  // REPORT BEING TAGGED:
  printf("I've been tagged by my child %d times\n",num);
  // Shows how many times child has "tagged" the parent

  // INCREMENT COUNTER:
  num++;
  // Track that we've been tagged one more time
  // This counter controls the parent's loop
}

int main(){

  // INSTALL SIGNAL HANDLERS:
  signal(SIGUSR1,handler1);
  // When SIGUSR1 arrives, call handler1
  // (Child will receive SIGUSR1 from parent)

  signal(SIGUSR2,handler2);
  // When SIGUSR2 arrives, call handler2
  // (Parent will receive SIGUSR2 from child)
  //
  // NOTE: Both parent and child install both handlers
  // But parent only receives SIGUSR2, child only receives SIGUSR1

  // FORK TO CREATE CHILD:
  int pid = fork();
  // After fork: two processes exist
  // Both have handlers installed
  // Both have num = 0 (but separate copies)

  // ERROR HANDLING:
  if(pid < 0){
    // Fork failed
    return 1;
  }

  // CHILD PROCESS CODE:
  else if(pid == 0){
    // child logic

    // CHILD'S MAIN LOOP:
    while(num < 5){
      // Continue until we've been tagged 5 times by parent
      // 'num' is incremented by handler1 each time SIGUSR1 arrives

      // TAG THE PARENT:
      kill(getppid(),SIGUSR2);
      // kill(PID, SIGNAL) - Send signal to a process
      // getppid() - Get parent's PID
      // Sends SIGUSR2 to parent → parent's handler2 will be called
      //
      // WHY "kill"?
      // Historic name - doesn't necessarily kill the process
      // It's the general "send signal" system call

      // WAIT BEFORE NEXT TAG:
      sleep(1);
      // Pause for 1 second
      // This:
      // - Slows down the "tagging" so we can observe it
      // - Gives parent time to tag us back
      // - Creates a back-and-forth rhythm
    }

    // CHILD EXITS:
    return 0;
    // After being tagged 5 times, child exits normally
    //
    // NOTE: Child exits when num reaches 5
    // This happens after receiving 5 SIGUSR1 signals from parent
  }

  // PARENT PROCESS CODE:
  else{
    // parent logic

    // PARENT'S MAIN LOOP:
    while(num < 5){
      // Continue until we've been tagged 5 times by child

      // TAG THE CHILD:
      kill(pid,SIGUSR1);
      // Send SIGUSR1 to child (using child's PID from fork)
      // Child's handler1 will be called

      // WAIT BEFORE NEXT TAG:
      sleep(1);
      // Pause for 1 second
      // Gives child time to tag us back
    }

    // TERMINATE THE CHILD:
    kill(pid,SIGTERM);
    // After our loop ends (num >= 5), tell child to terminate
    // SIGTERM is a polite "please exit" signal
    //
    // WHY SEND SIGTERM?
    // - Child might still be in its loop (race condition)
    // - Child might be sleeping
    // - SIGTERM ensures child exits so we can wait() for it
    //
    // DEFAULT SIGTERM BEHAVIOR:
    // Terminates the process (we didn't install a handler for it)

    // WAIT FOR CHILD TO FINISH:
    wait(0);
    // wait(0) is equivalent to wait(NULL)
    // Blocks until child terminates
    // Collects child's exit status (we ignore it)
    // Prevents child from becoming a zombie
    //
    // AFTER wait() RETURNS:
    // Child is fully cleaned up
    // We can exit safely
  }

  // PARENT EXITS:
  return 0;
  // Parent returns after child has been cleaned up
}

// EXECUTION TRACE:
//
// Time    Process    num    Action                          Output
// ----    -------    ---    ------                          ------
// T0      Parent     0      Install handlers
// T1      Parent     0      fork()
// T2      Parent     0      pid > 0, enter parent code
//         Child      0      pid == 0, enter child code
// T3      Child      0      num < 5? Yes
//         Parent     0      num < 5? Yes
// T4      Child      0      kill(parent, SIGUSR2)
//         Parent     0      kill(child, SIGUSR1)
// T5      Parent     0      SIGUSR2 arrives → handler2
//         Child      0      SIGUSR1 arrives → handler1
// T6      Parent            printf("...by child 0 times")   "I've been tagged by child 0 times"
//         Child             printf("...by parent 0 times")  "I've been tagged by parent 0 times"
// T7      Parent     1      num++
//         Child      1      num++
// T8      Both       1      sleep(1)
// T9      Both       1      Loop continues, num < 5? Yes
// T10     Both       1      kill(...) send signals
// T11     Both            Handlers called
// T12     Parent            printf("...by child 1 times")   "I've been tagged by child 1 times"
//         Child             printf("...by parent 1 times")  "I've been tagged by parent 1 times"
// T13     Both       2      num++
// [Pattern continues: send signal, handle, increment, sleep, repeat]
// ...
// T?      Parent     5      num < 5? No, exit loop
//         Child      5      num < 5? No, exit loop
// T?      Parent            kill(child, SIGTERM)
// T?      Child             Receives SIGTERM, terminates
// T?      Parent            wait(0) collects child
// T?      Parent            return 0

// TYPICAL OUTPUT (order may vary slightly):
// I've been tagged by my parent 0 times
// I've been tagged by my child 0 times
// I've been tagged by my parent 1 times
// I've been tagged by my child 1 times
// I've been tagged by my parent 2 times
// I've been tagged by my child 2 times
// I've been tagged by my parent 3 times
// I've been tagged by my child 3 times
// I've been tagged by my parent 4 times
// I've been tagged by my child 4 times

// CONCEPTUAL EXPLANATION:
//
// THE "TAG" GAME:
// Parent and child play a game of tag:
// 1. Parent sends SIGUSR1 to child ("tag, you're it!")
// 2. Child receives signal, increments counter
// 3. Child sends SIGUSR2 to parent ("tag back!")
// 4. Parent receives signal, increments counter
// 5. Repeat until both have been tagged 5 times
//
// SIGNAL-BASED IPC:
// This demonstrates using signals for inter-process communication:
// - Processes can signal each other
// - Each signal can carry minimal information (which signal?)
// - Handlers can respond to signals
// - Useful for simple notifications, not data transfer
//
// WHY TWO DIFFERENT SIGNALS?
// - SIGUSR1 for parent→child
// - SIGUSR2 for child→parent
// This makes it clear who is signaling whom
// Could use same signal with different handlers, but this is clearer

// SIGNAL DELIVERY WITH kill():
//
// FUNCTION SIGNATURE:
// int kill(pid_t pid, int sig);
//
// PARAMETERS:
// - pid: Process ID to send signal to
// - sig: Signal number to send
//
// RETURN VALUE:
// - 0 on success
// - -1 on error (permission denied, no such process, etc.)
//
// SPECIAL PID VALUES:
// - pid > 0: Send to specific process
// - pid == 0: Send to all processes in current process group
// - pid == -1: Send to all processes we have permission to signal
// - pid < -1: Send to process group |pid|

// RACE CONDITIONS:
//
// POTENTIAL RACE:
// Parent and child loops might not stay synchronized
// One might finish before the other
//
// WHAT PROTECTS US:
// - Both sleep(1) provides rhythm
// - SIGTERM ensures child exits even if it's ahead
// - wait() ensures parent doesn't exit before child
//
// COULD STILL HAPPEN:
// If parent's loop finishes first:
// - Parent sends SIGTERM
// - Child terminates (even if it's in the middle of sleep)
// - Parent wait()s and cleans up
// - Everything works fine
//
// If child's loop finishes first:
// - Child returns 0 and starts exiting
// - Parent might still be in loop
// - Parent's kill(pid, SIGUSR1) still works (child is still alive)
// - Eventually parent's loop ends, sends SIGTERM
// - Parent wait()s (might return immediately if child already exited)

// MEMORY SEPARATION:
//
// IMPORTANT CONCEPT:
// After fork(), parent and child have SEPARATE memory:
// - Each has its own 'num' variable
// - Incrementing one doesn't affect the other
// - Both count their own signal receipts independently
//
// THIS IS WHY:
// Both can count to 5 independently
// Parent's num counts SIGUSR2 (from child)
// Child's num counts SIGUSR1 (from parent)

// COMMON QUESTIONS:
//
// Q: Why does parent send SIGTERM?
// A: To ensure child exits, even if there's a race condition.
//    If child is still in its loop or sleeping, SIGTERM terminates it.
//
// Q: What if signals arrive too fast?
// A: Signals of the same type don't queue infinitely.
//    Multiple SIGUSR1s before handler finishes might be merged.
//    sleep(1) prevents this by slowing down the rate.
//
// Q: Why do both install both handlers?
// A: After fork(), child inherits parent's signal handlers.
//    Since we install before fork(), both have both handlers.
//    Each only receives "their" signal, so only one handler activates per process.
//
// Q: Can child send SIGUSR1 instead of SIGUSR2?
// A: Yes, but then both would use same handler and same counter.
//    Using different signals makes it clearer and easier to debug.
//
// Q: What happens if child exits before parent sends SIGTERM?
// A: kill() returns -1 with errno = ESRCH (no such process).
//    wait() returns immediately with child's status.
//    No harm done - program still works correctly.

// SIGNAL SAFETY REMINDER:
//
// printf() IN HANDLERS:
// Technically not async-signal-safe
// For pedagogy, we use printf() because it's familiar
// Production code should use write():
//   write(1, "Tagged!\n", 8);
//
// SAFE OPERATIONS IN HANDLERS:
// - Modifying volatile sig_atomic_t variables
// - Calling signal(), kill(), getpid()
// - Calling write() (not printf!)
// - Setting flags that main() checks

// REAL-WORLD APPLICATIONS:
//
// SIGNALS FOR NOTIFICATIONS:
// - SIGHUP: Reload configuration (many daemons)
// - SIGUSR1: Toggle debug mode
// - SIGUSR2: Dump statistics
// - Custom application-specific meanings
//
// PARENT-CHILD COORDINATION:
// - Parent signals child to start next phase
// - Child signals parent when task complete
// - Signals as simple synchronization mechanism
//
// LIMITATIONS:
// - Signals carry minimal information (which signal?)
// - For data transfer, use pipes, shared memory, sockets
// - Signals are for simple notifications

// TRY IT:
// Compile: gcc tag.c -o tag
// Run: ./tag
//
// Expected output (order may vary):
// I've been tagged by my parent 0 times
// I've been tagged by my child 0 times
// I've been tagged by my parent 1 times
// I've been tagged by my child 1 times
// I've been tagged by my parent 2 times
// I've been tagged by my child 2 times
// I've been tagged by my parent 3 times
// I've been tagged by my child 3 times
// I've been tagged by my parent 4 times
// I've been tagged by my child 4 times
//
// Experiments:
// 1. Change loop condition to different values:
//    while(num < 10)
//    Now they tag each other 10 times
//
// 2. Remove sleep() calls and see signals arrive faster
//
// 3. Add unique messages to each handler:
//    handler1: printf("Child tagged by parent...\n");
//    handler2: printf("Parent tagged by child...\n");
//
// 4. Print PIDs to see who is who:
//    printf("Process %d tagged %d times\n", getpid(), num);
//
// 5. Try sending signals from terminal while it runs:
//    Find PIDs with ps, then: kill -SIGUSR1 <child_pid>
//    See the counter increment unexpectedly!
//
// 6. Remove SIGTERM and wait() to see what happens:
//    Parent might exit before child, child becomes orphan
