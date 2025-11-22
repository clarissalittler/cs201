# A Comprehensive Guide to Inter-Process Communication (IPC) in Linux

## Table of Contents

1. [Introduction to IPC](#introduction-to-ipc)
2. [Why Do We Need IPC?](#why-do-we-need-ipc)
3. [Overview of IPC Mechanisms](#overview-of-ipc-mechanisms)
4. [Pipes - Anonymous Communication Channels](#pipes---anonymous-communication-channels)
5. [Named Pipes (FIFOs) - Persistent Pipes](#named-pipes-fifos---persistent-pipes)
6. [Message Queues - Structured Messaging](#message-queues---structured-messaging)
7. [Shared Memory - High-Performance Data Sharing](#shared-memory---high-performance-data-sharing)
8. [Sockets - Universal Communication](#sockets---universal-communication)
9. [Comparing IPC Mechanisms](#comparing-ipc-mechanisms)
10. [Best Practices and Patterns](#best-practices-and-patterns)
11. [Troubleshooting and Common Pitfalls](#troubleshooting-and-common-pitfalls)
12. [Conclusion and Further Resources](#conclusion-and-further-resources)

---

## Introduction to IPC

**Inter-Process Communication (IPC)** refers to the mechanisms provided by an operating system that allow processes to exchange data and coordinate their activities. In Unix-like systems (including Linux), processes are isolated from each other by default - each process has its own address space, and one process cannot directly access another process's memory. This isolation is fundamental to system stability and security, but it creates a challenge: how do processes communicate when they need to work together?

IPC mechanisms solve this problem by providing controlled ways for processes to:
- Exchange data
- Synchronize their actions
- Coordinate access to shared resources
- Notify each other of events

### The Process Isolation Problem

When you fork a process in Unix/Linux, the child process gets a copy of the parent's memory. However, changes made in one process don't affect the other:

```c
#include <stdio.h>
#include <unistd.h>

int main() {
    int value = 100;

    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        value = 200;
        printf("Child: value = %d\n", value);  // Prints 200
    } else {
        // Parent process
        sleep(1);  // Wait for child to modify value
        printf("Parent: value = %d\n", value);  // Still prints 100!
    }

    return 0;
}
```

**Output:**
```
Child: value = 200
Parent: value = 100
```

As you can see, even though the child modified `value`, the parent's copy remains unchanged. This is where IPC comes in - it provides explicit mechanisms for processes to share information.

---

## Why Do We Need IPC?

IPC is essential for building modern applications. Here are some common scenarios where IPC is crucial:

### 1. **Modular System Design**
Breaking a large application into smaller, cooperating processes offers several advantages:
- **Isolation**: A crash in one process doesn't bring down the entire system
- **Security**: Different processes can run with different privileges
- **Maintenance**: Individual components can be updated independently
- **Resource management**: Operating system can better manage and schedule resources

**Example:** A web server might use separate processes to handle different clients, preventing one misbehaving client from affecting others.

### 2. **Parallel Processing**
Multiple processes can work on different parts of a problem simultaneously:
- **Performance**: Take advantage of multi-core processors
- **Responsiveness**: Keep UI responsive while background tasks run
- **Scalability**: Distribute work across multiple processes

**Example:** A video transcoding application might use one process per CPU core, coordinating through IPC to process different segments of a video file.

### 3. **Client-Server Architecture**
Many applications follow a client-server model:
- **Web servers**: Handle requests from multiple clients
- **Database servers**: Manage concurrent access to data
- **Print servers**: Queue and manage print jobs
- **File servers**: Share files across a network

### 4. **System Services**
Operating system services and daemons need to communicate:
- **Logging**: Applications send log messages to a central logging daemon
- **Configuration**: Processes notify others when configuration changes
- **Monitoring**: System monitoring tools gather data from various services

### 5. **Legacy Integration**
Sometimes you need to integrate with existing programs:
- Capture output from existing command-line tools
- Chain together multiple utilities (like Unix pipelines)
- Coordinate between programs written in different languages

---

## Overview of IPC Mechanisms

Linux provides several IPC mechanisms, each with different characteristics and use cases:

### Quick Comparison

| Mechanism | Directionality | Speed | Use Case | Complexity |
|-----------|----------------|-------|----------|------------|
| **Pipes** | Unidirectional | Fast | Parent-child communication | Simple |
| **Named Pipes (FIFOs)** | Unidirectional | Fast | Unrelated processes, simple streaming | Simple |
| **Message Queues** | Bidirectional | Medium | Structured messages, priority handling | Medium |
| **Shared Memory** | Bidirectional | Very Fast | Large data sharing, high performance | Complex |
| **Unix Domain Sockets** | Bidirectional | Fast | Local IPC, flexible | Medium |
| **Network Sockets** | Bidirectional | Medium-Slow | Network communication, client-server | Medium-Complex |

### Choosing the Right Mechanism

- **Use pipes** when: Parent-child communication, simple data streaming
- **Use named pipes** when: Unrelated processes need simple one-way communication
- **Use message queues** when: Need structured messages with priorities
- **Use shared memory** when: Need highest performance for large data transfers
- **Use sockets** when: Need bidirectional communication, network capability, or flexible protocols

---

## Pipes - Anonymous Communication Channels

Pipes are the simplest form of IPC in Unix/Linux. They provide a unidirectional data channel between processes, typically between a parent and child process.

### Key Concepts

A pipe is essentially a buffer maintained by the kernel that:
- Has two ends: a **read end** and a **write end**
- Data written to the write end can be read from the read end
- Operates as a **FIFO** (First In, First Out) queue
- Has a limited buffer size (typically 4KB to 64KB)
- Exists only in kernel memory (not in the filesystem)

### Basic Pipe Operations

#### Creating a Pipe

```c
#include <unistd.h>

int pipe_fd[2];  // Array to hold file descriptors

if (pipe(pipe_fd) == -1) {
    perror("pipe");
    return 1;
}

// pipe_fd[0] is the read end
// pipe_fd[1] is the write end
```

#### The Fork-and-Pipe Pattern

The most common use of pipes is for parent-child communication:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipe_fd[2];
    pid_t pid;
    char message[] = "Hello from parent!";
    char buffer[100];

    // Create the pipe BEFORE forking
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid = fork();

    if (pid == 0) {
        // Child process - will read from pipe
        close(pipe_fd[1]);  // Close unused write end

        ssize_t bytes_read = read(pipe_fd[0], buffer, sizeof(buffer) - 1);
        buffer[bytes_read] = '\0';

        printf("Child received: %s\n", buffer);

        close(pipe_fd[0]);
        exit(0);
    } else {
        // Parent process - will write to pipe
        close(pipe_fd[0]);  // Close unused read end

        write(pipe_fd[1], message, strlen(message));

        close(pipe_fd[1]);  // Close write end to signal EOF
        wait(NULL);  // Wait for child to finish
    }

    return 0;
}
```

### Important Pipe Behaviors

#### 1. **Blocking Behavior**
- `read()` blocks when the pipe is empty until data arrives
- `write()` blocks when the pipe buffer is full
- `read()` returns 0 (EOF) when all write ends are closed

#### 2. **Closing Pipe Ends**
It's crucial to close unused pipe ends:

```c
// After fork, in child:
close(pipe_fd[1]);  // Child won't write, so close write end

// After fork, in parent:
close(pipe_fd[0]);  // Parent won't read, so close read end
```

**Why is this important?**
- If the child doesn't close the write end, `read()` will never return EOF (even when parent closes its write end)
- Helps the kernel detect when no more data will be written
- Prevents resource leaks

#### 3. **SIGPIPE Signal**
If a process tries to write to a pipe with no readers:
- The kernel sends a `SIGPIPE` signal to the writing process
- Default action is to terminate the process
- Can be handled with signal handlers or ignored

```c
#include <signal.h>

// Ignore SIGPIPE (write will return -1 with errno = EPIPE instead)
signal(SIGPIPE, SIG_IGN);
```

### Practical Example: Command Pipeline

Pipes are famously used for shell command pipelines. Here's how `ls | grep txt` works conceptually:

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipe_fd[2];

    pipe(pipe_fd);

    if (fork() == 0) {
        // First child: run "ls"
        close(pipe_fd[0]);  // Close read end

        // Redirect stdout to pipe write end
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);

        execlp("ls", "ls", NULL);
        perror("execlp ls");
        exit(1);
    }

    if (fork() == 0) {
        // Second child: run "grep txt"
        close(pipe_fd[1]);  // Close write end

        // Redirect stdin to pipe read end
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);

        execlp("grep", "grep", "txt", NULL);
        perror("execlp grep");
        exit(1);
    }

    // Parent
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    wait(NULL);
    wait(NULL);

    return 0;
}
```

### Pipe Limitations

1. **Unidirectional**: Data flows in only one direction
   - For two-way communication, you need two pipes

2. **Related Processes Only**: Pipes are typically used between parent and child
   - No filesystem presence means unrelated processes can't access them

3. **No Message Boundaries**: Pipes are byte streams
   - If you write "Hello" and "World" separately, reader might get "HelloWorld"
   - Need to implement your own protocol (delimiters, length prefixes, etc.)

4. **Limited Buffer Size**: Typically 4KB-64KB
   - Writing more than buffer size can block
   - Need to handle partial reads/writes for large data

### When to Use Pipes

✅ **Good for:**
- Parent-child communication
- Simple data streaming
- Command pipelines
- Capturing program output

❌ **Not suitable for:**
- Communication between unrelated processes
- Bidirectional communication (without using two pipes)
- Applications requiring message boundaries
- Very large data transfers

### Example: Bidirectional Communication with Two Pipes

```c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int parent_to_child[2];  // Parent writes, child reads
    int child_to_parent[2];  // Child writes, parent reads

    pipe(parent_to_child);
    pipe(child_to_parent);

    if (fork() == 0) {
        // Child process
        close(parent_to_child[1]);  // Close unused write end
        close(child_to_parent[0]);  // Close unused read end

        char buffer[100];
        read(parent_to_child[0], buffer, sizeof(buffer));
        printf("Child received: %s\n", buffer);

        char response[] = "Got it!";
        write(child_to_parent[1], response, strlen(response));

        close(parent_to_child[0]);
        close(child_to_parent[1]);
        exit(0);
    } else {
        // Parent process
        close(parent_to_child[0]);  // Close unused read end
        close(child_to_parent[1]);  // Close unused write end

        char message[] = "Hello child!";
        write(parent_to_child[1], message, strlen(message));

        char buffer[100];
        ssize_t n = read(child_to_parent[0], buffer, sizeof(buffer) - 1);
        buffer[n] = '\0';
        printf("Parent received: %s\n", buffer);

        close(parent_to_child[1]);
        close(child_to_parent[0]);
        wait(NULL);
    }

    return 0;
}
```

**See the full commented example:** `concurrency-tutorial/commented-examples/05-ipc/01-pipes.c`

---

## Named Pipes (FIFOs) - Persistent Pipes

Named pipes, also called **FIFOs** (First In, First Out), are similar to anonymous pipes but with one crucial difference: they have a name in the filesystem. This makes them accessible to unrelated processes.

### Key Characteristics

- **Filesystem Presence**: Appear as special files in the filesystem
- **Persistent**: Remain until explicitly deleted with `unlink()`
- **Unidirectional**: Like regular pipes, data flows one way
- **Blocking**: Opening and reading/writing can block
- **Accessible**: Any process with appropriate permissions can use them

### Creating Named Pipes

#### Method 1: Using the `mkfifo` Command

```bash
mkfifo /tmp/myfifo
ls -l /tmp/myfifo
# Output: prw-r--r-- 1 user user 0 Jan 1 12:00 /tmp/myfifo
#         ^
#         'p' indicates it's a pipe (FIFO)
```

#### Method 2: Using `mkfifo()` System Call

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

if (mkfifo("/tmp/myfifo", 0666) == -1) {
    if (errno != EEXIST) {  // OK if it already exists
        perror("mkfifo");
        return 1;
    }
}
```

### Opening Named Pipes

Named pipes can be opened for reading or writing using the standard `open()` call:

```c
#include <fcntl.h>

// Open for reading (blocks until a writer opens)
int read_fd = open("/tmp/myfifo", O_RDONLY);

// Open for writing (blocks until a reader opens)
int write_fd = open("/tmp/myfifo", O_WRONLY);
```

#### Non-Blocking Opens

To avoid blocking when opening:

```c
// Won't block, but returns -1 with errno=ENXIO if no reader
int fd = open("/tmp/myfifo", O_WRONLY | O_NONBLOCK);

if (fd == -1 && errno == ENXIO) {
    printf("No reader available\n");
    // Can retry or handle appropriately
}
```

### Complete Writer Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define FIFO_PATH "/tmp/example_fifo"

int main() {
    int fd;
    char message[100];

    // Create the FIFO
    if (mkfifo(FIFO_PATH, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }

    printf("Writer: Opening FIFO...\n");

    // Open FIFO for writing (blocks until reader opens)
    fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("Writer: FIFO opened. Enter messages (Ctrl+D to quit):\n");

    // Write messages to FIFO
    while (fgets(message, sizeof(message), stdin) != NULL) {
        write(fd, message, strlen(message));
    }

    close(fd);
    printf("Writer: Done.\n");

    return 0;
}
```

### Complete Reader Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_PATH "/tmp/example_fifo"

int main() {
    int fd;
    char buffer[100];
    ssize_t bytes_read;

    printf("Reader: Opening FIFO...\n");

    // Open FIFO for reading (blocks until writer opens)
    fd = open(FIFO_PATH, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("Reader: FIFO opened. Reading messages...\n");

    // Read messages until EOF (writer closes)
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("Received: %s", buffer);
    }

    close(fd);
    unlink(FIFO_PATH);  // Clean up
    printf("Reader: Done.\n");

    return 0;
}
```

### Running the Example

**Terminal 1:**
```bash
gcc writer.c -o writer
./writer
# Opens and waits for reader...
```

**Terminal 2:**
```bash
gcc reader.c -o reader
./reader
# Both programs now connected
```

Now messages typed in Terminal 1 appear in Terminal 2.

### FIFO Behavior Details

#### Opening Behavior

| Mode | Behavior |
|------|----------|
| `O_RDONLY` | Blocks until a process opens FIFO for writing |
| `O_WRONLY` | Blocks until a process opens FIFO for reading |
| `O_RDWR` | Opens immediately (not recommended - can cause issues) |
| `O_NONBLOCK | O_RDONLY` | Opens immediately, returns valid fd |
| `O_NONBLOCK | O_WRONLY` | Opens immediately if reader exists; fails with ENXIO if no reader |

#### Read/Write Behavior

- **Reading**: Blocks when FIFO is empty
- **Writing**: Blocks when FIFO buffer is full
- **EOF**: `read()` returns 0 when all writers close the FIFO
- **SIGPIPE**: Writing when no readers raises SIGPIPE (like regular pipes)

### Practical Example: Logger Service

A common use case is a centralized logging service:

**Logger Service (reader):**
```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define LOG_FIFO "/tmp/logpipe"

int main() {
    int fd;
    char buffer[1024];
    ssize_t bytes_read;
    FILE *log_file;

    // Create FIFO
    if (mkfifo(LOG_FIFO, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }

    log_file = fopen("system.log", "a");

    while (1) {
        // Open FIFO (blocks for writer)
        fd = open(LOG_FIFO, O_RDONLY);

        // Read log messages
        while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';

            time_t now = time(NULL);
            fprintf(log_file, "[%s] %s", ctime(&now), buffer);
            fflush(log_file);
        }

        close(fd);
        // Loop back to wait for next writer
    }

    fclose(log_file);
    unlink(LOG_FIFO);
    return 0;
}
```

**Application (writer):**
```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void log_message(const char *msg) {
    int fd = open("/tmp/logpipe", O_WRONLY);
    if (fd != -1) {
        write(fd, msg, strlen(msg));
        close(fd);
    }
}

int main() {
    log_message("Application started\n");
    // ... do work ...
    log_message("Processing complete\n");
    return 0;
}
```

### Cleanup

Always clean up FIFOs when done:

```c
unlink("/tmp/myfifo");
```

Or from the shell:
```bash
rm /tmp/myfifo
```

### When to Use Named Pipes

✅ **Good for:**
- Communication between unrelated processes
- Simple client-server patterns
- Logging and monitoring
- Command pipelines across different programs
- One-way data streaming

❌ **Not suitable for:**
- Bidirectional communication (need two FIFOs)
- Applications requiring message boundaries
- Network communication
- High-frequency, low-latency messaging

**See the full commented example:** `concurrency-tutorial/commented-examples/05-ipc/02-named-pipes.c`

---

## Message Queues - Structured Messaging

Message queues provide a way to pass **structured messages** between processes. Unlike pipes, message queues maintain message boundaries and support priority-based retrieval.

### Key Features

- **Message Boundaries**: Each send creates a discrete message
- **Type-Based Selection**: Retrieve messages by type/priority
- **Asynchronous**: Sender doesn't block waiting for receiver
- **Persistent**: Messages remain in queue until explicitly retrieved
- **Kernel-Managed**: Kernel maintains the queue

### Two Flavors: System V vs POSIX

Linux supports two message queue APIs:

| Feature | System V | POSIX |
|---------|----------|-------|
| **Creation** | `msgget()` | `mq_open()` |
| **Send** | `msgsnd()` | `mq_send()` |
| **Receive** | `msgrcv()` | `mq_receive()` |
| **Header** | `<sys/msg.h>` | `<mqueue.h>` |
| **Linking** | Default | `-lrt` |
| **Naming** | Integer keys | Filesystem-like paths |

We'll focus on System V (more traditional) and briefly cover POSIX.

### System V Message Queues

#### Creating a Message Queue

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// Generate a unique key
key_t key = ftok("/tmp/myfile", 'A');  // File must exist

// Create or get message queue
int msgid = msgget(key, IPC_CREAT | 0666);
if (msgid == -1) {
    perror("msgget");
    return 1;
}
```

#### Message Structure

Messages have a specific structure:

```c
struct message {
    long msg_type;     // Message type (must be > 0)
    char msg_text[100]; // Message data (can be any structure)
};
```

The `msg_type` field is mandatory and must be positive. It allows selective message retrieval.

#### Sending Messages

```c
#include <string.h>

struct message msg;

msg.msg_type = 1;  // Message type
strcpy(msg.msg_text, "Hello, Message Queue!");

// Send message
// Size is the size of the data ONLY (excluding msg_type)
if (msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
    perror("msgsnd");
    return 1;
}
```

**Important:** The size parameter should be `sizeof(message) - sizeof(long)`, or just the size of the data portion.

#### Receiving Messages

```c
struct message msg;

// Receive message of type 1
// 0 flag means block until message arrives
ssize_t bytes = msgrcv(msgid, &msg, sizeof(msg.msg_text), 1, 0);

if (bytes == -1) {
    perror("msgrcv");
    return 1;
}

printf("Received: %s\n", msg.msg_text);
```

#### Message Type Filtering

The `msgtyp` parameter in `msgrcv()` controls which message is retrieved:

```c
// msgrcv(msgid, &msg, size, msgtyp, flags)

// Get first message of any type
msgrcv(msgid, &msg, size, 0, 0);

// Get first message of type 5
msgrcv(msgid, &msg, size, 5, 0);

// Get first message with type <= 5 (lowest type first)
msgrcv(msgid, &msg, size, -5, 0);
```

### Complete Example: Priority Message System

**Sender:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_TEXT 100

struct message {
    long msg_type;
    char msg_text[MAX_TEXT];
};

int main() {
    key_t key;
    int msgid;
    struct message msg;

    // Create unique key
    key = ftok(".", 'M');

    // Create message queue
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        return 1;
    }

    printf("Message Queue created (ID: %d)\n", msgid);

    // Send messages with different priorities
    // Type 1 = Low priority
    msg.msg_type = 1;
    strcpy(msg.msg_text, "Low priority message");
    msgsnd(msgid, &msg, strlen(msg.msg_text) + 1, 0);

    // Type 2 = Medium priority
    msg.msg_type = 2;
    strcpy(msg.msg_text, "Medium priority message");
    msgsnd(msgid, &msg, strlen(msg.msg_text) + 1, 0);

    // Type 3 = High priority
    msg.msg_type = 3;
    strcpy(msg.msg_text, "High priority message");
    msgsnd(msgid, &msg, strlen(msg.msg_text) + 1, 0);

    printf("Sent 3 messages with different priorities\n");

    return 0;
}
```

**Receiver:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_TEXT 100

struct message {
    long msg_type;
    char msg_text[MAX_TEXT];
};

int main() {
    key_t key;
    int msgid;
    struct message msg;

    key = ftok(".", 'M');
    msgid = msgget(key, 0666);

    if (msgid == -1) {
        perror("msgget");
        return 1;
    }

    printf("Receiving messages (high to low priority):\n");

    // Receive high priority first (type 3)
    msgrcv(msgid, &msg, MAX_TEXT, 3, 0);
    printf("Priority 3: %s\n", msg.msg_text);

    // Then medium (type 2)
    msgrcv(msgid, &msg, MAX_TEXT, 2, 0);
    printf("Priority 2: %s\n", msg.msg_text);

    // Finally low (type 1)
    msgrcv(msgid, &msg, MAX_TEXT, 1, 0);
    printf("Priority 1: %s\n", msg.msg_text);

    // Cleanup: remove message queue
    msgctl(msgid, IPC_RMID, NULL);
    printf("Message queue removed\n");

    return 0;
}
```

**Output:**
```
Priority 3: High priority message
Priority 2: Medium priority message
Priority 1: Low priority message
```

### POSIX Message Queues

POSIX message queues offer a cleaner API:

**Sender:**
```c
#include <mqueue.h>
#include <string.h>
#include <stdio.h>

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char message[] = "Hello POSIX MQ!";

    // Set queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;      // Max messages in queue
    attr.mq_msgsize = 100;    // Max message size
    attr.mq_curmsgs = 0;

    // Open/create queue
    mq = mq_open("/myqueue", O_CREAT | O_WRONLY, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    // Send message (priority 0)
    if (mq_send(mq, message, strlen(message) + 1, 0) == -1) {
        perror("mq_send");
        return 1;
    }

    printf("Message sent\n");

    mq_close(mq);
    return 0;
}
```

**Receiver:**
```c
#include <mqueue.h>
#include <stdio.h>

int main() {
    mqd_t mq;
    char buffer[100];
    unsigned int prio;

    // Open existing queue
    mq = mq_open("/myqueue", O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    // Receive message
    ssize_t bytes = mq_receive(mq, buffer, sizeof(buffer), &prio);
    if (bytes == -1) {
        perror("mq_receive");
        return 1;
    }

    printf("Received (priority %u): %s\n", prio, buffer);

    mq_close(mq);
    mq_unlink("/myqueue");  // Remove queue

    return 0;
}
```

**Compile POSIX version:**
```bash
gcc sender.c -o sender -lrt
gcc receiver.c -o receiver -lrt
```

### Managing Message Queues

#### View Existing Queues

**System V:**
```bash
ipcs -q
```

**POSIX:**
```bash
ls -l /dev/mqueue/
```

#### Remove Queue

**System V (in code):**
```c
msgctl(msgid, IPC_RMID, NULL);
```

**System V (command line):**
```bash
ipcrm -q <msgid>
```

**POSIX (in code):**
```c
mq_unlink("/myqueue");
```

**POSIX (command line):**
```bash
rm /dev/mqueue/myqueue
```

### When to Use Message Queues

✅ **Good for:**
- Asynchronous communication
- Priority-based message handling
- Decoupling sender and receiver
- Work queues and task distribution
- Event notification systems

❌ **Not suitable for:**
- Large data transfers (use shared memory)
- Strict real-time requirements
- Network communication
- Very high message rates

**See also:** `ipc-lecture/msgqSender1.c` and `ipc-lecture/msgqReceiver1.c`

---

## Shared Memory - High-Performance Data Sharing

Shared memory is the **fastest** form of IPC because it allows multiple processes to access the same region of memory directly, without copying data through the kernel.

### Key Concepts

- **Direct Access**: Processes access shared memory like regular memory
- **No Kernel Overhead**: After setup, no kernel involvement in data transfer
- **No Synchronization**: You must implement your own synchronization
- **High Performance**: Ideal for large data transfers
- **Complexity**: Requires careful synchronization to avoid race conditions

### How Shared Memory Works

```
Process A Memory          Shared Memory          Process B Memory
┌─────────────────┐      ┌─────────────┐       ┌─────────────────┐
│   Stack         │      │             │       │   Stack         │
│   Heap          │      │  SHARED     │       │   Heap          │
│   Data          │──────▶  REGION    ◀───────│   Data          │
│   Code          │      │             │       │   Code          │
└─────────────────┘      └─────────────┘       └─────────────────┘
```

Both processes map the same physical memory into their address spaces.

### Two APIs: System V vs POSIX

| Feature | System V | POSIX |
|---------|----------|-------|
| **Create** | `shmget()` | `shm_open()` |
| **Attach** | `shmat()` | `mmap()` |
| **Detach** | `shmdt()` | `munmap()` |
| **Remove** | `shmctl()` | `shm_unlink()` |
| **Header** | `<sys/shm.h>` | `<sys/mman.h>` |

### System V Shared Memory

#### Creating and Attaching Shared Memory

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdio.h>

int main() {
    key_t key;
    int shmid;
    char *shared_data;

    // Generate unique key
    key = ftok(".", 'S');

    // Create shared memory segment (1024 bytes)
    shmid = shmget(key, 1024, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    printf("Shared memory created (ID: %d)\n", shmid);

    // Attach shared memory to our address space
    shared_data = (char *)shmat(shmid, NULL, 0);
    if (shared_data == (char *)-1) {
        perror("shmat");
        return 1;
    }

    printf("Attached at address: %p\n", (void *)shared_data);

    // Use shared memory like regular memory
    strcpy(shared_data, "Hello from shared memory!");
    printf("Wrote: %s\n", shared_data);

    // Detach from shared memory
    shmdt(shared_data);

    return 0;
}
```

#### Reading from Shared Memory (Another Process)

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    key_t key;
    int shmid;
    char *shared_data;

    key = ftok(".", 'S');

    // Get existing shared memory segment
    shmid = shmget(key, 1024, 0666);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    // Attach shared memory
    shared_data = (char *)shmat(shmid, NULL, 0);
    if (shared_data == (char *)-1) {
        perror("shmat");
        return 1;
    }

    sleep(1);  // Give writer time to write

    // Read from shared memory
    printf("Read: %s\n", shared_data);

    // Detach and cleanup
    shmdt(shared_data);
    shmctl(shmid, IPC_RMID, NULL);  // Remove shared memory

    return 0;
}
```

### The Synchronization Problem

Without synchronization, shared memory access can lead to race conditions:

```c
// DANGEROUS: Both processes doing this simultaneously
shared_data->counter++;  // This is NOT atomic!

// What actually happens:
// 1. Read current value
// 2. Increment it
// 3. Write it back
// Another process could interleave these steps!
```

### Synchronization with Semaphores

Semaphores are the traditional solution for synchronizing shared memory access:

```c
#include <sys/sem.h>

// Semaphore operations
struct sembuf {
    unsigned short sem_num;  // Semaphore number in set
    short sem_op;            // Operation: -1 (P/wait), +1 (V/signal)
    short sem_flg;           // Flags (0 or IPC_NOWAIT, SEM_UNDO)
};

// P operation (wait/acquire/decrement)
void sem_wait(int semid) {
    struct sembuf sb = {0, -1, 0};
    semop(semid, &sb, 1);
}

// V operation (signal/release/increment)
void sem_signal(int semid) {
    struct sembuf sb = {0, 1, 0};
    semop(semid, &sb, 1);
}
```

### Complete Producer-Consumer Example

This example demonstrates the classic producer-consumer pattern using shared memory and semaphores:

**Shared Data Structure:**
```c
typedef struct {
    int buffer[10];  // Circular buffer
    int in;          // Producer index
    int out;         // Consumer index
    int count;       // Number of items
} SharedData;
```

**Producer:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define NUM_ITEMS 20

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
} SharedData;

// Semaphore operations
void sem_wait(int semid, int sem_num) {
    struct sembuf sb = {sem_num, -1, 0};
    semop(semid, &sb, 1);
}

void sem_signal(int semid, int sem_num) {
    struct sembuf sb = {sem_num, 1, 0};
    semop(semid, &sb, 1);
}

int main() {
    key_t shm_key = ftok(".", 'S');
    key_t sem_key = ftok(".", 'E');

    // Create shared memory
    int shmid = shmget(shm_key, sizeof(SharedData), IPC_CREAT | 0666);
    SharedData *data = (SharedData *)shmat(shmid, NULL, 0);

    data->in = 0;
    data->out = 0;

    // Create semaphores: mutex, empty, full
    int semid = semget(sem_key, 3, IPC_CREAT | 0666);

    // Initialize semaphores
    semctl(semid, 0, SETVAL, 1);              // mutex = 1
    semctl(semid, 1, SETVAL, BUFFER_SIZE);    // empty = BUFFER_SIZE
    semctl(semid, 2, SETVAL, 0);              // full = 0

    printf("Producer: Starting to produce %d items\n", NUM_ITEMS);

    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = i + 1;

        sem_wait(semid, 1);  // Wait for empty slot
        sem_wait(semid, 0);  // Acquire mutex

        // Critical section: add item to buffer
        data->buffer[data->in] = item;
        printf("Producer: Produced item %d at index %d\n", item, data->in);
        data->in = (data->in + 1) % BUFFER_SIZE;

        sem_signal(semid, 0);  // Release mutex
        sem_signal(semid, 2);  // Signal full slot

        usleep(100000);  // 100ms delay
    }

    printf("Producer: Done\n");

    shmdt(data);
    return 0;
}
```

**Consumer:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define NUM_ITEMS 20

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
} SharedData;

void sem_wait(int semid, int sem_num) {
    struct sembuf sb = {sem_num, -1, 0};
    semop(semid, &sb, 1);
}

void sem_signal(int semid, int sem_num) {
    struct sembuf sb = {sem_num, 1, 0};
    semop(semid, &sb, 1);
}

int main() {
    key_t shm_key = ftok(".", 'S');
    key_t sem_key = ftok(".", 'E');

    // Attach to existing shared memory
    int shmid = shmget(shm_key, sizeof(SharedData), 0666);
    SharedData *data = (SharedData *)shmat(shmid, NULL, 0);

    // Get existing semaphores
    int semid = semget(sem_key, 3, 0666);

    printf("Consumer: Starting to consume %d items\n", NUM_ITEMS);

    for (int i = 0; i < NUM_ITEMS; i++) {
        sem_wait(semid, 2);  // Wait for full slot
        sem_wait(semid, 0);  // Acquire mutex

        // Critical section: remove item from buffer
        int item = data->buffer[data->out];
        printf("Consumer: Consumed item %d from index %d\n", item, data->out);
        data->out = (data->out + 1) % BUFFER_SIZE;

        sem_signal(semid, 0);  // Release mutex
        sem_signal(semid, 1);  // Signal empty slot

        usleep(150000);  // 150ms delay
    }

    printf("Consumer: Done\n");

    // Cleanup
    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID, 0);

    return 0;
}
```

### POSIX Shared Memory

POSIX shared memory uses `shm_open()` and `mmap()`:

**Writer:**
```c
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main() {
    const char *name = "/myshm";
    const int SIZE = 4096;

    // Create shared memory object
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    // Set size
    ftruncate(shm_fd, SIZE);

    // Map it into memory
    void *ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Write to shared memory
    sprintf((char *)ptr, "Hello from POSIX shared memory!");

    printf("Wrote to shared memory\n");

    munmap(ptr, SIZE);
    close(shm_fd);

    return 0;
}
```

**Reader:**
```c
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    const char *name = "/myshm";
    const int SIZE = 4096;

    // Open existing shared memory
    int shm_fd = shm_open(name, O_RDONLY, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    // Map it into memory
    void *ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Read from shared memory
    printf("Read: %s\n", (char *)ptr);

    // Cleanup
    munmap(ptr, SIZE);
    close(shm_fd);
    shm_unlink(name);  // Remove shared memory object

    return 0;
}
```

**Compile POSIX version:**
```bash
gcc writer.c -o writer -lrt
gcc reader.c -o reader -lrt
```

### Managing Shared Memory

#### View Shared Memory Segments

**System V:**
```bash
ipcs -m
```

**POSIX:**
```bash
ls -l /dev/shm/
```

#### Remove Shared Memory

**System V:**
```bash
ipcrm -m <shmid>
```

**POSIX:**
```bash
rm /dev/shm/myshm
```

### When to Use Shared Memory

✅ **Good for:**
- High-performance data transfer
- Large data structures
- Real-time applications
- Producer-consumer patterns
- Shared data structures (databases, caches)

❌ **Not suitable for:**
- Simple message passing
- Network communication
- When synchronization is difficult
- When processes are on different machines

**See the full commented example:** `concurrency-tutorial/commented-examples/05-ipc/03-shared-memory.c`

---

## Sockets - Universal Communication

Sockets are the most versatile IPC mechanism, supporting both local and network communication. They provide bidirectional, connection-oriented or connectionless communication.

### Socket Types

#### 1. **Unix Domain Sockets** (Local IPC)
- Communication on same machine
- Higher performance than network sockets
- Access control via filesystem permissions
- Bidirectional communication

#### 2. **Network Sockets** (TCP/UDP)
- Communication across network
- TCP: Reliable, connection-oriented
- UDP: Unreliable, connectionless
- Standard for client-server applications

### Socket Fundamentals

#### Socket Families (Domains)

```c
AF_UNIX   // Unix domain sockets (local IPC)
AF_INET   // IPv4 Internet protocols
AF_INET6  // IPv6 Internet protocols
```

#### Socket Types

```c
SOCK_STREAM  // TCP - reliable, connection-oriented, byte stream
SOCK_DGRAM   // UDP - unreliable, connectionless, datagrams
```

### Unix Domain Sockets

Perfect for local IPC with bidirectional communication:

#### Server (Listener)

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/my_socket"
#define BUFFER_SIZE 256

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];

    // 1. Create socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    // 2. Setup address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Remove socket if it already exists
    unlink(SOCKET_PATH);

    // 3. Bind socket to address
    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    // 4. Listen for connections (backlog of 5)
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("Server listening on %s\n", SOCKET_PATH);

    // 5. Accept connection
    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("accept");
        close(server_fd);
        return 1;
    }

    printf("Client connected\n");

    // 6. Communicate
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_read <= 0) {
            break;  // Client disconnected
        }

        printf("Received: %s", buffer);

        // Echo back
        send(client_fd, buffer, bytes_read, 0);
    }

    // 7. Cleanup
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}
```

#### Client

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/my_socket"
#define BUFFER_SIZE 256

int main() {
    int sock_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];

    // 1. Create socket
    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return 1;
    }

    // 2. Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // 3. Connect to server
    if (connect(sock_fd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1) {
        perror("connect");
        close(sock_fd);
        return 1;
    }

    printf("Connected to server. Type messages (Ctrl+D to quit):\n");

    // 4. Communicate
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        // Send message
        send(sock_fd, buffer, strlen(buffer), 0);

        // Receive echo
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes > 0) {
            printf("Server echo: %s", buffer);
        }
    }

    // 5. Cleanup
    close(sock_fd);

    return 0;
}
```

### TCP Network Sockets

For network communication between different machines:

#### TCP Server

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    // Set SO_REUSEADDR to avoid "Address already in use" error
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Accept from any interface
    server_addr.sin_port = htons(PORT);        // Convert to network byte order

    // 3. Bind
    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    // 4. Listen
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("Server listening on port %d\n", PORT);

    // 5. Accept connection
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        return 1;
    }

    printf("Client connected from %s:%d\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    // 6. Echo loop
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_read <= 0) {
            break;
        }

        printf("Received: %s", buffer);
        send(client_fd, buffer, bytes_read, 0);
    }

    close(client_fd);
    close(server_fd);

    return 0;
}
```

#### TCP Client

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // 1. Create socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        return 1;
    }

    // 2. Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IP address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock_fd);
        return 1;
    }

    // 3. Connect to server
    if (connect(sock_fd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock_fd);
        return 1;
    }

    printf("Connected to server. Type messages:\n");

    // 4. Communication loop
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        send(sock_fd, buffer, strlen(buffer), 0);

        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes > 0) {
            printf("Echo: %s", buffer);
        }
    }

    close(sock_fd);
    return 0;
}
```

### Multi-Client Server with Threads

Handle multiple clients concurrently:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

        if (bytes <= 0) {
            break;
        }

        printf("Client %d: %s", client_fd, buffer);
        send(client_fd, buffer, bytes, 0);
    }

    close(client_fd);
    printf("Client %d disconnected\n", client_fd);

    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 10);

    printf("Multi-client server listening on port %d\n", PORT);

    while (1) {
        int *client_fd = malloc(sizeof(int));
        *client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

        if (*client_fd < 0) {
            perror("accept");
            free(client_fd);
            continue;
        }

        printf("New client connected: %d\n", *client_fd);

        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, client_fd);
        pthread_detach(thread);  // Auto-cleanup when thread exits
    }

    close(server_fd);
    return 0;
}
```

**Compile with threads:**
```bash
gcc server.c -o server -pthread
```

### Socket Best Practices

1. **Always check return values**
   ```c
   if (socket(...) < 0) {
       perror("socket");
       // Handle error
   }
   ```

2. **Use SO_REUSEADDR** to avoid "Address already in use"
   ```c
   int opt = 1;
   setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
   ```

3. **Handle SIGPIPE** for broken connections
   ```c
   signal(SIGPIPE, SIG_IGN);
   ```

4. **Close sockets** when done
   ```c
   close(socket_fd);
   ```

5. **Set timeouts** for production code
   ```c
   struct timeval timeout;
   timeout.tv_sec = 5;
   timeout.tv_usec = 0;
   setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
   ```

### When to Use Sockets

✅ **Good for:**
- Client-server applications
- Network communication
- Bidirectional communication
- Flexible protocols
- Multi-client scenarios

❌ **Not suitable for:**
- Simple parent-child communication (use pipes)
- Highest performance local IPC (use shared memory)
- When simplicity is key (use simpler mechanisms)

**See also:**
- `concurrency-tutorial/commented-examples/05-ipc/04-socket-ipc.c`
- `ipc-lecture/echoserver.c` and `ipc-lecture/echoclient.c`
- `concurrencyGuide.org` (Socket section)

---

## Comparing IPC Mechanisms

### Performance Comparison

| Mechanism | Throughput | Latency | CPU Usage | Notes |
|-----------|------------|---------|-----------|-------|
| **Shared Memory** | ★★★★★ | ★★★★★ | ★★★★★ | Fastest, but requires synchronization |
| **Pipes** | ★★★★☆ | ★★★★☆ | ★★★★☆ | Fast for small data |
| **Unix Domain Sockets** | ★★★★☆ | ★★★☆☆ | ★★★☆☆ | Good balance |
| **Message Queues** | ★★★☆☆ | ★★★☆☆ | ★★★☆☆ | Message overhead |
| **Network Sockets** | ★★☆☆☆ | ★★☆☆☆ | ★★☆☆☆ | Protocol overhead |

### Feature Comparison

| Feature | Pipes | FIFOs | Msg Queues | Shared Mem | Sockets |
|---------|-------|-------|------------|------------|---------|
| **Bidirectional** | ❌ | ❌ | ✅ | ✅ | ✅ |
| **Unrelated Processes** | ❌ | ✅ | ✅ | ✅ | ✅ |
| **Network Capable** | ❌ | ❌ | ❌ | ❌ | ✅ |
| **Message Boundaries** | ❌ | ❌ | ✅ | Manual | Stream/Datagram |
| **Synchronization** | Built-in | Built-in | Built-in | Manual | Built-in |
| **Persistence** | ❌ | ✅ | ✅ | ✅ | ❌ |

### Use Case Decision Tree

```
Need network communication?
├─ Yes → Use Network Sockets (TCP/UDP)
└─ No → Need highest performance?
    ├─ Yes → Use Shared Memory + Semaphores
    └─ No → Need message boundaries?
        ├─ Yes → Use Message Queues
        └─ No → Related processes?
            ├─ Yes → Use Pipes
            └─ No → Need bidirectional?
                ├─ Yes → Use Unix Domain Sockets
                └─ No → Use Named Pipes (FIFOs)
