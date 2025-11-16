// PEDAGOGICAL PURPOSE:
// This program simulates a "fight" between parent and child using signals for attacks.
// Key learning objectives:
// 1. Complex signal-based inter-process communication
// 2. Using signals to transfer simple state information
// 3. Race conditions and their effects on program behavior
// 4. Why sprintf() and write() are safer than printf() in handlers
// 5. Seeding random number generators differently in parent and child
// 6. Handling sleep() interruption by signals
// 7. Using multiple signals for different purposes (attack, end game)
// 8. Signal handlers modifying multiple global variables

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Contains fork(), sleep(), getpid(), getppid(), write()
#include <signal.h>      // Contains signal(), kill(), SIGUSR1, SIGUSR2, SIGTERM
#include <sys/wait.h>    // Contains wait()
#include <time.h>        // Contains time() for seeding rand()
#include <string.h>      // Contains strlen() for write()

// GAME STATE VARIABLES:
volatile sig_atomic_t hp = 50;
// Hit Points - health of this process
// Starts at 50, decreases when hit by opponent
// When hp <= 0, the process has "died"
// WHY volatile sig_atomic_t? Signal handler modifies it

volatile sig_atomic_t stillFighting = 1;
// Flag indicating if this process is still in the fight
// 1 = still fighting, 0 = fight is over
// Changed by signal handlers and checked in main loop

volatile sig_atomic_t won = 1;
// Flag indicating if this process won or lost
// 1 = won (opponent died first), 0 = lost (we died)
// Set to 0 by hitHandler when hp <= 0

// CONSTANT FOR CLARITY:
#define STDOUT 1
// File descriptor 1 is standard output
// Used with write() system call

// HIT HANDLER:
// Called when this process receives SIGUSR1 (attack from opponent)
void hitHandler(int sig){
  // CHECK IF STILL ALIVE:
  if(hp > 0){
    // We're still alive, process the hit

    // CALCULATE DAMAGE:
    int d = rand()%5+1;
    // Random damage between 1 and 5
    // rand()%5 gives 0-4, +1 gives 1-5
    //
    // WHY rand() IN HANDLER?
    // Technically questionable (rand() might not be signal-safe)
    // But works in practice for this demo
    // Each process has its own rand() state (separate after fork)

    // CONSTRUCT MESSAGE:
    char str[50];
    sprintf(str,"I, # %d, have been hit! I took %d damage!\n",getpid(),d);
    // sprintf() writes formatted string to buffer
    // Includes process ID to distinguish parent from child
    // Includes damage amount for drama
    //
    // WHY sprintf() INSTEAD OF printf()?
    // sprintf() writes to buffer (safer than printf())
    // printf() uses buffered I/O that can corrupt
    // We'll use write() to output the buffer safely

    // OUTPUT MESSAGE SAFELY:
    write(STDOUT,str,strlen(str));
    // write() is async-signal-safe (printf() is not!)
    // STDOUT = 1 (standard output)
    // strlen(str) gives number of characters to write
    //
    // WHY write() INSTEAD OF printf()?
    // printf() can corrupt internal buffers if called from signal handler
    // write() is a simple system call, safe in handlers
    // The commented printf() line shows what NOT to do:
    //printf("I, # %d, have been hit! I took %d damage!\n",getpid(),d);

    // APPLY DAMAGE:
    hp = hp - d;
    // Reduce our hit points by the damage amount
    // If this brings us to 0 or below, we'll die

    // CHECK FOR DEATH:
    if(hp <= 0){
      // We've been defeated!

      // STOP FIGHTING:
      stillFighting = 0;
      // Exit our fight loop
      // We'll announce death and exit

      // MARK AS LOST:
      won = 0;
      // We lost the fight (opponent beat us)
      // This affects our death message
    }
  }
  // If hp <= 0 before this signal, we're already dead
  // Ignore additional hits (can't kill a dead process twice!)
}

