# Concurrency Examples Index

This document lists all the original examples in this tutorial with brief descriptions of the concepts they demonstrate.

## Process Examples (fork-examples)

### Basic Process Creation

- **fork1.c** - Demonstrates basic process creation using fork() without any synchronization
- **fork2.c** - Shows how parent and child processes can be identified by return value from fork()
- **fork3.c** - Illustrates process hierarchy with multiple generations of child processes
- **fork4.c** - Demonstrates how child processes inherit open file descriptors from parents
- **fork5.c** - Shows how exit status is passed from child to parent with wait()

### Process IDs

- **pid1.c** - Illustrates how to get and use process IDs (PID) and parent PIDs
- **pid2.c** - Demonstrates parent-child relationships through PIDs and waiting
- **pid3.c** - Shows how parent can wait for specific child processes with waitpid()
- **pid4.c** - Creates multiple child processes and waits for them in order

## Thread Examples (thread-examples)

### Basic Threading

- **thread1.c** - Demonstrates basic thread creation and joining with pthreads
- **thread2.c** - Illustrates race conditions when threads access shared data without synchronization
- **threadResult.c** - Shows how to return values from threads back to the main thread

### Advanced Threading

- **threads1.c** - Creates multiple threads that execute the same function
- **threads2.c** - Demonstrates passing different arguments to multiple threads
- **threads3.c** - Illustrates how to create threads that run different functions
- **threads4.c** - Shows how to synchronize between worker threads and a main thread

## Synchronization Examples (synchronization-examples)

### Mutexes

- **mut1.c** - Basic mutex usage for protecting a shared resource
- **mut2.c** - Demonstrates potential issues when mutexes are not properly used
- **threadmutex.c** - Uses mutexes to protect a shared counter in a multi-threaded program

### Semaphores

- **sem1.c** - Basic semaphore usage for counting and limiting resource access
- **sem2.c** - Shows how semaphores can coordinate actions between threads

### Mutex and Semaphore Combination

- **mutSem.c** - Demonstrates using both mutexes and semaphores together for complex synchronization

### Dining Philosophers Problem

- **philo1.c** - Classic deadlock scenario with no solution (will deadlock)
- **philo2.c** - Solves deadlock using randomized resource acquisition
- **philo3.c** - Solves deadlock using resource hierarchy (resource ordering)
- **philo4.c** - Attempted deadlock solution using semaphores
- **philo5.c** - Complete deadlock solution using a resource manager approach

## Inter-Process Communication Examples (ipc-examples)

### Pipes

- **pipe1.c** - Basic pipe communication between parent and child processes

### Named Pipes (FIFOs)

- **namedPipesServer1.c** - Server-side of named pipe communication
- **namedPipesClient1.c** - Client-side of named pipe communication

### Shared Memory

- **sharedMemprod.c** - Producer process that writes to shared memory
- **sharedMemcons.c** - Consumer process that reads from shared memory

## Socket Examples (socket-intro and exercises)

### Basic TCP Sockets

- **socket-intro/firstsocket.c** - Basic TCP server that sends a greeting message
- **socket-intro/echoserver.c** - Simple TCP echo server (single client)
- **socket-intro/webby.c** - Basic HTTP server demonstrating web protocols

### Advanced Socket Programming

- **ipc-lecture/echoserver.c** - Multi-threaded echo server handling multiple clients
- **ipc-lecture/echoclient.c** - Echo client with signal handling
- **ipc-lecture/fileserver.c** - File transfer server
- **ipc-lecture/fileclient.c** - File transfer client

### Socket Exercises

- **exercises/05-sockets/exercise1.c** - Basic TCP echo server skeleton
- **exercises/05-sockets/exercise2.c** - UDP echo server skeleton
- **exercises/05-sockets/exercise3.c** - Multi-client server skeleton
- **exercises/05-sockets/exercise4.c** - File transfer application skeleton
- **exercises/05-sockets/exercise5.c** - Chat room server skeleton

## Summary of Key Concepts

1. **Process Management**: Fork, process hierarchy, process IDs, waiting for processes
2. **Thread Management**: Thread creation, passing arguments, returning values
3. **Synchronization Primitives**: Mutexes, semaphores, condition variables
4. **Concurrency Problems**: Race conditions, deadlocks, dining philosophers
5. **Inter-Process Communication**: Pipes, named pipes, shared memory
6. **Network Programming**: TCP/UDP sockets, client-server architecture, multi-client handling

These examples provide a solid foundation for understanding concurrency in Unix-like systems through practical implementations of key concepts.