```

---

## Best Practices and Patterns

### 1. Error Handling

Always check return values and provide meaningful error messages:

```c
if (pipe(pipe_fd) == -1) {
    fprintf(stderr, "ERROR: Failed to create pipe: %s\n", strerror(errno));
    return EXIT_FAILURE;
}
```

### 2. Resource Cleanup

Ensure resources are cleaned up even on error paths:

```c
int fd = open("/tmp/fifo", O_RDONLY);
if (fd == -1) {
    perror("open");
    return 1;
}

// Do work...

// Always clean up
close(fd);
unlink("/tmp/fifo");
```

Better pattern with goto:

```c
int ret = EXIT_SUCCESS;
int fd = -1;

fd = open("/tmp/fifo", O_RDONLY);
if (fd == -1) {
    perror("open");
    ret = EXIT_FAILURE;
    goto cleanup;
}

// Do work...

cleanup:
    if (fd != -1) close(fd);
    unlink("/tmp/fifo");
    return ret;
```

### 3. Producer-Consumer Pattern

```c
// Producer
while (have_data()) {
    wait_for_empty_slot();
    lock();
    add_to_buffer(data);
    unlock();
    signal_data_available();
}

// Consumer
while (need_data()) {
    wait_for_data_available();
    lock();
    data = get_from_buffer();
    unlock();
    signal_empty_slot();
    process(data);
}
```

### 4. Handle Partial Reads/Writes

Sockets and pipes may not transfer all data at once:

```c
ssize_t write_all(int fd, const void *buf, size_t count) {
    size_t total = 0;
    const char *ptr = buf;

    while (total < count) {
        ssize_t written = write(fd, ptr + total, count - total);
        if (written <= 0) {
            return -1;  // Error
        }
        total += written;
    }

    return total;
}

