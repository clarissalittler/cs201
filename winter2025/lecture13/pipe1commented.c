// PEDAGOGICAL PURPOSE:
// This program demonstrates UNNAMED PIPES for inter-process communication (IPC).
// Key learning objectives:
// 1. Understanding how pipes enable parent-child process communication
// 2. Learning the pipe() system call and how it creates a one-way communication channel
// 3. Understanding the pipe file descriptor pair (read end and write end)
// 4. Learning why and when to close unused pipe ends
// 5. Understanding that pipes only work between related processes (parent-child)
// 6. Seeing how fork() duplicates file descriptors (including pipes)
// 7. Learning the producer-consumer pattern with pipes
// 8. Understanding that pipes are unidirectional (one-way communication)

#include <stdio.h>
#include <unistd.h>     // Provides pipe(), fork(), read(), write(), close()
#include <string.h>     // Provides strlen()
#include <sys/types.h>  // Provides pid_t type

int main() {
    // PIPE FILE DESCRIPTOR ARRAY:
    // pipe() creates TWO file descriptors (a pair)
    //
    // pipefd[2]:
    //   - Array of 2 integers
    //   - Will be filled by pipe() system call
    //   - pipefd[0]: READ end of the pipe
    //   - pipefd[1]: WRITE end of the pipe
    //
    // WHY TWO FILE DESCRIPTORS?
    //   - Pipes are UNIDIRECTIONAL (one-way)
    //   - Data flows from write end → read end
    //   - Like a physical pipe: pour water in one end, it comes out the other
    //   - You can't reverse the direction
    //
    // MEMORY LAYOUT:
    //   Before pipe(): pipefd[0] = garbage, pipefd[1] = garbage
    //   After pipe():  pipefd[0] = 3 (read), pipefd[1] = 4 (write)
    //                  (actual numbers may vary)
    int pipefd[2];        // pipefd[0] is for reading, pipefd[1] is for writing

    // PROCESS ID VARIABLE:
    // Will store the return value of fork()
    //   - In parent: pid > 0 (child's process ID)
    //   - In child: pid = 0
    //   - On error: pid < 0
    pid_t pid;

    // DATA TO SEND:
    // Message that child will send to parent through the pipe
    // Note: Initialized with explicit length
    char write_msg[] = "Hello from the child process!";

    // BUFFER FOR RECEIVING:
    // Parent will read the message into this buffer
    // WHY 100?
    //   - Large enough to hold our message (30 chars + null terminator)
    //   - Leaving extra space is safe practice
    //   - In real code, should match expected message size
    char read_msg[100];

    // STEP 1: CREATE THE PIPE
    // This creates the communication channel BEFORE forking
    //
    // pipe(pipefd):
    //   - Creates a pipe and stores file descriptors in pipefd
    //   - pipefd[0] = read end
    //   - pipefd[1] = write end
    //   - Returns 0 on success, -1 on failure
    //
    // WHY CREATE PIPE BEFORE fork()?
    //   - fork() duplicates file descriptors
    //   - Both parent and child will have access to SAME pipe
    //   - If we created pipe after fork(), parent and child would have DIFFERENT pipes
    //   - The pipe is the communication channel between them
    //
    // WHAT DOES pipe() ACTUALLY CREATE?
    //   - A kernel buffer (typically 64 KB)
    //   - Data written to pipefd[1] is buffered in kernel
    //   - Data read from pipefd[0] comes from this buffer
    //   - FIFO: First In, First Out
    if(pipe(pipefd) == -1) {
      perror("Pipe failed");
      return 1;
    }

    // INFORMATIVE OUTPUT:
    // Show which file descriptors we got for the pipe
    //
    // getpid(): Returns process ID of calling process
    // pipefd[0]: Read end FD (usually 3 if stdin/stdout/stderr are 0/1/2)
    // pipefd[1]: Write end FD (usually 4, following the read end)
    //
    // WHY PRINT THIS?
    //   - Educational - see that pipes are just file descriptors
    //   - Helps understand that FDs are small integers
    //   - Shows that both parent and child see same FD numbers initially
    printf("Our, %d, reading handle is %d our writing handle is %d\n",
           getpid(), pipefd[0], pipefd[1]);

    // STEP 2: FORK A CHILD PROCESS
    // Creates a copy of the current process
    //
    // WHAT GETS DUPLICATED?
    //   - Memory (code, data, stack, heap)
    //   - File descriptors (including pipefd[0] and pipefd[1])
    //   - Process ID is DIFFERENT
    //
    // AFTER fork():
    //   - Parent has: pipefd[0] = 3, pipefd[1] = 4
    //   - Child has:  pipefd[0] = 3, pipefd[1] = 4
    //   - SAME FDs, point to SAME kernel pipe buffer
    //   - Both can read/write, but we'll split the roles
    pid = fork();

    // CHECK FORK RESULT:
    if (pid < 0) {
        // Fork failed
        // Reasons: out of memory, process limit reached
        perror("Fork failed");
        return 1;
    }
    else if (pid == 0) {
        // CHILD PROCESS CODE
        // This block runs ONLY in the child
        // The child is the WRITER (producer)

        // STEP 3a: CLOSE THE READ END IN CHILD
        // WHY CLOSE THE READ END?
        //   - Child only needs to WRITE
        //   - Closing unused FDs is good practice
        //   - Prevents accidental reads
        //   - Releases system resources
        //   - Helps with EOF detection (explained below)
        //
        // CRITICAL FOR EOF:
        //   - Reader gets EOF when ALL write ends are closed
        //   - If child keeps read end open, parent never gets EOF
        //   - Even though child isn't using it, having it open matters
        //   - Must close ALL unused ends for proper operation
        close(pipefd[0]); // Close the read end of the pipe

        // STEP 4a: WRITE MESSAGE TO PIPE
        // Send the message through the pipe to parent
        //
        // write(pipefd[1], write_msg, strlen(write_msg) + 1):
        //   - pipefd[1]: Write end of pipe
        //   - write_msg: Data to send
        //   - strlen(write_msg) + 1: Number of bytes to send
        //     - strlen() returns length WITHOUT null terminator
        //     - +1 includes the null terminator
        //     - This way parent receives a proper C string
        //
        // WHY +1 FOR NULL TERMINATOR?
        //   - C strings are null-terminated
        //   - Without it, parent might read "Hello from the child process!garbage"
        //   - Including it ensures proper string handling
        //
        // WHAT HAPPENS:
        //   - Data is copied to kernel pipe buffer
        //   - write() returns immediately (usually)
        //   - If buffer is full, write() blocks until space available
        //   - Parent can read this data from pipefd[0]
        write(pipefd[1], write_msg, strlen(write_msg) + 1); // Send message to parent

        // STEP 5a: CLOSE WRITE END AFTER SENDING
        // WHY CLOSE AFTER WRITING?
        //   - We're done sending data
        //   - Signals to reader that no more data is coming
        //   - When all write ends close, reader gets EOF (read returns 0)
        //   - Good practice to close when done
        //   - Child process will exit, which would close it anyway
        //   - But explicit close is clearer
        close(pipefd[1]); // Close the write end of the pipe after writing

        // Child process ends here
        // Could call exit(0) explicitly, but return from main also exits
    }
    else {
        // PARENT PROCESS CODE
        // This block runs ONLY in the parent
        // The parent is the READER (consumer)

        // STEP 3b: CLOSE THE WRITE END IN PARENT
        // WHY CLOSE THE WRITE END?
        //   - Parent only needs to READ
        //   - Same reasons as child closing read end
        //   - Good practice, resource management, prevents errors
        //
        // CRITICAL FOR EOF:
        //   - If parent keeps write end open, it never gets EOF from pipe
        //   - Even though parent doesn't write, having FD open matters
        //   - Parent would block forever waiting for data
        close(pipefd[1]); // Close the write end of the pipe

        // STEP 4b: READ MESSAGE FROM PIPE
        // Receive the message that child sent
        //
        // read(pipefd[0], read_msg, sizeof(read_msg)):
        //   - pipefd[0]: Read end of pipe
        //   - read_msg: Buffer to store received data
        //   - sizeof(read_msg): Maximum bytes to read (100)
        //
        // BEHAVIOR:
        //   - Blocks until data is available (or EOF)
        //   - Returns number of bytes read
        //   - Returns 0 on EOF (all write ends closed)
        //   - Returns -1 on error
        //
        // IN OUR CASE:
        //   - Child wrote 30 bytes ("Hello from the child process!" + null)
        //   - Parent reads up to 100 bytes
        //   - Actually receives 30 bytes
        //   - read_msg now contains "Hello from the child process!\0"
        read(pipefd[0], read_msg, sizeof(read_msg)); // Read the message from the child

        // STEP 5b: PRINT THE RECEIVED MESSAGE
        // Display what we received from the child
        printf("Parent received: %s\n", read_msg);

        // STEP 6b: CLOSE READ END AFTER READING
        // Done with the pipe, close it
        close(pipefd[0]); // Close the read end of the pipe after reading
    }

    return 0;
}

