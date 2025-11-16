// PEDAGOGICAL PURPOSE:
// This program demonstrates a NAMED PIPE (FIFO) CLIENT using threads for bidirectional communication.
// Key learning objectives:
// 1. Understanding client-side FIFO usage
// 2. Learning to use threads for simultaneous read/write on FIFOs
// 3. Understanding the pthread library for creating threads
// 4. Learning why we need threads for interactive chat (non-blocking I/O simulation)
// 5. Understanding fflush() for immediate output without newline
// 6. Seeing how client and server coordinate FIFO opening order
// 7. Learning about thread functions and void* return type
// 8. Understanding the importance of strcspn() for removing newlines

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Provides read(), write(), close()
#include <fcntl.h>       // Provides open(), O_RDONLY, O_WRONLY
#include <sys/stat.h>    // Provides mkfifo() (not used here, but included)
#include <string.h>      // Provides memset(), strlen(), strcspn()
#include <pthread.h>     // Provides pthread_create(), pthread_t

// BUFFER SIZE:
// Constant for message buffer size
// Same as server (1024 bytes)
const int bSize = 1024;

// GLOBAL FILE DESCRIPTORS:
// WHY GLOBAL?
//   - Needed by both main thread and reader thread
//   - fdWrite used in main() for sending messages
//   - fdRead used in readFromServer() for receiving messages
//   - Threads share global variables
//
// ALTERNATIVE:
//   - Could pass via pthread_create() argument
//   - Global is simpler for this example
int fdWrite, fdRead;

// THREAD FUNCTION: READ FROM SERVER
// This function runs in a SEPARATE thread
//
// PURPOSE:
//   - Continuously reads messages from server
//   - Runs concurrently with main thread
//   - Allows user to type while receiving messages
//
// SIGNATURE:
//   - void* return type (pthread requirement)
//   - void* arg parameter (pthread requirement)
//   - arg is unused in this example (NULL passed)
//
// WHY SEPARATE THREAD?
//   - Main thread handles user input (fgets)
//   - This thread handles server messages
//   - Without threads: would block on either read() OR fgets()
//   - With threads: can do both simultaneously
//   - Like having two people: one listening, one talking
void *readFromServer(void *arg){
  char buffer[bSize];

  // INFINITE LOOP:
  // Thread runs forever reading messages
  // Exits only when:
  //   - read() returns 0 (server closed)
  //   - read() returns -1 (error)
  while(1){
    // CLEAR BUFFER:
    // Prepare for new message
    memset(buffer, 0, bSize);

    // READ FROM SERVER:
    // Reads from serverToClient FIFO
    // BLOCKS if no data available (waits for server)
    //
    // RETURN VALUES:
    //   - bytesRead > 0: Received data
    //   - bytesRead = 0: Server closed connection (EOF)
    //   - bytesRead < 0: Error
    int bytesRead = read(fdRead, buffer, bSize);

    if(bytesRead > 0){
      // SERVER SENT A MESSAGE

      // PRINT THE MESSAGE:
      // Display what server sent
      printf("Server: %s\n", buffer);

      // PRINT PROMPT FOR NEXT INPUT:
      // Reminds user they can type
      // Shows that we're ready for input
      printf("You: ");

      // FLUSH OUTPUT BUFFER:
      // WHY fflush()?
      //   - printf() normally buffers output
      //   - Without newline, "You: " stays in buffer
      //   - fflush() forces immediate display
      //   - Essential for interactive programs
      //
      // WITHOUT fflush():
      //   - Prompt appears after you type, not before
      //   - Confusing user experience
      //
      // WITH fflush():
      //   - Prompt appears immediately
      //   - User knows it's their turn to type
      fflush(stdout); // needed for when you don't have a \n in the printf
    }
    else if(bytesRead == 0){
      // SERVER CLOSED CONNECTION
      //
      // WHY RETURN NULL?
      //   - Thread function must return void*
      //   - NULL is conventional for "no return value"
      //   - Indicates thread is terminating normally
      //
      // WHAT HAPPENS NEXT?
      //   - Thread terminates
      //   - Main thread may or may not notice
      //   - Program should exit (server is gone)
      return NULL;
    }
  }
}

