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

### 🎯 Practice Exercise: Word Counter Pipeline

**Challenge:** Create a program that counts words in a text file using pipes, similar to `cat file.txt | wc -w`.

**Requirements:**
- Parent process reads a file and writes to pipe
- Child process reads from pipe and counts words
- Print the total word count

<details>
<summary><b>💡 Solution</b></summary>

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        // Child: count words from pipe
        close(pipe_fd[1]);  // Close write end

        char buffer[4096];
        int word_count = 0;
        int in_word = 0;
        ssize_t bytes_read;

        while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
            for (ssize_t i = 0; i < bytes_read; i++) {
                if (isspace(buffer[i])) {
                    if (in_word) {
                        word_count++;
                        in_word = 0;
                    }
                } else {
                    in_word = 1;
                }
            }
        }

        // Count last word if file doesn't end with whitespace
        if (in_word) {
            word_count++;
        }

        printf("Word count: %d\n", word_count);

        close(pipe_fd[0]);
        exit(0);

    } else {
        // Parent: read file and write to pipe
        close(pipe_fd[0]);  // Close read end

        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("fopen");
            close(pipe_fd[1]);
            return 1;
        }

        char buffer[4096];
        size_t bytes_read;

        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            write(pipe_fd[1], buffer, bytes_read);
        }

        fclose(file);
        close(pipe_fd[1]);  // Signal EOF to child

        wait(NULL);
    }

    return 0;
}
```

**How it works:**
1. Parent opens the file and reads it in chunks
2. Parent writes chunks to the pipe
3. Child reads from pipe and counts words by detecting whitespace boundaries
4. Parent closes write end when done, signaling EOF to child
5. Child prints final count

**Test it:**
```bash
gcc word_counter.c -o word_counter
echo "Hello world from pipes!" > test.txt
./word_counter test.txt
# Output: Word count: 4
```

**Key Learning Points:**
- Proper pipe end closure is critical
- EOF detection (read returns 0) happens when all write ends close
- Pipes are byte streams, not message streams
</details>

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

### 🎯 Practice Exercise: Temperature Monitor System

**Challenge:** Build a temperature monitoring system where sensor processes send readings to a monitor process via a named pipe.

**Requirements:**
- Monitor process creates a FIFO and reads temperature data
- Sensor processes send readings in format: "SensorID:Temperature"
- Monitor logs all readings and alerts if temp > 80°C

<details>
<summary><b>💡 Solution</b></summary>

**monitor.c:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define FIFO_PATH "/tmp/temp_fifo"

int main() {
    int fd;
    char buffer[256];
    ssize_t bytes_read;

    // Create FIFO
    if (mkfifo(FIFO_PATH, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }

    printf("Temperature Monitor started. Waiting for sensors...\n");

    // Open FIFO for reading
    fd = open(FIFO_PATH, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("Monitor active. Reading temperature data...\n");

    // Read and process temperature readings
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';

        // Parse sensor ID and temperature
        char sensor_id[32];
        float temperature;

        if (sscanf(buffer, "%31[^:]:%f", sensor_id, &temperature) == 2) {
            printf("[%s] Temperature: %.1f°C", sensor_id, temperature);

            if (temperature > 80.0) {
                printf(" ⚠️  HIGH TEMPERATURE ALERT!\n");
            } else {
                printf(" ✓\n");
            }
        }
    }

    close(fd);
    unlink(FIFO_PATH);
    printf("Monitor shutting down.\n");

    return 0;
}
```

**sensor.c:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define FIFO_PATH "/tmp/temp_fifo"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <sensor_id>\n", argv[0]);
        return 1;
    }

    char *sensor_id = argv[1];
    int fd;
    char message[256];

    printf("Sensor [%s] starting...\n", sensor_id);

    // Seed random number generator
    srand(time(NULL) + getpid());

    // Send 5 temperature readings
    for (int i = 0; i < 5; i++) {
        // Generate random temperature between 60 and 100°C
        float temperature = 60.0 + (rand() % 40);

        // Format message
        snprintf(message, sizeof(message), "%s:%.1f", sensor_id, temperature);

        // Open FIFO for writing
        fd = open(FIFO_PATH, O_WRONLY);
        if (fd == -1) {
            perror("open");
            return 1;
        }

        // Send temperature reading
        write(fd, message, strlen(message));
        close(fd);

        printf("Sensor [%s] sent: %.1f°C\n", sensor_id, temperature);
        sleep(1);
    }

    printf("Sensor [%s] finished.\n", sensor_id);
    return 0;
}
```

**How to run:**
```bash
# Compile
gcc monitor.c -o monitor
gcc sensor.c -o sensor

