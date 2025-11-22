# Lecture 11 - Signal-Based Inter-Process Communication

This directory contains a C program demonstrating advanced signal handling and inter-process communication through signals.

## Contents

This lecture explores creative use of signals for inter-process communication and game-like interactions between parent and child processes.

### Signal Communication Example

- **signalFight.c** - Interactive battle simulation using signals
- **signalFightcommented.c** - Detailed explanation of signal-based IPC

## Compilation

### Compile the example:
```bash
make
```

### Compile manually:
```bash
gcc -o signalFight signalFight.c
```

### Clean up:
```bash
make clean
```

## Running the Example

### Signal Fight - Process Battle Simulation
```bash
./signalFight
```

**Expected behavior:**
- Creates a parent and child process that "fight" each other
- Each process has health points (HP) starting at 50
- Processes send SIGUSR1 signals to attack each other
- Each hit causes random damage (1-5 points)
- Processes print their current HP and damage taken
- Battle continues until one process reaches 0 HP
- Winner is the last process standing
- Uses SIGUSR2 to signal end of combat

**Sample output:**
```
I, mr. 12345, have 50 hp left
I, # 12346, have been hit! I took 3 damage!
I, mr. 12346, have 47 hp left
I, # 12345, have been hit! I took 5 damage!
I, mr. 12345, have 45 hp left
...
I, # 12346, have been hit! I took 4 damage!
I, mr. 12346, just died!
```

**Timing:**
- Each process sleeps for random duration (1-4 seconds) between attacks
- Combat is asynchronous - both processes act independently
- One process will eventually deplete the other's HP

## Key Concepts Demonstrated

### Signal Handling
- **signal()** - Installing signal handlers
  ```c
  signal(SIGUSR1, hitHandler);
  signal(SIGUSR2, endHandler);
  ```
- Custom signal handlers with specific behaviors
- Multiple signal handlers in one program

### Signal Types Used
- **SIGUSR1** - User-defined signal 1 (used for "attacks")
- **SIGUSR2** - User-defined signal 2 (used to end the fight)
- Custom semantics assigned to standard signals

### Inter-Process Communication
- **kill()** - Send signal to another process
  ```c
  kill(enemy_pid, SIGUSR1);
  ```
- Bidirectional communication using signals
- Parent-child interaction via signals

### Signal Handler Best Practices
- **Volatile variables** - Variables modified in signal handlers
  ```c
  volatile int hp = 50;
  volatile bool stillFighting = true;
  ```
- Ensures compiler doesn't optimize away changes
- Required for signal handler safety

### Process Interaction Patterns
- Parent-child signaling
- getpid() - Get current process ID
- getppid() - Get parent process ID
- Using PIDs to target signals

### State Management
- Global variables shared between main code and handlers
- Health points (hp) tracking
- Combat status flags (stillFighting, won)
- Event-driven state changes

### Randomization in Concurrent Processes
- Different random seeds for parent and child
  ```c
  srand(time(0));        // Parent seed
  srand(rand());         // Child gets different seed
  ```
- Ensures varied combat outcomes

### Process Lifecycle
- fork() to create child
- wait() to synchronize termination
- Proper cleanup when process dies

## Key Concepts Explained

### Signal Handlers
Signal handlers are functions called asynchronously when a signal arrives:
```c
void hitHandler(int sig) {
    // Called when SIGUSR1 received
    // Reduces HP based on random damage
}

void endHandler(int sig) {
    // Called when SIGUSR2 received
    // Ends the combat loop
}
```

### Volatile Keyword
The `volatile` keyword is crucial in signal handlers:
- Prevents compiler optimization that assumes variables don't change unexpectedly
- Tells compiler variable may be modified by external events (signals)
- Without `volatile`, signal handler changes might be ignored

### Combat Loop Pattern
```c
while (stillFighting) {
    // Print status
    // Send attack signal to enemy
    // Sleep random duration
}
// Send end signal
```

## Troubleshooting

### Process Doesn't Stop
If the program runs indefinitely:
- Kill manually: `killall signalFight`
- Check that signal handlers properly set flags
- Verify SIGUSR2 handler sets stillFighting to false

### Unexpected Behavior
- Signals may be delivered while process is sleeping
- Signal handler interrupts normal execution
- Race conditions possible between signal delivery and handling

### Compilation Warnings
- Ensure `volatile` is used for variables accessed in signal handlers
- Include all necessary headers:
  ```c
  #include <signal.h>
  #include <stdbool.h>
  #include <unistd.h>
  #include <sys/wait.h>
  ```

### Random Numbers Not Random
- If same results every time, check seeding
- Parent and child should have different seeds
- Call srand() before generating random numbers

## Advanced Topics

### Signal Safety
- Signal handlers should be **async-signal-safe**
- Avoid calling most library functions in handlers
- printf() is technically unsafe in signal handlers (but often works)
- Better practice: set flags and handle in main loop

### Signal Delivery
- Signals are delivered asynchronously
- Handler may interrupt any instruction
- Multiple signals of same type may be merged
- Signal delivery is not queued (except real-time signals)

### Alternative: Safer Signal Handling
Modern code should consider:
- sigaction() instead of signal() for more control
- Blocking signals during critical sections
- Using signalfd() for synchronous signal handling

## Notes

- This is a creative educational example, not production-quality code
- Real applications should use more robust IPC mechanisms for critical communication
- Signals are better suited for simple notifications than complex data transfer
- The volatile keyword is essential for correctness with signal handlers
- Each run produces different results due to randomization
- Process death is detected when HP reaches 0
- Parent always waits for child to terminate
- The commented version explains each line in detail
- Understanding signal-based IPC is important for systems programming
- This demonstrates that signals can create interactive behaviors between processes
- Great example of event-driven programming in C

## Educational Value

This example teaches:
1. How signals can create interactive behaviors
2. Proper use of volatile in signal contexts
3. Managing state across asynchronous events
4. Parent-child process communication
5. Event-driven programming patterns
6. Race conditions and non-deterministic behavior
7. Process lifecycle management

The "game" format makes signal handling concepts more engaging and memorable!
