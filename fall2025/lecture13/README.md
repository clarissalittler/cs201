# Lecture 13 - Signals and Process Control

This directory contains examples demonstrating signals, signal handlers, and process control for Fall 2025 Lecture 13.

## Contents

### Basic Process Examples
- **pidLoop.c** - Simple loop printing process ID
- **pidLoopcommented.c** - Detailed explanation of getpid()

### Signal Handler Examples
- **sigusr1.c** - Handling user-defined signals (SIGUSR1, SIGUSR2)
- **sigusr1commented.c** - Detailed explanation of signal() function
- **sigusr2.c** - More complex signal handling
- **sigusr2commented.c** - Detailed explanation
- **sigusr3.c** - Advanced signal handling patterns
- **sigusr3commented.c** - Detailed explanation

### Signal Blocking and Control
- **uninterruptable.c** - Blocking signals to create critical sections
- **uninterruptablecommented.c** - Detailed explanation of signal masking

### Process Management with Signals
- **forkIncremental.c** - Multi-process program using signals for coordination
- **forkIncrementalcommented.c** - Detailed explanation of process coordination

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make pidLoop
make sigusr1
make forkIncremental
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Basic PID Loop
```bash
./pidLoop
```

**Expected behavior:**
- Prints "I'm mr. [PID]" repeatedly every second
- Press Ctrl+C (SIGINT) to terminate
- Note the process ID for sending signals

### User-Defined Signals
**Terminal 1:**
```bash
./sigusr1
# Note the PID printed, e.g., 12345
```

**Terminal 2:**
```bash
kill -SIGUSR2 12345  # Sends SIGUSR2 signal
kill -SIGUSR1 12345  # Sends SIGUSR1 signal (terminates)
```

**Expected behavior:**
- Program loops, printing message with its PID
- SIGUSR2: Prints "Did you say something?" and continues
- SIGUSR1: Prints "I guess we're done?" and exits
- Demonstrates custom signal handlers

### Advanced Signal Handling
```bash
./sigusr2
./sigusr3
```

**Expected behavior:**
- sigusr2: More complex signal handling scenarios
- sigusr3: Advanced patterns with multiple signals
- Use `kill -SIGUSR1` or `kill -SIGUSR2` from another terminal

### Uninterruptable Sections
```bash
./uninterruptable
```

**Expected behavior:**
- Blocks certain signals during critical sections
- Press Ctrl+C during different phases
- Some phases are interruptible, others are not
- Demonstrates signal masking

### Fork with Signals
```bash
./forkIncremental
```

**Expected behavior:**
- Creates multiple processes
- Uses signals to coordinate between parent and children
- Demonstrates inter-process communication via signals
- Shows practical use of SIGUSR1/SIGUSR2 for coordination

## Key Concepts Demonstrated

### Signals
- **Definition:** Asynchronous notifications sent to processes
- **Source:** Kernel, other processes, or the process itself
- **Purpose:** Interrupt normal control flow for events
- **Delivery:** Handled immediately (when not blocked)

### Common Signals
- **SIGINT (2)** - Interrupt (Ctrl+C)
- **SIGTERM (15)** - Termination request (polite kill)
- **SIGKILL (9)** - Force kill (cannot be caught or ignored)
- **SIGSEGV (11)** - Segmentation fault
- **SIGFPE (8)** - Floating point exception (e.g., divide by zero)
- **SIGALRM (14)** - Alarm clock (from alarm())
- **SIGSTOP (19)** - Stop process (cannot be caught)
- **SIGTSTP (20)** - Terminal stop (Ctrl+Z, can be caught)
- **SIGCHLD (17)** - Child process terminated
- **SIGUSR1 (10)** - User-defined signal 1
- **SIGUSR2 (12)** - User-defined signal 2

### Signal Handling
**Default actions:**
- Term - Terminate the process
- Ign - Ignore the signal
- Core - Terminate and dump core
- Stop - Stop the process
- Cont - Continue if stopped

