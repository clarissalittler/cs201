#include <stdio.h> // Includes standard input/output library for functions like printf
#include <stdlib.h> // Includes standard library for functions like rand, malloc, exit
#include <unistd.h> // Includes POSIX operating system API for functions like fork, getpid
#include <signal.h> // Includes signal handling functions like signal, kill
#include <stdbool.h> // Includes boolean data type
#include <sys/wait.h> // Includes functions for waiting on child processes
#include <time.h> // Includes time functions for random number generation

// Declare global variables
// 'hp' stores the health points of the process, initialized to 50
volatile int hp = 50;
// 'stillFighting' indicates if the process is still in combat, initialized to true
volatile bool stillFighting = true;
// 'won' indicates if the process won the fight, initialized to true
volatile bool won = true;

// Signal handler for SIGUSR1, which simulates an enemy hit
void hitHandler(int sig) {
  // Check if the process is still alive
  if (hp > 0) {
    // Generate random damage between 1 and 5
    int d = rand() % 5 + 1;
    // Print message indicating damage taken
    printf("I, # %d, have been hit! I took %d damage!\n", getpid(), d);
    // Reduce health points by damage
    hp = hp - d;
    // Check if health points have reached zero
    if (hp <= 0) {
      // Set 'stillFighting' to false since the process is dead
      stillFighting = false;
      // Set 'won' to false as the process lost the fight
      won = false;
    }
  }
}

// Signal handler for SIGUSR2, which signals the end of the fight
void endHandler(int sig) {
  // Set 'stillFighting' to false to end the fight loop
  stillFighting = false;
}

// Function to simulate the combat loop
void fightLoop(int enemy) {
  // Continue the loop while 'stillFighting' is true
  while (stillFighting) {
    // Print current health points
    printf("I, mr. %d, have %d hp left\n", getpid(), hp);
    // Send SIGUSR1 signal to the enemy process
    kill(enemy, SIGUSR1);
    // Sleep for a random time between 1 and 4 seconds
    sleep(rand() % 4 + 1);
  }
  // Send SIGUSR2 signal to the enemy process to signal the end of the fight
  kill(enemy, SIGUSR2);
  // Print a message if the process died
  if (!won) {
    printf("I, mr. %d, just died!\n", getpid());
  }
}

int main() {
  // Set signal handlers for SIGUSR1 and SIGUSR2
  signal(SIGUSR1, hitHandler);
  signal(SIGUSR2, endHandler);

  // Fork a child process
  int pid = fork();

  // Seed the random number generator with current time
  srand(time(0));

  // Child process code
  if (pid == 0) {
    // Seed the random number generator with a different seed than the parent
    srand(rand());
    // Call fightLoop with the parent process ID as the enemy
    fightLoop(getppid());
  }
  // Parent process code
  else {
    // Call fightLoop with the child process ID as the enemy
    fightLoop(pid);
    // Wait for the child process to terminate
    wait(0);
  }

  // Return 0 to indicate successful execution
  return 0;
}
