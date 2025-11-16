// PEDAGOGICAL PURPOSE:
// This program demonstrates a NAMED PIPE (FIFO) SERVER for inter-process communication.
// Key learning objectives:
// 1. Understanding NAMED PIPES (FIFOs) vs unnamed pipes
// 2. Learning how to create FIFOs with mkfifo() system call
// 3. Understanding that FIFOs have filesystem names and work between unrelated processes
// 4. Learning bidirectional communication using TWO named pipes
// 5. Understanding signal handling for cleanup (SIGINT)
// 6. Seeing blocking behavior of open() on FIFOs
// 7. Learning about EOF handling and pipe reopening
// 8. Understanding the importance of unlink() to remove FIFOs

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Provides read(), write(), close(), unlink()
#include <fcntl.h>       // Provides open(), O_RDONLY, O_WRONLY
#include <sys/stat.h>    // Provides mkfifo(), file permissions
#include <string.h>      // Provides memset()
#include <signal.h>      // Provides signal(), SIGINT

// GLOBAL FILE DESCRIPTORS:
// WHY GLOBAL?
//   - Needed in signal handler (cleaner function)
//   - Signal handler can't access local variables
//   - Must be accessible from both main() and cleaner()
//
// IMPORTANT: Initialized to prevent cleanup of invalid FDs
int fdRead, fdWrite;

// SIGNAL HANDLER FOR CLEANUP:
// Called when user presses Ctrl+C (SIGINT)
//
// PURPOSE:
//   - Clean up resources before exiting
//   - Close file descriptors
//   - Remove FIFO files from filesystem
//   - Provide graceful shutdown
//
// WHY NECESSARY?
//   - FIFOs persist in filesystem
//   - If not removed, they remain after program exits
//   - Can cause issues on next run
//   - Closing FDs releases kernel resources
void cleaner(int sig){
  printf("\n Cleaning up! Goodbye!\n");

  // CLOSE FILE DESCRIPTORS:
  // Release kernel resources
  // Signals to client that server is closing
  close(fdRead);
  close(fdWrite);

  // REMOVE FIFO FILES:
  // unlink() removes file from filesystem
  //
  // "serverToClient": FIFO for server→client data
  // "clientToServer": FIFO for client→server data
  //
  // WHY UNLINK?
  //   - FIFOs are files - they persist
  //   - Next run would fail if files already exist
  //   - Clean slate for next execution
  //   - Good resource management
  //
  // WHAT IF FILES DON'T EXIST?
  //   - unlink() fails silently (we don't check return)
  //   - Not a problem - we're exiting anyway
  unlink("serverToClient");
  unlink("clientToServer");

  // EXIT PROCESS:
  // 0 indicates success
  // Program terminates here
  exit(0);
}