# Terminal 1: Start monitor
./monitor

# Terminal 2: Start sensor 1
./sensor Kitchen

# Terminal 3: Start sensor 2
./sensor LivingRoom

# Output example:
# [Kitchen] Temperature: 73.0°C ✓
# [LivingRoom] Temperature: 85.0°C ⚠️  HIGH TEMPERATURE ALERT!
```

**Key Learning Points:**
- Multiple writers can write to the same FIFO
- FIFO automatically serializes writes
- Reader gets messages in order they were written
- Each `open()` for writing blocks until reader is ready
- Messages need a protocol (we used "ID:Temp" format)
</details>

---

## Message Queues - Structured Messaging

Message queues provide a way to pass **structured messages** between processes. Unlike pipes, message queues maintain message boundaries and support priority-based retrieval.

### Key Features

- **Message Boundaries**: Each send creates a discrete message
- **Priority-Based Retrieval**: Messages can have priorities
- **Asynchronous**: Sender doesn't block waiting for receiver
- **Persistent**: Messages remain in queue until explicitly retrieved
- **Kernel-Managed**: Kernel maintains the queue

### Modern Approach: POSIX Message Queues

**POSIX message queues** are the recommended approach for new code. They provide a clean, filesystem-like interface and integrate well with modern Linux systems.

**Why use POSIX?**
- ✅ Cleaner API with consistent naming
- ✅ Built-in priority support
- ✅ Filesystem-like paths (e.g., `/myqueue`)
- ✅ Integrates with standard tools (`ls /dev/mqueue/`)
- ✅ Better documentation and examples

**When you might see System V:** Legacy codebases, older textbooks, and some production systems still use the older System V API (`msgget()`, `msgsnd()`, etc.). We'll cover that briefly at the end of this section.

### Basic Operations

POSIX message queues use a simple, filesystem-like interface:

#### Creating/Opening a Queue

```c
#include <mqueue.h>
#include <fcntl.h>

mqd_t mq;
struct mq_attr attr;

// Configure queue attributes
attr.mq_flags = 0;
attr.mq_maxmsg = 10;      // Max messages in queue
attr.mq_msgsize = 256;    // Max message size (bytes)
attr.mq_curmsgs = 0;      // Current number of messages

// Create queue (filesystem-like path)
mq = mq_open("/myqueue", O_CREAT | O_WRONLY, 0644, &attr);
if (mq == (mqd_t)-1) {
    perror("mq_open");
    return 1;
}
```

#### Sending Messages

```c
const char *message = "Hello, POSIX Message Queue!";
unsigned int priority = 5;  // Higher = higher priority (0-31)

// Send message with priority
if (mq_send(mq, message, strlen(message) + 1, priority) == -1) {
    perror("mq_send");
    return 1;
}
```

#### Receiving Messages

```c
char buffer[256];
unsigned int received_priority;
struct mq_attr attr;

// Get queue attributes to know buffer size
mq_getattr(mq, &attr);

// Receive highest priority message
ssize_t bytes = mq_receive(mq, buffer, attr.mq_msgsize, &received_priority);
if (bytes == -1) {
    perror("mq_receive");
    return 1;
}

buffer[bytes] = '\0';
printf("Received (priority %u): %s\n", received_priority, buffer);
```

### Complete Example

**Sender:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>

int main() {
    mqd_t mq;
    struct mq_attr attr;

    // Configure queue
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 256;
    attr.mq_curmsgs = 0;

    // Create/open queue
    mq = mq_open("/demo_queue", O_CREAT | O_WRONLY, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    // Send messages with different priorities
    mq_send(mq, "Low priority task", 18, 1);
    mq_send(mq, "Medium priority task", 21, 5);
    mq_send(mq, "High priority task", 19, 10);

    printf("Sent 3 messages\n");

    mq_close(mq);
    return 0;
}
```

