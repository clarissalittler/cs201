# Lecture 8 - Advanced Signals and Signal Interactions

This directory contains examples for Winter 2025 Lecture 8, covering advanced signal handling, signal masking, timers, and inter-process signal communication.

## Contents

### Signal Handling and Control
- **randtest.c** - Random number generation with signals
- **randtestcommented.c** - Detailed explanation of signal-safe randomization
- **alarms.c** - Using alarm() and SIGALRM
- **alarmscommented.c** - Timer-based signal demonstration
- **stopTest.c** - SIGSTOP and SIGCONT handling
- **stopTestcommented.c** - Process suspension and resumption
- **stopTest2.c** - Alternative stop/continue example
- **stopTest2commented.c** - Additional stop signal patterns

### Advanced Signal Communication
- **signalFight.c** - Parent-child signal exchange (competitive)
- **signalFightcommented.c** - Complex signal interaction between processes

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make alarms
make signalFight
make stopTest
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Random Number with Signals
```bash
./randtest
```
**Expected behavior:**
- Demonstrates signal-safe operations
- Shows proper handling of random number generation

### Alarm Signals
```bash
./alarms
```
**Expected behavior:**
- Sets alarm for future time
- SIGALRM delivered after specified seconds
- Handler executes when alarm fires
- Program can set multiple alarms

### Stop and Continue
```bash
./stopTest
```
**Expected behavior:**
- Demonstrates SIGSTOP (suspend process)
- Shows SIGCONT (resume process)
- Process pauses and resumes execution

In another terminal, try:
```bash
kill -STOP <pid>
kill -CONT <pid>
```

### Signal Fight (Parent-Child Competition)
```bash
./signalFight
```
**Expected behavior:**
- Parent and child exchange signals rapidly
- Demonstrates signal racing conditions
- Shows importance of signal handling timing
- Illustrates process synchronization challenges

## Key Concepts Demonstrated

### Signal Masking

Temporarily block signals during critical sections:

```c
sigset_t mask, oldmask;

sigemptyset(&mask);           // Initialize empty set
sigaddset(&mask, SIGUSR1);    // Add SIGUSR1 to set

// Block SIGUSR1
sigprocmask(SIG_BLOCK, &mask, &oldmask);

// Critical section - SIGUSR1 is queued, not delivered
// ...

// Unblock SIGUSR1
sigprocmask(SIG_SETMASK, &oldmask, NULL);
```

### Signal Set Operations
- **sigemptyset(&set)**: Initialize empty signal set
- **sigfillset(&set)**: Add all signals to set
- **sigaddset(&set, signum)**: Add signal to set
- **sigdelset(&set, signum)**: Remove signal from set
- **sigismember(&set, signum)**: Check if signal in set

### Raising Signals to Self

```c
raise(SIGINT);     // Send SIGINT to current process
```

Equivalent to:
```c
kill(getpid(), SIGINT);
```

### Alarm Timers

```c
alarm(5);  // Schedule SIGALRM in 5 seconds
```

**Important notes:**
- Only one alarm per process at a time
- New alarm() call cancels previous alarm
- Returns seconds remaining on previous alarm
- alarm(0) cancels current alarm

### Advanced Signal Handlers

#### Using sigaction (Preferred over signal())

```c
struct sigaction sa;
sa.sa_handler = my_handler;
sa.sa_flags = 0;
sigemptyset(&sa.sa_mask);

sigaction(SIGINT, &sa, NULL);
```

**Advantages over signal():**
- More portable behavior
- Better control over signal handling
- Can specify additional flags
- Can block signals during handler execution

### Signal Handler Flags
- **SA_RESTART**: Restart interrupted system calls
- **SA_NOCLDSTOP**: Don't receive SIGCHLD when child stops
- **SA_NODEFER**: Don't block signal during handler
- **SA_RESETHAND**: Reset to SIG_DFL after handling once

### Inter-Process Signal Communication

**Sending signals between processes:**
```c
// Parent to child
kill(child_pid, SIGUSR1);

// Child to parent
kill(getppid(), SIGUSR2);
```

**Synchronization pattern:**
1. Parent sets up signal handler
2. Parent forks child
3. Both wait for signals from each other
4. Exchange signals for coordination

### Signal Safety

**Async-signal-safe functions** (safe to call in handlers):
- write()
- _exit()
- signal()
- sigaction()
- Most system calls

**NOT safe in signal handlers:**
- printf() (uses buffered I/O)
- malloc()/free() (global state)
- Most library functions

### Race Conditions with Signals

Signals can arrive at unexpected times:

```c
// UNSAFE:
if (!signal_received) {
    // Signal might arrive HERE!
    do_something();
}
```

**Solution: Use signal masking or atomic operations**

```c
sigset_t mask;
sigemptyset(&mask);
sigaddset(&mask, SIGUSR1);

sigprocmask(SIG_BLOCK, &mask, NULL);
// Critical section
if (!signal_received) {
    do_something();
}
sigprocmask(SIG_UNBLOCK, &mask, NULL);
```

### SIGSTOP and SIGCONT

**Special signals:**
- **SIGSTOP**: Cannot be caught or ignored - always suspends process
- **SIGCONT**: Resumes stopped process
- Used by job control (Ctrl+Z in shell)

**Process states:**
- Running → SIGSTOP → Stopped
- Stopped → SIGCONT → Running

## Signal Numbers Reference

Common signals:
- **SIGHUP (1)**: Hangup (terminal closed)
- **SIGINT (2)**: Interrupt (Ctrl+C)
- **SIGQUIT (3)**: Quit (Ctrl+\)
- **SIGILL (4)**: Illegal instruction
- **SIGABRT (6)**: Abort
- **SIGFPE (8)**: Floating point exception
- **SIGKILL (9)**: Kill (uncatchable)
- **SIGSEGV (11)**: Segmentation fault
- **SIGPIPE (13)**: Broken pipe
- **SIGALRM (14)**: Alarm timer
- **SIGTERM (15)**: Termination request
- **SIGUSR1 (10)**: User-defined 1
- **SIGUSR2 (12)**: User-defined 2
- **SIGCHLD (17)**: Child status changed
- **SIGSTOP (19)**: Stop (uncatchable)
- **SIGCONT (18)**: Continue

## Troubleshooting

### Signal handler not executing?
- Verify signal number is correct
- Check handler is registered before signal sent
- Ensure signal isn't blocked (check signal mask)

### Program crashes in signal handler?
- Avoid calling non-async-signal-safe functions
- Don't use printf() - use write() instead
- Check for re-entrancy issues

### Signals getting lost?
- Signals are not queued in standard signal handling
- Multiple instances of same signal may be merged
- Use real-time signals (SIGRTMIN-SIGRTMAX) for queuing

### Race conditions?
- Use signal masking for critical sections
- Consider using sigwait() or signalfd() for synchronous handling
- Be aware of TOCTTOU (Time Of Check To Time Of Use) issues

## Notes

- Signal handling is inherently asynchronous and complex
- Always use sigaction() instead of signal() for new code
- Keep signal handlers short and simple
- Signal masks are inherited across fork()
- execve() resets signal handlers to default
- Understanding signals is crucial for robust system programming
- Modern alternatives: signalfd(), pthread signals for threads
- Signals are Unix's form of software interrupts