int main(){

  // PRINT SERVER PID:
  // Useful for debugging and process management
  //   - Can kill specific process: kill 12345
  //   - Can attach debugger: gdb -p 12345
  //   - Shows server is running
  printf("I am %d\n", getpid());

  // BUFFER SIZE AND BUFFER:
  // 1024 bytes for messages
  // Constant for clarity and maintainability
  const int bSize = 1024;
  char buffer[bSize];

  // STEP 1: CREATE NAMED PIPES (FIFOs)
  //
  // mkfifo(name, permissions):
  //   - Creates a FIFO special file
  //   - name: Path in filesystem (like a regular file)
  //   - permissions: Access permissions (like chmod)
  //
  // "clientToServer":
  //   - FIFO for client→server communication
  //   - Client writes, server reads
  //
  // "serverToClient":
  //   - FIFO for server→client communication
  //   - Server writes, client reads
  //
  // PERMISSIONS 0666:
  //   - 0: Octal prefix
  //   - 6: Owner can read/write (4+2)
  //   - 6: Group can read/write (4+2)
  //   - 6: Others can read/write (4+2)
  //   - Like chmod 666 filename
  //   - Allows any user to connect
  //
  // WHY TWO FIFOs?
  //   - FIFOs are unidirectional (like unnamed pipes)
  //   - Need one for each direction
  //   - Prevents confusion and enables full-duplex communication
  //
  // WHAT IF FIFOs ALREADY EXIST?
  //   - mkfifo() fails with error EEXIST
  //   - Common when server crashed without cleanup
  //   - Solution: rm clientToServer serverToClient
  //   - Or: check error and continue if EEXIST
  mkfifo("clientToServer", 0666);
  mkfifo("serverToClient", 0666);

  printf("Server started. Waiting for yapping...\n");

  // STEP 2: OPEN THE FIFOs
  //
  // CRITICAL: Order matters!
  //   - open() on FIFO BLOCKS until other end opens
  //   - Server opens write end first
  //   - Server then opens read end
  //   - Client must open in opposite order (read first, write second)
  //   - If both try to open write end first: DEADLOCK
  //
  // fdWrite = open("serverToClient", O_WRONLY):
  //   - Open for writing only
  //   - Server will write messages to client through this
  //   - BLOCKS here until client opens read end
  //
  // WHY BLOCKS?
  //   - FIFO needs both ends open
  //   - Prevents writing to nowhere
  //   - Ensures client is ready to receive
  //
  // WHEN UNBLOCKS?
  //   - When client runs: open("serverToClient", O_RDONLY)
  //   - Both ends are now open
  //   - Communication can begin
  fdWrite = open("serverToClient", O_WRONLY);

  // fdRead = open("clientToServer", O_RDONLY):
  //   - Open for reading only
  //   - Server will read messages from client through this
  //   - May block if client hasn't opened write end yet
  //   - Usually doesn't block because client opens write end after read end
  fdRead = open("clientToServer", O_RDONLY);

  // STEP 3: SET UP SIGNAL HANDLER
  //
  // signal(SIGINT, cleaner):
  //   - SIGINT: Interrupt signal (Ctrl+C)
  //   - cleaner: Function to call when SIGINT received
  //
  // WHY HERE INSTEAD OF EARLIER?
  //   - FIFOs are now created and open
  //   - cleaner needs to close FDs and unlink files
  //   - If signal handler was set earlier and user hit Ctrl+C during mkfifo,
  //     we'd try to close invalid FDs
  //   - Setting it here ensures resources exist before cleanup
  signal(SIGINT, cleaner);

  // STEP 4: MAIN ECHO LOOP
  // Server echoes back whatever client sends
  //
  // while(1): Infinite loop - server runs until interrupted
  while(1){
    // CLEAR BUFFER:
    // Prevent old data from contaminating new reads
    // Essential for correct string handling
    memset(buffer, 0, bSize);

    // READ FROM CLIENT:
    // Reads up to bSize bytes from clientToServer FIFO
    //
    // RETURN VALUE bytesRead:
    //   - POSITIVE: Number of bytes read (normal case)
    //   - ZERO: EOF - all write ends of FIFO are closed
    //   - NEGATIVE: Error occurred
    //
    // BLOCKING:
    //   - Blocks if FIFO is empty (no data available)
    //   - Waits for client to write data
    int bytesRead = read(fdRead, buffer, bSize);

    if(bytesRead > 0){
      // CLIENT SENT DATA

      // PRINT TO SERVER CONSOLE:
      // Shows what client sent
      // Useful for debugging and monitoring
      printf("Client: %s\n", buffer);

      // ECHO BACK TO CLIENT:
      // Send exact same data back
      // write() sends bytesRead bytes (not strlen)
      //   - bytesRead is exact number received
      //   - Handles binary data correctly
      //   - More robust than strlen() which only works for strings
      write(fdWrite, buffer, bytesRead);
     }
    else if(bytesRead == 0){
      // CLIENT CLOSED CONNECTION
      //
      // WHY bytesRead == 0?
      //   - Client closed write end of clientToServer
      //   - All write ends must be closed for EOF
      //   - This is normal when client disconnects
      //
      // REOPEN STRATEGY:
      // Close current FD and reopen to wait for new client
      //
      // WHY CLOSE AND REOPEN?
      //   - After EOF, read() will keep returning 0
      //   - Can't use same FD to wait for new client
      //   - Reopening FIFO resets the connection
      //   - Allows server to handle multiple clients sequentially
      //
      // PROCESS:
      //   1. Close current read FD
      //   2. Reopen clientToServer FIFO
      //   3. Blocks until new client connects
      //   4. If open fails, client shut down, break loop
      close(fdRead);
      fdRead = open("clientToServer", O_RDONLY);

      // CHECK IF REOPEN SUCCEEDED:
      // If fdRead < 0, open() failed
      //   - Could be no more clients
      //   - FIFO was removed
      //   - System error
      // Break loop to shutdown server
      if(fdRead < 0){
        break;
      }
    }
    else {
      // ERROR OCCURRED
      //
      // bytesRead < 0 means read() failed
      // Reasons:
      //   - FIFO closed unexpectedly
      //   - Invalid file descriptor
      //   - System error
      //
      // Response: Exit the loop, cleanup will happen in cleaner()
      break;
    }
  }

  // CLEANUP ON NORMAL EXIT:
  // If loop exits normally (not via signal)
  // Close file descriptors
  close(fdRead);
  close(fdWrite);

  // REMOVE FIFO FILES:
  // Clean up filesystem
  unlink("clientToServer");
  unlink("serverToClient");

  // Note: If server exits via signal (Ctrl+C), cleaner() does cleanup
  // This code only runs if loop breaks (client error or shutdown)
}

// EXPECTED OUTPUT (SERVER):
// I am 12345
// Server started. Waiting for yapping...
// (blocks until client connects)
// Client: Hello
// Client: How are you?
// Client: Goodbye
// (if client disconnects and reconnects)
// Client: New session
// (if Ctrl+C pressed)
//  Cleaning up! Goodbye!