// END HANDLER:
// Called when opponent dies and sends SIGUSR2 to end the fight
void endHandler(int sig){
  // STOP FIGHTING:
  stillFighting = 0;
  // Exit the fight loop
  // We won! (opponent sent this signal because they died)
  // won stays = 1 (we didn't set it to 0)
}

// FIGHT LOOP FUNCTION:
// Main combat logic - attack enemy and wait for counterattack
void fightLoop(int enemy){
  // PARAMETER: enemy = PID of opponent process
  // Parent calls with child's PID
  // Child calls with parent's PID (getppid())

  // FIGHT UNTIL DONE:
  while(stillFighting){
    // Continue fighting until:
    // - We die (hp <= 0, stillFighting set to 0)
    // - Opponent dies (they send SIGUSR2, stillFighting set to 0)

    // REPORT STATUS:
    char str[50];
    sprintf(str,"I, mr. %d, have %d hp left\n",getpid(),hp);
    write(STDOUT,str,strlen(str));
    // Show our current health
    // Helps track the fight progress

    // ATTACK ENEMY:
    kill(enemy,SIGUSR1);
    // Send SIGUSR1 to opponent
    // This triggers their hitHandler
    // They take random damage

    // REST BEFORE NEXT ATTACK:
    int rest;
    do {
      rest = sleep(rand()%4+1);
      // Sleep for 1-4 seconds (random)
      // Adds unpredictability to the fight
      // Different attack rates for each process
      //
      // WHY THE do-while LOOP?
      // sleep() can be interrupted by signals!
      // If interrupted, sleep() returns time remaining
      // We loop until rest = 0 (full sleep completed)
      //
      // WHAT HAPPENS:
      // - Call sleep(3)
      // - After 1 second, SIGUSR1 arrives
      // - sleep() interrupted, returns 2 (seconds remaining)
      // - Loop again: sleep(2)
      // - Complete the sleep
      // - rest = 0, exit loop
      //
      // WHY LOOP?
      // Without loop, signals would make attacks faster
      // With loop, we ensure consistent rest time
    } while(rest > 0);
    // Keep sleeping until we've slept the full duration
  }

  // FIGHT ENDED - ANNOUNCE RESULT:

  // TELL OPPONENT FIGHT IS OVER:
  kill(enemy,SIGUSR2);
  // Send SIGUSR2 to opponent
  // This triggers their endHandler
  // Ensures both processes know fight is over
  //
  // TWO CASES:
  // 1. We died: Tell opponent they won
  // 2. We won: Tell opponent to stop (redundant, they're dead)

  // DEATH MESSAGE (if we lost):
  if(!won){
    // We lost the fight (hp <= 0)

    char str[50];
    sprintf(str,"I, mr. %d, just died!\n",getpid());
    write(STDOUT,str,strlen(str));
    // Announce our death
    // Lets user know who lost
  }
  // If we won (won = 1), no message
  // Opponent already announced their death
}

