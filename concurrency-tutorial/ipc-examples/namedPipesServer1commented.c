// this is the server
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

/*
 * Global file descriptors for the named pipes
 * 
 * TEACHING POINT: By making these global, the signal handler can access them
 * for cleanup. This follows the pattern of initializing resources early and
 * cleaning them up on program termination or interruption.
 */
int fdRead, fdWrite;

/*
 * Signal handler for clean termination
 * 
 * TEACHING POINT: This demonstrates proper resource cleanup when handling
 * signals like SIGINT (Ctrl+C). Named pipes must be explicitly unlinked
 * to remove them from the filesystem, unlike regular pipes.
 * 
 * Without this handler, the FIFOs would remain in the filesystem after
 * the server terminates, potentially causing issues on restart.
 */
void cleaner(int sig){
  printf("\n Cleaning up! Goodbye!\n");
  
  /*
   * Close the pipe file descriptors
   */
  close(fdRead);
  close(fdWrite);
  
  /*
   * Remove the named pipes from the filesystem
   * 
   * TEACHING POINT: Named pipes (FIFOs) persist in the filesystem
   * and must be explicitly unlinked when no longer needed.
   */
  unlink("serverToClient");
  unlink("clientToServer");
  
  /*
   * Terminate the program
   */
  exit(0);
}

int main(){
  /*
   * Print the server's process ID
   * 
   * TEACHING POINT: This is useful for debugging and for sending
   * signals to the server process if needed.
   */
  printf("I am %d\n",getpid());
  
  /*
   * Define buffer size and create a buffer for message handling
   */
  const int bSize = 1024;
  char buffer[bSize];

  /*
   * Create the named pipes (FIFOs)
   * 
   * TEACHING POINT: Named pipes are created in the filesystem and
   * persist until explicitly deleted. Unlike regular pipes, they
   * have pathnames and can be accessed by unrelated processes.
   * 
   * The 0666 parameter specifies permissions (read/write for all users)
   */
  mkfifo("clientToServer", 0666);
  mkfifo("serverToClient", 0666);

  printf("Server started. Waiting for yapping...\n");
  
  /*
   * Open the named pipes for reading and writing
   * 
   * TEACHING POINT: Unlike anonymous pipes, named pipes must be
   * explicitly opened with open(). Their order matters here:
   * 
   * 1. Open the write pipe first, then the read pipe
   * This is important because opening a FIFO blocks until the other
   * end is opened by another process (unless O_NONBLOCK is specified).
   * 
   * By opening write then read, we match the client's read then write
   * pattern, preventing deadlock.
   */
  fdWrite = open("serverToClient", O_WRONLY);
  fdRead = open("clientToServer", O_RDONLY);

  /*
   * Register the signal handler for SIGINT (Ctrl+C)
   * 
   * TEACHING POINT: This ensures resources are properly cleaned up
   * even if the server is terminated with Ctrl+C.
   */
  signal(SIGINT, cleaner);

  /*
   * Main server loop
   * 
   * TEACHING POINT: This implements a simple echo server: it reads
   * messages from the client and writes them back unchanged.
   */
  while(1){
    /*
     * Clear the buffer before reading
     * 
     * TEACHING POINT: This prevents partial messages from being
     * combined with previous message data.
     */
    memset(buffer, 0, bSize);
    
    /*
     * Read from the "clientToServer" pipe
     */
    int bytesRead = read(fdRead, buffer, bSize);
    
    if(bytesRead > 0){
      /*
       * If we received data, echo it back to the client
       * 
       * TEACHING POINT: This implements the core of the echo server:
       * 1. Print the received message to the server console
       * 2. Write the same message back to the client through the other pipe
       */
      printf("Client: %s\n", buffer);
      write(fdWrite, buffer, bytesRead);
     }
    else if(bytesRead == 0){
      /*
       * If read returned 0, the client closed their end of the pipe
       * 
       * TEACHING POINT: This shows how to handle client disconnection
       * gracefully. The server:
       * 1. Closes the current read pipe
       * 2. Re-opens it, waiting for a new client connection
       * 3. Breaks the loop if re-opening fails
       * 
       * This allows the server to handle multiple sequential client
       * connections without restarting.
       */
      close(fdRead);
      fdRead = open("clientToServer", O_RDONLY);
      if(fdRead < 0){
        break;
      }
    }
    else {
      /*
       * If read returned a negative value, an error occurred
       */
      break;
    }
  }

  /*
   * Clean up resources before exiting
   * 
   * TEACHING POINT: Always clean up resources, even in error cases:
   * 1. Close file descriptors to release system resources
   * 2. Unlink named pipes to remove them from the filesystem
   */
  close(fdRead);
  close(fdWrite);
  unlink("clientToServer");
  unlink("serverToClient");
}