// EXPECTED OUTPUT:
// Our, 12345, reading handle is 3 our writing handle is 4
// Parent received: Hello from the child process!
//
// NOTE: The process IDs will vary each time

// THE PIPE DATA FLOW:
//
// 1. pipe() creates kernel buffer
//
// 2. fork() duplicates file descriptors
//    Parent: pipefd[0]=3, pipefd[1]=4
//    Child:  pipefd[0]=3, pipefd[1]=4
//    Both point to SAME kernel buffer
//
// 3. Child closes pipefd[0], parent closes pipefd[1]
//    Child:  pipefd[1]=4 (write only)
//    Parent: pipefd[0]=3 (read only)
//
// 4. Child writes to pipefd[1]
//    Data: "Hello from the child process!\0"
//    Goes into kernel buffer
//
// 5. Parent reads from pipefd[0]
//    Data comes from kernel buffer
//    Parent receives: "Hello from the child process!\0"
//
// 6. Both close their ends
//    Kernel buffer is deallocated

// PIPE PROPERTIES:
//
// 1. UNIDIRECTIONAL:
//    - Data flows one way only (write end → read end)
//    - For bidirectional, need TWO pipes
//
// 2. FIFO (FIRST IN, FIRST OUT):
//    - Data is read in the order it was written
//    - Like a queue
//
// 3. BLOCKING:
//    - read() blocks if pipe is empty (waits for data)
//    - write() blocks if pipe is full (waits for space)
//
// 4. ATOMIC WRITES UP TO PIPE_BUF:
//    - PIPE_BUF is typically 4096 bytes
//    - Writes smaller than PIPE_BUF are atomic (won't interleave)
//    - Larger writes might be split
//
// 5. RELATED PROCESSES ONLY:
//    - Pipe only works between parent-child or siblings
//    - Can't use pipe for unrelated processes
//    - For unrelated processes: use named pipes (FIFOs)

