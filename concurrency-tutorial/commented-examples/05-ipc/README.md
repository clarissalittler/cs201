# IPC (Inter-Process Communication) Examples

This directory contains fully-commented examples demonstrating various IPC mechanisms in Unix/Linux systems.

## Contents

1. **01-pipes.c** - Anonymous pipes for parent-child communication
2. **02-named-pipes.c** - Named pipes (FIFOs) for unrelated process communication
3. **03-shared-memory.c** - System V shared memory with semaphore synchronization
4. **04-socket-ipc.c** - Unix domain sockets for local IPC

## Compilation

### Compile all examples:
```bash
make
```

### Compile individual examples:
```bash
make 01-pipes
make 02-named-pipes
make 03-shared-memory
make 04-socket-ipc
```

### Clean up compiled binaries:
```bash
make clean
```

## Running the Examples

### 01-pipes
Simple single-process example demonstrating parent-child communication:
```bash
./01-pipes
```

**Expected output:**
- Shows pipe creation, forking, and bidirectional message passing
- Parent sends messages to child via pipe
- Both processes coordinate and exit cleanly

### 02-named-pipes
Requires **two terminals** - demonstrates communication between unrelated processes:

**Terminal 1 (Writer):**
```bash
./02-named-pipes writer
```

**Terminal 2 (Reader):**
```bash
./02-named-pipes reader
```

**Expected output:**
- Writer creates FIFO and sends 5 messages
- Reader receives all messages
- Reader cleans up the FIFO when done

**Note:** Start the writer first, then the reader.

### 03-shared-memory
Requires **two terminals** - demonstrates producer-consumer pattern:

**Terminal 1 (Producer):**
```bash
./03-shared-memory producer
```

**Terminal 2 (Consumer):**
```bash
./03-shared-memory consumer
```

**Expected output:**
- Producer creates shared memory and produces 10 items
- Consumer consumes items as they're produced
- Semaphores ensure proper synchronization
- Consumer cleans up shared memory when done

**Note:** Start the producer first, then the consumer.

### 04-socket-ipc
Requires **two terminals** - demonstrates Unix domain socket communication:

**Terminal 1 (Server):**
```bash
./04-socket-ipc server
```

**Terminal 2 (Client):**
```bash
./04-socket-ipc client
```

**Expected output:**
- Server creates Unix domain socket and listens
- Client connects and sends 5 messages
- Server responds to each message
- Both processes exit cleanly

**Note:** Start the server first, then the client.

## Key Concepts

### Pipes (01-pipes.c)
- **Use case:** Parent-child process communication
- **Characteristics:** Unidirectional, buffered, anonymous
- **Key functions:** `pipe()`, `fork()`, `read()`, `write()`

### Named Pipes (02-named-pipes.c)
- **Use case:** Unrelated process communication via filesystem
- **Characteristics:** Unidirectional, persistent, named
- **Key functions:** `mkfifo()`, `open()`, `read()`, `write()`, `unlink()`

### Shared Memory (03-shared-memory.c)
- **Use case:** High-performance data sharing between processes
- **Characteristics:** Fastest IPC, requires explicit synchronization
- **Key functions:** `shmget()`, `shmat()`, `shmdt()`, `shmctl()`, `semget()`, `semop()`

### Unix Domain Sockets (04-socket-ipc.c)
- **Use case:** Bidirectional local IPC with familiar socket API
- **Characteristics:** Connection-oriented, bidirectional, flexible
- **Key functions:** `socket()`, `bind()`, `listen()`, `accept()`, `connect()`, `send()`, `recv()`

## Troubleshooting

### "FIFO already exists" error
If 02-named-pipes fails with EEXIST, remove the old FIFO:
```bash
rm /tmp/example_fifo
```

### "Shared memory already exists" error
Clean up leftover shared memory segments:
```bash
ipcs -m  # List shared memory segments
ipcrm -m <shmid>  # Remove specific segment
```

### "Socket already exists" error
Remove the old socket file:
```bash
rm /tmp/example_socket
```

## Further Reading

See the main concurrency guide in `../../concurrencyGuide.org` for detailed explanations of all IPC mechanisms.