ssize_t read_all(int fd, void *buf, size_t count) {
    size_t total = 0;
    char *ptr = buf;

    while (total < count) {
        ssize_t bytes_read = read(fd, ptr + total, count - total);
        if (bytes_read < 0) {
            return -1;  // Error
        }
        if (bytes_read == 0) {
            break;  // EOF
        }
        total += bytes_read;
    }

    return total;
}
```

### 5. Signal Handling

Handle signals gracefully:

```c
#include <signal.h>

volatile sig_atomic_t keep_running = 1;

void signal_handler(int sig) {
    keep_running = 0;
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    while (keep_running) {
        // Do work
    }

    // Clean shutdown
    cleanup();

    return 0;
}
```

### 6. Avoid Deadlocks

Always acquire locks in the same order:

```c
// Process A and B both need lock1 and lock2

// CORRECT: Always acquire in same order
lock(lock1);
lock(lock2);
// ... critical section ...
unlock(lock2);
unlock(lock1);

// WRONG: Process A acquires lock1 then lock2
//        Process B acquires lock2 then lock1
//        → Deadlock!
```

### 7. Use Timeouts

Avoid hanging forever:

```c
struct timespec timeout;
timeout.tv_sec = 5;
timeout.tv_nsec = 0;

struct sembuf sb = {0, -1, 0};
if (semtimedop(semid, &sb, 1, &timeout) == -1) {
    if (errno == EAGAIN) {
        fprintf(stderr, "Timeout waiting for semaphore\n");
    }
}
```

---

## Troubleshooting and Common Pitfalls

### Pipes

**Problem:** `read()` never returns even though writer finished

**Solution:** Make sure all write ends are closed
```c
// In parent after fork:
close(pipe_fd[1]);  // Close parent's write end if not using it
```

**Problem:** Broken pipe (SIGPIPE)

**Solution:** Handle the signal or check for closed readers
```c
signal(SIGPIPE, SIG_IGN);
```

### FIFOs

**Problem:** `open()` blocks forever

**Solution:** Ensure reader and writer both try to open
```c
// Or use non-blocking open:
int fd = open(FIFO_PATH, O_WRONLY | O_NONBLOCK);
```

**Problem:** "File exists" error

**Solution:** Handle EEXIST when creating FIFO
```c
if (mkfifo(path, 0666) == -1 && errno != EEXIST) {
    perror("mkfifo");
}
```

### Shared Memory

**Problem:** Segmentation fault accessing shared memory

**Solution:** Check that memory is attached
```c
char *ptr = (char *)shmat(shmid, NULL, 0);
if (ptr == (char *)-1) {
    perror("shmat");
    // Don't use ptr!
}
```

**Problem:** Race conditions

**Solution:** Always use synchronization
```c
// Use semaphores, mutexes, or other synchronization
```

**Problem:** Shared memory persists after crash

**Solution:** Clean up manually
```bash
ipcs -m  # List shared memory
ipcrm -m <shmid>  # Remove it
```

### Sockets

**Problem:** "Address already in use"

**Solution:** Set SO_REUSEADDR
```c
int opt = 1;
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```

**Problem:** Client can't connect

**Solution:** Ensure server is listening and firewall allows connection
```bash
netstat -an | grep 8080  # Check if port is listening
```

**Problem:** Data corruption or incomplete messages

**Solution:** Implement a protocol with length prefix or delimiters
```c
// Send length first, then data
uint32_t len = htonl(data_length);
send(sock, &len, sizeof(len), 0);
send(sock, data, data_length, 0);
```

### Message Queues

**Problem:** Messages not received

**Solution:** Check message type matches
```c
// Sender
msg.msg_type = 1;
msgsnd(msgid, &msg, size, 0);

