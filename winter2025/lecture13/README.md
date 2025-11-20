# Lecture 13 - Inter-Process Communication

This directory contains IPC examples for Winter 2025 Lecture 13.

## Contents

### Message Queues (System V)
- **task_distributor.c** - Distributes tasks to workers via message queue
- **worker.c** - Worker process that receives and processes tasks

### Shared Memory (POSIX)
- **sharedMemprod.c** - Shared memory producer with semaphore synchronization
- **sharedMemcons.c** - Shared memory consumer
- **shm_creator.c** - Creates shared memory segment
- **shm_client.c** - Accesses shared memory segment
- **sharedstruct.h** - Shared data structure definitions

### Named Pipes (FIFOs)
- **chat_server.c** - Interactive chat server using FIFOs
- **chat_client.c** - Chat client

### Sockets
- **fileserver.c** - File transfer server
- **fileclient.c** - File transfer client

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific examples:
```bash
make task_distributor worker
make sharedMemprod sharedMemcons
make chat_server chat_client
make fileserver fileclient
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Task Distribution System (Message Queues)

This demonstrates a work distribution pattern using System V message queues.

**Terminal 1 (Distributor):**
```bash
./task_distributor
```

**Terminal 2 (Worker 1):**
```bash
./worker
```

**Terminal 3 (Worker 2):**
```bash
./worker
```

**Terminal 4 (Worker 3):**
```bash
./worker
```

**Expected behavior:**
- Distributor sends 5 tasks
- Workers compete to process tasks (each processes 2 tasks except one processes 1)
- Distributor waits for all results
- Message queue is cleaned up automatically

**Note:** Start distributor first, then workers within a few seconds.

### Shared Memory Producer-Consumer

**Terminal 1 (Producer):**
```bash
./sharedMemprod
```

**Terminal 2 (Consumer):**
```bash
./sharedMemcons
```

**Expected behavior:**
- Producer creates shared memory and writes 10 items
- Consumer reads items as they're produced
- Semaphores ensure synchronization
- Consumer cleans up shared memory

**Note:** Start producer first.

### Alternative: Shared Memory Creator/Client

**Terminal 1 (Creator):**
```bash
./shm_creator
```

**Terminal 2 (Client):**
```bash
./shm_client
```

### Chat System (Named Pipes)

**Terminal 1 (Server):**
```bash
./chat_server
```

**Terminal 2 (Client):**
```bash
./chat_client
```

**Expected behavior:**
- Interactive bidirectional chat
- Type messages and press Enter
- Type 'exit' to quit
- FIFOs are cleaned up on exit

**Note:** Start server first.

### File Transfer (Sockets)

**Terminal 1 (Server):**
```bash
./fileserver
```

**Terminal 2 (Client):**
```bash
./fileclient
```

## Key Concepts Demonstrated

### Message Queues
- Work distribution pattern
- Multiple workers competing for tasks
- Structured messages with types
- Bidirectional communication (tasks and results)

### Shared Memory
- Producer-consumer pattern
- Semaphore-based synchronization
- High-performance data sharing
- Proper cleanup procedures

### Named Pipes (FIFOs)
- Bidirectional communication using two FIFOs
- Interactive applications
- Blocking vs non-blocking I/O
- Signal handling for cleanup

### Sockets
- Client-server pattern
- File transfer over network
- Multi-threaded server design

## Troubleshooting

### Message queue already exists:
```bash
ipcs -q  # List message queues
ipcrm -q <msqid>  # Remove specific queue
```

### Shared memory already exists:
```bash
ipcs -m  # List shared memory
ipcrm -m <shmid>  # Remove specific segment
```

### FIFOs already exist:
```bash
rm fifo_server fifo_client
```

### Worker processes orphaned:
```bash
pkill worker
```

## Notes

- Examples use both POSIX and System V IPC mechanisms
- Message queue example works best with 3 workers to distribute 5 tasks
- All examples include proper cleanup on normal exit
- Use Ctrl+C to interrupt programs (cleanup handlers are in place)
