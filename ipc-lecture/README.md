# IPC Lecture Examples

This directory contains examples demonstrating various IPC (Inter-Process Communication) mechanisms for the operating systems course.

## Contents

### Message Queues (POSIX)
- **msgqSender1.c** - POSIX message queue sender
- **msgqReceiver1.c** - POSIX message queue receiver

### Shared Memory (POSIX)
- **sharedMemprod.c** - Shared memory producer
- **sharedMemcons.c** - Shared memory consumer
- **sharedstruct.h** - Shared data structure definitions

### Sockets
- **echoserver.c** - Multi-threaded echo server
- **echoclient.c** - Echo client with signal handling
- **fileserver.c** - File transfer server
- **fileclient.c** - File transfer client

### Named Pipes (FIFOs)
- **namedPipesServer1.c** - FIFO server
- **namedPipesClient1.c** - FIFO client

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific examples:
```bash
make msgqSender1
make msgqReceiver1
make sharedMemprod sharedMemcons
make echoserver echoclient
make fileserver fileclient
make namedPipesServer1 namedPipesClient1
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Message Queues

**Terminal 1 (Sender):**
```bash
./msgqSender1
```
Type messages and press Enter. Type 'exit' to quit.

**Terminal 2 (Receiver):**
```bash
./msgqReceiver1
```
Receives and displays messages. Exits when it receives 'exit'.

**Note:** Start receiver first to avoid missing messages.

### Shared Memory

**Terminal 1 (Producer):**
```bash
./sharedMemprod
```

**Terminal 2 (Consumer):**
```bash
./sharedMemcons
```

**Note:** Start producer first to create the shared memory segment.

### Echo Server/Client

**Terminal 1 (Server):**
```bash
./echoserver
```

**Terminal 2 (Client):**
```bash
./echoclient
```
Type messages and they will be echoed back.

### File Server/Client

**Terminal 1 (Server):**
```bash
./fileserver
```

**Terminal 2 (Client):**
```bash
./fileclient
```
Request files from the server.

### Named Pipes

**Terminal 1 (Server):**
```bash
./namedPipesServer1
```

**Terminal 2 (Client):**
```bash
./namedPipesClient1
```

**Note:** Start server first.

## Cleanup

### Remove POSIX message queue:
```bash
rm /dev/mqueue/ourQueue
```

### Remove shared memory segment:
```bash
ipcs -m  # List segments
ipcrm -m <shmid>  # Remove specific segment
```
Or use `shm_unlink` (done automatically by consumer).

### Remove named pipes:
Cleaned up automatically by the programs.

## Notes

- Most examples require two terminals to demonstrate IPC between processes
- Some examples use POSIX IPC (message queues, shared memory) which requires `-lrt` linker flag
- Socket examples use threads, requiring `-pthread` flag
- See individual source files for detailed comments and explanations