int main(){
  // COMMENTED CODE - ALTERNATIVE SIGNAL HANDLING:
  /*
  struct sigaction sig;
  sig.sa_handler = hitHandler;
  sig.sa_flags = SA_RESTART;
  sigemptyset(&sig.sa_mask);
  sigaction(SIGUSR1, &sig, NULL);
  */
  // This code shows sigaction() approach (more advanced)
  // SA_RESTART would auto-restart sleep() when interrupted
  // We use simple signal() instead for pedagogy
  // With signal(), we manually handle sleep() interruption

  // INSTALL SIGNAL HANDLERS:
  signal(SIGUSR1,hitHandler);
  // SIGUSR1 = attack from opponent
  // Triggers damage calculation and hp reduction

  signal(SIGUSR2,endHandler);
  // SIGUSR2 = opponent died, fight is over
  // Triggers fight loop exit

  // CREATE CHILD PROCESS:
  int pid = fork();
  // Parent gets child's PID
  // Child gets 0
  //
  // AFTER fork():
  // - Both have hp = 50
  // - Both have stillFighting = 1
  // - Both have won = 1
  // - Both have signal handlers installed
  // BUT: Separate memory! Changes in one don't affect the other

  // SEED RANDOM NUMBER GENERATOR:
  srand(time(0) ^ getpid());
  // Seed rand() differently for parent and child
  //
  // BREAKDOWN:
  // - time(0) = current time in seconds (same for both)
  // - getpid() = process ID (DIFFERENT for parent and child!)
  // - XOR (^) combines them uniquely
  //
  // RESULT:
  // Parent and child get different random sequences
  // This makes damage amounts different
  // Makes fight outcome unpredictable
  //
  // WHY AFTER fork()?
  // If we seeded before fork(), both would have same seed
  // Both would generate identical random numbers
  // Fight would be deterministic!

  // CHILD PROCESS CODE:
  if(pid == 0){
    // We are the child

    // FIGHT PARENT:
    fightLoop(getppid());
    // getppid() returns parent's PID
    // Child attacks parent until fight ends
    //
    // WHEN DOES THIS RETURN?
    // - Child dies (hp <= 0)
    // - Parent dies (sends SIGUSR2)
  }

  // PARENT PROCESS CODE:
  else {
    // We are the parent

    // FIGHT CHILD:
    fightLoop(pid);
    // 'pid' is child's PID from fork()
    // Parent attacks child until fight ends

    // WAIT FOR CHILD TO FINISH:
    wait(0);
    // wait(0) = wait(NULL)
    // Collects child's exit status
    // Prevents zombie process
    //
    // WHY NECESSARY?
    // - Child might still be cleaning up
    // - Need to reap zombie
    // - Ensures clean exit
  }

  // PROCESS EXITS:
  return 0;
  // Both parent and child exit here
  // Fight is over, winner has been determined
}

// EXECUTION TRACE (simplified, actual output varies):
//
// Time    Parent (PID 1000)                    Child (PID 1001)
// ----    -----------------                    ----------------
// T0      Install handlers, fork()             Created, install handlers
// T1      Seed rand with 1000                  Seed rand with 1001
// T2      Enter fightLoop(1001)                Enter fightLoop(1000)
// T3      Report: "I, mr. 1000, have 50 hp"    Report: "I, mr. 1001, have 50 hp"
// T4      kill(1001, SIGUSR1) → attack         kill(1000, SIGUSR1) → attack
// T5      Receive SIGUSR1 from child           Receive SIGUSR1 from parent
// T6      hitHandler: damage = 3, hp = 47      hitHandler: damage = 2, hp = 48
// T7      "I, # 1000, hit! 3 damage!"          "I, # 1001, hit! 2 damage!"
// T8      sleep(random 1-4)                    sleep(random 1-4)
// [Fight continues with mutual attacks]
// ...
// T?      hp = 2                               hp = 5
// T?      Receive SIGUSR1, damage = 4          Report: "I, mr. 1001, have 5 hp"
// T?      hp = -2 (died!)                      kill(1000, SIGUSR1)
// T?      stillFighting = 0, won = 0           sleep...
// T?      "I, mr. 1000, just died!"            Receive SIGUSR2 (we won!)
// T?      kill(1001, SIGUSR2)                  stillFighting = 0, won = 1
// T?      return 0                             return 0
// T?      wait(0) collects child
// T?      return 0

// TYPICAL OUTPUT (varies each run due to randomness):
// I, mr. 12345, have 50 hp left
// I, mr. 12346, have 50 hp left
// I, # 12345, have been hit! I took 3 damage!
// I, # 12346, have been hit! I took 4 damage!
// I, mr. 12345, have 47 hp left
// I, mr. 12346, have 46 hp left
// I, # 12345, have been hit! I took 2 damage!
// I, # 12346, have been hit! I took 5 damage!
// [... many more exchanges ...]
// I, mr. 12345, have 3 hp left
// I, # 12345, have been hit! I took 4 damage!
// I, mr. 12345, just died!
// [Process 12346 wins!]