**Receiver:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>

int main() {
    mqd_t mq;
    char buffer[256];
    unsigned int priority;
    struct mq_attr attr;

    // Open existing queue
    mq = mq_open("/demo_queue", O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    mq_getattr(mq, &attr);

    printf("Receiving messages (highest priority first):\n");

    // Receive all 3 messages (automatically sorted by priority)
    for (int i = 0; i < 3; i++) {
        ssize_t bytes = mq_receive(mq, buffer, attr.mq_msgsize, &priority);
        buffer[bytes] = '\0';
        printf("[Priority %u]: %s\n", priority, buffer);
    }

    mq_close(mq);
    mq_unlink("/demo_queue");  // Remove queue

    return 0;
}
```

**Compile & Run:**
```bash
gcc sender.c -o sender -lrt
gcc receiver.c -o receiver -lrt

./sender
./receiver

# Output:
# [Priority 10]: High priority task
# [Priority 5]: Medium priority task
# [Priority 1]: Low priority task
```

### Managing Message Queues

#### View Queues

```bash
# List all POSIX message queues
ls -l /dev/mqueue/
```

#### Remove Queue

**In code:**
```c
mq_unlink("/myqueue");
```

**Command line:**
```bash
rm /dev/mqueue/myqueue
```

#### Get Queue Info

```c
struct mq_attr attr;
mq_getattr(mq, &attr);

printf("Max messages: %ld\n", attr.mq_maxmsg);
printf("Max message size: %ld\n", attr.mq_msgsize);
printf("Current messages: %ld\n", attr.mq_curmsgs);
```

### Legacy Alternative: System V Message Queues

You may encounter System V message queues in older codebases. The API is more complex:

```c
#include <sys/msg.h>
#include <sys/ipc.h>

// Create queue using integer key
key_t key = ftok(".", 'Q');
int msgid = msgget(key, IPC_CREAT | 0666);

// Message must have type field
struct {
    long msg_type;  // Must be > 0
    char text[100];
} msg;

// Send
msg.msg_type = 1;
msgsnd(msgid, &msg, sizeof(msg.text), 0);

// Receive (size is WITHOUT msg_type field!)
msgrcv(msgid, &msg, sizeof(msg.text), 1, 0);

// Cleanup
msgctl(msgid, IPC_RMID, NULL);
```

**Key Differences from POSIX:**

| Aspect | System V | POSIX |
|--------|----------|-------|
| **Naming** | Integer keys via `ftok()` | Filesystem paths (`"/myqueue"`) |
| **API** | `msgget()`, `msgsnd()`, `msgrcv()` | `mq_open()`, `mq_send()`, `mq_receive()` |
| **Priority** | Manual via `msg_type` filtering | Built-in priority parameter |
| **Management** | `ipcs -q`, `ipcrm -q` | `ls /dev/mqueue/`, `rm` |
| **Linking** | Default | Requires `-lrt` |

**When to use System V:** Only when maintaining legacy code or working with systems that don't support POSIX message queues.

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

### 🎯 Practice Exercise: Task Queue with Priorities

**Challenge:** Create a work queue system where a manager process sends tasks with priorities and worker processes fetch them based on priority.

**Requirements:**
- Manager sends tasks with priorities (1=low, 2=normal, 3=high)
- Workers fetch highest priority task first
- Use POSIX message queues for cleaner API

<details>
<summary><b>💡 Solution</b></summary>

**manager.c:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>

#define QUEUE_NAME "/task_queue"
#define MAX_MSG_SIZE 256

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char message[MAX_MSG_SIZE];

    // Configure queue
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Create queue
    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    printf("Task Manager: Sending tasks...\n");

    // Send tasks with different priorities
    const char *tasks[] = {
        "Process invoice #1001",
        "URGENT: Server maintenance",
        "Generate weekly report",
        "CRITICAL: Security patch",
        "Update customer records"
    };

    unsigned int priorities[] = {2, 3, 1, 3, 2};

    for (int i = 0; i < 5; i++) {
        snprintf(message, MAX_MSG_SIZE, "Task %d: %s", i+1, tasks[i]);

        if (mq_send(mq, message, strlen(message) + 1, priorities[i]) == -1) {
            perror("mq_send");
            continue;
        }

        printf("Sent (priority %u): %s\n", priorities[i], tasks[i]);
        sleep(1);
    }

    printf("Manager: All tasks sent.\n");
    mq_close(mq);

    return 0;
}
```

**worker.c:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <unistd.h>

#define QUEUE_NAME "/task_queue"
#define MAX_MSG_SIZE 256

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <worker_id>\n", argv[0]);
        return 1;
    }

    char *worker_id = argv[1];
    mqd_t mq;
    char buffer[MAX_MSG_SIZE];
    unsigned int priority;
    struct mq_attr attr;

    // Open existing queue
    mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    // Get queue attributes
    mq_getattr(mq, &attr);

    printf("Worker [%s]: Ready to process tasks\n", worker_id);

    // Process tasks until queue is empty
    while (1) {
        ssize_t bytes = mq_receive(mq, buffer, attr.mq_msgsize, &priority);

        if (bytes == -1) {
            break;  // Queue empty or error
        }

        buffer[bytes] = '\0';

        printf("Worker [%s]: Processing (priority %u): %s\n",
               worker_id, priority, buffer);

        // Simulate work
        sleep(2);

        printf("Worker [%s]: Completed task\n", worker_id);
    }

    printf("Worker [%s]: No more tasks, shutting down\n", worker_id);

    mq_close(mq);

    // Last worker cleanup
    if (strcmp(worker_id, "cleanup") == 0) {
        mq_unlink(QUEUE_NAME);
        printf("Queue cleaned up\n");
    }

    return 0;
}
```

**How to run:**
```bash
# Compile (link with -lrt)
gcc manager.c -o manager -lrt
gcc worker.c -o worker -lrt