// WHY CLOSE UNUSED ENDS?
//
// REASON 1: RESOURCE MANAGEMENT
//   - Each open FD uses system resources
//   - Closing releases resources
//
// REASON 2: EOF DETECTION
//   - read() returns 0 (EOF) when ALL write ends are closed
//   - If child keeps read end open, parent might block forever
//   - If parent keeps write end open, parent's read() won't get EOF
//
// EXAMPLE OF PROBLEM:
//   Child writes data but doesn't close pipefd[1]
//   Parent reads data, then tries to read again
//   Child exits, but pipefd[1] is still "open" in parent!
//   Parent's second read() blocks forever waiting for more data
//
// REASON 3: PREVENT DEADLOCK
//   - If both try to write to same pipe, both might block
//   - Closing unused ends enforces clear roles

// COMMON MISTAKES:
//
// MISTAKE 1: Creating pipe after fork()
//   fork();
//   pipe(pipefd);  // WRONG!
//   Result: Parent and child have different pipes, can't communicate
//   Fix: Create pipe BEFORE fork()
//
// MISTAKE 2: Not closing unused ends
//   // Child doesn't close pipefd[0]
//   // Parent doesn't close pipefd[1]
//   Result: EOF never received, potential blocking, resource leak
//   Fix: Always close unused ends immediately after fork()
//
// MISTAKE 3: Closing wrong end
//   Child: close(pipefd[1]);  // WRONG! Child needs to write
//   Result: Child can't send data, parent waits forever
//   Fix: Child closes pipefd[0], parent closes pipefd[1]
//
// MISTAKE 4: Not including null terminator
//   write(pipefd[1], write_msg, strlen(write_msg));  // No +1
//   Result: Parent receives string without null terminator
//   Fix: Add +1 to include null terminator for C strings
//
// MISTAKE 5: Using wrong file descriptor numbers
//   write(pipefd[0], ...);  // WRONG! [0] is read end
//   Result: Error or unexpected behavior
//   Fix: Write to pipefd[1], read from pipefd[0]