// Receiver must use same type
msgrcv(msgid, &msg, size, 1, 0);  // Type must match!
```

**Problem:** Queue fills up

**Solution:** Increase queue size or consume messages faster
```bash
# Check queue limits
ipcs -q -l

# Or use POSIX MQ with custom limits
struct mq_attr attr;
attr.mq_maxmsg = 100;  // Increase from default
```

---

## Conclusion and Further Resources

### Summary

IPC mechanisms in Linux provide powerful tools for process communication:

- **Pipes**: Simple, fast, perfect for parent-child communication
- **FIFOs**: Pipes with filesystem presence for unrelated processes
- **Message Queues**: Structured messaging with priorities
- **Shared Memory**: Highest performance for large data sharing
- **Sockets**: Universal solution for local and network communication

### Choosing the Right Mechanism

1. **Start simple**: Use pipes for basic parent-child communication
2. **Scale up**: Use sockets for flexibility and bidirectional communication
3. **Optimize**: Use shared memory when performance is critical
4. **Specialize**: Use message queues when you need message structure and priorities

### Additional Resources

#### Man Pages
```bash
man pipe
man mkfifo
man msgget
man shmget
man socket
man mmap
```

#### Books
- "Advanced Programming in the UNIX Environment" by W. Richard Stevens
- "The Linux Programming Interface" by Michael Kerrisk
- "Unix Network Programming" by W. Richard Stevens

#### Online Resources
- Linux man pages: https://man7.org/linux/man-pages/
- POSIX specification: https://pubs.opengroup.org/onlinepubs/9699919799/
- Beej's Guide to IPC: https://beej.us/guide/bgipc/

#### Example Code in This Repository

- **Pipes**: `concurrency-tutorial/commented-examples/05-ipc/01-pipes.c`
- **Named Pipes**: `concurrency-tutorial/commented-examples/05-ipc/02-named-pipes.c`
- **Shared Memory**: `concurrency-tutorial/commented-examples/05-ipc/03-shared-memory.c`
- **Sockets**: `concurrency-tutorial/commented-examples/05-ipc/04-socket-ipc.c`
- **Robust Versions**: `concurrency-tutorial/commented-examples/05-ipc-robust/`
- **Lecture Examples**: `ipc-lecture/`
- **Theory**: `concurrency-tutorial/concurrencyGuide.org`

### Practice Exercises

1. **Build a chat application** using Unix domain sockets
2. **Implement a task queue** using message queues
3. **Create a shared memory database** with proper synchronization
4. **Build a pipeline** of processes using pipes (like `cat file.txt | grep pattern | wc -l`)
5. **Develop a client-server application** using TCP sockets

### Final Thoughts

IPC is fundamental to building robust, scalable systems. Master these mechanisms and you'll be able to:

- Design complex multi-process applications
- Build high-performance distributed systems
- Debug inter-process communication issues
- Choose the right tool for each communication pattern

Remember: **Start simple, measure performance, and optimize only when needed.**

---

**Tutorial Version**: 1.0
**Last Updated**: 2025
**License**: Educational use
**Repository**: cs201 - Operating Systems Course Materials