# Terminal 1: Start manager (sends tasks)
./manager

# Terminal 2: Start worker 1
./worker Worker1

# Terminal 3: Start worker 2
./worker Worker2

# Cleanup (after workers finish)
./worker cleanup

# Expected output (workers get highest priority first):
# Worker [Worker1]: Processing (priority 3): Task 2: URGENT: Server maintenance
# Worker [Worker2]: Processing (priority 3): Task 4: CRITICAL: Security patch
# Worker [Worker1]: Processing (priority 2): Task 1: Process invoice #1001
# ...
```

**Key Learning Points:**
- Higher priority numbers = higher priority (processed first)
- POSIX MQ automatically handles priority ordering
- Multiple consumers can read from same queue
- Message boundaries are preserved
- `-lrt` flag required for POSIX message queues
</details>

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

### Modern Approach: POSIX Shared Memory

**POSIX shared memory** uses `shm_open()` and `mmap()`, providing a cleaner interface that integrates well with standard file I/O concepts.

**Why use POSIX?**
- ✅ Filesystem-like naming (`"/myshm"`)
- ✅ Works with standard `mmap()` (also used for file mapping)
- ✅ Consistent with POSIX standards
- ✅ Easier cleanup with `shm_unlink()`
- ✅ Visible in `/dev/shm/`

**When you might see System V:** Legacy code often uses `shmget()`, `shmat()`, and related calls. We'll cover that briefly at the end.

### Basic Operations

POSIX shared memory uses `shm_open()` to create a shared memory object, then `mmap()` to map it into the process's address space.

#### Creating Shared Memory

```c
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main() {
    const char *name = "/myshm";  // Filesystem-like name
    const int SIZE = 4096;

    // 1. Create shared memory object
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    // 2. Set size
    ftruncate(shm_fd, SIZE);

    // 3. Map into memory
    void *ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE,
                     MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // 4. Use like regular memory
    sprintf((char *)ptr, "Hello from POSIX shared memory!");
    printf("Wrote to shared memory\n");

    // 5. Cleanup
    munmap(ptr, SIZE);
    close(shm_fd);

    return 0;
}
```

#### Reading from Shared Memory

```c
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    const char *name = "/myshm";
    const int SIZE = 4096;

    // 1. Open existing shared memory
    int shm_fd = shm_open(name, O_RDONLY, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    // 2. Map into memory (read-only)
    void *ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // 3. Read from shared memory
    printf("Read: %s\n", (char *)ptr);

    // 4. Cleanup
    munmap(ptr, SIZE);
    close(shm_fd);
    shm_unlink(name);  // Remove shared memory object

    return 0;
}
```

**Compile with:**
```bash
gcc writer.c -o writer -lrt
gcc reader.c -o reader -lrt
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