**Custom handlers:**
```c
void handler(int signum) {
    // Signal-safe code only!
}
signal(SIGUSR1, handler);
```

### Signal Function
- **signal(signum, handler)** - Install signal handler
- **handler** can be:
  - Custom function: `void func(int)`
  - SIG_IGN: Ignore signal
  - SIG_DFL: Use default action

### Signal Safety
**Async-signal-safe functions only:**
- write(), _exit(), abort()
- **NOT safe:** printf(), malloc(), most library functions

**Why?** Signal handlers interrupt execution unpredictably
- Could interrupt non-reentrant function
- Could cause deadlock or corruption

**Best practice:** Set flag, handle in main loop

### Volatile sig_atomic_t
```c
volatile sig_atomic_t flag = 0;

void handler(int sig) {
    flag = 1;  // Safe: atomic write
}
```

- **volatile** - Prevent compiler optimization
- **sig_atomic_t** - Guaranteed atomic reads/writes
- Use for flags modified by signal handlers

### Sending Signals
**From command line:**
```bash
kill -SIGTERM 12345   # Send SIGTERM to PID 12345
kill -9 12345         # Send SIGKILL (force kill)
killall program_name  # Send signal to all matching processes
```

**From C program:**
```c
kill(pid, SIGUSR1);   // Send SIGUSR1 to process
raise(SIGTERM);       // Send signal to self
```

### Blocking Signals
```c
sigset_t mask;
sigemptyset(&mask);
sigaddset(&mask, SIGUSR1);
sigprocmask(SIG_BLOCK, &mask, NULL);    // Block SIGUSR1
// Critical section
sigprocmask(SIG_UNBLOCK, &mask, NULL);  // Unblock
```

- Create uninterruptible critical sections
- Signals are queued while blocked
- Delivered when unblocked

### Exceptions and Interrupts
**Hardware interrupts:**
- Keyboard, mouse, network card
- Handled by OS interrupt table
- Asynchronous to program execution

**Software exceptions:**
- **Faults** - Recoverable errors (page fault, segfault)
- **Traps** - Intentional (system calls)
- **Aborts** - Serious errors (hardware failure)

### Process Context
**What's in a process:**
- Code (text section)
- Register state
- Memory mappings (virtual address space)
- File descriptor table
- Working directory
- Environment variables
- Signal handlers
- Process ID, parent PID, user ID

**Context switch:**
- Save all register state
- Save memory mappings
- Load new process state
- Expensive operation (microseconds)

## Notes

- SIGKILL and SIGSTOP cannot be caught, blocked, or ignored
- Signal handlers should be very short and simple
- Use write() not printf() in signal handlers (printf not signal-safe)
- Signals are not queued beyond one pending signal per type
- Race conditions possible between checking flag and waiting
- Modern alternative: sigaction() (more reliable than signal())
- Signals can interrupt system calls (EINTR error)
- Child processes inherit signal handlers (cleared by exec)
- Use signals for exceptional events, not regular IPC
- SIGUSR1 and SIGUSR2 are for application-specific purposes

## Troubleshooting

### Signal handler not called:
- Check signal number is correct
- Verify signal() call succeeds
- Some signals (SIGKILL, SIGSTOP) cannot be caught
- Signal might be blocked

### Program crashes in signal handler:
- Check only signal-safe functions are used
- Don't use printf(), malloc(), etc.
- Use write() instead of printf()
- Keep handler minimal

### Race condition with signal flag:
- Use volatile sig_atomic_t for flags
- Check flag in safe location (not mid-operation)
- Consider using sigaction() with SA_RESTART

### kill command fails:
- Check process ID is correct (use ps aux | grep)
- Verify you have permission to signal the process
- Process may have already terminated

### Program hangs after signal:
- Check for blocked signals
- Verify handler doesn't deadlock
- Look for interrupted system calls

## Resources

- Chapter 8 of "Computer Systems: A Programmer's Perspective" - Exceptional Control Flow
- `man signal`, `man sigaction`, `man kill`
- `man 7 signal` - Overview of signals
- POSIX signal safety requirements