// NAMED PIPES (FIFOs) vs UNNAMED PIPES:
//
// UNNAMED PIPES (pipe()):
//   - No filesystem name
//   - Only work between related processes (parent-child)
//   - Created with pipe() system call
//   - Automatically removed when processes exit
//   - More efficient (no filesystem overhead)
//   - Use for: Parent-child communication
//
// NAMED PIPES (mkfifo()):
//   - Have filesystem name (path)
//   - Work between ANY processes (related or unrelated)
//   - Created with mkfifo() system call
//   - Persist until explicitly removed (unlink)
//   - Slightly slower (filesystem overhead)
//   - Use for: Unrelated process communication

// HOW FIFOs WORK:
//
// 1. CREATE FIFO:
//    mkfifo("myfifo", 0666) creates a special file
//    $ ls -l myfifo
//    prw-rw-rw- 1 user user 0 Nov 16 10:00 myfifo
//    Notice 'p' at start (pipe type)
//
// 2. OPEN BLOCKS:
//    Server: open("myfifo", O_WRONLY) blocks
//    Client: open("myfifo", O_RDONLY) unblocks both
//
// 3. READ/WRITE:
//    Like regular files, but data doesn't go to disk
//    Goes to kernel buffer, read by other process
//
// 4. DATA FLOW:
//    Writer → Kernel Buffer → Reader
//    FIFO itself is just a name, data is in memory

// COMMON MISTAKES:
//
// MISTAKE 1: Wrong open order causing deadlock
//   // Both processes do:
//   fd = open("fifo", O_WRONLY);  // DEADLOCK!
//   Result: Both block forever waiting for reader
//   Fix: One opens read first, other opens write first
//
// MISTAKE 2: Forgetting to unlink FIFO
//   // No unlink() call
//   Result: FIFO remains in filesystem
//   Next run: mkfifo() fails because file exists
//   Fix: Always unlink() in cleanup code
//
// MISTAKE 3: Not handling bytesRead == 0
//   if (bytesRead > 0) { ... }
//   // No else if (bytesRead == 0)
//   Result: Infinite loop reading EOF
//   Fix: Check for 0 and handle disconnection
//
// MISTAKE 4: Using same FIFO for both directions
//   // Try to read and write from same FIFO
//   Result: Deadlock or data confusion
//   Fix: Use two separate FIFOs (one per direction)
//
// MISTAKE 5: Not using signal handler
//   // No signal(SIGINT, cleaner)
//   Result: Ctrl+C leaves FIFOs in filesystem
//   Fix: Always set up cleanup signal handler

// WHY REOPEN ON EOF?
//
// SCENARIO:
//   1. Client connects, server reads data
//   2. Client disconnects (closes write end)
//   3. Server's read() returns 0 (EOF)
//   4. If server tries read() again: returns 0 immediately
//   5. Loop runs forever doing nothing (busy-wait)
//
// SOLUTION:
//   - Close and reopen the FIFO
//   - New open() blocks until new client connects
//   - Server can handle multiple clients sequentially
//   - Each client gets clean session

// FIFO PERMISSIONS:
//
// 0666 = rw-rw-rw-
//   - Owner: read + write
//   - Group: read + write
//   - Others: read + write
//
// WHY 0666?
//   - Allows any user to connect as client
//   - Useful for demonstration/testing
//   - Production might use 0600 (owner only)
//
// MODIFIED BY UMASK:
//   - Actual permissions = 0666 & ~umask
//   - If umask = 0022: result = 0644 (rw-r--r--)
//   - Check with: umask command

// FILESYSTEM VISIBILITY:
//
// While server is running:
//   $ ls -l
//   prw-rw-rw- 1 user user 0 Nov 16 10:00 clientToServer
//   prw-rw-rw- 1 user user 0 Nov 16 10:00 serverToClient
//
// After cleanup:
//   Files are removed
//   $ ls -l
//   (FIFOs not shown)
//
// If server crashes (no cleanup):
//   Files remain
//   Next run: mkfifo() fails
//   Manual fix: rm clientToServer serverToClient

// TRY IT:
// 1. Compile: gcc namedPipesServer1.c -o server
// 2. Run: ./server
// 3. Server prints PID and waits
// 4. In another terminal: compile and run client
// 5. Send messages from client, see server echo
// 6. Disconnect client (Ctrl+C), server waits for new client
// 7. Connect new client, continue chatting
// 8. Press Ctrl+C on server, see cleanup message
// 9. Check: ls clientToServer serverToClient (should not exist)
// 10. EXPERIMENT: Run server, then ls -l to see FIFOs
// 11. EXPERIMENT: Try running two servers (second fails - FIFOs exist)
// 12. EXPERIMENT: Comment out signal handler, press Ctrl+C
//     Notice FIFOs remain in filesystem
// 13. CHALLENGE: Modify to handle multiple concurrent clients (use threads)
// 14. CHALLENGE: Add timestamp to each message
// 15. CHALLENGE: Log all messages to a file