### Managing Shared Memory

```bash
# View POSIX shared memory
ls -l /dev/shm/

# Remove shared memory
rm /dev/shm/myshm

# Or in code:
shm_unlink("/myshm");
```

### Legacy Alternative: System V Shared Memory

You may encounter System V shared memory in older code. The API uses integer keys and separate attach/detach operations:

```c
#include <sys/shm.h>
#include <sys/ipc.h>

// Create using integer key
key_t key = ftok(".", 'S');
int shmid = shmget(key, 1024, IPC_CREAT | 0666);

// Attach to address space
char *ptr = (char *)shmat(shmid, NULL, 0);

// Use normally
strcpy(ptr, "Hello");

// Detach and remove
shmdt(ptr);
shmctl(shmid, IPC_RMID, NULL);
```

**Key Differences from POSIX:**

| Aspect | System V | POSIX |
|--------|----------|-------|
| **Naming** | Integer keys via `ftok()` | Filesystem paths (`"/myshm"`) |
| **API** | `shmget()`, `shmat()`, `shmdt()` | `shm_open()`, `mmap()`, `munmap()` |
| **Visibility** | `ipcs -m` | `ls /dev/shm/` |
| **Cleanup** | `shmctl(IPC_RMID)` or `ipcrm -m` | `shm_unlink()` or `rm` |
| **Integration** | Separate from file I/O | Uses standard `mmap()` |

**When to use System V:** Only when maintaining legacy code or interfacing with older systems.

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

### 🎯 Practice Exercise: Image Processing with Shared Memory

**Challenge:** Build a system where one process loads an image into shared memory and multiple worker processes apply filters concurrently.

**Requirements:**
- Main process creates shared memory for image data
- Workers read from shared memory and write results
- Use semaphores for synchronization
- Simple grayscale conversion as the "filter"

<details>
<summary><b>💡 Solution</b></summary>

**image_loader.c:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#define SHM_NAME "/image_data"
#define SEM_NAME "/image_sem"
#define IMAGE_SIZE 1024  // Simplified: 32x32 RGB image

typedef struct {
    unsigned char pixels[IMAGE_SIZE][3];  // RGB pixels
    int width;
    int height;
    int ready;  // Flag: 1 when image is loaded
} ImageData;

