# Inter-Process Communication (IPC) Exercises

This section focuses on various mechanisms for inter-process communication: pipes, named pipes, message queues, shared memory, and Unix domain sockets.

## Learning Objectives

After completing these exercises, you will be able to:
- Use anonymous pipes for parent-child process communication
- Implement named pipes (FIFOs) for unrelated process communication
- Work with POSIX message queues for asynchronous messaging
- Utilize shared memory with semaphores for synchronized data sharing
- Create Unix domain socket servers and clients for local communication
- Understand the trade-offs between different IPC mechanisms

## Prerequisites

- Completion of basic process exercises
- Understanding of process creation with `fork()`
- Basic knowledge of file descriptors
- Familiarity with synchronization concepts

## Exercises

### Exercise 1: Parent-Child Pipe Communication 🟢
**File**: `exercise1.c`
**Concepts**: Anonymous pipes, bidirectional communication

Create a program that uses two pipes to enable bidirectional communication between parent and child processes. The parent sends numbers to the child, and the child responds with the square of each number.

**Requirements**:
- Create two pipes (one for each direction)
- Parent sends numbers 1-5 to child
- Child calculates squares and sends results back
- Parent prints the results

### Exercise 2: Named Pipe Chat 🟢
**Files**: `exercise2_server.c`, `exercise2_client.c`
**Concepts**: Named pipes (FIFOs), unrelated processes

Implement a simple chat system using named pipes. The server and client are separate programs that communicate through two FIFOs (one for each direction).

**Requirements**:
- Server creates two named pipes
- Client connects to existing pipes
- Both can send messages to each other
- Handle cleanup on termination (SIGINT)

### Exercise 3: Message Queue Chat 🟡
**Files**: `exercise3_sender.c`, `exercise3_receiver.c`
**Concepts**: POSIX message queues, priority messaging

Create a messaging system using POSIX message queues. The sender can send messages of different priorities, and the receiver processes them in priority order.

**Requirements**:
- Sender creates a message queue
- Support priority levels (0=low, 1=medium, 2=high)
- Receiver retrieves messages in priority order
- Implement graceful shutdown with "exit" message

### Exercise 4: Shared Memory Counter 🟡
**Files**: `exercise4_producer.c`, `exercise4_consumer.c`, `exercise4_shared.h`
**Concepts**: POSIX shared memory, semaphores, synchronization

Implement a producer-consumer system using shared memory. The producer generates data and the consumer processes it, with semaphores ensuring proper synchronization.

**Requirements**:
- Define shared data structure in header file
- Producer writes 10 values to shared memory
- Consumer reads and displays values
- Use semaphores to prevent race conditions
- Implement proper cleanup

### Exercise 5: Unix Domain Socket Echo Server 🔴
**Files**: `exercise5_server.c`, `exercise5_client.c`
**Concepts**: Unix domain sockets, concurrent servers, threading

Create a multi-client echo server using Unix domain sockets. The server should handle multiple clients concurrently using threads.

**Requirements**:
- Server listens on a Unix domain socket
- Support multiple simultaneous clients
- Each client connection handled in a separate thread
- Echo messages back to clients
- Implement proper socket cleanup
- Client can send multiple messages

## Compilation

### Exercise 1 (Anonymous Pipes)
```bash
gcc -o exercise1 exercise1.c -Wall -Wextra -g
```

### Exercise 2 (Named Pipes)
```bash
gcc -o exercise2_server exercise2_server.c -Wall -Wextra -g
gcc -o exercise2_client exercise2_client.c -Wall -Wextra -g -pthread
```

### Exercise 3 (Message Queues)
```bash
gcc -o exercise3_sender exercise3_sender.c -Wall -Wextra -g -lrt
gcc -o exercise3_receiver exercise3_receiver.c -Wall -Wextra -g -lrt
```

### Exercise 4 (Shared Memory)
```bash
gcc -o exercise4_producer exercise4_producer.c -Wall -Wextra -g -lrt -pthread
gcc -o exercise4_consumer exercise4_consumer.c -Wall -Wextra -g -lrt -pthread
```

### Exercise 5 (Unix Domain Sockets)
```bash
gcc -o exercise5_server exercise5_server.c -Wall -Wextra -g -pthread
gcc -o exercise5_client exercise5_client.c -Wall -Wextra -g
```

Or use the Makefile:
```bash
make          # Compile all exercises
make clean    # Remove compiled files
```

## Tips

### Anonymous Pipes
- `pipe()` creates a unidirectional communication channel
- File descriptor `[0]` is for reading, `[1]` is for writing
- Close unused ends to avoid deadlocks
- Use two pipes for bidirectional communication

### Named Pipes (FIFOs)
- Created with `mkfifo()` function
- Persist in filesystem until unlinked
- Multiple processes can open the same FIFO
- Blocking by default when opening for read/write
- Remember to `unlink()` to remove from filesystem

### Message Queues
- POSIX message queues support priority-based delivery
- Queue names must start with `/`
- Link with `-lrt` flag
- Use `mq_unlink()` to remove queue
- Messages are delivered in priority order (highest first)

### Shared Memory
- POSIX shared memory created with `shm_open()`
- Must use `ftruncate()` to set size
- Map with `mmap()` to get pointer
- Use semaphores to synchronize access
- Clean up with `munmap()`, `shm_unlink()`
- Link with `-lrt` and `-pthread` flags

### Unix Domain Sockets
- Use `AF_UNIX` address family
- Socket file persists in filesystem
- More efficient than TCP/IP for local communication
- Remember to unlink socket file on cleanup
- Use threads to handle multiple clients concurrently

## Common Pitfalls

1. **Forgetting to close unused pipe ends** - Can cause deadlocks
2. **Not unlinking FIFOs or socket files** - Leaves artifacts in filesystem
3. **Missing synchronization in shared memory** - Causes race conditions
4. **Not linking required libraries** - Use `-lrt` for message queues/shared memory
5. **Ignoring signal handling** - Makes cleanup difficult
6. **Buffer overflow** - Always null-terminate strings and check sizes

## Testing Your Solutions

### Exercise 1
```bash
./exercise1
```

### Exercise 2
```bash
# Terminal 1
./exercise2_server

# Terminal 2
./exercise2_client
```

### Exercise 3
```bash
# Terminal 1
./exercise3_receiver

# Terminal 2
./exercise3_sender
```

### Exercise 4
```bash
# Terminal 1 - Start consumer first
./exercise4_consumer

# Terminal 2
./exercise4_producer
```

### Exercise 5
```bash
# Terminal 1
./exercise5_server

# Terminal 2
./exercise5_client

# Terminal 3 (optional - test multiple clients)
./exercise5_client
```

## Further Reading

- `man 2 pipe` - Anonymous pipes
- `man 3 mkfifo` - Named pipes
- `man 7 mq_overview` - POSIX message queues
- `man 7 shm_overview` - POSIX shared memory
- `man 7 unix` - Unix domain sockets
- `man 7 sem_overview` - POSIX semaphores