// CONCEPTUAL EXPLANATION:
//
// SIGNAL-BASED COMBAT:
// Each process represents a "fighter"
// SIGUSR1 = attack signal (causes damage)
// SIGUSR2 = surrender signal (opponent died)
//
// GAME MECHANICS:
// 1. Both start with 50 hp
// 2. Take turns attacking (signal each other)
// 3. Each attack deals 1-5 random damage
// 4. First to reach 0 hp loses
// 5. Winner determined by who died first
//
// RANDOMNESS:
// - Damage amount (1-5)
// - Attack frequency (sleep 1-4 seconds)
// - Different seed for each process
// - Outcome unpredictable
//
// SIGNAL SAFETY:
// Uses write() instead of printf() in handlers
// Uses sprintf() to prepare message, then write()
// This is more signal-safe than direct printf()

// RACE CONDITIONS:
//
// POTENTIAL RACES:
// 1. Both die simultaneously
//    - Both receive killing blow before checking
//    - Both announce death
//    - No clear winner
//
// 2. Signal arrives during sleep
//    - sleep() returns early
//    - do-while loop handles this
//    - Continues sleeping
//
// 3. SIGUSR2 crosses with SIGUSR1
//    - Winner sends SIGUSR2
//    - Loser's attack (SIGUSR1) arrives after
//    - hitHandler checks hp > 0, ignores late attack
//
// PROTECTIONS:
// - Check hp > 0 before processing hit
// - do-while loop to complete sleep
// - SIGUSR2 guarantees both processes stop

// SIGNAL HANDLER SAFETY ANALYSIS:
//
// QUESTIONABLE PRACTICES:
// 1. rand() in signal handler
//    - Not guaranteed signal-safe
//    - Works in practice
//    - Production code should avoid
//
// 2. sprintf() in signal handler
//    - Not guaranteed signal-safe
//    - Usually works
//    - Alternative: pre-format messages
//
// GOOD PRACTICES:
// 1. write() instead of printf()
//    - write() is async-signal-safe
//    - printf() can corrupt buffers
//
// 2. volatile sig_atomic_t for flags
//    - Atomic access guaranteed
//    - Compiler won't optimize away
//
// 3. Minimal handler logic
//    - Calculate, update, return quickly
//    - Don't call complex functions

// COMMON QUESTIONS:
//
// Q: Why do we need the do-while loop for sleep()?
// A: Signals interrupt sleep(), which returns early.
//    The loop ensures we sleep the full duration.
//
// Q: Can both processes die at the same time?
// A: Yes! If killing blows cross in transit.
//    Both could announce death. It's a tie!
//
// Q: Why seed rand() after fork()?
// A: To ensure parent and child get different random sequences.
//    Different damage and timings make fight unpredictable.
//
// Q: Why is sprintf() used instead of printf() in handlers?
// A: To prepare the message in a buffer, then use write().
//    write() is signal-safe; printf() is not.
//
// Q: What if child exits before parent calls wait()?
// A: Child becomes zombie briefly. wait() reaps it. No problem.
//
// Q: Can I make one fighter stronger?
// A: Yes! Change initial hp, or damage calculation, etc.

// TRY IT:
// Compile: gcc signalFight.c -o signalFight
// Run: ./signalFight
//
// Observe:
// - Random fight progression
// - One process eventually dies
// - Different outcome each run
//
// Experiments:
// 1. Change initial hp to 10:
//    volatile sig_atomic_t hp = 10;
//    Fights end faster
//
// 2. Change damage range to 1-10:
//    int d = rand()%10+1;
//    More dramatic swings in battle
//
// 3. Remove do-while loop from sleep:
//    sleep(rand()%4+1);  // Without loop
//    Fight becomes faster (interrupted sleeps)
//
// 4. Print both PIDs at start:
//    printf("Parent: %d, Child: %d\n", getpid(), pid);
//    Track who is who
//
// 5. Add special moves:
//    if (rand()%10 == 0) d = 10;  // Critical hit!
//
// 6. Make parent stronger:
//    In parent code before fightLoop:
//    if (pid != 0) hp = 100;  // Parent gets more hp