int main() {
    int shm_fd;
    ImageData *img;
    sem_t *sem;

    // Create shared memory
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    // Set size
    ftruncate(shm_fd, sizeof(ImageData));

    // Map to memory
    img = mmap(0, sizeof(ImageData), PROT_READ | PROT_WRITE,
               MAP_SHARED, shm_fd, 0);
    if (img == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Create semaphore
    sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    // Initialize image (create test pattern)
    printf("Loader: Creating test image...\n");

    sem_wait(sem);  // Lock

    img->width = 32;
    img->height = 32;

    // Create a simple gradient pattern
    for (int i = 0; i < IMAGE_SIZE; i++) {
        img->pixels[i][0] = (i * 255) / IMAGE_SIZE;  // R
        img->pixels[i][1] = ((IMAGE_SIZE - i) * 255) / IMAGE_SIZE;  // G
        img->pixels[i][2] = 128;  // B
    }

    img->ready = 1;  // Signal image is ready

    sem_post(sem);  // Unlock

    printf("Loader: Image loaded into shared memory\n");
    printf("Loader: Size: %dx%d\n", img->width, img->height);
    printf("Loader: Workers can now process the image\n");

    // Keep running for workers to access
    printf("Press Enter to cleanup...\n");
    getchar();

    // Cleanup
    munmap(img, sizeof(ImageData));
    close(shm_fd);
    shm_unlink(SHM_NAME);
    sem_close(sem);
    sem_unlink(SEM_NAME);

    printf("Loader: Cleaned up\n");

    return 0;
}
```

**filter_worker.c:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#define SHM_NAME "/image_data"
#define SEM_NAME "/image_sem"
#define IMAGE_SIZE 1024

typedef struct {
    unsigned char pixels[IMAGE_SIZE][3];
    int width;
    int height;
    int ready;
} ImageData;

// Convert RGB to grayscale
unsigned char to_grayscale(unsigned char r, unsigned char g, unsigned char b) {
    return (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <worker_id>\n", argv[0]);
        return 1;
    }

    char *worker_id = argv[1];
    int shm_fd;
    ImageData *img;
    sem_t *sem;

    printf("Worker [%s]: Starting...\n", worker_id);

    // Open shared memory
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        fprintf(stderr, "Worker: Make sure loader is running first!\n");
        return 1;
    }

    // Map to memory
    img = mmap(0, sizeof(ImageData), PROT_READ | PROT_WRITE,
               MAP_SHARED, shm_fd, 0);
    if (img == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Open semaphore
    sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    // Wait for image to be ready
    while (!img->ready) {
        printf("Worker [%s]: Waiting for image...\n", worker_id);
        sleep(1);
    }

    printf("Worker [%s]: Image found, applying grayscale filter...\n", worker_id);

    // Process pixels (each worker processes a portion)
    sem_wait(sem);  // Lock

    int start = (rand() % 4) * 256;  // Random section
    int end = start + 256;

    for (int i = start; i < end && i < IMAGE_SIZE; i++) {
        unsigned char gray = to_grayscale(
            img->pixels[i][0],
            img->pixels[i][1],
            img->pixels[i][2]
        );

        // Write back as grayscale
        img->pixels[i][0] = gray;
        img->pixels[i][1] = gray;
        img->pixels[i][2] = gray;
    }

    sem_post(sem);  // Unlock

    printf("Worker [%s]: Processed pixels %d-%d\n", worker_id, start, end);

    // Calculate average brightness
    int total_brightness = 0;
    for (int i = 0; i < IMAGE_SIZE; i++) {
        total_brightness += img->pixels[i][0];
    }
    int avg = total_brightness / IMAGE_SIZE;

    printf("Worker [%s]: Average brightness: %d/255\n", worker_id, avg);

    // Cleanup
    munmap(img, sizeof(ImageData));
    close(shm_fd);
    sem_close(sem);

    return 0;
}
```

**How to run:**
```bash
# Compile (link with -lrt)
gcc image_loader.c -o loader -lrt
gcc filter_worker.c -o worker -lrt

# Terminal 1: Start loader
./loader

# Terminal 2, 3, 4: Start workers
./worker Worker1
./worker Worker2
./worker Worker3

# Output:
# Loader: Image loaded into shared memory
# Worker [Worker1]: Processed pixels 256-512
# Worker [Worker2]: Processed pixels 512-768
# Worker [Worker3]: Processed pixels 0-256
# Worker [Worker1]: Average brightness: 142/255
```

**Key Learning Points:**
- Shared memory is perfect for large data structures
- Semaphores prevent race conditions during access
- Multiple processes can read simultaneously, but write access must be synchronized
- POSIX shared memory (`shm_open`/`mmap`) is cleaner than System V
- Data persists in shared memory until explicitly removed
- Workers can process different sections concurrently for parallelism
</details>

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

### 🎯 Practice Exercise: Simple Chat Server

**Challenge:** Build a multi-client chat server where clients can send messages that are broadcast to all connected clients.

**Requirements:**
- Server accepts multiple clients using threads
- Messages from one client are broadcast to all others
- Clients can join with a nickname
- Server displays join/leave notifications

<details>
<summary><b>💡 Solution</b></summary>

**chat_server.c:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8888
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    char nickname[32];
    int active;
} Client;

Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast_message(const char *message, int sender_socket) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].socket != sender_socket) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE + 64];
    int client_index = -1;

    // Find slot for this client
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].socket = client_socket;
            clients[i].active = 1;
            client_index = i;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    // Get nickname
    send(client_socket, "Enter your nickname: ", 21, 0);
    ssize_t bytes = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes <= 0) {
        close(client_socket);
        return NULL;
    }

    buffer[bytes] = '\0';
    // Remove newline
    if (buffer[bytes - 1] == '\n') {
        buffer[bytes - 1] = '\0';
    }

    strncpy(clients[client_index].nickname, buffer, 31);
    clients[client_index].nickname[31] = '\0';

    // Announce join
    snprintf(message, sizeof(message), "*** %s has joined the chat ***\n",
             clients[client_index].nickname);
    printf("%s", message);
    broadcast_message(message, client_socket);

    // Handle messages
    while ((bytes = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes] = '\0';

        // Format and broadcast message
        snprintf(message, sizeof(message), "[%s]: %s",
                 clients[client_index].nickname, buffer);
        printf("%s", message);
        broadcast_message(message, client_socket);
    }

    // Client disconnected
    snprintf(message, sizeof(message), "*** %s has left the chat ***\n",
             clients[client_index].nickname);
    printf("%s", message);
    broadcast_message(message, -1);

    // Cleanup
    pthread_mutex_lock(&clients_mutex);
    clients[client_index].active = 0;
    pthread_mutex_unlock(&clients_mutex);

    close(client_socket);
    return NULL;
}