// PIPE VS. OTHER IPC MECHANISMS:
//
// PIPES:
//   - Simple, fast
//   - Only works between related processes
//   - Unidirectional
//   - Automatically cleaned up when processes exit
//   - Use for parent-child communication
//
// NAMED PIPES (FIFOs):
//   - Like pipes but have filesystem name
//   - Works between unrelated processes
//   - Still unidirectional
//   - Must be explicitly removed
//   - Use for unrelated process communication
//
// SOCKETS:
//   - Bidirectional
//   - Works across network
//   - Works between any processes
//   - More complex setup
//   - Use for network communication or local complex IPC
//
// SHARED MEMORY:
//   - Fastest IPC
//   - Direct memory access
//   - Requires synchronization (semaphores)
//   - More complex
//   - Use when performance is critical
//
// MESSAGE QUEUES:
//   - Structured messages
//   - Multiple readers/writers
//   - Message prioritization
//   - More complex
//   - Use for complex message passing

// BIDIRECTIONAL COMMUNICATION:
//
// For two-way communication, need TWO pipes:
//
// int pipe1[2];  // Parent → Child
// int pipe2[2];  // Child → Parent
//
// pipe(pipe1);
// pipe(pipe2);
// fork();
//
// if (pid == 0) {  // Child
//   close(pipe1[1]);  // Close write end of pipe1
//   close(pipe2[0]);  // Close read end of pipe2
//   read(pipe1[0], ...);   // Read from parent
//   write(pipe2[1], ...);  // Write to parent
// } else {  // Parent
//   close(pipe1[0]);  // Close read end of pipe1
//   close(pipe2[1]);  // Close write end of pipe2
//   write(pipe1[1], ...);  // Write to child
//   read(pipe2[0], ...);   // Read from child
// }

// REAL-WORLD USES OF PIPES:
//
// 1. SHELL PIPELINES:
//    $ ls | grep .c | wc -l
//    Shell creates pipes between ls, grep, and wc
//
// 2. POPEN() FUNCTION:
//    FILE* fp = popen("ls", "r");
//    Internally uses pipe and fork
//
// 3. PROCESS COMMUNICATION:
//    Parent process launches child to do work
//    Child sends results back through pipe
//
// 4. DATA STREAMING:
//    Producer process generates data
//    Consumer process processes data
//    Pipe buffers between them

// TRY IT:
// 1. Compile: gcc pipe1.c -o pipe1
// 2. Run: ./pipe1
// 3. Observe the output showing pipe FDs and message
// 4. EXPERIMENT: Print getpid() in both child and parent
//    See that they're different processes
// 5. EXPERIMENT: Comment out close(pipefd[0]) in child
//    Program still works, but not best practice
// 6. EXPERIMENT: Send multiple messages from child
//    write(pipefd[1], "Message 1", ...);
//    write(pipefd[1], "Message 2", ...);
//    Parent needs multiple reads to get both
// 7. EXPERIMENT: Make parent write and child read (swap roles)
// 8. EXPERIMENT: Try reading twice in parent without child sending twice
//    Parent blocks on second read (no more data)
// 9. CHALLENGE: Create bidirectional communication with two pipes
// 10. CHALLENGE: Send a struct through the pipe
//     struct data { int x; char msg[50]; };
// 11. CHALLENGE: Send multiple messages and read them all in a loop
// 12. CHALLENGE: Measure time to send 1 million bytes through pipe