int main(){
  char message[bSize];
  pthread_t thread;  // Thread handle for reader thread

  // STEP 1: OPEN THE FIFOs
  //
  // CRITICAL: Order must be OPPOSITE of server!
  //   - Server opens: write first, then read
  //   - Client opens: read first, then write
  //   - If both open same direction first: DEADLOCK
  //
  // WHY THIS ORDER?
  //   - open() on FIFO blocks until other end opens
  //   - Client opens read end of serverToClient
  //   - Server opens write end of serverToClient
  //   - Both unblock, continue to next open
  //   - Client opens write end of clientToServer
  //   - Server opens read end of clientToServer
  //   - All four FDs now open, communication can begin
  //
  // fdRead = open("serverToClient", O_RDONLY):
  //   - Open for reading messages FROM server
  //   - Client reads what server writes
  //   - BLOCKS until server opens write end
  //
  // fdWrite = open("clientToServer", O_WRONLY):
  //   - Open for writing messages TO server
  //   - Client writes what server reads
  //   - BLOCKS until server opens read end
  fdRead = open("serverToClient", O_RDONLY);
  fdWrite = open("clientToServer", O_WRONLY);

  // STEP 2: CREATE READER THREAD
  //
  // pthread_create(handle, attributes, function, argument):
  //   - &thread: Pointer to pthread_t variable to store thread ID
  //   - NULL: Default thread attributes (no special settings)
  //   - readFromServer: Function to run in new thread
  //   - NULL: Argument to pass to readFromServer (none needed)
  //
  // WHAT HAPPENS:
  //   - New thread is created
  //   - New thread starts executing readFromServer()
  //   - Main thread continues executing HERE (doesn't wait)
  //   - Now we have TWO threads running simultaneously:
  //     - Main thread: Handles user input (below)
  //     - Reader thread: Handles server messages (readFromServer)
  //
  // WHY THREADS?
  //   - Need to read from server AND read from user simultaneously
  //   - Single thread would block on one or the other
  //   - Threads allow both operations concurrently
  //   - Chat application requires this responsiveness
  pthread_create(&thread, NULL, readFromServer, NULL);

  // INFORMATIVE MESSAGE:
  // Tell user they're connected and ready to chat
  printf("Connected to the chat. Type your messages below.\nYou: ");

  // STEP 3: MAIN INPUT LOOP
  // Read user input and send to server
  //
  // while(fgets(...) != NULL):
  //   - Reads a line from stdin (user input)
  //   - Stores in message buffer
  //   - Returns NULL on EOF (Ctrl+D) or error
  //   - Loop exits when user presses Ctrl+D
  while(fgets(message, bSize, stdin) != NULL){

    // REMOVE NEWLINE CHARACTER:
    // fgets() includes the newline '\n' in the string
    //
    // strcspn(message, "\n"):
    //   - Finds position of '\n' in message
    //   - Returns index of newline
    //
    // message[strcspn(message, "\n")] = '\0':
    //   - Replace newline with null terminator
    //   - Removes the newline from the string
    //
    // WHY REMOVE NEWLINE?
    //   - Server echoes back exactly what we send
    //   - If we send "Hello\n", server echoes "Hello\n"
    //   - When we print "Server: Hello\n", we add another newline
    //   - Result: "Server: Hello\n\n" (extra blank line)
    //   - Cleaner to remove newline before sending
    //
    // EXAMPLE:
    //   User types: "Hello<Enter>"
    //   fgets reads: "Hello\n\0"
    //   strcspn returns: 5 (position of \n)
    //   After replacement: "Hello\0"
    message[strcspn(message, "\n")] = '\0';

    // SKIP EMPTY MESSAGES:
    // If user just pressed Enter without typing anything
    //
    // strlen(message) == 0:
    //   - Message is empty (just null terminator)
    //   - No point sending to server
    //
    // continue:
    //   - Skip to next iteration
    //   - Prompt appears again for new input
    if (strlen(message) == 0) continue;

    // SEND MESSAGE TO SERVER:
    // Write to clientToServer FIFO
    //
    // write(fdWrite, message, strlen(message)):
    //   - fdWrite: Write end of clientToServer
    //   - message: The user's message
    //   - strlen(message): Number of bytes to send (without null terminator)
    //
    // NOTE: We don't send null terminator here
    //   - Server uses bytesRead to know length
    //   - Server's buffer is memset to 0, so null-terminated anyway
    //   - Could send strlen(message) + 1 to include null terminator
    //
    // RETURN VALUE CHECK:
    //   - write() returns number of bytes written
    //   - Returns < 0 on error
    //   - If error: server probably disconnected, break loop
    if (write(fdWrite, message, strlen(message)) < 0) {
      break;
    }

    // PROMPT FOR NEXT MESSAGE:
    // Show "You: " for next input
    printf("You: ");

    // FLUSH OUTPUT:
    // Ensure prompt appears immediately
    // Without this, prompt might not show until after you type
    fflush(stdout); // needed for when you don't have a \n in the printf
  }

  // CLEANUP:
  // User pressed Ctrl+D or write() failed
  // Close file descriptors
  close(fdWrite);
  close(fdRead);

  return 0;
}

// EXPECTED OUTPUT (CLIENT SIDE):
// Connected to the chat. Type your messages below.
// You: Hello
// Server: Hello
// You: How are you?
// Server: How are you?
// You: Goodbye
// Server: Goodbye
// You: ^D
// (program exits)

// THE THREADING MODEL:
//
// Main Thread:
//   - Runs main() function
//   - Reads user input with fgets()
//   - Sends messages to server with write()
//   - Blocks on fgets() waiting for user
//
// Reader Thread:
//   - Runs readFromServer() function
//   - Reads server messages with read()
//   - Prints messages to console
//   - Blocks on read() waiting for server
//
// CONCURRENCY:
//   - Both threads run simultaneously
//   - While main thread waits for user input
//   - Reader thread can receive and display server messages
//   - This makes chat feel responsive and interactive