int main() {
    int server_fd, *client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Initialize clients
    memset(clients, 0, sizeof(clients));

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Listen
    listen(server_fd, 5);

    printf("Chat Server started on port %d\n", PORT);
    printf("Waiting for clients...\n");

    while (1) {
        client_fd = malloc(sizeof(int));
        *client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

        if (*client_fd < 0) {
            free(client_fd);
            continue;
        }

        printf("New connection from %s:%d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, client_fd);
        pthread_detach(thread);
    }

    close(server_fd);
    return 0;
}
```

**chat_client.c:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int sock_fd;

void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes;

    while ((bytes = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
        fflush(stdout);
    }

    printf("\nDisconnected from server\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }

    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Connect to server
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    if (connect(sock_fd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect");
        return 1;
    }

    printf("Connected to chat server!\n");

    // Start receive thread
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);
    pthread_detach(recv_thread);

    // Send messages
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        send(sock_fd, buffer, strlen(buffer), 0);
    }

    close(sock_fd);
    return 0;
}
```

**How to run:**
```bash
# Compile
gcc chat_server.c -o chat_server -pthread
gcc chat_client.c -o chat_client -pthread

# Terminal 1: Start server
./chat_server

# Terminal 2: Client 1
./chat_client 127.0.0.1
Enter your nickname: Alice

# Terminal 3: Client 2
./chat_client 127.0.0.1
Enter your nickname: Bob

# Now Alice and Bob can chat!
# Alice: Hello!
# [Alice]: Hello!
# Bob: Hi Alice!
# [Bob]: Hi Alice!
```

**Key Learning Points:**
- Threads allow handling multiple clients concurrently
- Mutex protects shared client list from race conditions
- `broadcast_message()` sends to all clients except sender
- Each client has a receive thread for incoming messages
- Proper cleanup when clients disconnect
- Server can run indefinitely, accepting new clients
</details>

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

**"Advanced Programming in the UNIX Environment" by W. Richard Stevens & Stephen A. Rago**
- Amazon: https://www.amazon.com/Advanced-Programming-UNIX-Environment-3rd/dp/0321637739
- Publisher (Addison-Wesley): https://www.pearson.com/en-us/subject-catalog/p/advanced-programming-in-the-unix-environment/P200000000493
- Often available at university libraries
- Considered the "bible" of Unix systems programming

**"The Linux Programming Interface" by Michael Kerrisk**
- Amazon: https://www.amazon.com/Linux-Programming-Interface-System-Handbook/dp/1593272200
- No Starch Press: https://nostarch.com/tlpi
- Author's website with errata: https://man7.org/tlpi/
- Comprehensive coverage of Linux/UNIX system programming

**"Unix Network Programming" by W. Richard Stevens**
- Volume 1 (Sockets): https://www.amazon.com/Unix-Network-Programming-Sockets-Networking/dp/0131411551
- Volume 2 (IPC): https://www.amazon.com/UNIX-Network-Programming-Vol-Interprocess/dp/0130810819
- Classic reference for network programming and IPC
- Volume 2 specifically focuses on IPC mechanisms

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