// WHY fflush(stdout)?
//
// BUFFERING IN C:
//   - printf() doesn't immediately print
//   - Output goes to buffer first
//   - Buffer is flushed:
//     - When newline '\n' is printed
//     - When buffer is full
//     - When fflush() is called
//     - When program exits
//
// WITHOUT fflush():
//   printf("You: ");  // No newline, stays in buffer
//   fgets(...);       // User types, but "You: " hasn't appeared yet!
//   // Confusing experience
//
// WITH fflush():
//   printf("You: ");
//   fflush(stdout);   // Forces "You: " to appear immediately
//   fgets(...);       // User sees prompt before typing
//   // Better user experience

// FIFO OPENING ORDER:
//
// SERVER                          CLIENT
// ------                          ------
// mkfifo("serverToClient")        (FIFOs already exist)
// mkfifo("clientToServer")
//
// fdWrite = open(                 fdRead = open(
//   "serverToClient",               "serverToClient",
//   O_WRONLY);                      O_RDONLY);
// [BLOCKS]                        [BLOCKS then UNBLOCKS]
// [UNBLOCKS]
//
// fdRead = open(                  fdWrite = open(
//   "clientToServer",               "clientToServer",
//   O_RDONLY);                      O_WRONLY);
// [BLOCKS then UNBLOCKS]          [BLOCKS then UNBLOCKS]
//
// All four FDs now open
// Communication begins

// COMMON MISTAKES:
//
// MISTAKE 1: Wrong FIFO opening order
//   fdWrite = open("clientToServer", O_WRONLY);  // Opens write first
//   fdRead = open("serverToClient", O_RDONLY);
//   Result: Deadlock if server opens in same order
//   Fix: Client must open opposite order from server
//
// MISTAKE 2: Not using threads
//   // No pthread_create, just read and write in one thread
//   Result: Can't receive messages while typing
//   Can't type while waiting for server
//   Fix: Use threads for simultaneous I/O
//
// MISTAKE 3: Not removing newline
//   // Don't use strcspn to remove \n
//   Result: Server echoes "message\n", client prints "Server: message\n\n"
//   Extra blank lines in output
//   Fix: Remove newline before sending
//
// MISTAKE 4: Not flushing output
//   printf("You: ");  // No fflush
//   Result: Prompt doesn't appear until after you type
//   Fix: fflush(stdout) after prompt
//
// MISTAKE 5: Forgetting to close FDs
//   // No close() calls
//   Result: File descriptors leak
//   Server might not get EOF
//   Fix: Always close FDs when done

// THREAD SAFETY CONCERNS:
//
// IN THIS PROGRAM:
//   - Main thread only writes to fdWrite
//   - Reader thread only reads from fdRead
//   - No shared writable data
//   - Therefore: Thread-safe (no race conditions)
//
// IF WE SHARED DATA:
//   - Need mutexes for synchronization
//   - Example: Both threads writing to same buffer
//   - Would need pthread_mutex_lock/unlock
//   - Not needed here because operations are separate

// ALTERNATIVE APPROACHES:
//
// APPROACH 1: Non-blocking I/O
//   fcntl(fdRead, F_SETFL, O_NONBLOCK);
//   while(1) {
//     read(fdRead, ...);  // Returns immediately if no data
//     // Check stdin for user input
//   }
//   Pro: No threads needed
//   Con: Busy-waiting, uses CPU
//
// APPROACH 2: select() system call
//   select() to wait on multiple file descriptors
//   Wait for either fdRead OR stdin to have data
//   Pro: Efficient, no threads, no busy-waiting
//   Con: More complex code
//
// APPROACH 3: Threads (our approach)
//   One thread per I/O source
//   Pro: Simple, clear code
//   Con: Thread overhead

// TRY IT:
// 1. First, compile and run server: ./server
// 2. Compile client: gcc namedPipesClient1.c -o client -pthread
//    (Note: -pthread flag is REQUIRED for pthread functions)
// 3. Run client: ./client
// 4. Type messages, see server echo them back
// 5. While typing a message, have friend send you one from server
//    See that you receive it while typing (concurrency!)
// 6. Press Ctrl+D to exit client gracefully
// 7. EXPERIMENT: Comment out pthread_create and readFromServer
//    See how client can only send, not receive
// 8. EXPERIMENT: Remove fflush(stdout), see delayed prompts
// 9. EXPERIMENT: Don't remove newline with strcspn()
//    See extra blank lines in output
// 10. EXPERIMENT: Add printf in readFromServer to show raw bytes received
// 11. CHALLENGE: Modify to handle "quit" command to exit gracefully
// 12. CHALLENGE: Add colors to distinguish your messages from server's
// 13. CHALLENGE: Add timestamps to each message
// 14. CHALLENGE: Implement file transfer (send file contents through FIFO)
